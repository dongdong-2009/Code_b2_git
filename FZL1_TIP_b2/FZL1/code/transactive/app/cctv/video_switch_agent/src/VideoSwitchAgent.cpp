/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Katherine Thomson
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Implements the IGenericAgentUser interface.  Handles Agent specific
  * tasks required on start up and shutdown, as well as process
  * management messages and instructions originating from the System Controller.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <map>

#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/VisualStateUpdate_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/DefaultEntity.h"
#include "core/naming/src/Naming.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "app/cctv/video_switch_agent/src/VideoSwitchAgent.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "app/cctv/video_switch_agent/src/EntityCreationFactory.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include <boost/foreach.hpp>


// Failover
namespace
{
    template< typename T > class ThreadedMemberFunction : public TA_Base_Core::Thread
    {
        public:

            typedef void (T::*MemberFunction)();
            typedef std::vector< MemberFunction > MemberFunctionList;

            static ThreadedMemberFunction& alloc(T& o)
            {
                instance()->clear();
                instance() = new ThreadedMemberFunction(o);
                return *instance();
            }

            void add(MemberFunction mf)
            {
                m_members.push_back(mf);
            }

            virtual void run()
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Entered: ThreadedMemberFunction, there are [%d] member functions to call", m_members.size());
                for(typename MemberFunctionList::iterator mfIter = m_members.begin(); mfIter != m_members.end(); ++mfIter)
                {
                    try
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "calling [%d] member function", std::distance(m_members.begin(), mfIter));
                        (m_object.*(*mfIter))();
                    }
                    catch(...)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "");
                    }
                }
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Exited: ThreadedMemberFunction");
            }

            virtual void terminate() {}

        private:

            ThreadedMemberFunction(T& o)
                : m_object(o)
            {
            }

            ~ThreadedMemberFunction()
            {
                terminateAndWait();
            }

            static ThreadedMemberFunction*& instance()
            {
                static ThreadedMemberFunction* m_instance = NULL;
                return m_instance;
            }

            static void clear()
            {
                delete instance();
                instance() = NULL;
            }

            T& m_object;
            MemberFunctionList m_members;
    };

    typedef ThreadedMemberFunction< TA_IRS_App::VideoSwitchAgent > VideoSwitchAgentThreadedMemberFunction;

	static const std::string CCTV_ALARM_ENTITY_TYPE = "CctvAlarm";
}

