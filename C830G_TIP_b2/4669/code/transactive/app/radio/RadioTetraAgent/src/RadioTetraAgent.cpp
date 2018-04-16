/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/src/RadioTetraAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2009/03/19 17:00:25 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef __WIN32__
#pragma warning(disable:4503)
#endif


#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioDirectoryEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioStateSynchronisationEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioGlobalEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/exceptions/src/FatalConfigurationException.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/KeepSessionAliveThreadManager.h"

#include "app/radio/RadioTetraAgent/src/Radio.h"
#include "app/radio/RadioTetraAgent/src/RadioServant.h"
#include "app/radio/RadioTetraAgent/src/RadioDirectoryServant.h"
#include "app/radio/RadioTetraAgent/src/RadioStateSynchronisationServant.h"
#include "app/radio/RadioTetraAgent/src/RadioTetraAgent.h"
#include "app/radio/RadioTetraAgent/src/RadioConfigurationException.h"

using TA_Base_Bus::IEntity;
using TA_Base_Core::IEntityData;
using TA_Base_Core::ConfigUpdateDetails;
using TA_Base_Core::StateUpdateMessageCorbaDef;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

#include "RadioSession.h"
#include "CallGenerator.h"


//limin++, failover
namespace
{
    template< typename T > class ThreadedMemberFunction : public TA_Base_Core::Thread
    {
        typedef void ( T::* MemberFunction )();
        typedef std::vector< MemberFunction > MemberFunctionList;
        
    public:
        
        static ThreadedMemberFunction& alloc( T& o, MemberFunction mf = NULL )
        {
			TA_Base_Core::ThreadGuard( lock() );

            static ThreadedMemberFunction* instance = NULL;
            
            instance = new ThreadedMemberFunction( o, instance );
            
            if ( NULL != mf )
            {
                instance->add( mf );
                
                instance->start();
            }
            
            return *instance;
        }
        
        void add( MemberFunction mf )
        {
            m_members.push_back( mf );
        }
        
    protected:
        
        virtual void run()
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Entered: ThreadedMemberFunction, there are [%d] member function(s) to call", m_members.size() );
            
            for ( typename MemberFunctionList::iterator mfIter = m_members.begin(); mfIter != m_members.end(); ++mfIter )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ThreadedMemberFunction: calling [%d] member function", std::distance( m_members.begin(), mfIter ) );
                    
                    ( m_object.*( *mfIter ) )();
                }
                catch (...)
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "" );
                }
            }
            
            clearPrivious();
            
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function Exited: ThreadedMemberFunction" );
        }
        
        virtual void terminate(){}
        
    private:
        
        ThreadedMemberFunction( T& o, ThreadedMemberFunction* privious )
            : m_object( o ),
              m_privious( privious )
        {
        }

        ThreadedMemberFunction( const ThreadedMemberFunction& );
        ThreadedMemberFunction& operator= ( const ThreadedMemberFunction& );

        ~ThreadedMemberFunction()
        {
            terminateAndWait();
        }
        
        void clearPrivious()
        {
            delete m_privious;
            m_privious = NULL;
        }
        
        static TA_Base_Core::NonReEntrantThreadLockable& lock()
        {
            static TA_Base_Core::NonReEntrantThreadLockable lock;
            return lock;
        }
        
    private:
        
        T& m_object;
        MemberFunctionList m_members;
        ThreadedMemberFunction* m_privious;
    };
    
    typedef ThreadedMemberFunction< TA_IRS_App::RadioTetraAgent > RadioTetraAgentThreadedMemberFunction;
}
//limin++, failover


namespace TA_IRS_App
{
    //
    // Constructor
    //
    RadioTetraAgent::RadioTetraAgent(int argc, char* argv[]) 
		            :	m_genericAgent(NULL), //,m_alarmAgentImpl(NULL)
						m_radioServant(NULL),
						m_callGenerator(NULL),
						m_agentMode(INITIAL),
						m_fullState(false),
						m_dataPointFactory(NULL),
						m_scadaProxyFactory(NULL),
						m_summaryProcessor(NULL),
						m_dpBulkPolledInterface(NULL),
						m_dpAccessInterface(NULL),
						m_dnBulkPolledInterface(NULL),
						m_dnAccessInterface(NULL)
    {
		FUNCTION_ENTRY("Constructor");

		if ( NULL == m_dataPointFactory )
		{
			m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);
		}

