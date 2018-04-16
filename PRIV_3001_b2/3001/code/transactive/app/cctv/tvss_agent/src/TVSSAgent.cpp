/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/radio/RadioTetraAgent/src/RadioTetraAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2011/01/04 10:43:52 $
  * Last modified by:  $Author:  $
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4503)
#endif


#include "app/cctv/tvss_agent/src/TVSSAgent.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSManager.h"
#include "app/cctv/tvss_agent/src/TVSSAlarmProcessor.h"
#include "app/cctv/tvss_agent/src/TrainBorneHeartBeatProcessor.h"
//#include "app/cctv/video_switch_agent/TVSSProtocolLibrary/src/AgentModeMonitor.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
//#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"//limin

#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"


//#include "core/exceptions/src/TVSSAgentException.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/utilities/src/RunParams.h"


//#include "core\data_access_interface\entity_access\src\atsagententitydata.h"
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;


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
    
	typedef ThreadedMemberFunction< TA_IRS_App::TVSSAgent > TVSSAgentThreadedMemberFunction;
}

namespace TA_IRS_App
{
	/**
      * Constructor
      */
	TVSSAgent::TVSSAgent(int argc, char *argv[])
		: m_genericAgent(NULL), m_dataPointFactory(NULL),
		  m_linkStatusThread(NULL), m_scadaProxyFactory(NULL),
		  m_dpBulkPolledInterface(NULL), m_dpAccessInterface(NULL),
		  m_dnBulkPolledInterface(NULL), m_dnAccessInterface(NULL),
		  m_tvssServant(NULL)
	{
		FUNCTION_ENTRY("Constructor: TVSSAgent(int argc, char *argv[])" );
		m_tvssDataPointList.clear();
		m_dataNodes.clear();
		m_monitorPKeyList.clear();
		m_monitorConsoleMap.clear();
		m_linkStatusThread = new TVSSServerLinkStatusUpdateThread();
		m_dataPointFactory = new TA_Base_Bus::DataPointFactory( this );
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

       
        m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);
		
	
        TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();
	//	TA_Base_Core::AtsAgentEntityDataPtr AtsData = boost::dynamic_pointer_cast<TA_Base_Core::AtsAgentEntityData>(
	//				 m_genericAgent->getAgentEntityConfiguration());
		//m_entityData = boost::dynamic_pointer_cast<TA_Base_Core::TVSSAgentEntityData>(entityData);
		//m_entityData = boost::dynamic_pointer_cast<TA_Base_Core:EntityData>(entityData);
		m_entityData = entityData;
        //TA_IRS_Bus::TrainAgentProxyFactory::getInstance().agentInitialise( m_genericAgent->getAgentEntityConfiguration() );
		unsigned long key = entityData->getKey();
		unsigned long locationKey = entityData->getLocation();
        unsigned long typeKey = entityData->getTypeKey();
		std::string strLocationName = entityData->getLocationName();
        m_agentAssetName = entityData->getName();
		TA_Base_Core::EntityHelper help( key,"TVSSAgent");
		std::string strPrimaryAddress=help.getParameter("PrimaryTCPAddress");
		std::string strSecondaryAddress=help.getParameter("SecondaryTCPAddress");
		std::string strPrimaryTrainborneAddress=help.getParameter("PrimaryTrainborneAddress");
		std::string strSecondaryTrainborneAddress=help.getParameter("SecondaryTrainborneAddress");
		std::string strTimeout=help.getParameter("TVSSTimeOut");
		std::string strCheckAlarmIntervel=help.getParameter("TVSSCheckAlarmInterval");
		unsigned long timeout = atol(strTimeout.c_str());
		unsigned long checkLaramIntervel = atol(strCheckAlarmIntervel.c_str());

		std::string strTvssTrainborneHeartBeatInterval=help.getParameter("TVSSTrainborneHeartbeatInterval");
        unsigned long tvssTrainborneHeartBeatInterval = atol(strTvssTrainborneHeartBeatInterval.c_str());

        char buff1[50];            
        char buff2[50];            
        sprintf(buff1,"%lu",key);
        sprintf(buff2,"%lu",typeKey);
        RunParams::getInstance().set("entityKey", buff1);
        RunParams::getInstance().set("entityTypeKey", buff2);

