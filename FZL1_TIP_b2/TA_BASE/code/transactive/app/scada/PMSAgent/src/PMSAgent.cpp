/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL6_TIP/3002/transactive/app/scada/PMSAgent/src/PMSAgent.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2012/06/12 13:35:44 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	PMSAgent implementes the GenericAgent interface.  PMSAgent monitors and controls the
  * the entities associated with its process, such as PMS RTU and the DataPoint entities.
  *
  */

#include "app/scada/PMSAgent/src/PMSAgent.h"
#include "bus/scada/rtu_library/src/RTUFactory.h"
#include "bus/scada/datapoint_library/src/AlarmAckStateProcessor.h"

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

namespace TA_Base_App
{
	const unsigned long PROXY_DP_MONITORING_PERIOD_SECS = 20;
	PMSAgent::PMSAgent():
	m_pmsAgentPrimaryKey (0),
    m_proxyDpMonitoringPeriodSecs (PROXY_DP_MONITORING_PERIOD_SECS),
    m_operationMode (TA_Base_Core::NotApplicable),
    m_isRTUSwitchingDisabled (false),
    m_analogueDpUpdateInterval (0),
    m_stationName (""),
    m_agentName (""),
	m_agentAssetName (""),
	m_rtuFactory(0)

	{
		// get a handle to the RTUFactory
		m_rtuFactory = &(TA_Base_Bus::RTUFactory::getInstance());

		//TA_ASSERT ((0 != m_rtuFactory), "RTU Factory not created");
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "start--PMSAgent::PMSAgent in construction...");
	}
	
	PMSAgent::~PMSAgent()
	{
		if (NULL != m_rtuFactory)
		{
			TA_Base_Bus::RTUFactory::removeInstance();
			m_rtuFactory = 0;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish call RTUFactory::removeInstance");
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish call PMSAgent::~PMSAgent()");
	}


	void PMSAgent::loadAgentParamValues()
	{
		// get configuration data for this PMS Agent
		TA_Base_Core::IEntityDataPtr entityData = TA_Base_Bus::ScadaAgentUser::getAgentEntityConfiguration();
		if ( NULL == entityData )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PMSAgent::loadAgentParamValues()--No data from getAgentEntityConfiguration()");
			return;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In PMSAgent::loadAgentParamValues()");

		// dynamic cast the entity data to PMSAgentEntityData
		TA_Base_Core::PMSAgentEntityDataPtr pmsAgentEntityData;
		pmsAgentEntityData = 
			boost::dynamic_pointer_cast<TA_Base_Core::PMSAgentEntityData>(entityData);

		// get the primary key of this PMSAgent
		m_pmsAgentPrimaryKey = pmsAgentEntityData->getKey();

		// get the agent asset name
		m_agentAssetName = pmsAgentEntityData->getAssetName();

		// get indication if the RTU switching functionality is disabled
        m_isRTUSwitchingDisabled = pmsAgentEntityData->getIsRTUSwitchingDisabled();
        m_analogueDpUpdateInterval = pmsAgentEntityData->getAnalogueDataPointUpdateInterval();

		// get the configured station name
		m_stationName = pmsAgentEntityData->getStationName();

		// get the configured health monitoring period for the proxy data points
		// use the defaulted value if not configured in database
		m_proxyDpMonitoringPeriodSecs = pmsAgentEntityData->getProxyDpHealthMonitoringPeriodSecs();
		if (0 == m_proxyDpMonitoringPeriodSecs)
		{
			m_proxyDpMonitoringPeriodSecs = PROXY_DP_MONITORING_PERIOD_SECS;
		}
	}


	void PMSAgent::startProcessors()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In PMSAgent::startProcessors()");
		createRTUWorkers();
	}


	void PMSAgent::stopProcessors()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In PMSAgent::stopProcessors()");
		m_rtuFactory->setRTUToMonitorMode();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish RTUManagerMap setToMonitor");
	}


	void PMSAgent::createRTUWorkers()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In PMSAgent::createRTUWorkers()");
		// if does not have the agent's pkey
		if (0 == m_pmsAgentPrimaryKey)
		{
			loadAgentParamValues();
            m_rtuFactory->setAgentAssetName(m_agentAssetName);
			m_rtuFactory->setIsRTUSwitchingDisabled(m_isRTUSwitchingDisabled);
            m_rtuFactory->setStationName(m_stationName);
            m_rtuFactory->setAnalogueDpUpdateInterval(m_analogueDpUpdateInterval); 
				
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManagers set AgentAssetName,StationName,AnalogueDpUpdateInterval");
		}
		m_rtuFactory->createRTUWorkers();

		std::map < unsigned long, TA_Base_Bus::DataPoint *> dataPointList; 
		TA_Base_Bus::ScadaAgentUser::getAllDataPoints(dataPointList);

        // add datapoint to rtu manager except virtual datapoint
		m_rtuFactory->addDataPointList(dataPointList);

    }


	TA_Base_Bus::IEntity* PMSAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		//
		// Note: the objects of interface IEntityData and IEntity are not owned
		// by this specific agent, but by the GenericAgent.  As such no object
		// clean up shall be done with regards to these objects.
		//
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In PMSAgent::createEntity()");

		std::string entityDataType = EntityData->getType();
		// if the specified entity is of the RTU type
		if (TA_Base_Core::RTUEntityData::getStaticType() == entityDataType)
		{
			m_rtuFactory->createRTUEntity(EntityData);
		}
		// if the specified entity is of the type DataPoint
		else if ("Station" == entityDataType)
		{
			m_rtuFactory->addStationEntities(EntityData);
		}
		else if( "StationSystem" == entityDataType )
		{
			m_rtuFactory->addStationEntities(EntityData);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Flow should not come here, this should be handled elsewhere e.g. ScadaAgent");
		}


		return NULL;
	}

	bool PMSAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
											  const TA_Base_Core::SharedIEntityDataList& entityDataList,
											  TA_Base_Bus::IEntityList& createdEntities)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In PMSAgent::createAllEntities() - Empty!" );

		return true;
	}


	void PMSAgent::entityCreationComplete()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In PMSAgent::entityCreationComplete()." );
	}

	void PMSAgent::agentTerminate()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In PMSAgent::agentTerminate()" );

        // make sure the processors are all terminated
		stopProcessors();
		
    }


	void PMSAgent::agentSetMonitor()
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PMSAgent is being set to monitor...");
	///may be dont need m_operationMode
        if (m_operationMode == TA_Base_Core::Monitor)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");
			return;
        }
        // set the flag to indicate the agent is now in Standby mode
		m_operationMode = TA_Base_Core::Monitor;     
		registerForMonitorMessages();

        // set rtu manager to monitor mode
		m_rtuFactory->setRTUToMonitorMode();        
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PMSAgent for Station %s has been started up in MONITOR mode", m_stationName.c_str());
	}

    void PMSAgent::agentSetControl()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "PMSAgent is being set to control...");

        if (m_operationMode == TA_Base_Core::Control)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetControl() called when agent is already in control");
			return;
        }
		
        // set the flag to indicate the agent is now in Control mode
        m_operationMode = TA_Base_Core::Control;      

        // register this agent for Control messages
        //TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
	    registerForControlMessages();
		m_rtuFactory->setRTUToControlMode();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "PMSAgent for Station %s has been started up in CONTROL mode", m_stationName.c_str());
    }
    

    void PMSAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::processOnlineUpdate().");
		unsigned long updateEventPKey = updateEvent.getKey();

		// if the update event is for PMSAgent
		if (m_pmsAgentPrimaryKey == updateEventPKey)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received config update for PMSAgent...");

			// update PMSAgent configuration
			updatePMSAgentConfiguration(updateEvent);
			return;
		}
		m_rtuFactory->updateRTUConfiguration(updateEvent);
		updateDataPointConfiguration (updateEvent);
	}


	void PMSAgent::startPMSAgent(int argc, char* argv[])
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::startPMSAgent().");
		// start the processors to process data point events
		initialize(argc, argv);
		
		startProcessors();

		startScadaAgent();
	}


	void PMSAgent::registerForControlMessages()
	{
		// nothing to do at this stage
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::registerForControlMessages().");
	}


	void PMSAgent::registerForMonitorMessages()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::registerForMonitorMessages().");
	}


	void PMSAgent::updatePMSAgentConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::updatePMSAgentConfiguration().");
		std::string msg ("");

		switch (updateEvent.getModifications())
		{
			// only if the PMSAgent has been updated in the database
			case TA_Base_Core::Update:
			{
				// get the list of changed parameter from the config update
				const std::vector< std::string > * changes = &(updateEvent.getChangedParams());

				if ((0 != changes) && (false == changes->empty()))
				{
					// get configuration data for this PMS Agent
					TA_Base_Core::IEntityDataPtr entityData = TA_Base_Bus::ScadaAgentUser::getAgentEntityConfiguration();

					// dynamic cast the entity data to PMSAgentEntityData
					TA_Base_Core::PMSAgentEntityDataPtr pmsAgentEntityData;
					pmsAgentEntityData = 
						boost::dynamic_pointer_cast<TA_Base_Core::PMSAgentEntityData>(entityData);

					// invalidate existing data so that new data is loaded from database
					pmsAgentEntityData->invalidate();

					for (unsigned int i = 0; i < changes->size(); i++)
					{
						// RTU switching disabled
						if (TA_Base_Core::PMSAgentEntityData::RTU_SWITCHING_DISABLED == (*changes)[i])
						{
                            m_isRTUSwitchingDisabled = pmsAgentEntityData->getIsRTUSwitchingDisabled();
							m_rtuFactory->setIsRTUSwitchingDisabled(m_isRTUSwitchingDisabled);
						}

						// Proxy DataPoint Health Monitoring Period
						else if (TA_Base_Core::PMSAgentEntityData::PROXY_DP_HEALTH_MONITORING_PERIOD_SECS == (*changes)[i])
						{
							// proxy health monitoring is no longer available in 
							// the latest ScadaProxyFactory --> therefore this will do nothing
						}

                        // Analogue DataPoint Update Interval // TD16169
                        else if ( TA_Base_Core::PMSAgentEntityData::ANALOGUE_DP_UPDATE_INTERVAL == (*changes)[i] )
                        {
                            m_analogueDpUpdateInterval = pmsAgentEntityData->getAnalogueDataPointUpdateInterval();
							m_rtuFactory->setAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
						}
                        else
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Configuration update changes is not for PMSAgentEntityData");
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
	}


    void PMSAgent::updateDataPointConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::updateDataPointConfiguration().");
		unsigned long updateEventPKey = updateEvent.getKey();
	
		std::map < unsigned long, TA_Base_Bus::DataPoint *> dataPointList ;
		TA_Base_Bus::ScadaAgentUser::getAllDataPoints(dataPointList);

		m_rtuFactory->updateDataPointConfiguration(updateEvent, dataPointList);

	}

	void PMSAgent::notifyGroupOffline(const std::string& group)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::notifyGroupOffline().");
		// nothing to do at this stage
	}


	void PMSAgent::notifyGroupOnline(const std::string& group)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::notifyGroupOnline().");
		// nothing to do at this stage
	}

    void PMSAgent::registerForStateUpdates()
    {
 		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::registerForStateUpdates().");
	}

    void PMSAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::receiveSpecialisedMessage().");
	}
}
