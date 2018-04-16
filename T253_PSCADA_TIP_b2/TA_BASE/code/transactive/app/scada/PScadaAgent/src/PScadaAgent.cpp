/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/PScadaAgent/src/PScadaAgent.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	PScadaAgent implementes the GenericAgent interface.  PScadaAgent monitors and controls the
  * the entities associated with its process, such as PSCADA Controller and the DataPoint entities.
  *
  */

#include "app/scada/PScadaAgent/src/PScadaAgent.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/AlarmAckStateProcessor.h"
#include "bus/scada/datanode_library/src/DataNode.h"

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

    PScadaAgent::PScadaAgent()
    :
	m_pscadaAgentPrimaryKey (0),
    m_proxyDpMonitoringPeriodSecs (PROXY_DP_MONITORING_PERIOD_SECS),
    m_operationMode (TA_Base_Core::NotApplicable),
    m_isRTUSwitchingDisabled (false),
    m_analogueDpUpdateInterval (0),
    m_stationName (""),
    m_agentName (""),
	m_agentAssetName ("")
	{
		FUNCTION_ENTRY("PScadaAgent::PScadaAgent");
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Get AgentName = %s",m_agentName.c_str());
		FUNCTION_EXIT("PScadaAgent::PScadaAgent");
	}


	PScadaAgent::~PScadaAgent()
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "PScadaAgent::~PScadaAgent");
	}


	void PScadaAgent::loadAgentParamValues()
	{
		FUNCTION_ENTRY("loadAgentParamValues");
		// get configuration data for this PScadaAgent
        TA_Base_Core::IEntityDataPtr entityData = TA_Base_Bus::ScadaAgentUser::getAgentEntityConfiguration();
        if (NULL == entityData)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PScadaAgent::loadAgentParamValues()--No data from getAgentEntityConfiguration()");
            return;
        }

		// dynamic cast the entity data to PMSAgentEntityData
		TA_Base_Core::PMSAgentEntityDataPtr pscadaAgentEntityData;
		pscadaAgentEntityData = 
			boost::dynamic_pointer_cast<TA_Base_Core::PMSAgentEntityData>(entityData);

		// get the primary key of this PScadaAgent
		m_pscadaAgentPrimaryKey = pscadaAgentEntityData->getKey();

		// get the agent asset name
		m_agentAssetName = pscadaAgentEntityData->getAssetName();

		// get the configured station name
		m_stationName = pscadaAgentEntityData->getStationName();

		// get the configured health monitoring period for the proxy data points
		// use the defaulted value if not configured in database
		m_proxyDpMonitoringPeriodSecs = pscadaAgentEntityData->getProxyDpHealthMonitoringPeriodSecs();
		if (0 == m_proxyDpMonitoringPeriodSecs)
		{
			m_proxyDpMonitoringPeriodSecs = PROXY_DP_MONITORING_PERIOD_SECS;
		}
		FUNCTION_EXIT("loadAgentParamValues");
	}


	void PScadaAgent::startProcessors()
	{
		FUNCTION_ENTRY("startProcessors");
        createPScadaWorkers();
		FUNCTION_EXIT("startProcessors");
	}


	void PScadaAgent::stopProcessors()
	{
		FUNCTION_ENTRY("stopProcessors");
		m_pscadaManager.stopProcess();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish to stop PScadaManager");
		FUNCTION_EXIT("stopProcessors");
	}


	void PScadaAgent::createPScadaWorkers()
	{
		FUNCTION_ENTRY("createPScadaWorkers");
		// if does not have the agent's pkey
		if (0 == m_pscadaAgentPrimaryKey)
		{
			loadAgentParamValues();
            m_pscadaManager.setAgentAssetName(m_agentAssetName);
            m_pscadaManager.setStationName(m_stationName);
        }

        // add datapoint to pscada manager except virtual datapoint
        std::map< unsigned long, TA_Base_Bus::DataPoint * > datapoints;
        ScadaAgentUser::getAllDataPoints(datapoints);
        for (std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp = datapoints.begin(); itrDp != datapoints.end(); ++itrDp)
        {
            if (false == itrDp->second->isVirtual())
            {
                m_pscadaManager.addDataPoint(itrDp->second);
            }
        }
		FUNCTION_EXIT("createPScadaWorkers");
    }

	TA_Base_Bus::IEntity* PScadaAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
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
			// create and return the PScadaUnit entity
			return m_pscadaManager.createPScadaUnitEntity(EntityData);
		}
		
		FUNCTION_EXIT("createEntity");

		return NULL;
	}

	bool PScadaAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
											  const TA_Base_Core::SharedIEntityDataList& entityDataList,
											  TA_Base_Bus::IEntityList& createdEntities)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In PScadaAgent::createAllEntities() - Empty!");
		return true;
	}


	void PScadaAgent::entityCreationComplete()
	{
		// loading complete, free some memory
		FUNCTION_ENTRY("entityCreationComplete");
		FUNCTION_EXIT("entityCreationComplete");
	}

	void PScadaAgent::agentTerminate()
	{
		FUNCTION_ENTRY("agentTerminate");
        // make sure the processors are all terminated
		stopProcessors();
		FUNCTION_EXIT("agentTerminate");
    }


	void PScadaAgent::agentSetMonitor()
	{
		FUNCTION_ENTRY("agentSetMonitor");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PScadaAgent is being set to monitor...");

        if (m_operationMode == TA_Base_Core::Monitor)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");
			return;
        }
        // set the flag to indicate the agent is now in Standby mode
		m_operationMode = TA_Base_Core::Monitor;
		        
        // set PScada manager to monitor mode
        m_pscadaManager.setToMonitor();
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PScadaManager is set to monitor.");
		registerForMonitorMessages();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PScadaAgent for Station %s has been started up in MONITOR mode",
            m_stationName.c_str());
		FUNCTION_EXIT("agentSetMonitor");
	}

    void PScadaAgent::agentSetControl()
    {
		FUNCTION_ENTRY("agentSetControl");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PScadaAgent is being set to control...");

        if (m_operationMode == TA_Base_Core::Control)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetControl() called when agent is already in control");
			return;
        }

        // set the flag to indicate the agent is now in Control mode
        m_operationMode = TA_Base_Core::Control;
        registerForControlMessages();
        m_pscadaManager.setToControl();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PScadaManager is set to control.");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PScadaAgent for Station %s has been started up in CONTROL mode",
            m_stationName.c_str());
		FUNCTION_EXIT("agentSetControl");
    }
    

    void PScadaAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("processOnlineUpdate");
		unsigned long updateEventPKey = updateEvent.getKey();

		// if the update event is for PScadaAgent
		if (m_pscadaAgentPrimaryKey == updateEventPKey)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received config update for PScadaAgent...");

			// update PScadaAgent configuration
			updatePScadaAgentConfiguration(updateEvent);
			return;
		}

		// if the update event is for PScadaUnit
		if (m_pscadaManager.updatePScadaUnitConfiguration(updateEvent))
		{
            // already processed, no need go further
			return;
		}

		// else update data point configuration
		updateDataPointConfiguration (updateEvent);
		FUNCTION_EXIT("processOnlineUpdate");
	}


	void PScadaAgent::startPScadaAgent(int argc, char* argv[])
	{
		FUNCTION_ENTRY("startPScadaAgent");
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

		// check the runtime parameter OperationMode
		std::string operationMode ("");

        initialize(argc, argv);
		startProcessors();
        startScadaAgent();

		FUNCTION_EXIT("startPScadaAgent");
	}


	void PScadaAgent::registerForControlMessages()
	{
		FUNCTION_ENTRY("registerForControlMessages");
		// nothing to do at this stage
		FUNCTION_EXIT("registerForControlMessages");
	}


	void PScadaAgent::registerForMonitorMessages()
	{
		FUNCTION_ENTRY("registerForMonitorMessages");
		FUNCTION_EXIT("registerForMonitorMessages");
	}


	void PScadaAgent::updatePScadaAgentConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		FUNCTION_ENTRY("updatePScadaAgentConfiguration");
		std::string msg ("");

		switch (updateEvent.getModifications())
		{
			// only if the PScadaAgent has been updated in the database
			case TA_Base_Core::Update:
			{
				// get the list of changed parameter from the config update
				const std::vector< std::string > * changes = &(updateEvent.getChangedParams());

				if ((0 != changes) && (false == changes->empty()))
				{
					// get configuration data for this PScada Agent
					TA_Base_Core::IEntityDataPtr entityData = TA_Base_Bus::ScadaAgentUser::getAgentEntityConfiguration();

					// dynamic cast the entity data to PMSAgentEntityData
					TA_Base_Core::PMSAgentEntityDataPtr PScadaAgentEntityData;
					PScadaAgentEntityData = 
						boost::dynamic_pointer_cast<TA_Base_Core::PMSAgentEntityData>(entityData);

					// invalidate existing data so that new data is loaded from database
					PScadaAgentEntityData->invalidate();

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
                            m_analogueDpUpdateInterval = PScadaAgentEntityData->getAnalogueDataPointUpdateInterval();
                            //m_pscadaManager.setAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
                        }
                        else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Configuration update changes is not for PScadaAgentEntityData");
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
		FUNCTION_EXIT("updatePScadaAgentConfiguration");
	}


    void PScadaAgent::updateDataPointConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
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
                m_pscadaManager.removeDataPoint(dp);
                break;
                
            case TA_Base_Core::Create:
                m_pscadaManager.addDataPoint(dp);
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
					// update the PScadaUnit's with the data point whose update priority
					// has been changed
					//
					m_pscadaManager.removeDataPoint (dp);
					m_pscadaManager.addDataPoint (dp);
				}
			}

			dp = 0;
		}
		FUNCTION_EXIT("updateDataPointConfiguration");
	}

	void PScadaAgent::notifyGroupOffline(const std::string& group)
	{
		// nothing to do at this stage
		FUNCTION_ENTRY("notifyGroupOffline");
		FUNCTION_EXIT("notifyGroupOffline");
	}


	void PScadaAgent::notifyGroupOnline(const std::string& group)
	{
		// nothing to do at this stage
		FUNCTION_ENTRY("notifyGroupOnline");
		FUNCTION_EXIT("notifyGroupOnline");
	}

		
    void PScadaAgent::registerForStateUpdates()
    {
		FUNCTION_ENTRY("registerForStateUpdates");
		FUNCTION_EXIT("registerForStateUpdates");
   }

	
    void PScadaAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
		FUNCTION_ENTRY("receiveSpecialisedMessage");
		FUNCTION_EXIT("receiveSpecialisedMessage");
    }

	void PScadaAgent::handleDoubleAgentsControlRestored( )
	{
		FUNCTION_ENTRY( "handleDoubleAgentsControlRestored" );
		if (m_operationMode == TA_Base_Core::Control)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Double control occurs, need to reset PscadaManager status" );
			m_pscadaManager.setToMonitor();
			TA_Base_Core::Thread::sleep( 1000 ); // sleep 1 sec
			m_pscadaManager.setToControl();
		}
		FUNCTION_EXIT( "handleDoubleAgentsControlRestored" );
	}

}
