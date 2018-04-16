//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/3001/transactive/app/wild/wild_agent/src/WildAgent.cpp $
// @author:  Andy Siow
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
//
// Wild Agent implementation file. 
//
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <time.h>

#include "core/utilities/src/RunParams.h"
#include "core/message/types/PMSAgentStateUpdate_MessageTypes.h"
#include "core/message/types/PMSAgentStateUpdateRequest_MessageTypes.h"
#include "core/message/types/WILDAgentStateUpdateRequest_MessageTypes.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
//#include "core/message/src/MessageSubscriptionManager.h"//limin, failover issue
#include "core/message/src/MessagePublicationManager.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/entity_access/src/WILDAgentEntityData.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/StateUpdateMessageSender.h"
#include "core/message/types/WILDAgentStateUpdate_MessageTypes.h"
#include "core/exceptions/src/WILDAgentExceptions.h"
#include "core/exceptions/src/GenericAgentException.h"


#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"

#include "app/wild/wild_agent/src/WildAgent.h"
#include "app/wild/wild_agent/src/ATSTrainTableSubscriber.h"
#include "app/wild/wild_agent/src/DataPointCache.h"
#include "app/wild/wild_agent/src/WILDConfiguration.h"
#include "app/wild/wild_agent/src/SingletonTrainDataMap.h"

const std::string OPERATIONMODE_PARAM = "OperationMode";


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
    
    typedef ThreadedMemberFunction< TA_IRS_App::WILDAgent > WILDAgentThreadedMemberFunction;
}
//limin++, failover




