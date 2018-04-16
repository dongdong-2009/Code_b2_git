/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/AtsAgent.cpp $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4503)
#endif

#include "app/ats/AtsAgent/src/AtcDataTable.h"
#include "app/ats/AtsAgent/src/AtsAgent.h"
#include "app/ats/AtsAgent/src/AtsAgentRunParams.h"
#include "app/ats/AtsAgent/src/AtsWatchDogTable.h"
#include "app/ats/AtsAgent/src/DssTable.h"
#include "app/ats/AtsAgent/src/ElectricalSubsectionTable.h"
#include "app/ats/AtsAgent/src/IscsWatchDogTable.h"
#include "app/ats/AtsAgent/src/PlatformTable.h"
#include "app/ats/AtsAgent/src/TrainCctvCamera.h"
#include "app/ats/AtsAgent/src/TrainTable.h"
#include "app/ats/AtsAgent/src/WashTable.h"
#include "app/ats/AtsAgent/src/WildTable.h"
#include "app/ats/AtsAgent/src/AtsServerLinkStatusUpdateThread.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"//limin

#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/exceptions/src/AtsAgentException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/message/types/DataPointStateUpdate_MessageTypes.h"
#include "core/utilities/src/RunParams.h"

#include <time.h>

#include <sstream>


using TA_Base_Core::DataPointStateUpdate::DataPointValueStatusUpdate;
using TA_Base_Core::DataPointStateUpdate::DataPointInternalStateUpdate;
using TA_Base_Core::DataPointStateUpdate::DataPointHealthStateUpdate;

namespace TA_IRS_App
{
    /**
      * Constructor
      */
	AtsAgent::AtsAgent(int argc, char* argv[])
	: m_genericAgent(NULL), 
	  m_dataPointFactory (NULL),
      m_linkStatusThread( NULL ),
      m_tableManager( NULL ),
      m_dataManager( NULL ),
	  m_twpentityData(NULL), //TD15213
	  m_psdentityData(NULL), //TD15213
      m_operationModeManager( NULL ),
      m_scadaProxyFactory( NULL ),
      m_summaryProcessor( NULL ),
	  m_dpBulkPolledInterface (NULL),
	  m_dpAccessInterface (NULL),
	  m_dnBulkPolledInterface(NULL),
	  m_dnAccessInterface( NULL )
	{
        FUNCTION_ENTRY( "AtsAgent" );

        m_atsDataPointList.clear();

		m_dataPointFactory = new TA_Base_Bus::DataPointFactory( this );
        
        m_linkStatusThread = new AtsServerLinkStatusUpdateThread();

		// get a handle to the ScadaProxyFactory
		// Setup the generic agent
		m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);

		if (NULL == m_genericAgent)
		{
			TA_THROW(TA_Base_Core::AtsAgentException("Error: Generic Agent not constructed"));
		}
		
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
        
        // register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_genericAgent->getAgentEntityConfiguration()->getAgentName() , m_genericAgent->getEntityMap());

