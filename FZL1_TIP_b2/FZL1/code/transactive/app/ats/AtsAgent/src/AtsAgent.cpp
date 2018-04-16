/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/ats/AtsAgent/src/AtsAgent.cpp $
  * @author:   Ross Tucker
  * @version:  $Revision: #4 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2013/03/22 10:46:05 $
  * Last mofified by:  $Author: peter.wong $
  *
  * This base implementation should be extended by all Agents
  * within TransActive.
  */

#ifdef __WIN32__
#pragma warning(disable:4786 4503)
#endif


#include "app/ats/AtsAgent/src/AtsAgent.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datapoint_library/src/AlarmAckStateProcessor.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"//limin

#include "core/alarm/src/AlarmHelperManager.h"
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
#include "app/ats/AtsAgent/ModBusProtocolHandler/src/AtsModBusManager.h"
#include "app/ats/AtsAgent/TimetableManager/src/TimetableManager.h"

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
      m_scadaProxyFactory( NULL ),
      m_summaryProcessor( NULL ),
	  m_dnBulkPolledInterface(NULL),
	  m_dnAccessInterface( NULL ),
	  m_operationMode(TA_Base_Core::NotApplicable),
	  m_atsModBusManager(NULL),
	  m_timeTableManager(NULL)
	{
        FUNCTION_ENTRY( "AtsAgent" );
		m_dataNodes.clear();
        
		m_genericAgent = new TA_Base_Bus::GenericAgent(argc, argv, this);
      
        // register the local entities into ScadaProxyFactory
		m_scadaProxyFactory->registerLocalEntities(m_genericAgent->getAgentEntityConfiguration()->getAgentName() , m_genericAgent->getEntityMap());

		// set the proxies to monitor mode
		m_scadaProxyFactory->setProxiesToMonitorMode();

		
		// create SummaryProcessor
        m_summaryProcessor = TA_Base_Bus::SummaryProcessor::getInstance();
		m_summaryProcessor->setEntityMap(m_genericAgent->getEntityMap());

		m_atsModBusManager->initializeAtsEntity(m_genericAgent->getAgentEntityConfiguration());
		initializeLocalData();

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

		if (NULL != m_atsModBusManager )
		{
			m_atsModBusManager->setOperationMode(TA_Base_Core::Monitor);
			delete m_atsModBusManager;
			m_atsModBusManager = NULL;
		}

		if (NULL != m_timeTableManager)
		{
			delete m_timeTableManager;
			m_timeTableManager = NULL;
		}

		m_dataNodes.clear();

		m_scadaProxyFactory->setProxiesToMonitorMode();

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

		// clean up handle to SummaryProcessor
		if ( NULL != m_summaryProcessor )
		{
			TA_Base_Bus::SummaryProcessor::removeInstance(m_summaryProcessor);
			m_summaryProcessor = NULL;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to stop SummaryProcessor");
		}

		// remove handle to ScadaProxyFactory
		if ( NULL != m_scadaProxyFactory )
		{
			TA_Base_Bus::ScadaProxyFactory::removeInstance();
			m_scadaProxyFactory = 0;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to call ScadaProxyFactory::removeInstance");
		}

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to clean AlarmHelperManager");

		TA_Base_Core::SingletonTimerUtil::removeInstance();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish remove SingletonTimerUtil" );

		// clean up the GenericAgent
		if (0 != m_genericAgent)
		{
			try
			{
				m_genericAgent->deactivateAndDeleteServant();
				m_genericAgent = NULL;
			}
			catch (...)
			{
				// do nothing 
			}
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to call GenericAgent::deactivateAndDeleteServant");

	}

	void AtsAgent::beforeGetDescendantsOfAgent(TA_Base_Core::IEntityDataPtr agentEntity)
	{
		m_atsModBusManager = new AtsModBusManager;
		m_scadaProxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
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
		agentSetMonitor();
		stopProcessors();
		    
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

		//For RTU Entity and SIG Datapoint
		m_atsModBusManager->createEntity(EntityData);

		if(entityDataType == TA_Base_Core::DataNodeEntityData::getStaticType())
		{
			TA_Base_Bus::DataNode* dn = NULL;
			try
			{
                dn = new TA_Base_Bus::DataNode (boost::dynamic_pointer_cast<TA_Base_Core::DataNodeEntityData>(EntityData), m_persistence);
                
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
		
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "AtsAgent is being set to Monitor Mode.");

		if (m_operationMode == TA_Base_Core::Monitor)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");
			return;
		}

		m_operationMode = TA_Base_Core::Monitor;

		m_atsModBusManager->setOperationMode(m_operationMode);

		if (NULL != m_timeTableManager)
		{
			m_timeTableManager->OnMonitorMode();
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

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "AtsAgent is being set to Control Mode.");

		if (m_operationMode == TA_Base_Core::Control)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetControl() called when agent is already in control");
			return;
		}

		m_operationMode = TA_Base_Core::Control;

		// set all datanodes to control mode
		for ( DataNodeList::reverse_iterator itr = m_dataNodes.rbegin(); itr != m_dataNodes.rend(); ++itr )
		{
			(*itr)->setOperationMode( true );
		}
		// set the proxies to control mode
		m_scadaProxyFactory->setProxiesToControlMode();	

		m_atsModBusManager->setOperationMode(m_operationMode);

		if (NULL != m_timeTableManager)
		{
			m_timeTableManager->OnControlMode();
			m_timeTableManager->start();
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

				m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_genericAgent->getAgentEntityConfiguration()->getAgentName(), this );
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
				
				m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl ( m_genericAgent->getAgentEntityConfiguration()->getAgentName(), this );
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

	void AtsAgent::stopProcessors()
	{
		FUNCTION_ENTRY("stopProcessors");

		if ( NULL != m_summaryProcessor )
		{
			m_summaryProcessor->stopProcessor();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to stop SummaryProcessor");
		}

		if ( NULL != m_scadaProxyFactory )
		{
			m_scadaProxyFactory->stopProcessing();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to stop ScadaProxyFactory");
		}

		TA_Base_Bus::AlarmAckStateProcessor::getInstance().stopProcess();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish stop AlarmAckStateProcessor");

		TA_Base_Core::SingletonTimerUtil::getInstance().invalidate();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish invalidate SingletonTimerUtil" );

		FUNCTION_EXIT("stopProcessors");
	}

	void AtsAgent::initializeLocalData()
	{
		FUNCTION_ENTRY("initializeAtsEntityData");

		TA_Base_Core::AtsAgentEntityDataPtr agentEntityData;
		try
		{
			agentEntityData = 
				boost::dynamic_pointer_cast<TA_Base_Core::AtsAgentEntityData>
				(m_genericAgent->getAgentEntityConfiguration());

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugNorm, "Retrieving AtsAgent Entity Configuration");

			if(agentEntityData.get() == NULL)
			{
				TA_THROW(TA_Base_Core::GenericAgentException(TA_Base_Core::GenericAgentException::ENTITY_CONFIG_FAILURE, "Failed to retrieve AtsAgent entity data"));
			}

			TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType = agentEntityData->getAgentAtsLocationType();
			unsigned long timeTbleChckInterval = agentEntityData->getTimetblChkInterval();
			std::string timeTblDirPath = agentEntityData->getTimeTableDirPath();

			//TimeTableManager is only use at Occ 
			if (locationType == TA_Base_Core::AtsAgentEntityData::Occ)
			{
				m_timeTableManager = new TimetableManager(timeTbleChckInterval,timeTblDirPath);
			}
	
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

		FUNCTION_EXIT;
	}
}
