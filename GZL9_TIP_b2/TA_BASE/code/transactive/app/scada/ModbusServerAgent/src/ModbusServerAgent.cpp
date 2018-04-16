/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/ModbusServerAgent/src/ModbusServerAgent.cpp $
  * @author:	Darren Sampson
  * @version:	$Revision: #8 $
  *
  * Last modification:	$DateTime: 2017/07/06 11:21:55 $
  * Last modified by:	$Author: dhanshri $
  *
  * Description:
  *
  *	.
  *
  */

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ModbusServerAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/DataNodeAgentStateUpdate_MessageTypes.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/utilities/src/RunParams.h"

#include "bus/generic_agent/src/GenericAgent.h"

#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"

#include "app/scada/ModbusServerAgent/src/ModbusServerAgent.h"

#include "bus/modbus_comms/src/ModbusService.h"
#include "core/process_management/src/UtilityInitialiser.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::GenericAgentException;

namespace TA_Base_App
{
	ModbusServerAgent::ModbusServerAgent ( int argc, char* argv[] )
	:
    m_agentSetControl( false ),
    m_genericAgent( NULL ),
	m_ModbusPointManager(NULL)
	{

		// create handle to Generic Agent
		m_genericAgent = new TA_Base_Bus::GenericAgent ( argc, argv, this, 0, AGENT_USER_TYPE_NORMAL);

		if ( NULL == m_genericAgent )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failure to create GenericAgent object.");
			return ;
		}
		// get the agent name
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "m_agentName = %s", m_agentName.c_str() );
		createModbusManager();
	}

	ModbusServerAgent::~ModbusServerAgent()
	{
		if (NULL != m_ModbusPointManager)
		{
			//ModbusPointManager has data structures and threads to clear out hence better to wait for neat clean up
			delete m_ModbusPointManager;
			m_ModbusPointManager = NULL;
		}

		// stop and remove SingletonTimerUtil singleton
		TA_Base_Core::SingletonTimerUtil::getInstance().invalidate();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish invalidate SingletonTimerUtil" );
		TA_Base_Core::SingletonTimerUtil::removeInstance();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish remove SingletonTimerUtil" );
		// clean up the GenericAgent
		if ( NULL != m_genericAgent )
		{
			try
			{
				m_genericAgent->deactivateAndDeleteServant();
				m_genericAgent = NULL;
			}
			catch ( ... )
			{
                LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Failed to deactivate and delete m_genericAgent servant.");
			}
		}

		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish call GenericAgent::deactivateAndDeleteServant" );
		
	}

    void ModbusServerAgent::agentTerminate()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug," In ModbusServerAgent::agentTerminate()");
		//m_ModbusPointManager->terminateAndWait();
	}

	void ModbusServerAgent::createModbusManager()
	{
		TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();
		TA_Base_Core::ModbusServerAgentEntityDataPtr agentEntityData;
		agentEntityData = boost::dynamic_pointer_cast<TA_Base_Core::ModbusServerAgentEntityData>(entityData);

		m_ModbusPointManager = new ModbusPointManager(agentEntityData);
		if ( NULL == m_ModbusPointManager )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failure to create ModbusPointManager object.");
			return ;
		}
		else
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Created ModbusPointManager." );
		}
		m_ModbusPointManager->createAgentProxy();
	}

    bool ModbusServerAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
                                           const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                           TA_Base_Bus::IEntityList& createdEntities)
    {
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - createAllEntities" );
		// return true to indicate not to call createEntity
        return true;
    }


	TA_Base_Bus::IEntity* ModbusServerAgent::createEntity(TA_Base_Core::IEntityDataPtr entityData)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "In createEntity() - Empty routine." );

		return 0;
	}

    void ModbusServerAgent::entityCreationComplete()
    {
        // loading complete, free some memory
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - entityCreationComplete" );
    }

    /**
      * agentSetMonitor
      *
      * This method is called by GenericAgent whenever it receives
      * a request from the Process Monitor to change its operating state to monitor.
      * The derived agent should respond by subscribing/unsubscribing to/from the 
      * appropriate message channels and performing any other internal house keeping.
      */
    void ModbusServerAgent::agentSetMonitor()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In ModbusServerAgent::agentSetMonitor()");
		// if the agent was in Control mode before
		if ( true == m_agentSetControl )
		{
			// set the flag to indicate the agent is now in Standby mode
			m_agentSetControl = false;

            m_ModbusPointManager->stopSynchronise();
		}
	}

    /**
      * agentSetControl
      *
      * This method is called by GenericAgent whenever it receives
      * a request from the Process Monitor to change its operating state to control.
      * The derived agent should respond by subscribing/unsubscribing to/from the 
      * appropriate message channels and performing any other internal house keeping.
      */
	void ModbusServerAgent::agentSetControl()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In ModbusServerAgent::agentSetControl()");
		// if the agent was in Monitor mode before
		if ( false == m_agentSetControl )
		{
			// set the flag to indicate the agent is now in Control mode
			m_agentSetControl = true;
            
            m_ModbusPointManager->startSynchronise();
		}
	}


	/** 
	  * notifyGroupOffline
	  *
	  * This request is passed on by GenericAgent when it receives a call to 
      * notifyGroupOffline from the Process Monitor.
      *
      * @param The group name
	  */
	void ModbusServerAgent::notifyGroupOffline( const std::string& group )
	{
		// nothing to do at this stage
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - notifyGroupOffline" );
	}

	/** 
	  * notifyGroupOnline
	  *
	  * This request is passed on by GenericAgent when it receives a call to 
      * notifyGroupOnline from the Process Monitor.
      *
      * @param The group name
	  */
	void ModbusServerAgent::notifyGroupOnline( const std::string& group )
	{
		// nothing to do at this stage
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - notifyGroupOnline" );
	}

    /**
      * registerForStateUpdates
      *
      * This method forces agents to subscribe for either
      * state updates or state update requests, depending on whether or not they
      * are operating in monitor or control mode.
      */
	void ModbusServerAgent::registerForStateUpdates()
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - registerForStateUpdates" );
	}

    /**
      * receiveSpecialisedMessage
      *
      * This method is called whenever the Agent receives a state update or
      * state update request (depending on their mode of operation)
      */
    void ModbusServerAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - receiveSpecialisedMessage" );
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
    void ModbusServerAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Empty routine - processOnlineUpdate" );
	}


	void ModbusServerAgent::startModbusServerAgent()
	{
		// check the runtime parameter OperationMode
		std::string operationMode = TA_Base_Core::RunParams::getInstance().get( RPARAM_OPERATIONMODE );
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Starting agent with mode = %s", operationMode.c_str() );
		// if defined as "Control"
		if ( RPARAM_CONTROL == operationMode )
		{
			m_agentSetControl = false;
			agentSetControl();
		}
		else if ( RPARAM_MONITOR == operationMode )
		{
			m_agentSetControl = true;
			agentSetMonitor();
		}

		// start the Generic Agent loop
		if ( NULL != m_genericAgent )
		{
			m_genericAgent->run();
		}
	}
}
