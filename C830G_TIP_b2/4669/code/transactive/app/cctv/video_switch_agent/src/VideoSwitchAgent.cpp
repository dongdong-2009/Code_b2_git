/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/VideoSwitchAgent.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/VisualStateUpdate_MessageTypes.h"
#include "core/naming/src/Naming.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/generic_agent/src/IEntity.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "app/cctv/video_switch_agent/src/VideoSwitchAgent.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "app/cctv/video_switch_agent/src/EntityCreationFactory.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"


//limin++, failover
namespace
{
    template< typename T > class ThreadedMemberFunction : public TA_Base_Core::Thread
    {
    public:
        
        typedef void ( T::*MemberFunction )();
        typedef std::vector< MemberFunction > MemberFunctionList;
        
        static ThreadedMemberFunction& alloc( T& o )
        {
            instance()->clear();
            instance() = new ThreadedMemberFunction( o );
            
            return *instance();
        }
        
        void add( MemberFunction mf )
        {
            m_members.push_back( mf );
        }
        
        virtual void run()
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Entered: ThreadedMemberFunction, there are [%d] member functions to call", m_members.size() );
            
            for ( typename MemberFunctionList::iterator mfIter = m_members.begin(); mfIter != m_members.end(); ++mfIter )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "calling [%d] member function", std::distance( m_members.begin(), mfIter ) );
                    
                    ( m_object.*( *mfIter ) )();
                }
                catch (...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "" );
                }
            }
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Exited: ThreadedMemberFunction" );
        }
        
        virtual void terminate(){}
        
    private:
        
        ThreadedMemberFunction( T& o )
            : m_object( o )
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
}
//++limin, failover


namespace TA_IRS_App
{
    //
    // VideoSwitchAgent
    //
    VideoSwitchAgent::VideoSwitchAgent( int argc, char* argv[] )
        : m_genericAgent( 0 )
		, m_communicationsHandler( 0 ),
	      m_running(false),
		  m_dpBulkPolledInterface (NULL),
		  m_dpAccessInterface (NULL),
		  m_dnBulkPolledInterface(NULL),
		  m_dnAccessInterface(NULL),
		  m_scadaProxyFactory(NULL),
		  m_summaryProcessor(NULL),
          m_dataPointFactory (NULL)

    {
        FUNCTION_ENTRY( "VideoSwitchAgent" );

		// get a handle to the ScadaProxyFactory
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

        m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);
        COESwitchManager::getInstance().registerDataPointFactory(m_dataPointFactory);

        // Construct the GenericAgent object that manages this application.
        m_genericAgent = new TA_Base_Bus::GenericAgent( argc, argv, this );
		// get the agent name
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
        
        // Ensure correct agent entity type has been passed as command-line argument, else log error and throw exception to top level
        // as irrecoverable situation.
        m_agentConfig = 
			boost::dynamic_pointer_cast<TA_Base_Core::VideoSwitchAgent>(m_genericAgent->getAgentEntityConfiguration());

