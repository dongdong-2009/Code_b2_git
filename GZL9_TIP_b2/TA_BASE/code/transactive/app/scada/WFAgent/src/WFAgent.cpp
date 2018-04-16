/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/WFAgent/src/WFAgent.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	WfAgent implementes the GenericAgent interface.  WFAgent monitors and controls the
  * the entities associated with its process, such as WF Controller and the DataPoint entities.
  *
  */

#include "app/scada/WFAgent/src/WFAgent.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/AlarmAckStateProcessor.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/wf_global_inhibit/src/WFInhibit.h"
#include "bus/scada/wf_global_inhibit/src/WFGlobalInhibitState.h"

#include "core/data_access_interface/entity_access/src/PMSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/message/types/PMSAgentStateUpdate_MessageTypes.h"
#include "core/message/types/PMSAgentAudit_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/timers/src/StopwatchUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
	const unsigned long PROXY_DP_MONITORING_PERIOD_SECS = 20;

	WfAgent::WfAgent()
	:
	m_wfAgentPrimaryKey (0),
    m_proxyDpMonitoringPeriodSecs (PROXY_DP_MONITORING_PERIOD_SECS),
    m_operationMode (TA_Base_Core::NotApplicable),
    m_isRTUSwitchingDisabled (false),
    m_analogueDpUpdateInterval (0),
    m_stationName (""),
    m_agentName (""),
	m_agentAssetName (""),
	m_wfInhibit(NULL)	
	{
		FUNCTION_ENTRY("WfAgent::WfAgent");

		// get the agent name
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get AgentName = %s",m_agentName.c_str());

		FUNCTION_EXIT("WFAgent::WFAgent");
	}


	WfAgent::~WfAgent()
	{
		FUNCTION_ENTRY("WFAgent::~WFAgent");
		delete m_wfInhibit;
		m_wfInhibit = NULL;		
		TA_Base_Core::SingletonTimerUtil::removeInstance();
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "stop--finish remove SingletonTimerUtil" );
		FUNCTION_EXIT("WFAgent::~WFAgent");
	}


	void WfAgent::loadAgentParamValues()
	{
		FUNCTION_ENTRY("loadAgentParamValues");
		// get configuration data for this WFAgent
        TA_Base_Core::IEntityDataPtr entityData = TA_Base_Bus::ScadaAgentUser::getAgentEntityConfiguration();
        if (NULL == entityData)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "WFAgent::loadAgentParamValues()--No data from getAgentEntityConfiguration()");
            return;
        }

		// dynamic cast the entity data to PMSAgentEntityData
		TA_Base_Core::PMSAgentEntityDataPtr wfAgentEntityData;
		wfAgentEntityData = 
			boost::dynamic_pointer_cast<TA_Base_Core::PMSAgentEntityData>(entityData);

		// get the primary key of this WFAgent
		m_wfAgentPrimaryKey = wfAgentEntityData->getKey();

		// get the agent asset name
		m_agentAssetName = wfAgentEntityData->getAssetName();

		// get the configured station name
		m_stationName = wfAgentEntityData->getStationName();

		// get the configured health monitoring period for the proxy data points
		// use the defaulted value if not configured in database
		m_proxyDpMonitoringPeriodSecs = wfAgentEntityData->getProxyDpHealthMonitoringPeriodSecs();
		if (0 == m_proxyDpMonitoringPeriodSecs)
		{
			m_proxyDpMonitoringPeriodSecs = PROXY_DP_MONITORING_PERIOD_SECS;
		}
		FUNCTION_EXIT("loadAgentParamValues");
	}


	void WfAgent::startProcessors()
	{
		FUNCTION_ENTRY("startProcessors");
        createWfWorkers();
		FUNCTION_EXIT("startProcessors");
	}


	void WfAgent::stopProcessors()
	{
		FUNCTION_ENTRY("stopProcessors");

		m_wfManager.stopProcess();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to stop WfManager");

		FUNCTION_EXIT("stopProcessors");
	}


	void WfAgent::createWfWorkers()
	{
		FUNCTION_ENTRY("createWfWorkers");
		// if does not have the agent's pkey
		if (0 == m_wfAgentPrimaryKey)
		{
			loadAgentParamValues();
            m_wfManager.setAgentAssetName(m_agentAssetName);
            m_wfManager.setStationName(m_stationName);
        }

		std::string strAgentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get AgentName = %s",strAgentName.c_str());
		m_wfInhibit = new TA_Base_Bus::WFInhibit(strAgentName);
			
        // add datapoint to wf manager except virtual datapoint
        std::map< unsigned long, TA_Base_Bus::DataPoint * > datapoints;
        ScadaAgentUser::getAllDataPoints(datapoints);
        for (std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp = datapoints.begin(); itrDp != datapoints.end(); ++itrDp)
        {
			if (false == TA_Base_Bus::WFGlobalInhibitState::getInstance()->isGlobalInhibitDataPoint(itrDp->second))
			{
				if (false == itrDp->second->isVirtual())
				{
					m_wfManager.addDataPoint(itrDp->second);
				}
			}

			//TA_Base_Bus::WFGlobalInhibitState::getInstance()->isGlobalInhibitDataPoint(itrDp->second);
        }
		FUNCTION_EXIT("createWfWorkers");
    }

	TA_Base_Bus::IEntity* WfAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		//
		// Note: the objects of interface IEntityData and IEntity are not owned
		// by this specific agent, but by the GenericAgent.  As such no object
		// clean up shall be done with regards to these objects.
		//
		FUNCTION_ENTRY("createEntity");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start to create Entity %s (%ul)",
			EntityData->getName().c_str(), EntityData->getKey()  );

		std::string entityDataType = EntityData->getType();

		// if the specified entity is of the RTU type
		if (TA_Base_Core::RTUEntityData::getStaticType() == entityDataType)
		{
			// create and return the WfUnit entity
			return m_wfManager.createWfUnitEntity(EntityData);
		}

		FUNCTION_EXIT("createEntity");

		return NULL;
	}

	bool WfAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
											  const TA_Base_Core::SharedIEntityDataList& entityDataList,
											  TA_Base_Bus::IEntityList& createdEntities)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In WFAgent::createAllEntities() - Empty!");
		return true;
	}


	void WfAgent::entityCreationComplete()
	{
		// loading complete, free some memory
		FUNCTION_ENTRY("entityCreationComplete");
		FUNCTION_EXIT("entityCreationComplete");
	}

	void WfAgent::agentTerminate()
	{
		FUNCTION_ENTRY("agentTerminate");
        // make sure the processors are all terminated
		stopProcessors();
		FUNCTION_EXIT("agentTerminate");
    }


	void WfAgent::agentSetMonitor()
	{
		FUNCTION_ENTRY("agentSetMonitor");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "WFAgent is being set to monitor...");

        if (m_operationMode == TA_Base_Core::Monitor)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");
			return;
        }
        // set the flag to indicate the agent is now in Standby mode
		m_operationMode = TA_Base_Core::Monitor;

        // set WF manager to monitor mode
        m_wfManager.setToMonitor();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "WfManager is set to monitor.");
        
		registerForMonitorMessages();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "WFAgent for Station %s has been started up in MONITOR mode",
            m_stationName.c_str());
		FUNCTION_EXIT("agentSetMonitor");
	}

    void WfAgent::agentSetControl()
    {
		FUNCTION_ENTRY("agentSetControl");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "WFAgent is being set to control...");

        if (m_operationMode == TA_Base_Core::Control)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetControl() called when agent is already in control");
			return;
        }

        // set the flag to indicate the agent is now in Control mode
        m_operationMode = TA_Base_Core::Control;
        registerForControlMessages();
        m_wfManager.setToControl();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "WfManager is set to control.");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "WFAgent for Station %s has been started up in CONTROL mode",
            m_stationName.c_str());
			
		TA_Base_Bus::WFGlobalInhibitState::getInstance()->initWFGlobalInhibitState();
		FUNCTION_EXIT("agentSetControl");
    }
    

    void WfAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("processOnlineUpdate");
		unsigned long updateEventPKey = updateEvent.getKey();

		// if the update event is for WFAgent
		if (m_wfAgentPrimaryKey == updateEventPKey)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received config update for WFAgent...");

			// update WFAgent configuration
			updateWfAgentConfiguration(updateEvent);
			return;
		}

		// if the update event is for WfUnit
		if (m_wfManager.updateWfUnitConfiguration(updateEvent))
		{
            // already processed, no need go further
			return;
		}

		// else update data point configuration
		updateDataPointConfiguration (updateEvent);
		FUNCTION_EXIT("processOnlineUpdate");
	}


	void WfAgent::startWfAgent(int argc, char* argv[])
	{
		FUNCTION_ENTRY("startWfAgent");
		//
		// solution for CASE 1193
		//
		// create an instance of the class DataPointAgentCorbaDef_Impl
        // create an instance of the class DataPointAgentCorbaDef_Impl
        // m_dpBulkPolledInterface = new TA_Base_Bus::DataPointAgentCorbaDef_Impl ( m_agentName, this );
        // m_dpAccessInterface = new TA_Base_Bus::DataPointAccessCorbaDef_Impl(m_agentName, this);
        // m_dnBulkPolledInterface = new TA_Base_Bus::DataNodeAgentCorbaDef_Impl ( m_agentName, this );
        // m_dnAccessInterface = new TA_Base_Bus::DataNodeAccessCorbaDef_Impl(m_agentName, this);
        
		// else do nothing

        initialize(argc, argv);
		startProcessors();
        startScadaAgent();

		FUNCTION_EXIT("startWfAgent");
	}


	void WfAgent::registerForControlMessages()
	{
		FUNCTION_ENTRY("registerForControlMessages");
		// nothing to do at this stage
		FUNCTION_EXIT("registerForControlMessages");
	}


	void WfAgent::registerForMonitorMessages()
	{
		FUNCTION_ENTRY("registerForMonitorMessages");
		FUNCTION_EXIT("registerForMonitorMessages");
	}


	void WfAgent::updateWfAgentConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		FUNCTION_ENTRY("updateWfAgentConfiguration");
		std::string msg ("");

		switch (updateEvent.getModifications())
		{
			// only if the WFAgent has been updated in the database
			case TA_Base_Core::Update:
			{
				// get the list of changed parameter from the config update
				const std::vector< std::string > * changes = &(updateEvent.getChangedParams());

				if ((0 != changes) && (false == changes->empty()))
				{
					// get configuration data for this WFAgent
					TA_Base_Core::IEntityDataPtr entityData = TA_Base_Bus::ScadaAgentUser::getAgentEntityConfiguration();

					// dynamic cast the entity data to PMSAgentEntityData
					TA_Base_Core::PMSAgentEntityDataPtr WfAgentEntityData;
					WfAgentEntityData = 
						boost::dynamic_pointer_cast<TA_Base_Core::PMSAgentEntityData>(entityData);

					// invalidate existing data so that new data is loaded from database
					WfAgentEntityData->invalidate();

					for (unsigned int i = 0; i < changes->size(); i++)
					{

						// Proxy DataPoint Health Monitoring Period
						if (TA_Base_Core::PMSAgentEntityData::PROXY_DP_HEALTH_MONITORING_PERIOD_SECS == (*changes)[i])
						{
							// proxy health monitoring is no longer available in 
							// the latest ScadaProxyFactory --> therefore this will do nothing
						}

                        // Analogue DataPoint Update Interval // TD16169
                        else if ( TA_Base_Core::PMSAgentEntityData::ANALOGUE_DP_UPDATE_INTERVAL == (*changes)[i] )
                        {
                            m_analogueDpUpdateInterval = WfAgentEntityData->getAnalogueDataPointUpdateInterval();
                            //m_wfManager.setAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
                        }
                        else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Configuration update changes is not for WfAgentEntityData");
						}
					}
				}

				break;
			}

			// don't care in other case
			default:
			{
				// log audit message?
				break;
			}
		}
		FUNCTION_EXIT("updateWfAgentConfiguration");
	}


    void WfAgent::updateDataPointConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		FUNCTION_ENTRY("updateDataPointConfiguration");
		unsigned long updateEventPKey = updateEvent.getKey();

		// find the DataPoint object whose entity key match with the
		// key associated with the received message
        std::map < unsigned long, TA_Base_Bus::DataPoint *> dataPointList;
        TA_Base_Bus::ScadaAgentUser::getAllDataPoints(dataPointList);
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator dpIter = dataPointList.find (updateEventPKey);

		// if found one
		if (dpIter != dataPointList.end())
		{
			TA_Base_Bus::DataPoint * dp = dpIter->second;

			// get the type of updating event
			switch (updateEvent.getModifications())
			{
            case TA_Base_Core::Update:
                // pass the config update to the data point for updating
                dp->update(updateEvent);
                break;
            
            case TA_Base_Core::Delete:
                m_wfManager.removeDataPoint(dp);
                break;
                
            case TA_Base_Core::Create:
                m_wfManager.addDataPoint(dp);
                break;
            
            default:
                // do nothing, exit switch statement
                break;
			}


			//
			// check if the data point's update priority has been changed online
			//

			// get the list of changed parameter from the config update
			const std::vector< std::string > * configChanges = 0;
			configChanges = &updateEvent.getChangedParams();

			// for each of the changed parameters
			for (unsigned int i = 0; i < configChanges->size(); i++)
			{
				std::string paramName = (*configChanges)[i];

				// only interested in this parameter
				if (paramName == TA_Base_Core::DataPointEntityData::DATAPOINT_UPDATE_PRIORITY)
				{
					//
					// update the WfUnit's with the data point whose update priority
					// has been changed
					//
					m_wfManager.removeDataPoint (dp);
					m_wfManager.addDataPoint (dp);
				}
			}

			dp = 0;
		}
		FUNCTION_EXIT("updateDataPointConfiguration");
	}

	void WfAgent::notifyGroupOffline(const std::string& group)
	{
		// nothing to do at this stage
		FUNCTION_ENTRY("notifyGroupOffline");
		FUNCTION_EXIT("notifyGroupOffline");
	}


	void WfAgent::notifyGroupOnline(const std::string& group)
	{
		// nothing to do at this stage
		FUNCTION_ENTRY("notifyGroupOnline");
		FUNCTION_EXIT("notifyGroupOnline");
	}

	
    void WfAgent::registerForStateUpdates()
    {
		FUNCTION_ENTRY("registerForStateUpdates");
		FUNCTION_EXIT("registerForStateUpdates");
   }

	
    void WfAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
		FUNCTION_ENTRY("receiveSpecialisedMessage");
		FUNCTION_EXIT("receiveSpecialisedMessage");
    }

	void WfAgent::handleDoubleAgentsControlRestored( )
	{
		FUNCTION_ENTRY( "handleDoubleAgentsControlRestored" );
		if (m_operationMode == TA_Base_Core::Control)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Double control occurs, need to reset WfManager status" );
			m_wfManager.setToMonitor();
			TA_Base_Core::Thread::sleep( 1000 ); // sleep 1 sec
			m_wfManager.setToControl();
		}
		FUNCTION_EXIT( "handleDoubleAgentsControlRestored" );
	}

}