		try
		{
			// Get agent entity data
			try
			{
				LOGBEFORECOTS("boost::dynamic_pointer_cast");
				m_entityData = boost::dynamic_pointer_cast<TA_Base_Core::AtsAgentEntityData>(
					 m_genericAgent->getAgentEntityConfiguration());
				LOGAFTERCOTS("boost::dynamic_pointer_cast");
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Ats Agent Entity Data can not be retrieved");
			}

			if (m_entityData.get() == NULL)
			{
				TA_THROW(TA_Base_Core::AtsAgentException("Error: Ats Agent Entity Data can not be retrieved"));
			}

			unsigned long entityKey = m_entityData->getKey();
			unsigned long locationKey = m_entityData->getLocation();
			m_agentAssetName = m_entityData->getAssetName();

			// Find the ATS location type
			TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType = m_entityData->getAgentAtsLocationType();

			if (locationType != TA_Base_Core::AtsAgentEntityData::Occ && 
				locationType == TA_Base_Core::AtsAgentEntityData::Depot &&
				locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 && 
				locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 &&
				locationType == TA_Base_Core::AtsAgentEntityData::MainStation)
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException(
							"Configuration Error: Not a known location"));
			}

			// Get the base addresses of the Modbus tables
			int atsTablesBaseAddress = m_entityData->getAtsTablesBaseAddress();
			int iscsTablesBaseAddress = m_entityData->getIscsTablesBaseAddress();

            // set the session id (for acking alarms)
            std::string session = TA_Base_Core::SessionAccessFactory::getInstance().getSuperSessionId();
            TA_Base_Core::RunParams::getInstance().set(RPARAM_SESSIONID, session.c_str());

			// Create the supporting classes

            TA_IRS_Bus::TrainAgentProxyFactory::getInstance().agentInitialise(m_entityData);//limin, added

			// The Operation Mode Manager
			m_operationModeManager = new OperationModeManager(m_entityData);

			TA_ASSERT(m_operationModeManager != NULL, "OperationModeManager not created");

			// The Data Manager

			m_dataManager = new DataManager(m_entityData, m_genericAgent->getEntityMap(), *m_operationModeManager, *m_linkStatusThread);

			//TD15213++
			if( NULL != m_twpentityData )
			{
				m_dataManager->setMAtsTwpEntityData(m_twpentityData);			
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "fail to setMAtsTwpentitydata due to m_datamanage is NULL");
			}
			
			if( NULL != m_psdentityData )
			{
				m_dataManager->setMAtsPsdEntityData(m_psdentityData);			
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "fail to setMAtsPsdentitydata due to m_datamanage is NULL");
			}
			//++TD15213
			TA_ASSERT(m_dataManager != NULL, "DataManager not created");

			// The Table Manager
			m_tableManager = new TableManager(m_entityData, *m_dataManager);

			TA_ASSERT(m_tableManager != NULL, "TableManager not created");
		}
		catch (TA_Base_Core::FatalConfigurationException &e)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", e.what());

			m_genericAgent->terminateWithoutRestart();

			cleanUp();

			TA_THROW(TA_Base_Core::GenericAgentException(
						TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, 
						e.what()));
		}

		// register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_entityData->getAgentName() , m_genericAgent->getEntityMap());
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();
		
		// create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());


		FUNCTION_EXIT;
	} 

    /**
      * Destructor
      */
	AtsAgent::~AtsAgent()
	{
		FUNCTION_ENTRY("~AtsAgent()");

		cleanUp();

		FUNCTION_EXIT;
	}

	void AtsAgent::cleanUp()
	{
		m_atsDataPointList.clear();
		m_dataNodes.clear();

		// if bulk interface is created
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

		// Clean up Table Manager
		if (m_tableManager != NULL)
		{
			delete m_tableManager;
			m_tableManager = NULL;
		}

		// Clean up Data Manager
		if (m_dataManager != NULL)
		{
			delete m_dataManager;
			m_dataManager = NULL;
		}

		// Clean up Operation Mode Manager
		if (m_operationModeManager != NULL)
		{
			delete m_operationModeManager;
			m_operationModeManager = NULL;
		}

		// Clean up pointer to entity data
		//if (m_entityData != NULL)
		//{
		//	delete m_entityData;
		//	m_entityData = NULL;
		//}

		if( m_genericAgent != NULL )
		{
			m_genericAgent->deactivateAndDeleteServant();
			m_genericAgent = NULL;
		}
		
		if ( NULL != m_dataPointFactory )
		{
			delete m_dataPointFactory;
			m_dataPointFactory = NULL;
		}

		//TD15213
		if ( NULL != m_twpentityData)
		{
			delete m_twpentityData;
			m_twpentityData = NULL;
		}

		if ( NULL != m_psdentityData)
		{
			delete m_psdentityData;
			m_psdentityData = NULL;
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
	}

	//
	// IGenericAgentUser implementation
	//

    /**
      * agentTerminate
      *
      * This pure virtual method is called from the onTerminate
      * method in GenericAgent. This allows the agent to do any
      * of its own cleaning up before exiting.
      *
      * If the agent's entities implement CORBA interfaces,
      * this method should take care of deactivating them. It is not
      * possible for GenericAgent to do this because the IEntity 
      * interface is not a ServantBase.
      *
      * If the class implementing this interface holds the GenericAgent
      * as a member variable, DO NOT delete it inside this method.
      * GenericAgent waits for this call to return before shutting down
      * the ORB. Deleting the GenericAgent from within this method will
      * cause obvious problems.
      */
    void AtsAgent::agentTerminate()
	{
        FUNCTION_ENTRY( "agentTerminate" );
		
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

    bool AtsAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
        const TA_Base_Core::SharedIEntityDataList& entityDataList,
        TA_Base_Bus::IEntityList& createdEntities)
    {
        FUNCTION_ENTRY( "createAllEntities" );

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
    
    
    void AtsAgent::entityCreationComplete()
    {
        FUNCTION_ENTRY( "entityCreationComplete" );

        // loading complete, free some memory
        TA_Base_Core::DataNodeAccessFactory::getInstance()->destroyDataNodeHierarchyMap();

        FUNCTION_EXIT;
    }
    
    
    /**
      * createEntity
      *
      * This method takes Entity Configuration data stored in the database and
      * generates an object that implements the IEntity interface. The returned
      * objects are stored and managed within Generic Agent. The Agent can get a 
      * pointer to the objects via the getEntityMap method in GenericAgent.
      *
      * If create entity throws an exception when Generic Agent is creating its
      * vector of entities (in the GenericAgent::doEntityConfiguration method), 
      * the Generic Agent construction will fail.
      *
      * @param Entity data retrieved from the database.
      *
      * @return The constructed entity
      */
	TA_Base_Bus::IEntity* AtsAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
        FUNCTION_ENTRY( "createEntity" );

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
					m_atsDataPointList[ dataPointEntityData->getKey() ] = dataPoint;
			}

			if (!dataPointEntityData->getAllowWrite())
			{
				// Initialise the datapoint to not connected status
				TA_Base_Bus::DpValue dpValue = dataPoint->getCurrentValue();
    
				TA_Base_Bus::DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_BAD_NOT_CONNECTED );
				dataPoint->updateFieldState( newState );
			}

            m_linkStatusThread->setDataPoint( dataPoint );

			theEntity = dataPoint;
		}
		else if ( TA_Base_Core::TrainCctvCameraEntityData::getStaticType() == entityDataType )
		{
			// create and return the TrainCctvCamera entity
		    // cast the EntityData to DataPointEntityData
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
			TA_Base_Core::TrainCctvCameraEntityDataPtr trainCctvCameraEntityData = 
                boost::dynamic_pointer_cast< TA_Base_Core::TrainCctvCameraEntityData > ( EntityData );
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
			TrainCctvCamera* trainCctvCamera = NULL;
			
			try
			{
				trainCctvCamera = new TrainCctvCamera ( trainCctvCameraEntityData );
				trainCctvCamera->setAgentAssetName(m_agentAssetName);
			}
			catch (TA_Base_Core::FatalConfigurationException &e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", e.what());

				TA_THROW(TA_Base_Core::GenericAgentException(
						 TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, 
						 e.what()));
			}
			catch (...)
			{
			}

			if (trainCctvCamera == NULL)
			{
				std::ostringstream msg;
				msg << "Child entity TrainCctvCamera " << trainCctvCameraEntityData->getName() 
					<< " has not configured correctly and cannot be created";  

				TA_THROW(TA_Base_Core::GenericAgentException(
						 TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, 
						 msg.str().c_str()));
			}
			else
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugDebug,
					"Child entity TrainCctvCamera %s has been created",
					trainCctvCameraEntityData->getName().c_str());
			}

			theEntity = trainCctvCamera;
		}
		// TD15213
		else if( TA_Base_Core::AtsTwpEntityData::getStaticType() == entityDataType )
		{	
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
			TA_Base_Core::AtsTwpEntityDataPtr atstwpentitydata = 
			boost::dynamic_pointer_cast< TA_Base_Core::AtsTwpEntityData > ( EntityData );
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
			try
			{
				m_twpentityData = new TA_Base_Core::AtsTwpEntityData(atstwpentitydata->getKey());				
				
			}			
			catch (TA_Base_Core::FatalConfigurationException &e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
				"cannot set atstwpentitydata" );
				LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", e.what());

				TA_THROW(TA_Base_Core::GenericAgentException(
						 TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, 
						 e.what()));
			}
			catch (...)
			{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "create AtsTwpEntity exception");
			}
		}
		else if( TA_Base_Core::AtsPsdEntityData::getStaticType() == entityDataType )
		{	
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
			TA_Base_Core::AtsPsdEntityDataPtr atspsdentitydata = 
			boost::dynamic_pointer_cast< TA_Base_Core::AtsPsdEntityData > ( EntityData );
			LOGAFTERCOTS("boost::dynamic_pointer_cast");
			try		
			{
				m_psdentityData = new TA_Base_Core::AtsPsdEntityData(atspsdentitydata->getKey());				
				
            }
			catch (TA_Base_Core::FatalConfigurationException &e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo,
				"cannot set atspsdentitydata" );
				LOG_EXCEPTION_CATCH(SourceInfo, "FatalConfigurationException", e.what());

				TA_THROW(TA_Base_Core::GenericAgentException(
						 TA_Base_Core::GenericAgentException::CONFIGURATION_ERROR, 
						 e.what()));
			}
			catch (...)
			{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "create AtsPsdEntity exception");
			}
		}
		else if(entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType())
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
		}
		// otherwise don't care
		else
		{
			// do nothing
		}

		FUNCTION_EXIT;
		return theEntity;
    }


    /**
      * agentSetMonitor
      *
      * This pure virtual method is called by GenericAgent whenever it receives
      * a request from the Process Monitor to change its operating state to monitor.
      * The derived agent should respond by subscribing/unsubscribing to/from the 
      * appropriate message channels and performing any other internal house keeping.
      */
    void AtsAgent::agentSetMonitor()
	{
		FUNCTION_ENTRY("agentSetMonitor");
		
		if (m_operationModeManager->inControlMode() == false)
		{
			return;
		}

		m_operationModeManager->inControlMode(false);


		// if bulk interface is set up
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
		for ( DataNodeList::iterator itr = m_dataNodes.begin(); itr != m_dataNodes.end(); ++itr )
		{
			(*itr)->setOperationMode( false );
		}
		
		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();
		

		if (m_tableManager != NULL)
		{
			m_tableManager->setToMonitorMode();
		}

		//wenching++ (TD14526)
		if (m_dataManager != NULL)
		{
			m_dataManager->updateControlModeStatus(false);
		}
		//++wenching (TD14526)

		registerForStateUpdates();

		if (m_entityData.get() == NULL)
		{
			TA_THROW(TA_Base_Core::AtsAgentException("Error: Ats Agent Entity Data can not be retrieved"));
		}

		std::ostringstream msg;
		msg << "AtsAgent \"" 
			<< m_entityData->getName()
			<< "\" is running in MONITOR mode";

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "%s",
			msg.str().c_str());
		//submitAuditMessage( msg.str(), false);


		FUNCTION_EXIT;
    }

    /**
      * agentSetControl
      *
      * This pure virtual method is called by GenericAgent whenever it receives
      * a request from the Process Monitor to change its operating state to control.
      * The derived agent should respond by subscribing/unsubscribing to/from the 
      * appropriate message channels and performing any other internal house keeping.
      */
    void AtsAgent::agentSetControl()
	{
		FUNCTION_ENTRY("agentSetControl");
	
		if (m_operationModeManager->inControlMode() == true)
		{
			FUNCTION_EXIT;
			return;
		}

		if (m_entityData.get() == NULL)
		{
			TA_THROW(TA_Base_Core::AtsAgentException("Error: Ats Agent Entity Data can not be retrieved"));
		}

		// Set operation mode
		m_operationModeManager->inControlMode(true);

		// set all datanodes to control mode
		for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
		{
			(*itr)->setOperationMode( true );
		}
		// set the proxies to control mode
		m_scadaProxyFactory->setProxiesToControlMode();	


		registerForStateUpdates();

		if (m_tableManager != NULL)
		{
			m_tableManager->setToControlMode();
		}

		//wenching++ (TD14526)
		if (m_dataManager != NULL)
		{
			m_dataManager->updateControlModeStatus(true);
		}
		//++wenching (TD14526)

		// if bulk interface is set up
		if ( NULL != m_dpBulkPolledInterface )
		{
			// start receiving remote requests
			m_dpBulkPolledInterface->setToControl();
		}
		// if dp Access Interface interface is set up
		if ( NULL != m_dpBulkPolledInterface )
		{
			// start receiving remote requests
			m_dpAccessInterface->setToControl();
		}
		if ( NULL != m_dnBulkPolledInterface )
		{
			// start receiving remote requests
			m_dnBulkPolledInterface->setToControl();
		}
		// if dp Access Interface interface is set up
		if ( NULL != m_dnBulkPolledInterface )
		{
			// start receiving remote requests
			m_dnAccessInterface->setToControl();
		}


		// else do nothing
		std::ostringstream msg;
		msg << "AtsAgent \"" 
			<< m_entityData->getName()
			<< "\" is running in CONTROL mode";

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "%s",
			msg.str().c_str());
		//submitAuditMessage( msg.str(), false);



		FUNCTION_EXIT;
    }


	/** 
	  * notifyGroupOffline
	  *
	  * This request is passed on by GenericAgent when it receives a call to 
      * notifyGroupOffline from the Process Monitor.
      *
      * @param The group name
	  */
    void AtsAgent::notifyGroupOffline( const std::string& group )
	{
        FUNCTION_ENTRY( "notifyGroupOffline" );
		if ( "OCC" == group )
		{	

			m_operationModeManager->inIscsDegraded(true);
		}

		FUNCTION_EXIT;
    }


	/** 
	  * notifyGroupOnline
	  *
	  * This request is passed on by GenericAgent when it receives a call to 
      * notifyGroupOnline from the Process Monitor.
      *
      * @param The group name
	  */
    void AtsAgent::notifyGroupOnline( const std::string& group )
	{
        FUNCTION_ENTRY( "notifyGroupOnline" );
		if ( "OCC" == group )
		{	
			m_operationModeManager->inIscsDegraded(false);
		}

		FUNCTION_EXIT;
    }

    /**
      * registerForStateUpdates
      *
      * This pure virtual method forces agents to subscribe for either
      * state updates or state update requests, depending on whether or not they
      * are operating in monitor or control mode.
      */
    void AtsAgent::registerForStateUpdates()
	{
		FUNCTION_ENTRY("registerForStateUpdates");

		if (m_operationModeManager != NULL)
		{
			if( m_operationModeManager->inControlMode() )
			{
				//registerForControlMessages();
			}
			else
			{
				//registerForMonitorMessages();
			}
		}

 		FUNCTION_EXIT;
	}

    /**
      * receiveSpecialisedMessage
      *
      * This method is called whenever the Agent receives a state update or
      * state update request (depending on their mode of operation)
      */
    void AtsAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
		FUNCTION_ENTRY("receiveSpecialisedMessage(StateUpdateMessageCorbaDef)");
    
		FUNCTION_EXIT;
	}

    /**
      * processOnlineUpdate
      *
      * Generic Agent will handle the receiving of online updates. However, in some
      * cases, it will be necessary to pass the updates on to the registered agent.
      * For example, if a child entity needs to be updated.
      *
      * @param The update event
      */
    void AtsAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("processOnlineUpdate");
    
		std::ostringstream updateKey;
		updateKey.clear();
		updateKey << updateEvent.getKey();

		std::string agentKey = TA_Base_Core::RunParams::getInstance().get("entityKey");
 
		if ( agentKey == updateKey.str() )
		{
			// update Agent configuration
			updateConfiguration();
		}

		FUNCTION_EXIT;
	}

	//
	// Generic Agent interface function
	//

	/**
	  * run
	  *
	  * This method calls GenericAgent::run() which blocks until the System
	  * Controller shuts us down
	  */
	void AtsAgent::run()
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
					m_operationModeManager->inControlMode(false);
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

	// Utility functions
	void AtsAgent::updateConfiguration()
	{
        FUNCTION_ENTRY( "updateConfiguration" );
		FUNCTION_EXIT;
	}


	TA_Base_Bus::DataPoint * AtsAgent::getDataPoint ( unsigned long entityKey )
	{
        FUNCTION_ENTRY( "getDataPoint" );

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


	void AtsAgent::getAllDataPoints (std::map < unsigned long, TA_Base_Bus::DataPoint *>& dataPointList )
	{
        FUNCTION_ENTRY( "getAllDataPoints" );

		// stop multi threads accessing the list of DataPoint objects at the same time
		TA_Base_Core::ThreadGuard guard ( m_dataPointListLock );

		dataPointList.clear();

		dataPointList = m_atsDataPointList;

        FUNCTION_EXIT;
	}


	TA_Base_Bus::DataNode * AtsAgent::getDataNode( unsigned long entityKey )
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
	

	void AtsAgent::getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList )
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


    TA_Base_Bus::DataPointFactory* AtsAgent::getDataPointFactory()
    {
        FUNCTION_ENTRY( "getDataPointFactory" );

        TA_ASSERT(m_dataPointFactory != NULL, "m_dataPointFactory is not created successfully.");
        
        FUNCTION_EXIT;
        return m_dataPointFactory;
    }
    

}