		m_tvssServant = new TVSSServant(strPrimaryAddress,strSecondaryAddress,strPrimaryTrainborneAddress,strSecondaryTrainborneAddress,timeout);  // Do before generic as needed by child entities
		
        // Create the tvss object associated with this agent
   
        m_tvssServant->setEntity(*entityData);
        m_alarmProcessor = new TVSSAlarmProcessor(strLocationName,m_tvssServant,checkLaramIntervel);
        m_trainborneHeartbeatProcessor = new TrainBorneHeartBeatProcessor(m_tvssServant, tvssTrainborneHeartBeatInterval);
		//setInitialOperationMode();
		m_tvssServant->setAlarmProcessor(m_alarmProcessor);
		m_tvssServant->setTrainborneHeartbeatProcessor(m_trainborneHeartbeatProcessor);
		if(!m_tvssDataPointList.empty())
		{
			std::map< unsigned long,TA_Base_Bus::DataPoint* >::iterator dp_it = m_tvssDataPointList.begin();
			for(;dp_it!=m_tvssDataPointList.end();dp_it++)
			{
				TA_Base_Bus::DataPoint* dp = dp_it->second;
				m_alarmProcessor->AddDataPoint(dp->getDataPointName(),dp);
			}
		}
		m_scadaProxyFactory->registerLocalEntities(m_agentAssetName , m_genericAgent->getEntityMap());
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