namespace TA_IRS_App
{
    //
    // VideoSwitchAgent
    //
    VideoSwitchAgent::VideoSwitchAgent(int argc, char* argv[])
        : m_genericAgent(0),
          m_communicationsHandler(0),
          m_running(false)
    {
        FUNCTION_ENTRY("VideoSwitchAgent");
        // Construct the GenericAgent object that manages this application.
        m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);
        // get the agent name
        m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
        // Ensure correct agent entity type has been passed as command-line argument, else log error and throw exception to top level
        // as irrecoverable situation.
        m_agentConfig =
            boost::dynamic_pointer_cast<TA_Base_Core::VideoSwitchAgent>(m_genericAgent->getAgentEntityConfiguration());
        if(NULL == m_agentConfig.get())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "The VideoSwitchAgent was passed an incorrect entity type as agent entity.");
            cleanUp();
            TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::INVALID_ENTITY_TYPE));
        }
        // Create communications handler.
        try
        {
            m_communicationsHandler = std::auto_ptr< CommunicationsHandler >(new CommunicationsHandler(m_agentConfig));
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            // If this fails we must clean up what has been created, then throw to top level as
            // irrecoverable situation.
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Failed to create Communications Handler.");
            cleanUp();
            throw;
        }
        m_running = true;
        FUNCTION_EXIT;
    }


    //
    // ~VideoSwitchAgent
    //
    VideoSwitchAgent::~VideoSwitchAgent()
    {
        FUNCTION_ENTRY("Destructor");
        // If an exception is thrown here, there is nothing we can do,
        // but we should make sure no exceptions are thrown out of this destructor.
        try
        {
            cleanUp();
        }
        catch(const TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Reason unknown");
        }
        FUNCTION_EXIT;
    }


    //
    // agentRun
    //
    void VideoSwitchAgent::agentRun()
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "agentRun");
        TA_ASSERT(m_genericAgent != NULL, "GenericAgent has not been initialised");
        if(TA_Base_Core::Control == m_genericAgent->getOperationMode())
        {
            setChildrenToControl();
            agentSetControl();
        }
        else
        {
            setChildrenToMonitor();
            agentSetMonitor();
        }
        // Start GenericAgent running.
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Starting GenericAgent");
        m_genericAgent->run();
        LOG_FUNCTION_EXIT(SourceInfo, "agentRun");
    }


    //
    // agentTerminate
    //
    void VideoSwitchAgent::agentTerminate()
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "agentTerminate");
        // Stop accepting Corba requests by deactivating and deregistering the SwitchManager.
        // Don't delete it though - it's a singleton!
        SwitchManager::getInstance().deactivateServant();
        // All the entities that are stored in the Generic Agent entity map
        // are CORBA servants.  However, Generic Agent will call stop() on these
        // objects as part of it's onTerminate() method.  This will deactivate them,
        // so they can be safely deleted.
        // Stop receiving messages.
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(&m_stateUpdateSubscriber);
        LOG_FUNCTION_EXIT(SourceInfo, "agentTerminate");
    }

    bool VideoSwitchAgent::createAllEntities(TA_Base_Core::IEntityDataPtr agentEntityData,
                                             const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                             TA_Base_Bus::IEntityList& createdEntities)
    {
        //TA_IRS_Bus::TrainAgentProxyFactory::getInstance().agentInitialise(agentEntityData);
        bool createdDnSpecificUtils = false;
        unsigned long entityKey = 0;
		BOOST_FOREACH(const boost::shared_ptr<TA_Base_Core::IEntityData>& i, entityDataList)
		{
			try
			{
                entityKey = i->getKey();
				if(i->getType() == CCTV_ALARM_ENTITY_TYPE)
				{
					TA_Base_Core::DefaultEntity* cctvAlarmEntity = new TA_Base_Core::DefaultEntity(i->getKey(), i->getType());
					SwitchManager::getInstance().addCctvAlarmEntity(cctvAlarmEntity);
				}
				else
				{
	                createdEntities.push_back(TA_Base_Bus::IEntityList::value_type(i->getKey(), createEntity(i)));
				}
			}
            catch(TA_Base_Core::OperationModeException&)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                            "Can not create entity[%lu] in monitor mode.", entityKey);
                throw;
            }
		}
		TA_Base_Core::ILocation::ELocationType locationType = 
		    TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(agentEntityData->getLocationName());
		if(TA_Base_Core::ILocation::OCC == locationType)
		{
			// OCC Location should load all other stations video input objects
			std::vector<TA_Base_Core::ILocation*> locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
			BOOST_FOREACH(TA_Base_Core::ILocation* i, locations)
			{
				unsigned long locationKey = i->getKey();
				if(agentEntityData->getLocation() != locationKey)
				{
					
					TA_Base_Core::IEntityDataList cameraEntities =
						TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( TA_Base_Core::Camera::getStaticType(), locationKey);
					TA_Base_Core::IEntityDataList sequenceEntities =
						TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( TA_Base_Core::Sequence::getStaticType(), locationKey);

					LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "get camera entities at location:[%ld]",locationKey );				
					BOOST_FOREACH(TA_Base_Core::IEntityData* i, cameraEntities)
					{

						TA_Base_Core::IEntityDataPtr entityData = TA_Base_Core::SharedIEntityDataList::value_type(i);
						createdEntities.push_back(TA_Base_Bus::IEntityList::value_type(i->getKey(), createEntity(entityData)) );
						LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Insert Entity into createEntities:Key[%ld],Name[%s]",
							i->getKey(), i->getName().c_str());
					}
					LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "get sequence entities at location:[%ld]",locationKey );
					BOOST_FOREACH(TA_Base_Core::IEntityData* i, sequenceEntities)
					{
						TA_Base_Core::IEntityDataPtr entityData = TA_Base_Core::SharedIEntityDataList::value_type(i);
						createdEntities.push_back(TA_Base_Bus::IEntityList::value_type(i->getKey(), createEntity(entityData)) );
						LOG2( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Insert Entity into createEntities:Key[%ld],Name[%s]",
							i->getKey(), i->getName().c_str() );
					}
				}

			}
		}
		return true;
    }


    void VideoSwitchAgent::entityCreationComplete()
    {
    }



    //
    // createEntity
    //
    TA_Base_Bus::IEntity* VideoSwitchAgent::createEntity(TA_Base_Core::IEntityDataPtr entityData)
    {
        FUNCTION_ENTRY("createEntity");
        TA_Base_Bus::IEntity* theEntity = NULL;
        if(!m_running)
        {
	        std::string entityDataType = entityData->getType();
	        theEntity = EntityCreationFactory::getInstance().createEntity(entityData);
        }
        FUNCTION_EXIT;
        return theEntity;
    }


    //
    // getAgentConfig
    //
    TA_Base_Core::VideoSwitchAgentPtr& VideoSwitchAgent::getAgentConfig()
    {
        return m_agentConfig;
    }


    //
    // agentSetMonitor
    //
    void VideoSwitchAgent::agentSetMonitor()
    {
		FUNCTION_ENTRY("agentSetMonitor");
        // else do nothing
        // Stop accepting Corba requests by deactivating and deregistering the SwitchManager.
        // Don't delete it though - it's a singleton!
        SwitchManager::getInstance().deactivateServant();
        // Finally, tell the object responsible for communicating with the switching
        // hardware that the mode has changed.
        m_communicationsHandler->disconnect();
        AgentModeMonitor::getInstance().setToMonitorMode();
        // Get the current state from the Control mode agent.
        StateUpdateBroadcaster::getInstance().requestAgentState();
        FUNCTION_EXIT;
    }


    //
    // agentSetControl
    //
    void VideoSwitchAgent::agentSetControl()
    {
        FUNCTION_ENTRY("agentSetControl");
        AgentModeMonitor::getInstance().setToControlMode();
        //
        // Tell the object responsible for communicating with the switching hardware
        // that the mode has changed.
        //
        m_communicationsHandler->connect();
        //
        // Now that we're ready to accept Corba requests, register the SwitchManager.
        //
        SwitchManager::getInstance().activate();
        FUNCTION_EXIT;
    }


    //
    // notifyGroupOffline
    //
    void VideoSwitchAgent::notifyGroupOffline(const std::string& group)
    {
        // FUTURE TODO.
    }


    //
    // notifyGroupOnline
    //
    void VideoSwitchAgent::notifyGroupOnline(const std::string& group)
    {
    }


    //
    // setChildrenToControl
    //
    void VideoSwitchAgent::setChildrenToControl()
    {
        //
        // This should only be called once before GenericAgent::run is called.
        // Thereafter GenericAgent will set the children if mode changes.
        //
        typedef const std::map< unsigned long, TA_Base_Bus::IEntity* > EntityMap;
        EntityMap* entities = m_genericAgent->getEntityMap();
		BOOST_FOREACH(const EntityMap::value_type& i, *entities)
		{
			i.second->setToControlMode();
		}
    }


    //
    // setChildrenToMonitor
    //
    void VideoSwitchAgent::setChildrenToMonitor()
    {
        //
        // This should only be called once before GenericAgent::run is called.
        // Thereafter GenericAgent will set the children if mode changes.
        //
        typedef const std::map< unsigned long, TA_Base_Bus::IEntity* > EntityMap;
        EntityMap* entities = m_genericAgent->getEntityMap();
		BOOST_FOREACH(const EntityMap::value_type& i, *entities)
		{
			i.second->setToMonitorMode();
		}
    }

    //
    // registerForControlMessages
    //
    void VideoSwitchAgent::registerForControlMessages()
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(&m_stateUpdateSubscriber);
        // Subscribe for VideoSwitchAgentStateUpdateRequest messages.
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdateRequest,
            &m_stateUpdateSubscriber, TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str());
    }


    //
    // registerForMonitorMessages
    //
    void VideoSwitchAgent::registerForMonitorMessages()
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(&m_stateUpdateSubscriber);
        // Subscribe for VideoSwitchAgentStateUpdate messages.
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate,
            &m_stateUpdateSubscriber, TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str());
    }


    //
    // cleanUp
    //
    void VideoSwitchAgent::cleanUp()
    {
        FUNCTION_ENTRY("cleanUp");
        EntityCreationFactory::getInstance().shutdown();
        // Stop the subscriber from receiving messages.
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(&m_stateUpdateSubscriber);
        // Deactivate and delete the SwitchManager.
        SwitchManager::getInstance().deactivateAndDeleteServant();
        // Deactivate and delete the Generic Agent.
        m_genericAgent->deactivateAndDeleteServant();
        FUNCTION_EXIT;
    }


    //
    // registerForStateUpdates
    //
    void VideoSwitchAgent::registerForStateUpdates()
    {
        // Don't use m_genericAgent->getOperationMode() here, because m_genericAgent
        // calls this method as part of its construction.
        if(AgentModeMonitor::getInstance().isInControlMode())
        {
            VideoSwitchAgentThreadedMemberFunction& thrd = VideoSwitchAgentThreadedMemberFunction::alloc(*this);
            thrd.add(&VideoSwitchAgent::registerForControlMessages);
            thrd.start();
        }
        else
        {
            VideoSwitchAgentThreadedMemberFunction& thrd = VideoSwitchAgentThreadedMemberFunction::alloc(*this);
            thrd.add(&VideoSwitchAgent::registerForMonitorMessages);
            thrd.start();
        }
    }


    //
    // receiveSpecialisedMessage
    //
    void VideoSwitchAgent::receiveSpecialisedMessage(
        const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
        // This is handled by the StateUpdateSubscriber object.
    }


    //
    // processOnlineUpdate
    //
    void VideoSwitchAgent::processOnlineUpdate(
        const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
    }
} // TA_IRS_App