        if ( NULL == m_agentConfig.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "The VideoSwitchAgent was passed an incorrect entity type as agent entity." );
            cleanUp();
            TA_THROW( TA_Base_Core::GenericAgentException( TA_Base_Core::GenericAgentException::INVALID_ENTITY_TYPE ) );            
        }

        // Create communications handler.
        try
        {
            m_communicationsHandler = std::auto_ptr< CommunicationsHandler >( new CommunicationsHandler( m_agentConfig ) );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            // If this fails we must clean up what has been created, then throw to top level as
            // irrecoverable situation.
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to create Communications Handler." );
            cleanUp();
            throw;
        }

		// register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_agentName , m_genericAgent->getEntityMap());
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();
		
		// create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());


		m_running=true;
        FUNCTION_EXIT;
    }


    //
    // ~VideoSwitchAgent
    //
    VideoSwitchAgent::~VideoSwitchAgent()
    {
        FUNCTION_ENTRY( "Destructor" );

        // If an exception is thrown here, there is nothing we can do, 
        // but we should make sure no exceptions are thrown out of this destructor.
        try
        {
            cleanUp();
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "Reason unknown" );           
        }

        FUNCTION_EXIT;
    }


    //
    // agentRun
    //
    void VideoSwitchAgent::agentRun()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "agentRun" );

		TA_ASSERT( m_genericAgent != NULL, "GenericAgent has not been initialised" );

		// create an instance of the class DataPointAgentCorbaDef_Impl
		m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl (m_agentName , this );

		TA_ASSERT(m_dpBulkPolledInterface != NULL, "can't create bulk poll interface");
		if ( m_genericAgent->getOperationMode() == TA_Base_Core::Control )
		{
			// start receiving remote requests
			m_dpBulkPolledInterface->setToControl();
		}
		else
		{
			// stop receiving remote requests
			m_dpBulkPolledInterface->setToMonitor();
		}

		// create an instance of the class DataPointAgentCorbaDef_Impl
		m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl (m_agentName , this );

		TA_ASSERT(m_dpAccessInterface != NULL, "can't create datapoint access interface");

		if ( m_genericAgent->getOperationMode() == TA_Base_Core::Control )
		{
			// start receiving remote requests
			m_dpAccessInterface->setToControl();
		}
		else
		{
			// stop receiving remote requests
			m_dpAccessInterface->setToMonitor();
		}

		// create an instance of the class DataPointAgentCorbaDef_Impl
		m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl (m_agentName , this );
		
		TA_ASSERT(m_dnBulkPolledInterface != NULL, "can't create bulk poll interface");
		if ( m_genericAgent->getOperationMode() == TA_Base_Core::Control )
		{
			// start receiving remote requests
			m_dnBulkPolledInterface->setToControl();
		}
		else
		{
			// stop receiving remote requests
			m_dnBulkPolledInterface->setToMonitor();
		}
		
		// create an instance of the class DataPointAgentCorbaDef_Impl
		m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl (m_agentName , this );
		
		TA_ASSERT(m_dnAccessInterface != NULL, "can't create datapoint access interface");
		
		if ( m_genericAgent->getOperationMode() == TA_Base_Core::Control )
		{
			// start receiving remote requests
			m_dnAccessInterface->setToControl();
		}
		else
		{
			// stop receiving remote requests
			m_dnAccessInterface->setToMonitor();
		}
		
        //
        // Manually set all the monitored entities to the appropriate mode before starting
        // GenericAgent. Thereafter it will be responsible for mode changes as required.
        //
        if ( TA_Base_Core::Control == m_genericAgent->getOperationMode() )
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

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Starting GenericAgent" );
        m_genericAgent->run();

        LOG_FUNCTION_EXIT( SourceInfo, "agentRun" );
    }


    //
    // agentTerminate
    //
    void VideoSwitchAgent::agentTerminate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "agentTerminate" );

        // Stop accepting Corba requests by deactivating and deregistering the SwitchManager.
        // Don't delete it though - it's a singleton!
        
        SwitchManager::getInstance().deactivateServant();

        // All the entities that are stored in the Generic Agent entity map
        // are CORBA servants.  However, Generic Agent will call stop() on these
        // objects as part of it's onTerminate() method.  This will deactivate them,
        // so they can be safely deleted.
        
        // Stop receiving messages.

        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( &m_stateUpdateSubscriber );
		// if bulk interface is created
		if ( NULL != m_dpBulkPolledInterface )
		{
			delete m_dpBulkPolledInterface ;
			m_dpBulkPolledInterface = NULL;
		}

		if ( NULL != m_dpAccessInterface )
		{
			delete m_dpAccessInterface ;
			m_dpAccessInterface = NULL;
		}

		if ( NULL != m_dnBulkPolledInterface )
		{
			delete m_dnBulkPolledInterface ;
			m_dnBulkPolledInterface = NULL;
		}
		
		if ( NULL != m_dnAccessInterface )
		{
			delete m_dnAccessInterface ;
			m_dnAccessInterface = NULL;
		}

        LOG_FUNCTION_EXIT( SourceInfo, "agentTerminate" );
    }

    bool VideoSwitchAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
        const TA_Base_Core::SharedIEntityDataList& entityDataList,
        TA_Base_Bus::IEntityList& createdEntities)
    {
		TA_IRS_Bus::TrainAgentProxyFactory::getInstance().agentInitialise(agentEntityData);
        bool createdDnSpecificUtils = false;
        
        for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin();
        iter != entityDataList.end(); ++iter )
        {
            // only datanodes need these things, so if there are no datanodes, why create them?
            if ( (false == createdDnSpecificUtils) &&
                ((*iter)->getType() == TA_Base_Core::DataNodeEntityData::getStaticType()) )
            {
                // create an instance of DataNodeAccessFactory if not done already
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Start of create DataNode Hierarchy Map" );
                
                TA_Base_Core::DataNodeAccessFactory::getInstance()->createDataNodeHierarchyMap(agentEntityData->getKey(), entityDataList);
                
                // initialise ScadaPersistence
                m_persistence = TA_Base_Bus::ScadaPersistencePtr( TA_Base_Bus::ScadaPersistence::createScadaPersistenceForAgent(TA_Base_Bus::ScadaPersistence::DATANODE, agentEntityData->getKey()) );
                
                createdDnSpecificUtils = true;
            }
            
            createdEntities.push_back( 
                TA_Base_Bus::IEntityList::value_type( (*iter)->getKey(), createEntity(*iter)) );
        }
        
        // return true to indicate not to call createEntity
        return true;
    }
    
    
    void VideoSwitchAgent::entityCreationComplete()
    {
        // loading complete, free some memory
        TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
    }
    
    
    
    //
    // createEntity
    //
    TA_Base_Bus::IEntity* VideoSwitchAgent::createEntity(
        TA_Base_Core::IEntityDataPtr entityData )
    {
		FUNCTION_ENTRY("createEntity");

		if(m_running)
		{
			return NULL;
		}

		TA_Base_Bus::IEntity * theEntity = NULL;
		std::string entityDataType = entityData->getType();

		if(entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType())
		{
			TA_Base_Bus::DataNode* dn = NULL;
			try
			{
				dn = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(entityData), m_persistence);
                
                m_dataNodes.push_back( dn );
				theEntity = dn;
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    entityData->getName().c_str(), entityData->getKey() );
				
				// To ensure the generic agent is constructed even if one DataNode creation fails
			}
		}
		else
		{
			theEntity = EntityCreationFactory::getInstance().createEntity( entityData );
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
        LOG_FUNCTION_ENTRY( SourceInfo, "agentSetMonitor" );

        //TD16761 Mintao++
		// if bulk interface is set up
		if ( NULL != m_dpBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dpBulkPolledInterface->setToMonitor();
		}

		if ( NULL != m_dpAccessInterface )
		{
			// stop receiving remote requests
			m_dpAccessInterface->setToMonitor();
		}

		if ( NULL != m_dnBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dnBulkPolledInterface->setToMonitor();
		}
		
		if ( NULL != m_dnAccessInterface )
		{
			// stop receiving remote requests
			m_dnAccessInterface->setToMonitor();
		}

		// set all datanodes to monitor mode
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->setOperationMode( false );
		}
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

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


        LOG_FUNCTION_EXIT( SourceInfo, "agentSetMonitor" );
    }


    //
    // agentSetControl
    //
    void VideoSwitchAgent::agentSetControl()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "agentSetControl" );

		// set all datanodes to control mode
		for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
		{
			(*itr)->setOperationMode( true );
		}
		// set the proxies to control mode
		m_scadaProxyFactory->setProxiesToControlMode();	

        //TD16761 Mintao++
		// if bulk interface is set up
		if ( NULL != m_dpBulkPolledInterface )
		{
			// start receiving remote requests
			m_dpBulkPolledInterface->setToControl();
		}
		// else do nothing
		if ( NULL != m_dpAccessInterface )
		{
			// start receiving remote requests
			m_dpAccessInterface->setToControl();
		}

		if ( NULL != m_dnBulkPolledInterface )
		{
			// start receiving remote requests
			m_dnBulkPolledInterface->setToControl();
		}
		// else do nothing
		if ( NULL != m_dnAccessInterface )
		{
			// start receiving remote requests
			m_dnAccessInterface->setToControl();
		}

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

        LOG_FUNCTION_EXIT( SourceInfo, "agentSetControl" );
    }

       
    //
    // notifyGroupOffline
    //
    void VideoSwitchAgent::notifyGroupOffline( const std::string& group )
    {
        // FUTURE TODO.
    }


    //
    // notifyGroupOnline
    //
    void VideoSwitchAgent::notifyGroupOnline( const std::string& group )
    {
        // FUTURE TODO.
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
        EntityMap::const_iterator it = entities->begin();
        for( ; it != entities->end(); it++ )
        {
            it->second->setToControlMode();
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
        EntityMap::const_iterator it = entities->begin();
        for( ; it != entities->end(); it++ )
        {
            it->second->setToMonitorMode();
        }
    }

    //
    // registerForControlMessages
    //
    void VideoSwitchAgent::registerForControlMessages()
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( &m_stateUpdateSubscriber );

        // Subscribe for VideoSwitchAgentStateUpdateRequest messages.

        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage( 
            TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdateRequest, 
            &m_stateUpdateSubscriber, TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ).c_str() );
    }


    //
    // registerForMonitorMessages
    //
    void VideoSwitchAgent::registerForMonitorMessages()
    {
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( &m_stateUpdateSubscriber );

        // Subscribe for VideoSwitchAgentStateUpdate messages.

        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage( 
            TA_Base_Core::VisualStateUpdate::VideoSwitchAgentStateUpdate, 
            &m_stateUpdateSubscriber, TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ).c_str() );
    }


    //
    // cleanUp
    //
    void VideoSwitchAgent::cleanUp()
    {
        FUNCTION_ENTRY( "cleanUp" );

		m_dataNodes.clear();
	    
        EntityCreationFactory::getInstance().shutdown();
    
        // Stop the subscriber from receiving messages.
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( &m_stateUpdateSubscriber );
 
        // Deactivate and delete the SwitchManager.
        SwitchManager::getInstance().deactivateAndDeleteServant();

		// if bulk interface is created
		if ( NULL != m_dpBulkPolledInterface )
		{
			delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = NULL;
		}

		// if bulk interface is created
		if ( NULL != m_dpAccessInterface )
		{
			delete m_dpAccessInterface;
			m_dpAccessInterface = NULL;
		}

		// if bulk interface is created
		if ( NULL != m_dnBulkPolledInterface )
		{
			delete m_dnBulkPolledInterface;
			m_dnBulkPolledInterface = NULL;
		}
		
		// if bulk interface is created
		if ( NULL != m_dnAccessInterface )
		{
			delete m_dnAccessInterface;
			m_dnAccessInterface = NULL;
		}

        // Deactivate and delete the Generic Agent.
        m_genericAgent->deactivateAndDeleteServant();

		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();
		
		// clean up handle to SummaryProcessor
		if ( NULL != m_summaryProcessor )
		{
			TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
			m_summaryProcessor = NULL;
		}
		
		// remove handle to ScadaProxyFactory
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		m_scadaProxyFactory = NULL;

        FUNCTION_EXIT;
    }


    //
    // registerForStateUpdates
    //
    void VideoSwitchAgent::registerForStateUpdates()
    {
        // Don't use m_genericAgent->getOperationMode() here, because m_genericAgent
        // calls this method as part of its construction.

        if( AgentModeMonitor::getInstance().isInControlMode() )
        {
            //limin++--, failover
            VideoSwitchAgentThreadedMemberFunction& thrd = VideoSwitchAgentThreadedMemberFunction::alloc( *this );
            thrd.add( &VideoSwitchAgent::registerForControlMessages );
            thrd.start();
            //registerForControlMessages();
            //limin++--, failover
        }
        else
        {
            //limin++--, failover
            VideoSwitchAgentThreadedMemberFunction& thrd = VideoSwitchAgentThreadedMemberFunction::alloc( *this );
            thrd.add( &VideoSwitchAgent::registerForMonitorMessages );
            thrd.start();
            //registerForMonitorMessages();
            //limin++--, failover
        }
    }


    //
    // receiveSpecialisedMessage
    //
    void VideoSwitchAgent::receiveSpecialisedMessage(
        const TA_Base_Core::StateUpdateMessageCorbaDef& message )
    {
        // This is handled by the StateUpdateSubscriber object.
    }


    //
    // processOnlineUpdate
    //
    void VideoSwitchAgent::processOnlineUpdate(
        const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
//		//TODO: do this through generic interface
//		TA_Base_Bus::DataPoint* dataPoint = COESwitchManager::getInstance().getDataPointFromKey(updateEvent.getKey());
//		if(dataPoint!=0)
//		{
//			dataPoint->update(updateEvent);
//		}
    }

    //TD16761 Mintao++
	
	TA_Base_Bus::DataPoint * VideoSwitchAgent::getDataPoint ( unsigned long entityKey )
	{
		TA_Base_Bus::DataPoint* theDataPoint = NULL;

		try
		{
			// Find datapoint
			const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap = m_genericAgent->getEntityMap();
			std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator it = entityMap->find(entityKey);

			if (it != entityMap->end())
			{		
				theDataPoint = dynamic_cast<TA_Base_Bus::DataPoint*>((*it).second);
			}
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "getDataPoint() - Exception thrown while retrieving datapoint");
		}

		return theDataPoint;
	}

	void VideoSwitchAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
		// stop multi threads accessing the list of DataPoint objects at the same time

		dataPointList.clear();

		std::map < unsigned short, TA_Base_Bus::DataPoint *> localdatapointmap;
		localdatapointmap = COESwitchManager::getInstance().getAllDataPoints();
		std::map < unsigned short, TA_Base_Bus::DataPoint *>::iterator itr;
		for( itr = localdatapointmap.begin(); itr != localdatapointmap.end(); ++itr)
		{
			dataPointList[((unsigned long)itr->first)] = itr->second;		
		}
	}
    //TD16761 Mintao++

    TA_Base_Bus::DataPointFactory* VideoSwitchAgent::getDataPointFactory()
    {
        TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
        
        return m_dataPointFactory;
    }
    
	TA_Base_Bus::DataNode * VideoSwitchAgent::getDataNode( unsigned long entityKey )
	{
		// stop multi threads accessing the list of DataNode objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
		TA_Base_Bus::DataNode * dn = NULL;
        
        const std::map<unsigned long, TA_Base_Bus::IEntity*>* dataNodes = m_genericAgent->getEntityMap();
		std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator itr = dataNodes->find( entityKey );
		
		if ( itr != dataNodes->end() )
		{
			dn = dynamic_cast<TA_Base_Bus::DataNode *>(itr->second);
			
            if ( NULL == dn )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not convert entity %ul to a DataNode", entityKey );
			}
        }
		
		return dn;
	}
	
	void VideoSwitchAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
    {
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        
    }
} // TA_IRS_App
