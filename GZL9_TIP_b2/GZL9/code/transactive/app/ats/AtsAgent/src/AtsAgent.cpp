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
#include "app/ats/AtsAgent/GenaProtocolHandler/src/AtsGenaManager.h"
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
	AtsAgent::AtsAgent()
	: m_operationMode(TA_Base_Core::NotApplicable),
	  m_pAtsManager(NULL),
	  m_timeTableManager(NULL)
	{
        FUNCTION_ENTRY( "AtsAgent" );
		
		m_pAtsManager = new AtsGenaManager;        

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

		if (NULL != m_pAtsManager )
		{
			m_pAtsManager->setOperationMode(TA_Base_Core::Monitor);
			delete m_pAtsManager;
			m_pAtsManager = NULL;
		}

		if (NULL != m_timeTableManager)
		{
			delete m_timeTableManager;
			m_timeTableManager = NULL;
		}
        
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to call stop--finish to call AtsAgent::cleanUp()");
	}

	//
	// IScadaAgentUser implementation
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
		stopProcessors();
		FUNCTION_EXIT;
    }

    bool AtsAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
        const TA_Base_Core::SharedIEntityDataList& entityDataList,
        TA_Base_Bus::IEntityList& createdEntities)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In AtsAgent::createAllEntities()");
        
        for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin();
        iter != entityDataList.end(); ++iter )
        {
            m_pAtsManager->createEntity(*iter);
        }
        
        return true;
    }
    
    
    void AtsAgent::entityCreationComplete()
    {
        FUNCTION_ENTRY( "entityCreationComplete" );
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
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In AtsAgent::createEntity() - Empty!");
		return NULL;
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

		m_pAtsManager->setOperationMode(m_operationMode);

		if (NULL != m_timeTableManager)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "AtsAgent is being set to Monitor Mode:TimeTable");
			m_timeTableManager->OnMonitorMode();
		}
        
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

		m_pAtsManager->setOperationMode(m_operationMode);
		
		if (NULL != m_timeTableManager)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "AtsAgent is being set to Control Mode: TimeTable");
			m_timeTableManager->OnControlMode();
			m_timeTableManager->start();
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

    
	// Utility functions
	void AtsAgent::updateConfiguration()
	{
        FUNCTION_ENTRY( "updateConfiguration" );
		FUNCTION_EXIT;
	}

    void AtsAgent::startProcessors()
    {
        FUNCTION_ENTRY("startProcessors");
        TA_Base_Core::IEntityDataPtr entityData = TA_Base_Bus::ScadaAgentUser::getAgentEntityConfiguration();
        m_pAtsManager->initializeAtsEntity(entityData);
        initializeLocalData();
        std::map < unsigned long, TA_Base_Bus::DataPoint *> dataPointList;
        getAllDataPoints(dataPointList);
        ((AtsGenaManager*)m_pAtsManager)->addAllDataPoints(dataPointList);
		FUNCTION_EXIT("startProcessors");
    }

	void AtsAgent::stopProcessors()
	{
		FUNCTION_ENTRY("stopProcessors");
        
        if (NULL != m_pAtsManager )
        {
            m_pAtsManager->setOperationMode(TA_Base_Core::Monitor);
        }
        
        if (NULL != m_timeTableManager)
        {
            m_timeTableManager->OnMonitorMode();
        }
        
		FUNCTION_EXIT("stopProcessors");
	}
    
    void AtsAgent::startAtsAgent(int argc, char * argv[])
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::startAtsAgent().");
        initialize(argc, argv);
        startProcessors();
        startScadaAgent();
    }

	void AtsAgent::initializeLocalData()
	{
		FUNCTION_ENTRY("initializeAtsEntityData");

		TA_Base_Core::AtsAgentEntityDataPtr agentEntityData;
		try
		{
            TA_Base_Core::IEntityDataPtr entityData = TA_Base_Bus::ScadaAgentUser::getAgentEntityConfiguration();
			agentEntityData = 
				boost::dynamic_pointer_cast<TA_Base_Core::AtsAgentEntityData>(entityData);

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