		m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());
		
		loadMonitors(locationKey);
		loadTrainBorneCamerasAlarm(locationKey);
		m_tvssServant->setMonitorConsoleMap(m_monitorConsoleMap);
		// Create the TVSS object associated with this agent 
        // Now that we're ready to accept Corba requests, register the TVSSManager.
        //
		m_tvssServant->start();
		
		FUNCTION_EXIT;
	}
	
	TVSSAgent::~TVSSAgent()
	{
		FUNCTION_ENTRY("Destructor: ~TVSSAgent" );
		cleanUp();
		FUNCTION_EXIT;
	}

	void TVSSAgent::loadMonitors(unsigned long locationKey)
	{
		try
		{
			//TA_Base_Core::IEntityDataList entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(TYPE_TRAINBORNE_CAMERA_ALARM);
			TA_Base_Core::IEntityDataList entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation("VideoMonitor", locationKey);
			TA_Base_Core::IEntityDataList::iterator entity_it = entityList.begin();
			for(;entity_it!=entityList.end();entity_it++)
			{
				TA_Base_Core::IEntityDataPtr monitor(*entity_it);
				createMonitor(monitor);
			}
			
		}
		catch (const TA_Base_Core::DatabaseException& e)
		{
			// object resolution etc
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what() );
		}
		catch(const TA_Base_Core::DataException& e)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what() );
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Exception", "Unknown exception" );
		}
	}

	void TVSSAgent::loadTrainBorneCamerasAlarm(unsigned long locationKey)
	{
		try
		{
			TA_Base_Core::IEntityDataList entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TYPE_TRAINBORNE_CAMERA_ALARM, locationKey);

			TA_Base_Core::IEntityDataList::iterator entity_it = entityList.begin();
			for(;entity_it!=entityList.end();entity_it++)
			{
				TA_Base_Core::IEntityDataPtr alarm(*entity_it);
				createTrainBorneCameraAlarm(alarm);
			}
			m_tvssServant->setEntityAddressMap(m_alarmAddressMap);
			m_tvssServant->setEntityKeyMap(m_alarmKeyMap);
		}
		catch (const TA_Base_Core::DatabaseException& e)
		{
			// object resolution etc
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what() );
		}
		catch(const TA_Base_Core::DataException& e)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what() );
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Exception", "Unknown exception" );
		}
	}

	TrainBorneMonitor* TVSSAgent::createMonitor(TA_Base_Core::IEntityDataPtr entityData)
	{
		if(NULL==entityData)
		{
			LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Monitor entity data is null");
			return NULL;
		}
		unsigned long key = entityData->getKey();
		TA_Base_Core::EntityHelper help( key,"VideoMonitor");
		std::string strOrder=help.getParameter("DisplayOrder");
		int displayOrder=TA_Base_Core::EntityHelper::getIntegerData(strOrder);
		//   bool isConsoleMonitor = ( 0 != monitorEntity->getConsoleKey() );
		std::string strConsole=help.getParameter("Console");
		unsigned long consolekey=TA_Base_Core::EntityHelper::getUnsignedLongData(strConsole);
		//   delete monitorEntity;
		std::string strIsOverallMonitor = help.getParameter("IsOverallMonitor");
		bool bIsOverallMonitor = TA_Base_Core::EntityHelper::getBooleanData(strIsOverallMonitor);
		std::string entityName = entityData->getName();
		std::string entityDescription = entityData->getDescription();
		std::string entityAddr = entityData->getAddress();
		TrainBorneMonitor* mon = new TrainBorneMonitor(key, entityName,entityDescription ,entityAddr, displayOrder,consolekey,bIsOverallMonitor);
		m_monitorPKeyList.push_back(mon);
		if(consolekey>0)
			m_monitorConsoleMap[consolekey] = mon;
		
	}

	TrainBorneCameraAlarm*	TVSSAgent::createTrainBorneCameraAlarm(TA_Base_Core::IEntityDataPtr entityData)
	{
		if(NULL==entityData)
		{
			LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Camera Alarm entity data is null");
			return NULL;
		}
		TrainBorneCameraAlarm* alarm = new TrainBorneCameraAlarm;
		alarm->pkey = entityData->getKey();
		alarm->typeKey = entityData->getTypeKey();
		alarm->subSystemKey=entityData->getSubsystem();
		alarm->locationKey =entityData->getLocation();
		alarm->name = entityData->getName();
		alarm->address=entityData->getAddress();
		alarm->description = entityData->getDescription();
		TA_Base_Core::EntityHelper help( alarm->pkey,"TrainBorneCameraAlarm");
		alarm->assetName=help.getParameter("AssetName");
		alarm->cameraNumbers =help.getParameter("CameraNumbers");
		m_alarmAddressMap[alarm->address]=alarm;
		m_alarmKeyMap[alarm->pkey]=alarm;
		return alarm;
	}

	void TVSSAgent::cleanUp()
	{
		m_tvssDataPointList.clear();
		m_dataNodes.clear();
		std::list<TrainBorneMonitor*>::iterator monitor_it = m_monitorPKeyList.begin();
		for(;monitor_it!=m_monitorPKeyList.end();monitor_it++ )
		{
			TrainBorneMonitor* mon = *monitor_it;
			if(NULL!=mon)
			{
				delete mon;
			}
		}
		m_monitorPKeyList.clear();
		m_monitorConsoleMap.clear();
		std::map<unsigned long,TrainBorneCameraAlarm*>::iterator alarm_it = m_alarmKeyMap.begin();
		for(;alarm_it!=m_alarmKeyMap.end();alarm_it++)
		{
			TrainBorneCameraAlarm* alarm = (*alarm_it).second;
			if(NULL!=alarm)
			{
				delete alarm;
			}
		}
		m_alarmKeyMap.clear();
		m_alarmAddressMap.clear();
		// if bulk interface is created
		
        if(NULL != m_trainborneHeartbeatProcessor)
        {
            delete m_trainborneHeartbeatProcessor;
            m_trainborneHeartbeatProcessor = NULL;
        }
		if(NULL!=m_alarmProcessor)
		{
			delete m_alarmProcessor;
			m_alarmProcessor = NULL;
		}
		if(NULL!=m_tvssServant)
		{
			m_tvssServant->deactivateServant();
			delete m_tvssServant;
			m_tvssServant = NULL;
		}
		if(!m_tvssDataPointList.empty())
		{
			std::map< unsigned long,TA_Base_Bus::DataPoint* >::iterator dp_it = m_tvssDataPointList.begin();
			for(;dp_it!=m_tvssDataPointList.end();dp_it++)
			{
				delete dp_it->second;
			}
			m_tvssDataPointList.clear();
		}
		if ( NULL != m_dpBulkPolledInterface )
		{
			delete m_dpBulkPolledInterface;
			m_dpBulkPolledInterface = NULL;
		}

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
		if ( NULL != m_dnAccessInterface )
		{
			delete m_dnAccessInterface;
			m_dnAccessInterface = NULL;
		}

		if( m_genericAgent != NULL )
		{
			m_genericAgent->deactivateAndDeleteServant();
			m_genericAgent = NULL;
		}

		if (m_tvssServant!=NULL) delete m_tvssServant;

		if ( NULL != m_dataPointFactory)
		{
			delete m_dataPointFactory;
			m_dataPointFactory = NULL;
		}
	}

	void TVSSAgent::agentTerminate()
	{
		FUNCTION_ENTRY( "TVSSAgent: agentTerminate()" );

		delete m_dpBulkPolledInterface;
        m_dpBulkPolledInterface = NULL;

		delete m_dpAccessInterface;
        m_dpAccessInterface = NULL;

		delete m_dnBulkPolledInterface;
        m_dnBulkPolledInterface = NULL;
    
        delete m_dnAccessInterface;
        m_dnAccessInterface = NULL;

		FUNCTION_EXIT;
	}

	bool TVSSAgent::createAllEntities(TA_Base_Core::IEntityDataPtr agentEntityData, const TA_Base_Core::SharedIEntityDataList &entityDataList, TA_Base_Bus::IEntityList &createdEntities)
	{
		FUNCTION_ENTRY( "TVSSAgent: createAllEntities" );

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
        FUNCTION_EXIT;
        return true;
	}

	void TVSSAgent::entityCreationComplete()
	{
		FUNCTION_ENTRY( "TVSSAgent: entityCreationComplete" );

        // loading complete, free some memory
        TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();

        FUNCTION_EXIT;
	}

	TA_Base_Bus::IEntity* TVSSAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		FUNCTION_ENTRY( "TVSSAgent: createEntity" );
		// No child entities
		TA_Base_Bus::IEntity * theEntity = NULL;
		std::string entityDataType = EntityData->getType();

		// if the specified entity is of the type DataPoint
		if ( TA_Base_Core::DataPointEntityData::getStaticType() == entityDataType )
		{
			// create and return the DataPoint entity
		    // cast the EntityData to DataPointEntityData
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
			TA_Base_Core::DataPointEntityDataPtr dataPointEntityData = 
                boost::dynamic_pointer_cast< TA_Base_Core::DataPointEntityData > ( EntityData );
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
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
					m_tvssDataPointList[ dataPointEntityData->getKey() ] = dataPoint;
					
			}

		//	if (!dataPointEntityData->getAllowWrite())
		//	{
				// Initialise the datapoint to not connected status
		//		TA_Base_Bus::DpValue dpValue = dataPoint->getCurrentValue();
    
		//		TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED );
		//		dataPoint->updateFieldState( newState );
		//	}

		//	m_linkStatusThread->setDataPoint( dataPoint );
			
			theEntity = dataPoint;
		}
		/*else if(entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType())
		{
			TA_Base_Bus::DataNode* dn = NULL;
			try
			{
				LOGBEFORECOTS("boost::dynamic_pointer_cast");
                dn = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData), m_persistence);
                LOGAFTERCOTS("boost::dynamic_pointer_cast");
                m_dataNodes.push_back( dn );
				theEntity = dn;
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error creating DataNode %s (%ul)",
                    EntityData->getName().c_str(), EntityData->getKey() );
				
				// To ensure the generic agent is constructed even if one DataNode creation fails
			}
		}*/
		// otherwise don't care
		if(TRAINBORNE_MONITOR_TYPE==entityDataType)
		{
			createMonitor(EntityData);
		}
		else
		{
			// do nothing
		}

		FUNCTION_EXIT;
		return theEntity;
	}
	
	void TVSSAgent::agentSetMonitor()
	{
		FUNCTION_ENTRY("agentSetMonitor");
		m_tvssServant->setToMonitorMode();
		
		// set all datanodes to control mode
		//	for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
		//	{
		//		(*itr)->setOperationMode( true );
		//	}
		// set the proxies to control mode


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

		// set all datanodes to monitor mode
	//	for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
	//	{
	//		(*itr)->setOperationMode( false );
	//	}
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

	//	m_dataPointFactory->setToMonitor();
		
		registerForStateUpdates();

		if (m_entityData.get() == NULL)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "TVSS Agent Entity Data can not be retrieved");
		}

		std::ostringstream msg;
		msg << "TVSSsAgent \"" 
			<< m_entityData->getName()
			<< "\" is running in MONITOR mode";

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "%s",
			msg.str().c_str());
		//submitAuditMessage( msg.str(), false);

		FUNCTION_EXIT;
	}

	void TVSSAgent::registerForStateUpdates()
	{
		FUNCTION_ENTRY("TVSSAgent: registerForStateUpdates");

 		FUNCTION_EXIT;
	}

	void TVSSAgent::agentSetControl()
	{
		LOG_FUNCTION_ENTRY( SourceInfo, "TVSS: agentSetControl" );

		m_tvssServant->setToControlMode();

		// set all datanodes to control mode
	//	for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
	//	{
	//		(*itr)->setOperationMode( true );
	//	}
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
		

		// else do nothing
		std::ostringstream msg;
		msg << "TVSSAgent \"" 
			<< m_entityData->getName()
			<< "\" is running in CONTROL mode";

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "%s",
			msg.str().c_str());
		

		FUNCTION_EXIT;
 
	}

	void TVSSAgent::notifyGroupOffline(const std::string &group)
	{
		FUNCTION_ENTRY( "TVSSAgent: notifyGroupOffline" );

		FUNCTION_EXIT;
	}

	void TVSSAgent::notifyGroupOnline(const std::string &group)
	{
		FUNCTION_ENTRY( "TVSSAgent: notifyGroupOnline" );

		FUNCTION_EXIT;
	}
	
	void TVSSAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef &message)
	{
		FUNCTION_ENTRY("TVSSAgent: receiveSpecialisedMessage(StateUpdateMessageCorbaDef)");
    
		FUNCTION_EXIT;
	}
	
	void TVSSAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails &updateEvent)
	{
		FUNCTION_ENTRY("TVSSAgent: processOnlineUpdate");
   
		FUNCTION_EXIT;
	}
	
	TA_Base_Bus::DataPoint * TVSSAgent::getDataPoint(unsigned long entityKey)
	{
		FUNCTION_ENTRY( "TVSSAgent: getDataPoint" );

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

        FUNCTION_EXIT;
		return theDataPoint;
	}
	
	void TVSSAgent::getAllDataPoints(std::map<unsigned long,TA_Base_Bus::DataPoint*> &dataPointList)
	{
		FUNCTION_ENTRY( "getAllDataPoints" );

		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );

		dataPointList.clear();

		dataPointList = m_tvssDataPointList;

        FUNCTION_EXIT;
	}
	void TVSSAgent::run()
	{
		FUNCTION_ENTRY("TVSSAgent: run");
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

				m_linkStatusThread->start();//limin
				
				// if bulk interface is setup
				// create an instance of the class DataPointAgentCorbaDef_Impl
				m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_entityData->getName(), this );
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
				m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl ( m_entityData->getName(), this );
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

				m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_entityData->getName(), this );
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
				
				m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl ( m_entityData->getName(), this );
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


	void TVSSAgent::setInitialOperationMode()
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

		}

		FUNCTION_EXIT;

	}
	
	TA_Base_Bus::DataPointFactory* TVSSAgent::getDataPointFactory()
	{
		FUNCTION_ENTRY( "TVSS: getDataPointFactory" );

       // TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
        
        FUNCTION_EXIT;
        return m_dataPointFactory;
	}

	void TVSSAgent::getAllDataNodes(std::map<unsigned long,TA_Base_Bus::DataNode*> &dataNodeList)
	{
		FUNCTION_ENTRY( "getAllDataNodes" );

        TA_Base_Core::ThreadGuard guard ( m_dataNodeListLock );
		
        dataNodeList.clear();
        for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
        {
            dataNodeList[ (*itr)->getEntityKey() ] = *itr;
        }        

        FUNCTION_EXIT;
	}

	TA_Base_Bus::DataNode * TVSSAgent::getDataNode(unsigned long entityKey)
	{
		FUNCTION_ENTRY( "getDataNode" );

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
		
        FUNCTION_EXIT;
		return dn;
	}
}