namespace TA_IRS_App
{
	// 
    // Constructor
    //
    WILDAgent::WILDAgent(int argc, char* argv[])
    : m_genericAgent(NULL),
	  m_dataPointFactory(NULL),
	  m_dpBulkPolledInterface (NULL),
	  m_dpAccessInterface(NULL),
	  //m_stateUpdateRequestSender(NULL),
	  m_atsTrainSubscriber(NULL),
	  m_currentMode(TA_Base_Core::NotApplicable),
	  m_dnBulkPolledInterface(NULL),
	  m_dnAccessInterface(NULL),
	  m_scadaProxyFactory(NULL),
	  m_summaryProcessor(NULL)
    {
		FUNCTION_ENTRY("WILDAgent()");
		m_wildDataPointList.clear();
		m_dataNodes.clear();

		// get a handle to the ScadaProxyFactory
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
		try
		{
			// Setup for DataPointCorbaDef_Impl library.
			if ( NULL == m_dataPointFactory )
			{
				m_dataPointFactory = new TA_Base_Bus::DataPointFactory(this);
			}

			// Setup the generic agent
			m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);

			if (NULL == m_genericAgent)
			{
				cleanUp();

				TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::AGENT_ERROR, "Unknown error with WILD Agent"));
			}

            //limin--, failover issue
			//if (!m_stateUpdateRequestSender)
			//{
			//	m_stateUpdateRequestSender = 
			//		TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender( TA_Base_Core::WILDAgentStateUpdateRequest::Context );
			//}
            //--limin
			
			initialiseLocalVariables();
			DataPointCache::getInstance()->initialiseDataPointProxies();
		}
		catch(const TA_Base_Core::InvalidWILDConfigurationException& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "InvalidWILDConfigurationException", 
				"WILD Configuration error. Rethrow as Generic Agent exception");

			m_genericAgent->terminateWithoutRestart();

			cleanUp();

			FUNCTION_EXIT;

			TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, ex.what()));
		}
		catch(...)  // catch everything to prevent memory leaks from excaping.
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception", 
				"WILD Agent error. Rethrow as Generic Agent exception");
			
			cleanUp();

			FUNCTION_EXIT;
		
			TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::AGENT_ERROR, "Unknown error with WILD Agent"));
		}

		// get the agent name
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
		// register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_agentName , m_genericAgent->getEntityMap());
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

		// create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());

		FUNCTION_EXIT;
    }


    //
    // Destructor
    //
    WILDAgent::~WILDAgent()
    {
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "WILDAgent Destructor");

		cleanUp();

		LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "WILDAgent Destructor");
    }


	void WILDAgent::cleanUp()
	{
		LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "WILDAgent cleanUp");

		m_wildDataPointList.clear();
		// if bulk interface is created
		if ( NULL != m_dpBulkPolledInterface )
		{
			delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = NULL;
		}
		// else do nothing
		if ( NULL != m_dpAccessInterface )
		{
			delete m_dpAccessInterface;
			m_dpAccessInterface = NULL;
		}
		
		if ( NULL != m_dnBulkPolledInterface )
		{
            delete m_dnBulkPolledInterface;
			m_dnBulkPolledInterface = NULL;
		}
		
        if (NULL != m_dnAccessInterface)
        {
            delete m_dnAccessInterface;
            m_dnAccessInterface = NULL;
		}

        //limin--, fail over issue
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
		//	dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this));

		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
		//	dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this));
        //--limin

		if(m_atsTrainSubscriber != NULL)
		{
			delete m_atsTrainSubscriber;
			m_atsTrainSubscriber = 0;
		}

        //limin--, failover issue
		//if(m_stateUpdateRequestSender != NULL)
		//{
		//	delete m_stateUpdateRequestSender;
		//	m_stateUpdateRequestSender = 0;
		//}
        //--limin

		if(m_dataPointFactory != NULL)
		{
			delete m_dataPointFactory;
			m_dataPointFactory = 0;
		}

		DataPointCache::getInstance()->removeInstance();
		SingletonTrainDataMap::getInstance()->removeInstance();
		WILDConfiguration::getInstance()->removeInstance();

		if(m_genericAgent != NULL)
		{
			m_genericAgent->deactivateAndDeleteServant();
			m_genericAgent = 0;
		}
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

		LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "WILDAgent cleanUp");
	}

	void WILDAgent::initialiseLocalVariables()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "initialiseLocalVariables");

		TA_ASSERT(m_genericAgent, "m_genericAgent is NULL");
		
		TA_Base_Core::WILDAgentEntityDataPtr agentEntityData;
		try
		{
			// Important! Don't delete agentEntityData as generic agent will do it for you..
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
			agentEntityData = 
				boost::dynamic_pointer_cast<TA_Base_Core::WILDAgentEntityData>
				(m_genericAgent->getAgentEntityConfiguration());
			LOGAFTERCOTS("boost::dynamic_pointer_cast");
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugNorm, "Retrieving WILD Agent Entity Configuration");
			
			if(agentEntityData.get() == NULL)
			{
				TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::ENTITY_CONFIG_FAILURE, "Failed to retrieve WILD Agent entity data"));
			}

			WILDConfiguration::getInstance()->setAgentName(agentEntityData->getAgentName());
			WILDConfiguration::getInstance()->setAgentLocationKey(agentEntityData->getLocation());
			WILDConfiguration::getInstance()->setAgentKey(agentEntityData->getAgent());
			WILDConfiguration::getInstance()->setAgentSubsystemKey(agentEntityData->getSubsystem());
			WILDConfiguration::getInstance()->setAgentTypeKey(agentEntityData->getTypeKey());
			WILDConfiguration::getInstance()->setCorrelationThreshold(agentEntityData->getCorrelationThresholdTime());
			WILDConfiguration::getInstance()->setAgentLocationName(agentEntityData->getLocationName());
			WILDConfiguration::getInstance()->setAgentSubsystemName(agentEntityData->getSubsystemName());
			WILDConfiguration::getInstance()->setAgentATSWILDQueueSize(agentEntityData->getATSWILDQueueSize());
			WILDConfiguration::getInstance()->setWILDDetailsToken(agentEntityData->getWILDDetailsToken()); 
			WILDConfiguration::getInstance()->setAgentAssetName(agentEntityData->getAssetName());
		}
		catch(TA_Base_Core::DataException& dex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", 
				"Errors occured while trying to find an entity matching the one passed in");
	
			TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::ENTITY_CONFIG_FAILURE, dex.what()));
		}
		catch(TA_Base_Core::DatabaseException& dbex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", 
				"Errors occured while trying to retrieve the entity from the database.");

			TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::CANNOT_CONNECT_TO_DB, dbex.what()));
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "initialiseLocalVariables");
	}


	void WILDAgent::run()
    {
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "run");

		TA_ASSERT(m_genericAgent, "m_genericAgent is NULL");
		std::string operationMode = TA_Base_Core::RunParams::getInstance().get(OPERATIONMODE_PARAM.c_str());
		if(operationMode.compare(RPARAM_CONTROL) == 0)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Setting up WILD Agent in Control mode.");
			agentSetControl();
		}
		else if(operationMode.compare(RPARAM_MONITOR) == 0)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Setting up WILD Agent in Monitor mode.");
			agentSetMonitor();
			
			CORBA::Any data;

            //limin--, failover issue
			//m_stateUpdateRequestSender->sendStateUpdateMessage( 
			//	TA_Base_Core::WILDAgentStateUpdateRequest::WILDAgentStateUpdateRequest,
			//	WILDConfiguration::getInstance()->getAgentKey(),  // Not used
			//	WILDConfiguration::getInstance()->getAgentName(), 
			//	data );   //Not used
            //--limin
		}

		// start the Generic Agent loop
		if ( 0 != m_genericAgent )
		{
			// if bulk interface is setup
			// create an instance of the class DataPointAgentCorbaDef_Impl
			m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_agentName, this );
			TA_ASSERT(m_dpBulkPolledInterface != NULL, "can't create m_dpBulkPolledInterface");
			if (operationMode.compare(RPARAM_CONTROL) == 0)
			{
				// start receiving remote requests
				m_dpBulkPolledInterface->setToControl();
			}
			else
			{
				// stop receiving remote requests
				m_dpBulkPolledInterface->setToMonitor();
			}
			// else do nothing
			m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl ( m_agentName, this );
			TA_ASSERT(m_dpAccessInterface != NULL, "can't create m_dpAccessInterface");
			if (operationMode.compare(RPARAM_CONTROL) == 0)
			{
				// start receiving remote requests
				m_dpAccessInterface->setToControl();
			}
			else
			{
				// stop receiving remote requests
				m_dpAccessInterface->setToMonitor();
			}
			m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_agentName, this );
			TA_ASSERT(m_dnBulkPolledInterface != NULL, "can't create m_dnBulkPolledInterface");
			if (operationMode.compare(RPARAM_CONTROL) == 0)
			{
				// start receiving remote requests
				m_dnBulkPolledInterface->setToControl();
			}
			else
			{
				// stop receiving remote requests
				m_dnBulkPolledInterface->setToMonitor();
			}
			// else do nothing
			m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl(m_agentName, this);
			TA_ASSERT(m_dnAccessInterface != NULL, "can't create m_dnAccessInterface");
			if (operationMode.compare(RPARAM_CONTROL) == 0)
			{
				// start receiving remote requests
				m_dnAccessInterface->setToControl();
			}
			else
			{
				// stop receiving remote requests
				m_dnAccessInterface->setToMonitor();
			}

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugNorm, "Starting Generic Agent thread");
			m_genericAgent->run();
		}
		
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "run");
    }


	void WILDAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "receiveSpecialisedMessage: StateUpdateMessageCorbaDef");
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "receiveSpecialisedMessage: StateUpdateMessageCorbaDef");
	}


    //limin--, failover issue
	//void WILDAgent::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
	//{
	//	// Do nothing
	//}
    //--limin


	void WILDAgent::updateWILDAgentConfiguration(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "updateWILDAgentConfiguration");

		// get the list of changed parameter from the config update
		const std::vector< std::string > changes = updateEvent.getChangedParams();
		TA_ASSERT( changes.size() > 0, "Empty changes should have been prevented by GenericAgent");

		// get configuration data for this WILD Agent
		LOGBEFORECOTS("boost::dynamic_pointer_cast");
		TA_Base_Core::WILDAgentEntityDataPtr entityData = 
			boost::dynamic_pointer_cast<TA_Base_Core::WILDAgentEntityData>
			(m_genericAgent->getAgentEntityConfiguration());
		LOGAFTERCOTS("boost::dynamic_pointer_cast");
		TA_ASSERT(entityData.get()!=NULL, "Unexpected change in entityData class since initialisation");
		// We want the latest changes made to the database
		entityData->invalidate();

		for(unsigned int i=0; i<changes.size(); i++)
		{
			if(0 == TA_Base_Core::WILDAgentEntityData::CORRELATION_THRESHOLD.compare(changes[i]))
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugNorm, "Processing WILD Agent correlation threshold parameter");

				WILDConfiguration::getInstance()->setCorrelationThreshold(entityData->getCorrelationThresholdTime());
			}
			else if(0 == TA_Base_Core::WILDAgentEntityData::ATS_WILD_QUEUE_SIZE.compare(changes[i]))
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugNorm, "Processing WILD Agent ATS Queue size parameter");

				WILDConfiguration::getInstance()->setAgentATSWILDQueueSize(entityData->getATSWILDQueueSize());
			}
			else if(0 == TA_Base_Core::WILDAgentEntityData::CORRELATION_THRESHOLD_TIMEOUT.compare(changes[i]))
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugNorm, "Processing WILD Agent Correlation threshold timeout parameter.");
				WILDConfiguration::getInstance()->setCorrelationThresholdTimeoutSecs(entityData->getCorrelationThresholdTimeoutSecs());
			}
			else if(0 == TA_Base_Core::WILDAgentEntityData::WILD_DETAILS_TOKEN.compare(changes[i]))
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugNorm, "Processing WILDDetails token parameter.");
				WILDConfiguration::getInstance()->setWILDDetailsToken(entityData->getWILDDetailsToken());
			}
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "updateWILDAgentConfiguration");
	}


	void WILDAgent::agentTerminate()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "agentTerminate");

		// if bulk interface is created
		if ( NULL != m_dpBulkPolledInterface )
		{
			delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = NULL;
		}
		// else do nothing
		if ( NULL != m_dpAccessInterface )
		{
			delete m_dpAccessInterface;
			m_dpAccessInterface = NULL;
		}

		if ( NULL != m_dnBulkPolledInterface )
		{
            delete m_dnBulkPolledInterface;
			m_dnBulkPolledInterface = NULL;
		}
		
        if (NULL != m_dnAccessInterface)
        {
            delete m_dnAccessInterface;
            m_dnAccessInterface = NULL;
		}
		
		// The only threads that are running comes from messages. We prepare ourselves
		// for terminate by simply stopping message subscriptions first. Don't delete 
		// any member variables here as the Agent destructor will do that. 
		// Ref: Generic Agent usage guidelines.

        //limin--, failover issue
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Unsubscribing to CommsMessageCorbaDef messages");
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
		//	dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this));
		
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Unsubscribing to StateUpdateMessageCorbaDef messages");
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
		//	dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this));
        //--limin

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "agentTerminate");
	}


    bool WILDAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
        const TA_Base_Core::SharedIEntityDataList& entityDataList,
        TA_Base_Bus::IEntityList& createdEntities)
    {
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
    
    
    void WILDAgent::entityCreationComplete()
    {
        // loading complete, free some memory
        TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();
    }
    
    
    void WILDAgent::agentSetMonitor()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "agentSetMonitor");

		if (m_currentMode == TA_Base_Core::Monitor)
		{
			return;
		}

		m_currentMode = TA_Base_Core::Monitor;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Setting all data points to monitor mode..");
		DataPointCache::getInstance()->setDataPointsMode(false);
		// set all datanodes to monitor mode
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->setOperationMode( false );
		}
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

		// if bulk interface is set up
		if ( NULL != m_dpBulkPolledInterface )
		{
			// stop receiving remote requests
			m_dpBulkPolledInterface->setToMonitor();
		}
		// else do nothing
		if ( NULL != m_dpAccessInterface )
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
		
        if (NULL != m_dnAccessInterface)
        {
            m_dnAccessInterface->setToMonitor();
        }

		//registerForStateUpdates();
		registerForMonitorMessages();

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "agentSetMonitor");
	}


	void WILDAgent::agentSetControl()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "agentSetControl");

		if (m_currentMode == TA_Base_Core::Control)
		{
			return;
		}

		m_currentMode = TA_Base_Core::Control;

        //limin--, failover issue
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
		//	dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*>(this));
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
		//		dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this));
        //--limin

        //limin++--, for failover
        WILDAgentThreadedMemberFunction& thrd = WILDAgentThreadedMemberFunction::alloc( *this );
        thrd.add( &WILDAgent::registerForControlMessages );
        thrd.add( &WILDAgent::agentSetControlInThread );
        thrd.start();
		//registerForStateUpdates();
		//registerForControlMessages();

		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Setting all data points to Control mode...");
		//DataPointCache::getInstance()->setDataPointsMode(true);
		//// set all datanodes to control mode
		//for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
		//{
		//	(*itr)->setOperationMode( true );
		//}
        //limin++--, for failover

		// set the proxies to control mode
		//m_scadaProxyFactory->setProxiesToControlMode();
		//// if bulk interface is set up
		//if ( NULL != m_dpBulkPolledInterface )
		//{
		//	// start receiving remote requests
		//	m_dpBulkPolledInterface->setToControl();
		//}
		//// else do nothing
		//if ( NULL != m_dpAccessInterface )
		//{
		//	// start receiving remote requests
		//	m_dpAccessInterface->setToControl();
		//}
		//if ( NULL != m_dnBulkPolledInterface )
		//{
		//	// start receiving remote requests
		//	m_dnBulkPolledInterface->setToControl();
		//}
        //if (NULL != m_dnAccessInterface)
        //{
        //    m_dnAccessInterface->setToControl();
        //}
        //limin++--, for failover

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "agentSetControl");
	}


    void WILDAgent::agentSetControlInThread()
    {
        FUNCTION_ENTRY( "agentSetControlInThread" );
        
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Setting all data points to Control mode...");
        
        DataPointCache::getInstance()->setDataPointsMode( true );
        
        // set all datanodes to control mode
        for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
        {
            (*itr)->setOperationMode( true );
        }
        
        // set the proxies to control mode
        m_scadaProxyFactory->setProxiesToControlMode();
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

        if (NULL != m_dnAccessInterface)
        {
            m_dnAccessInterface->setToControl();
        }
        
        FUNCTION_EXIT;
    }

	
	TA_Base_Bus::IEntity* WILDAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "createEntity");
		//
		// Note: the objects of interface IEntityData and IEntity are not owned
		// by this specific agent, but by the GenericAgent.  As such no object
		// clean up shall be done with regards to these objects.
		//

		// if the specified entity is of the type DataPoint
		if ( TA_Base_Core::DataPointEntityData::getStaticType() == EntityData->getType() )
		{
			// create and return the DataPoint entity
			LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "createEntity");
			return createDataPointEntity ( EntityData );
		}
		else if( TA_Base_Core::DataNodeEntityData::getStaticType() == EntityData->getType() )
		{
			TA_Base_Bus::IEntity * theEntity = 0;
			TA_Base_Bus::DataNode* dn = NULL;
			try
			{
				LOGBEFORECOTS("boost::dynamic_pointer_cast");
				dn = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData), m_persistence);
				LOGAFTERCOTS("boost::dynamic_pointer_cast");
                m_dataNodes.push_back( dn );
				theEntity = dn;
				return theEntity;
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    EntityData->getName().c_str(), EntityData->getKey() );
				
				// To ensure the generic agent is constructed even if one DataNode creation fails
			}
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "createEntity");
		return NULL;
	}


	TA_Base_Bus::IEntity* WILDAgent::createDataPointEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "createDataPointEntity");

		TA_Base_Bus::DataPoint* theEntity = 0;
		LOGBEFORECOTS("boost::dynamic_pointer_cast");
		TA_Base_Core::DataPointEntityDataPtr dataPointEntityData = boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>( EntityData );
		LOGAFTERCOTS("boost::dynamic_pointer_cast");
		std::string dpAddress = dataPointEntityData->getAddress();
		
		// Format of address is x.trainid.x.x
		std::string::size_type pos1 = dpAddress.find_first_of('.');
		std::string::size_type pos2 = dpAddress.substr(pos1+1).find_first_of('.');
		std::istringstream trainId (dpAddress.substr(pos1+1, pos2));
		unsigned short train = 0;
		trainId >> train;

		std::map<unsigned short, int>::iterator trainNumberIt = m_trainNumbers.end();
		if (train != 0)
		{
			trainNumberIt = m_trainNumbers.find(train);
			if (trainNumberIt == m_trainNumbers.end())
			{
				if (m_trainNumbers.size() < 100)
				{
					std::pair<std::map<unsigned short, int>::iterator, bool> result = 
						m_trainNumbers.insert(std::map<unsigned short, int>::value_type(train,0));
					if (result.second)
					{
						trainNumberIt = result.first;
					}
				}
			}
		}
		
		if (trainNumberIt != m_trainNumbers.end())
		{
			theEntity = m_dataPointFactory->createDataPoint( dataPointEntityData );

			if ( 0 != theEntity )
			{
				// insert the newly created data point to internal list for further
				// referencing
				unsigned long wheelID = DataPointCache::getInstance()->convertAddressIntoID(dpAddress);
				DataPointCache::getInstance()->insertIntoVirtualDataPointList(theEntity->getEntityKey(), wheelID, *theEntity);
				m_wildDataPointList[ theEntity->getEntityKey() ] = theEntity;
			}
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
				"Design Capacity exceeded - more than 100 trains defined - entity creation skipped");
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "createDataPointEntity");
		return theEntity;
	}


	void WILDAgent::notifyGroupOffline(const std::string& group)
	{

	}


	void WILDAgent::notifyGroupOnline(const std::string& group)
	{

	}


	void WILDAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "processOnlineUpdate");

		if ( WILDConfiguration::getInstance()->getAgentKey() == updateEvent.getKey() )
		{
			// update WILD Agent configuration
			// Can only be an Update 
			updateWILDAgentConfiguration ( updateEvent );
		}
		else //it can be either datapoint update or datanode update
		{
			// Must be for the child entities (datapoints)
			// Can be an Update / Create / Delete modification.
//			DataPointCache::getInstance()->updateDataPointConfiguration ( updateEvent );
		}

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "processOnlineUpdate");
	}


	void WILDAgent::registerForStateUpdates()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "registerForStateUpdates");

		// Do nothing.. registering for State updates is delegated to 
		// registerForControlMessages, registerForMonitorMessages

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "registerForStateUpdates");
	}


	void WILDAgent::registerForControlMessages()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "registerForControlMessages");
		
		// This must be called only after initialiseLocalVariables is kicked off.
		if(!m_atsTrainSubscriber)
		{
			m_atsTrainSubscriber = new ATSTrainTableSubscriber();
		}

		DataPointCache::getInstance()->subscribeForAlarmUpdates();

		// The control agent should subscribe for state update requests so it can send all its udpate
		// to the monitoring agent. Note that WILDConfiguration cannot be used as m_generic agent
		// calls this before it finishes contructing. Use RunParams instead.

        //limin--, failover issue
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
		//	dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this));

		//TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
        //TA_Base_Core::WILDAgentStateUpdateRequest::WILDAgentStateUpdateRequest,
		//	dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this),
		//	TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));
        //--limin

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "registerForControlMessages");
	}


	void WILDAgent::registerForMonitorMessages()
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "registerForMonitorMessages");

		if(!m_atsTrainSubscriber)
		{
			m_atsTrainSubscriber = new ATSTrainTableSubscriber();
		}

		DataPointCache::getInstance()->unsubscribeForAlarmUpdates();
		//DataPointCache::getInstance()->subscribeForDataPointStateUpdates();

		// Note that WILDConfiguration cannot be used as m_generic agent
		// calls this before it finishes contructing. Use RunParams instead.

        //limin--, failover issue
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
		//	dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this));

		//TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage(
		//	TA_Base_Core::WILDAgentStateUpdate::WILDAgentStateUpdate,
		//	dynamic_cast<TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>*>(this),
		//	TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));
        //--limin

		LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "registerForMonitorMessages");
	}

	TA_Base_Bus::DataPoint * WILDAgent::getDataPoint ( unsigned long entityKey )
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
	
	void WILDAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );

		dataPointList.clear();

		dataPointList = m_wildDataPointList;
	}
    
    TA_Base_Bus::DataPointFactory* WILDAgent::getDataPointFactory()
    {
        TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
        
        return m_dataPointFactory;
    }
    
	TA_Base_Bus::DataNode * WILDAgent::getDataNode( unsigned long entityKey )
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

	void WILDAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
    {
        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        
    }

};