		m_dataPointList.clear();
		m_dataNodes.clear();

		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

        m_radioServant = new RadioServant();  // Do before generic as needed by child entities
        m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);

        TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();
        TA_IRS_Bus::TrainAgentProxyFactory::getInstance().agentInitialise( m_genericAgent->getAgentEntityConfiguration() );
		unsigned long key = entityData->getKey();
        unsigned long typeKey = entityData->getTypeKey();
        m_entityname = entityData->getName();

        char buff1[50] = {0};            
        char buff2[50] = {0};            
        sprintf(buff1,"%lu",key);
        sprintf(buff2,"%lu",typeKey);
        RunParams::getInstance().set("entityKey", buff1);
        RunParams::getInstance().set("entityTypeKey", buff2);

        TA_Base_Core::RadioGlobalEntityData*  globalEntity;
        globalEntity = TA_Base_Core::RadioGlobalEntityData::getRadioGlobalsEntity();

        // Create the radio object associated with this agent
        // (Radio sessions will be created as child objects)
        m_radioServant->setEntity(*entityData, *globalEntity);
        //
        // finally we can create our RadioSessionServants
        for ( std::vector<TA_Base_Core::RadioSessionEntityDataPtr>::iterator iter = m_radioSessionData.begin();
              iter != m_radioSessionData.end(); ++iter )
        {
			try
			{
				RadioSessionServant * radioSessionServant = new RadioSessionServant(
					m_radioServant->getRadio(),
					*(m_radioServant->getRadio().getAudioLinkServer((*iter)->getAudioLinkServerId())),
					*iter,
					*globalEntity
					/*,m_scada */);

				m_radioSessions.push_back(radioSessionServant);

				// normally done during createEntity()
				m_genericAgent->addEntity((*iter)->getKey(), radioSessionServant);
			}
			catch (RadioConfigurationException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "RadioConfigurationException: %s, %s", e.what(), m_entityname.c_str());
			}
        }
		setInitialOperationMode();

		m_scadaProxyFactory->registerLocalEntities(m_entityname , m_genericAgent->getEntityMap());
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

		m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());

		// Create the radio object associated with this agent 
		// (Radio sessions will be created as child objects)
		m_radioServant->start();

		FUNCTION_EXIT;
    }

    //
    // Destructor
    //
    RadioTetraAgent::~RadioTetraAgent()
    {
		FUNCTION_ENTRY("Destructor");
		m_dataPointList.clear();
		m_dataNodes.clear();

		if ( NULL != m_dataPointFactory)
		{
			delete m_dataPointFactory;
			m_dataPointFactory = NULL;
		}

		if ( NULL != m_dpBulkPolledInterface)
		{
			delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = NULL;
		}

		if ( NULL != m_dpAccessInterface)
		{
			delete m_dpAccessInterface;
			m_dpAccessInterface = NULL;
		}

		if ( NULL != m_dnBulkPolledInterface)
		{
			delete m_dnBulkPolledInterface;
			m_dnBulkPolledInterface = NULL;
		}

		if ( NULL != m_dnAccessInterface)
		{
			delete m_dnAccessInterface;
			m_dnAccessInterface = NULL;
		}

		if (m_radioServant!=NULL) delete m_radioServant;
		if (m_callGenerator!=NULL) delete m_callGenerator;

        if( m_genericAgent != NULL )
        {
            m_genericAgent->deactivateAndDeleteServant();
            m_genericAgent = 0;
        }

		if ( NULL != m_summaryProcessor )
		{
			TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
			m_summaryProcessor = NULL;
		}

		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		m_scadaProxyFactory = NULL;

		FUNCTION_EXIT;
    }


    //
    // agentTerminate
    //
    void RadioTetraAgent::agentTerminate()
    {
 		FUNCTION_ENTRY("agentTerminate");

        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);


		FUNCTION_EXIT;
    }

    bool RadioTetraAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
        const TA_Base_Core::SharedIEntityDataList& entityDataList,
        TA_Base_Bus::IEntityList& createdEntities)
    {
		bool createdDnSpecificUtils = false;
		
        for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin();
        iter != entityDataList.end(); ++iter )
        {
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
    
    
    void RadioTetraAgent::entityCreationComplete()
    {
        TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
    }
    
    
    
    //
    // createEntity
    //
    IEntity* RadioTetraAgent::createEntity(TA_Base_Core::IEntityDataPtr entityData)
    {
 		FUNCTION_ENTRY("createEntity");

        std::string entityType = entityData->getType();

        if (TA_Base_Core::RadioSessionEntityData::getStaticType() == entityType)
        {
            TA_Base_Core::RadioSessionEntityDataPtr radioSessionData =
                boost::dynamic_pointer_cast<TA_Base_Core::RadioSessionEntityData>(entityData);
            TA_ASSERT(radioSessionData, "Unable to cast to correct type");

            m_radioSessionData.push_back(radioSessionData);

            // haven't created an entity to return, yet.
            return 0;
        }
        else if (TA_Base_Core::RadioDirectoryEntityData::getStaticType() == entityType)
        {
            TA_Base_Core::RadioDirectoryEntityDataPtr radioDirectoryData =
                boost::dynamic_pointer_cast<TA_Base_Core::RadioDirectoryEntityData>(entityData);
            TA_ASSERT(radioDirectoryData, "Unable to cast to correct type");
            
            RadioDirectoryServant * newRDServant = new RadioDirectoryServant(m_radioServant->getRadio(), radioDirectoryData);

            m_radioServant->setDirectoryCallback(newRDServant);

            return newRDServant;
        }
        else if (TA_Base_Core::RadioStateSynchronisationEntityData::getStaticType() == entityType)
        {
            TA_Base_Core::RadioStateSynchronisationEntityDataPtr radioStateSynchronisationData =
                boost::dynamic_pointer_cast<TA_Base_Core::RadioStateSynchronisationEntityData>(entityData);
            TA_ASSERT(radioStateSynchronisationData, "Unable to cast to correct type");

            m_newRSSServant = new RadioStateSynchronisationServant(*this, radioStateSynchronisationData);

            return m_newRSSServant;
        }
        else if ( TA_Base_Core::DataPointEntityData::getStaticType() == entityType )
        {
			TA_Base_Core::DataPointEntityDataPtr dataPointEntityData = 
                boost::dynamic_pointer_cast< TA_Base_Core::DataPointEntityData > ( entityData );
			
			TA_Base_Bus::DataPoint* dataPoint = m_dataPointFactory->createDataPoint ( dataPointEntityData );
			
			if (dataPoint == NULL)
			{
				std::ostringstream msg;
				msg << "Child entity datapoint " << dataPointEntityData->getName() 
					<< " has not configured correctly and cannot be created";  
				
				TA_THROW(TA_Base_Core::GenericAgentException(
					TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, 
					msg.str().c_str()));
			}
			else
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug, "%s",
					"Child entity datapoint %s has been created",
					dataPointEntityData->getName().c_str());
				m_dataPointList[ dataPointEntityData->getKey() ] = dataPoint;
			}
			
			if (!dataPointEntityData->getAllowWrite())
			{
				// Initialise the datapoint to not connected status
				TA_Base_Bus::DpValue dpValue = dataPoint->getCurrentValue();
				
				TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED );
				dataPoint->updateFieldState( newState );
			}

			m_radioServant->setDataPointLink(dataPoint);
			
			return dataPoint;
        }
		else if(TA_Base_Core::DataNodeEntityData::getStaticType() == entityType)
		{
			TA_Base_Bus::DataNode* dataNode = NULL;
			try
			{
                dataNode = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(entityData), m_persistence);
                
                m_dataNodes.push_back( dataNode );
				return dataNode;
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    entityData->getName().c_str(), entityData->getKey() );
				
				// To ensure the generic agent is constructed even if one DataNode creation fails
			}
		}

        FUNCTION_EXIT;
        return NULL;
    }


    //
    // agentSetMonitor
    //
    void RadioTetraAgent::agentSetMonitor()
    {
 		FUNCTION_ENTRY("agentSetMonitor");

        TA_Base_Core::ThreadGuard guard(m_lockable);

        AgentMode prevMode = m_agentMode;

        m_agentMode = MONITOR;

        if (prevMode == INITIAL)
        {
            m_fullState = false;
        }
        if (prevMode == CONTROL)
        {
            m_fullState = false;
           // Disconnect from Radio Hardware
        }

		if ( NULL != m_dpBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dpBulkPolledInterface->setToMonitor();
		}
		// if dp Access Interface interface is set up
		if ( NULL != m_dpBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dpAccessInterface->setToMonitor();
		}
		// if bulk interface is set up
		if ( NULL != m_dnBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dnBulkPolledInterface->setToMonitor();
		}
		// if dn Access Interface interface is set up
		if ( NULL != m_dnBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dnAccessInterface->setToMonitor();
		}
		
		for ( DatapointList::iterator it = m_dataPointList.begin(); it != m_dataPointList.end(); it++ )
		{
			it->second->setToMonitorMode();
		}
		
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->setOperationMode( false );
		}
		
		m_scadaProxyFactory->setProxiesToMonitorMode();
        m_dataPointFactory->setToMonitor();

		MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		registerForStateUpdates();

        m_radioServant->setToMonitorMode();

		FUNCTION_EXIT;
    }

	void RadioTetraAgent::notifyGroupOffline( const std::string& group )
	{
	}

	void RadioTetraAgent::notifyGroupOnline( const std::string& group )
	{
	}


    //
    // agentSetControl
    //
    void RadioTetraAgent::agentSetControl()
    {
 		FUNCTION_ENTRY("agentSetControl");

        TA_Base_Core::ThreadGuard guard(m_lockable);

        AgentMode prevMode = m_agentMode;

        m_agentMode = CONTROL;

        if (prevMode == INITIAL)
        {
            m_fullState = true;
            // Connect to Radio Hardware
        }
        
        
        //limin++--, failover
        //m_radioServant->setToControlMode();

        RadioTetraAgentThreadedMemberFunction::alloc( *this, &RadioTetraAgent::agentSetControlInThread );
        //agentSetControlInThread();
        //limin++--, failover

		FUNCTION_EXIT;
    }


    //limin, failover
    void RadioTetraAgent::agentSetControlInThread()
    {
        FUNCTION_ENTRY( "agentSetControlInThread");

		m_radioServant->setToControlMode();        
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        registerForStateUpdates();

		for ( DatapointList::iterator it = m_dataPointList.begin(); it != m_dataPointList.end(); it++ )
		{
			it->second->setToControlMode();
		}
		
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->setToControlMode();
		}
		
		m_scadaProxyFactory->setProxiesToControlMode();
        m_dataPointFactory->setToControl();

		if ( NULL != m_dpBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dpBulkPolledInterface->setToControl();
		}
		// if dp Access Interface interface is set up
		if ( NULL != m_dpBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dpAccessInterface->setToControl();
		}
		// if bulk interface is set up
		if ( NULL != m_dnBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dnBulkPolledInterface->setToControl();
		}
		// if dn Access Interface interface is set up
		if ( NULL != m_dnBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dnAccessInterface->setToControl();
		}

		getSessionsForRadioLogin();

		FUNCTION_EXIT;
    }


    //
    // registerForControlMessages
    //
    void RadioTetraAgent::registerForControlMessages()
    {
 		FUNCTION_ENTRY("registerForControlMessages");
//          MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
//              TA_Core::TA_Message::AlarmStateUpdate::AlarmAgentStateRequest,
//              this,
//              RunParams::getInstance().get(RPARAM_ENTITYNAME));
		FUNCTION_EXIT;
    }


    //
    // registerForMonitorMessages
    //
    void RadioTetraAgent::registerForMonitorMessages()
    {
 		FUNCTION_ENTRY("registerForMonitorMessages");
		/*
        MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Core::TA_Message::AlarmStateUpdate::AlarmStateUpdate,
            this,
            RunParams::getInstance().get(RPARAM_ENTITYNAME));

        MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
            TA_Core::TA_Message::AlarmStateUpdate::QueueItemRemoved,
            this,
            RunParams::getInstance().get(RPARAM_ENTITYNAME));
			*/
 		FUNCTION_EXIT;
   }


    //
    // registerForStateUpdates
    //
    void RadioTetraAgent::registerForStateUpdates()
    {
 		FUNCTION_ENTRY("registerForStateUpdates");

        if( 0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_CONTROL) )
        {
            registerForControlMessages();
        }
        else
        {
            registerForMonitorMessages();
        }
 		FUNCTION_EXIT;
   }


    //
    // receiveSpecialisedMessage
    //
    void RadioTetraAgent::receiveSpecialisedMessage(const StateUpdateMessageCorbaDef& message)
    {
 		FUNCTION_ENTRY("receiveSpecialisedMessage");
        //CORBA::Any* temp;

        /*
		if( 0 == std::string(message.messageTypeKey).compare(TA_Core::TA_Message::AlarmStateUpdate::AlarmAgentStateRequest.getTypeKey()) )
        {
            receiveRequestForStateUpdateMsg();
            return;
        }

        if( 0 == TA_Core::TA_Message::AlarmStateUpdate::AlarmStateUpdate.getTypeKey().compare(message.messageTypeKey) )
        {
            if( (message.stateUpdateInfo >>= temp) != 0 )
            {
                AlarmMessageCorbaDef* alarm;
                AlarmUpdateCorbaDef* update;
                AlarmRuleCorbaDef* rule;

                if( ((*temp) >>= alarm) != 0 )
                {
                    receiveNewAlarmStateUpdateMsg(*alarm);
                }
                else
                {
                    if( ((*temp) >>= update) != 0 )
                    {
                        receiveAlarmUpdateStateUpdateMsg(*update);
                    }
                    else
                    {
                        if( ((*temp) >>= rule) != 0 )
                        {
                            receiveRuleUpdateStateUpdateMsg(*rule);
                        }
                    }
                }
                return;
            }
        }
        if( 0 == TA_Core::TA_Message::AlarmStateUpdate::QueueItemRemoved.getTypeKey().compare(message.messageTypeKey) )
        {
            if( (message.stateUpdateInfo >>= temp) != 0 )
            {
                AlarmAgentQueueItemCorbaDef* item;

                if( ((*temp) >>= item) != 0 )
                {
                    receiveQueueUpdateStateUpdateMsg(*item);
                }
            }
            return;
        }
		*/

		FUNCTION_EXIT;
    }


    //
    // processOnlineUpdate
    //
    void RadioTetraAgent::processOnlineUpdate(const ConfigUpdateDetails& updateEvent)
    {
 		FUNCTION_ENTRY("processOnlineUpdate");
		FUNCTION_EXIT;
    }


    //
    // run
    //
    void RadioTetraAgent::run()
    {
 		FUNCTION_ENTRY("run");


		if (NULL != m_genericAgent)
		{
			try
			{
				// get the agent name
				std::string agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
				
				TA_Base_Core::EOperationMode operationMode = m_genericAgent->getOperationMode();
				
				// Set the mode of the agent and the data points created for this agent
				const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap = m_genericAgent->getEntityMap();
				std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator it; 
				if( operationMode == TA_Base_Core::Control )
				{
					agentSetControl();
					for (it = entityMap->begin(); it != entityMap->end(); ++it)
					{
						TA_ASSERT((*it).second != NULL, "IEntity* in entity map is NULL");
						try
						{
							(*it).second->setToControlMode();
						}
						catch (...)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Exception caught when setting entity to monitor mode");
						}
					}
				}
				else
				{
					agentSetMonitor();
					for (it = entityMap->begin(); it != entityMap->end(); ++it)
					{
						TA_ASSERT((*it).second != NULL, "IEntity* in entity map is NULL");
						try
						{
							(*it).second->setToMonitorMode();
						}
						catch (...)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Exception caught when setting entity to monitor mode");
						}
					}
				}

                m_radioServant->startServerLinkStatusThread();//limin

				// if bulk interface is setup
				// create an instance of the class DataPointAgentCorbaDef_Impl
				m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_entityname, this );

				TA_ASSERT(m_dpBulkPolledInterface, "can't create the dp bulk interface.");
				if ( operationMode == TA_Base_Core::Control )
				{
					// start receiving remote requests
					m_dpBulkPolledInterface->setToControl();
				}
				else
				{
					// stop receiving remote requests
					m_dpBulkPolledInterface->setToMonitor();
				}
				
				m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl ( m_entityname, this );
				TA_ASSERT(m_dpAccessInterface, "can't create the datapoint access interface.");
				if ( operationMode == TA_Base_Core::Control )
				{
					// start receiving remote requests
					m_dpAccessInterface->setToControl();
				}
				else
				{
					// stop receiving remote requests
					m_dpAccessInterface->setToMonitor();
				}
				
				m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_entityname, this );
				TA_ASSERT(m_dnBulkPolledInterface, "can't create the dn bulk interface.");
				if ( operationMode == TA_Base_Core::Control )
				{
					// start receiving remote requests
					m_dnBulkPolledInterface->setToControl();
				}
				else
				{
					// stop receiving remote requests
					m_dnBulkPolledInterface->setToMonitor();
				}
				
				m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl ( m_entityname, this );
				TA_ASSERT(m_dnAccessInterface, "can't create the datanode access interface.");
				if ( operationMode == TA_Base_Core::Control )
				{
					// start receiving remote requests
					m_dnAccessInterface->setToControl();
				}
				else
				{
					// stop receiving remote requests
					m_dnAccessInterface->setToMonitor();
				}

				m_genericAgent->run();
			}
			catch (TA_Base_Core::FatalConfigurationException &e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", e.what());
				
				m_genericAgent->terminateWithoutRestart();
				
				TA_THROW(TA_Base_Core::GenericAgentException(
					TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, 
					e.what()));
			}
		}

		FUNCTION_EXIT;
    }


	void RadioTetraAgent::setInitialOperationMode()
	{
 		FUNCTION_ENTRY("setInitialOperationMode");

		// Determine the initial operation mode - done here not constructor as RunParams not ready there
		// The generic agent does not do this and let you know though the setxxxxxxxMode interfaces (how rude of it!)
		if (TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE)==RPARAM_CONTROL)
		{
            agentSetControl();
        }
		else if (TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE)==RPARAM_MONITOR)
		{
			agentSetMonitor();

			const std::map<unsigned long, IEntity*>* entityMap = m_genericAgent->getEntityMap();
			std::map<unsigned long, IEntity*>::const_iterator  mapIT;
			for (mapIT=entityMap->begin();mapIT!=entityMap->end();mapIT++)
			{
				mapIT->second->setToMonitorMode();
			}
		}
		
		FUNCTION_EXIT;
	}

    void RadioTetraAgent::updateCallInRadioCallStack(const CallDetailsType& callDetails)
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"RadioServant->updateCallInRadioCallStack... CallID = %d SessionRef = %d",callDetails.callID,callDetails.sessionRef);
        TA_Base_Core::ThreadGuard guard(m_lockable);

        m_radioServant->updateCallInRadioCallStack(callDetails);
    }

    void RadioTetraAgent::removeCallFromRadioCallStack(CallID callID, unsigned long consoleID)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "RadioServant->removeCallFromRadioCallStack... CallID = %d, consoleID = %d",callID, consoleID);
        TA_Base_Core::ThreadGuard guard(m_lockable);

        m_radioServant->removeCallFromRadioCallStack(callID,consoleID);
    }

	void RadioTetraAgent::updateRadioSession(unsigned long servantEntityKey,
										     const RadioSessionType& radioSession)
	{
		TA_Base_Core::ThreadGuard guard(m_lockable);

		// find the entity
		const std::map<unsigned long, IEntity*>* entityMap = this->m_genericAgent->getEntityMap();

		std::map<unsigned long, IEntity*>::const_iterator it = entityMap->find(servantEntityKey);

		if (it != entityMap->end())
		{
			RadioSessionServant* radioSessionServant = dynamic_cast<RadioSessionServant*> (it->second);

			if (0 != radioSessionServant)
			{
				radioSessionServant->updateRadioSession(radioSession);
			}
		}
	}

	void RadioTetraAgent::updateRadioMonitoredCallProgression(unsigned long monitorReference)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioServant->updateRadioMonitoredCallProgression" );
		TA_Base_Core::ThreadGuard guard(m_lockable);

		m_radioServant->updateRadioMonitoredCallProgression(monitorReference);

	}


	void RadioTetraAgent::removeMonitoredSubscriberDetails(unsigned long monitorReference, long serverIndex)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioServant->updateRadioMonitoredCallProgression" );
		TA_Base_Core::ThreadGuard guard(m_lockable);

        m_radioServant->removeMonitoredSubscriberDetails(monitorReference, serverIndex);
	}

	void RadioTetraAgent::removeSessionFromAudioMap(SessionID sessionReference, long serverIndex)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioServant->removeSessionFromAudioMap" );
		 TA_Base_Core::ThreadGuard guard(m_lockable);

		 m_radioServant->removeSessionFromAudioMap(sessionReference, serverIndex);
	}

	void RadioTetraAgent::updateAudioEntryInSessionMap(SessionID sessionReference, AudioReference audioReference,
									   const AudioEntryType& audioEntry)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioServant->updateAudioEntryInSessionMap" );
		TA_Base_Core::ThreadGuard guard(m_lockable);
		
		m_radioServant->updateAudioEntryInSessionMap(sessionReference, audioReference, audioEntry);

	}

	void RadioTetraAgent::updateRadioResourceManagerVars(SessionID sessionReference, CallReference callReference,
														 int currentVolume, long serverIndex)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioServant->updateRadioResourceManagerVars" );
		TA_Base_Core::ThreadGuard guard(m_lockable);

		 m_radioServant->updateRadioResourceManagerVars(sessionReference, callReference, currentVolume, serverIndex);

	}

	void RadioTetraAgent::updateAuthorisationRequest(const AuthorisationRequestType& authorisationRequest, long serverIndex)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioServant->updateAuthorisationRequest" );
		TA_Base_Core::ThreadGuard guard(m_lockable);

		m_radioServant->updateAuthorisationRequest(authorisationRequest, serverIndex);

	}
	void RadioTetraAgent::removeAuthorisationRequest(CallID callID, long serverIndex)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioServant->removeAuthorisationRequest" );
		TA_Base_Core::ThreadGuard guard(m_lockable);

		m_radioServant->removeAuthorisationRequest(callID, serverIndex);

	}
    void RadioTetraAgent::clearFullState()
    {
		FUNCTION_ENTRY("clearFullState");
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioServant->clearFullState" );
        TA_Base_Core::ThreadGuard guard(m_lockable);

        m_radioServant->clearFullState();

		// clear the state of the radio session servant entities
		const std::map<unsigned long, IEntity*>* entityMap = this->m_genericAgent->getEntityMap();
		std::map<unsigned long, IEntity*>::const_iterator it;

		for (it = entityMap->begin(); it != entityMap->end(); it++)
		{
			RadioSessionServant* radioSessionServant = dynamic_cast<RadioSessionServant*> (it->second);

			if (0 != radioSessionServant)
			{
				radioSessionServant->clearFullState();
			}
		}

        if (m_agentMode == MONITOR && m_fullState)
        {
            // Reconnect to Radio Hardware
        }
        else if (m_agentMode == CONTROL && !m_fullState)
        {
            // Connect to Radio Hardware
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioServant->resetConnection" );
        }
		FUNCTION_EXIT;
    }

    void RadioTetraAgent::applyFullState(const RadioTetraAgentType& state)
    {
		FUNCTION_ENTRY("applyFullState");
        TA_Base_Core::ThreadGuard guard(m_lockable);


		// apply the state on the radio session servant entities
		RadioSessionServantTypeList::const_iterator it;

		for (it = state.radioSessionServants.begin(); it != state.radioSessionServants.end(); it++)
		{
			// find the entity with the entity key
			const std::map<unsigned long, IEntity*>* entityMap = this->m_genericAgent->getEntityMap();
			
			std::map<unsigned long, IEntity*>::const_iterator eit = entityMap->find(it->entityKey);
			
			if (eit != entityMap->end())
			{
				RadioSessionServant* radioSessionServant = dynamic_cast<RadioSessionServant*> (eit->second);
				
				if (0 != radioSessionServant)
				{
					radioSessionServant->applyFullState(*it);
				}
			}
		}
        // apply to radio (and tcp servers) after radio sessions
        // the tcp server requires radio sessions to have up to date data
        m_radioServant->applyFullState(state.radioServant);

        if (m_agentMode == MONITOR && !m_fullState)
        {
            m_fullState = true;

            // Connect to Radio Hardware
        }
		FUNCTION_EXIT;
    }

    void RadioTetraAgent::getFullState(RadioTetraAgentType& state)
    {
		FUNCTION_ENTRY("getFullState");
        TA_Base_Core::ThreadGuard guard(m_lockable);

        m_radioServant->getFullState(state.radioServant);

		// get the state of the radio session servant entities
		const std::map<unsigned long, IEntity*>* entityMap = this->m_genericAgent->getEntityMap();
		std::map<unsigned long, IEntity*>::const_iterator it;

		for (it = entityMap->begin(); it != entityMap->end(); it++)
		{
			RadioSessionServant* radioSessionServant = dynamic_cast<RadioSessionServant*> (it->second);

			if (0 != radioSessionServant)
			{
				RadioSessionServantType radioSessionServantData;

				radioSessionServant->getFullState(radioSessionServantData);
				state.radioSessionServants.push_back(radioSessionServantData);
			}
		}
		FUNCTION_EXIT;
    }

    void RadioTetraAgent::notifyStateUpdatesComplete()
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "notifyStateUpdatesComplete" );
        TA_Base_Core::ThreadGuard guard(m_lockable);

        if (m_agentMode == CONTROL && !m_fullState)
        {
            m_fullState = true;
            // Connect to Radio Hardware
        }
    }

	void RadioTetraAgent::updateConnectionSwitch(EControlConnectionStatus controlConnectionStatus, unsigned long TcpServerIndex)
	{
		m_radioServant->getRadio().updateControlConnectionState(controlConnectionStatus, TcpServerIndex);
	}

	
	TA_Base_Bus::DataPoint * RadioTetraAgent::getDataPoint ( unsigned long entityKey )
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
	
	void RadioTetraAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );
		
		dataPointList.clear();
		
		dataPointList = m_dataPointList;
	}
	
	TA_Base_Bus::DataNode * RadioTetraAgent::getDataNode( unsigned long entityKey )
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
	
	void RadioTetraAgent::getAllDataNodes (std::map<unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
    {
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        
    }

	TA_Base_Bus::DataPointFactory* RadioTetraAgent::getDataPointFactory()
    {
        TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
        
        return m_dataPointFactory;
    }

	void RadioTetraAgent::getSessionsForRadioLogin()
	{
		FUNCTION_ENTRY("getSessionsForRadioLogin");

		std::map< unsigned long, std::string > sessionsToLogin;
		std::vector<TA_IRS_App::RadioSessionServant*>::iterator itr = m_radioSessions.begin();

		for ( ; itr != m_radioSessions.end(); itr ++ )
		{
			sessionsToLogin.insert( std::pair<unsigned long, std::string>((*itr)->getConsoleID(), "" ) );
		}

		std::vector<TA_Base_Bus::SessionInfo> sessinInfos;
		
		try
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Try to get active sessions");

			const char * loc = (RunParams::getInstance().get(RPARAM_LOCATIONKEY)).c_str();
			TA_Base_Bus::AuthenticationAgentNamedObj & authAgent = 
				TA_Base_Bus::KeepSessionAliveThreadManager::getInstance().getAuthCorbaNameObject( atol(loc) );
			TA_Base_Bus::AuthenticationLibrary athl;
			athl.getActiveSessionsForOneLocation( std::string("Not Used"), authAgent, sessinInfos);

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Got active sessions");
		}
		catch( ... )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Could not get active sessions");
			return;
		}

		std::vector<TA_Base_Bus::SessionInfo>::iterator it = sessinInfos.begin();
		
		for ( ; it != sessinInfos.end(); it ++)
		{

			std::map< unsigned long, std::string >::iterator result = 
				sessionsToLogin.find( it->WorkstationId );
			
			if ( result != sessionsToLogin.end() )
			{

				if ( result->second == "" )
				{
					sessionsToLogin[ result->first ] = it->SessionId;
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Set sessions for console %d", result->first);
				} 
				else
				{
					sessionsToLogin.erase( result->first );
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Erase sessions for console %d", result->first);
				}
			}
		}

		std::map< unsigned long, std::string >::iterator itrLast = sessionsToLogin.begin();

		std::vector<TA_IRS_App::RadioSessionServant*>::iterator itr2 = m_radioSessions.begin();

		for ( ; itrLast != sessionsToLogin.end(); itrLast ++ )
		{
			for ( ; itr2 != m_radioSessions.end(); itr2 ++ )
			{
				if ( itrLast->first == (*itr2)->getConsoleID() )
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Try to login sessions for console %d", itrLast->first);
					(*itr2)->sessionLogin( "Not Used", itrLast->second.c_str() );
				}
			}
		}

		FUNCTION_EXIT;
	}

};
