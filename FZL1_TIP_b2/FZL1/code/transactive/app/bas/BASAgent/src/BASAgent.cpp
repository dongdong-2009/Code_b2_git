/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/FZL1/transactive/app/bas/BASAgent/src/BASAgent.cpp $
  * @author:	Raymond Pau
  * @version:	$Revision: #10 $
  *
  * Last modification:	$DateTime: 2017/06/06 17:36:53 $
  * Last modified by:	$Author: yong.liu $
  *
  * Description:
  *
  *
  */

#include "app/bas/BASAgent/src/BASAgent.h"
#include "app/bas/BASAgent/src/BasAgentController.h"
#include "app/bas/BASAgent/src/BASAgentParams.h"
#include "app/bas/BASAgent/src/DataPointUpdateProcessor.h"
#include "app/bas/BASAgent/src/BasCachedConfig.h"
#include "app/bas/BASAgent/src/StationModeManager.h"
#include "app/bas/BASAgent/src/PeriodicThread.h"
#include "app/bas/BASAgent/src/TimetableManager.h"
#include "app/bas/BASAgent/src/BasEventSubmitter.h"
#include "bus/scada/datapoint_library/src/AlarmAckStateProcessor.h"
#include "bus/scada/rtu_library/src/RTUFactory.h"


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

	BASAgent::BASAgent ()
	:
	m_basAgentPrimaryKey (0),
    m_proxyDpMonitoringPeriodSecs (PROXY_DP_MONITORING_PERIOD_SECS),
    m_operationMode (TA_Base_Core::NotApplicable),
    m_isRTUSwitchingDisabled (false),
    m_analogueDpUpdateInterval (0),
    m_stationName (""),
    m_agentName (""),
	m_agentAssetName (""),
	m_rtuFactory(0),
    m_stationModeExecutionImpl(NULL),
    m_timetableExecutionImpl(NULL)
	{
		// get a handle to the RTUFactory
		m_rtuFactory = &(TA_Base_Bus::RTUFactory::getInstance());
		//TA_ASSERT ((0 != m_rtuFactory), "RTU Factory not created");
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "BASAgent::BASAgent in construction...");
	}


	BASAgent::~BASAgent()
	{

		if (NULL != m_rtuFactory)
		{
			TA_Base_Bus::RTUFactory::removeInstance();
			m_rtuFactory = 0;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "stop--finish call RTUFactory::removeInstance");
		}
        
        if (NULL != m_stationModeExecutionImpl)
        {
            delete m_stationModeExecutionImpl;
            m_stationModeExecutionImpl = NULL;
        }
        
        if (NULL != m_timetableExecutionImpl)
        {
            delete m_timetableExecutionImpl;
            m_timetableExecutionImpl = NULL;
        }

        if (NULL != m_basAgentController)
        {
            delete m_basAgentController;
            m_basAgentController = NULL;
        }

	}

	void BASAgent::loadAgentParamValues()
	{
// 		// get configuration data for this PMS Agent
// 		TA_Base_Core::IEntityDataPtr entityData = m_genericAgent->getAgentEntityConfiguration();
// 
// 		// dynamic cast the entity data to PMSAgentEntityData
// 		TA_Base_Core::PMSAgentEntityDataPtr pmsAgentEntityData;
// 		pmsAgentEntityData = 
// 			boost::dynamic_pointer_cast<TA_Base_Core::PMSAgentEntityData>(entityData);
// 
// 		// get the primary key of this PMSAgent
// 		m_pmsAgentPrimaryKey = pmsAgentEntityData->getKey();
// 
// 		// get the agent asset name
// 		m_agentAssetName = pmsAgentEntityData->getAssetName();
// 
// 		// get indication if the RTU switching functionality is disabled
//         m_isRTUSwitchingDisabled = pmsAgentEntityData->getIsRTUSwitchingDisabled();
//         m_analogueDpUpdateInterval = pmsAgentEntityData->getAnalogueDataPointUpdateInterval();
// 
// 		// get the configured station name
// 		m_stationName = pmsAgentEntityData->getStationName();
// 
// 		// get the configured health monitoring period for the proxy data points
// 		// use the defaulted value if not configured in database
// 		m_proxyDpMonitoringPeriodSecs = pmsAgentEntityData->getProxyDpHealthMonitoringPeriodSecs();
// 		if (0 == m_proxyDpMonitoringPeriodSecs)
// 		{
// 			m_proxyDpMonitoringPeriodSecs = PROXY_DP_MONITORING_PERIOD_SECS;
// 		}

		TA_Base_Core::IEntityDataPtr entityData = TA_Base_Bus::ScadaAgentUser::getAgentEntityConfiguration();

		TA_Base_Core::StationBASAgentEntityDataPtr basAgentEntityData;
		basAgentEntityData = boost::dynamic_pointer_cast<TA_Base_Core::StationBASAgentEntityData>(entityData);

		m_basAgentPrimaryKey = basAgentEntityData->getKey();
		m_agentAssetName = basAgentEntityData->getAssetName();
		m_isRTUSwitchingDisabled = basAgentEntityData->getIsRTUSwitchingDisabled();
		m_analogueDpUpdateInterval = basAgentEntityData->getAnalogueDataPointUpdateInterval();
		m_stationName = basAgentEntityData->getStationName();
		// get the agent name
		m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str();			
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get parameter values of Station Bas Agent. BasAgentPrimaryKey=%lu, AgentAssetName=%s, IsRTUSwitchingDisabled=%d, AnalogueDpUpdateInterval=%d,StationName=%s",
			m_basAgentPrimaryKey, m_agentAssetName.c_str(), m_isRTUSwitchingDisabled, m_analogueDpUpdateInterval, m_stationName.c_str() );

        //BAS Self Logic
		std::map < unsigned long, TA_Base_Bus::DataPoint *> dpList;
		getAllDataPoints(dpList);
        BasCachedConfig::getInstance().initCache(basAgentEntityData, dpList);

        m_stationModeExecutionImpl = new TA_IRS_App::StationModeManager();
        m_timetableExecutionImpl = new TA_IRS_App::TimetableManager();

        m_basAgentController = new BasAgentController(*this, *m_stationModeExecutionImpl, *m_timetableExecutionImpl);

        m_timetableExecutionImpl->setControlMode(isAgentRunningControlMode());
        m_stationModeExecutionImpl->setControlMode(isAgentRunningControlMode());
        BasEventSubmitter::instance()->setControlMode(isAgentRunningControlMode());

        DataPointUpdateProcessor::instance()->registerFireModeChange();
        DataPointUpdateProcessor::instance()->registerBasParameterChangeEvent();
		
		m_proxyDpMonitoringPeriodSecs = PROXY_DP_MONITORING_PERIOD_SECS;
		
	}


	void BASAgent::startProcessors()
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In BASAgent::startProcessors()");
		createRTUWorkers();
	
        if (NULL != m_basAgentController)
        {
            try
            {
				m_basAgentController->activateServantWithName(m_agentName);
            }
            catch (std::exception& expWhat)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
            }
        }
	}


	void BASAgent::stopProcessors()
    {
        if ( NULL != m_basAgentController)
        {
            try
            {
                m_basAgentController->deactivateServant();
            }
            catch (std::exception& expWhat)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
            }
        }

		m_rtuFactory->setRTUToMonitorMode();
	}


	void BASAgent::createRTUWorkers()
	{
		// if does not have the agent's pkey
		if (0 == m_basAgentPrimaryKey)
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


	TA_Base_Bus::IEntity* BASAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		//
		// Note: the objects of interface IEntityData and IEntity are not owned
		// by this specific agent, but by the GenericAgent.  As such no object
		// clean up shall be done with regards to these objects.
		//
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In BASAgent::createEntity()");

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

	bool BASAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
											  const TA_Base_Core::SharedIEntityDataList& entityDataList,
											  TA_Base_Bus::IEntityList& createdEntities)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In BASAgent::createAllEntities() - Empty!" );

		return true;
	}


	void BASAgent::entityCreationComplete()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In BASAgent::entityCreationComplete()." );
	}

	void BASAgent::agentTerminate()
	{
        // make sure the processors are all terminated
		stopProcessors();
		
    }


	void BASAgent::agentSetMonitor()
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "BASAgent is being set to monitor...");

        if (m_operationMode == TA_Base_Core::Monitor)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");
			return;
        }
        // set the flag to indicate the agent is now in Standby mode
		m_operationMode = TA_Base_Core::Monitor;

        // register this agent for Monitor messages
		//TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		registerForMonitorMessages();
		m_rtuFactory->setRTUToMonitorMode();        

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "BASAgent for Station %s has been started up in MONITOR mode",
            m_stationName.c_str());

        m_timetableExecutionImpl->setControlMode(false);
        m_stationModeExecutionImpl->setControlMode(false);
        BasEventSubmitter::instance()->setControlMode(false);
	}

    void BASAgent::agentSetControl()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "BASAgent is being set to control...");

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

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "BASAgent for Station %s has been started up in CONTROL mode",
            m_stationName.c_str());

        m_timetableExecutionImpl->setControlMode(true);
        m_stationModeExecutionImpl->setControlMode(true);
        BasEventSubmitter::instance()->setControlMode(true);
    }
    

    void BASAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		unsigned long updateEventPKey = updateEvent.getKey();

		// if the update event is for BASAgent
		if (m_basAgentPrimaryKey == updateEventPKey)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received config update for BASAgent...");

			// update BASAgent configuration
			updateBASAgentConfiguration(updateEvent);
			return;
		}
		m_rtuFactory->updateRTUConfiguration(updateEvent);
		updateDataPointConfiguration (updateEvent);
	}


	void BASAgent::startBASAgent(int argc, char* argv[])
	{
		// start the processors to process data point events
		initialize(argc, argv);
		
		startProcessors();

		startScadaAgent();

	}

	void BASAgent::checkOperationMode()
	{
		if (!isAgentRunningControlMode())
		{
			throw (TA_Base_Core::OperationModeException(RPARAM_MONITOR));
		}
	}

	bool BASAgent::isAgentRunningControlMode()
	{
		return m_operationMode == TA_Base_Core::Control ? true : false;
	}

	void BASAgent::registerForControlMessages()
	{
		// nothing to do at this stage
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In BASAgent::registerForControlMessages().");
	}


	void BASAgent::registerForMonitorMessages()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In BASAgent::registerForMonitorMessages().");
	}



	void BASAgent::updateBASAgentConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In BASAgent::updatePMSAgentConfiguration().");
		std::string msg ("");

		switch (updateEvent.getModifications())
		{
			// only if the BASAgent has been updated in the database
			case TA_Base_Core::Update:
			{
				// get the list of changed parameter from the config update
				const std::vector< std::string > * changes = &(updateEvent.getChangedParams());

				if ((0 != changes) && !changes->empty())
				{
					// get configuration data for this BASAgent
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


    void BASAgent::updateDataPointConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In BASAgent::updateDataPointConfiguration().");
		unsigned long updateEventPKey = updateEvent.getKey();
	
		std::map < unsigned long, TA_Base_Bus::DataPoint *> dataPointList ;
		TA_Base_Bus::ScadaAgentUser::getAllDataPoints(dataPointList);

		m_rtuFactory->updateDataPointConfiguration(updateEvent, dataPointList);

	}

	void BASAgent::notifyGroupOffline(const std::string& group)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In BASAgent::notifyGroupOffline().");
		// nothing to do at this stage
	}


	void BASAgent::notifyGroupOnline(const std::string& group)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In BASAgent::notifyGroupOnline().");
		// nothing to do at this stage
	}

		
    void BASAgent::registerForStateUpdates()
    {
 		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In BASAgent::registerForStateUpdates().");
    }

	
    void BASAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
 		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In BASAgent::receiveSpecialisedMessage().");
    }
}

