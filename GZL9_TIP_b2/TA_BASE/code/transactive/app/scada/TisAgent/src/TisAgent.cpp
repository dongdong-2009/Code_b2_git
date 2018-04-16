/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/Base/transactive/app/scada/TisAgent/src/TisAgent.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #8 $
  *
  * Last modification:	$DateTime: 2011/03/17 16:52:06 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  *	TisAgent implementes the GenericAgent interface.  TisAgent monitors and controls the
  * the entities associated with its process, such as RTU and the DataPoint entities.
  *
  */

#include "TisAgent.h"
#include "TrainInfoManager.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAgentCorbaDef_Impl.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"
#include "bus/scada/datanode_library/src/SummaryProcessor.h"
#include "bus/scada/business_library/src/BusinessManager.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TisTrainAlarmAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/exceptions/src/DataException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/GenericAgentAudit_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/timers/src/StopwatchUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/GenericAgentException.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    const unsigned long PROXY_DP_MONITORING_PERIOD_SECS = 20;

	TisAgent::TisAgent ()
	:
	m_tisAgentPrimaryKey (0),    
    // m_proxyDpMonitoringPeriodSecs (PROXY_DP_MONITORING_PERIOD_SECS),
    m_isRTUSwitchingDisabled(false),
    m_operationMode (TA_Base_Core::NotApplicable),
    m_analogueDpUpdateInterval(0),
    m_stationName (""),
    m_agentName (""),
	m_agentAssetName (""),
    m_tisDpProcessor(NULL)
    // m_tisProcessDataBlock(NULL)
	{
	
	}


	TisAgent::~TisAgent()
	{
        TrainInfoManager::removeInstance();
	}


	void TisAgent::loadAgentParamValues()
	{
		// get configuration data for this Agent
		TA_Base_Core::IEntityDataPtr entityData = TA_Base_Bus::ScadaAgentUser::getAgentEntityConfiguration();
        if ( NULL == entityData )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "PMSAgent::loadAgentParamValues()--No data from getAgentEntityConfiguration()");
			return;
		}
        
		// dynamic cast the entity data to TisTrainAlarmAgentEntityData
		TA_Base_Core::TisTrainAlarmAgentEntityDataPtr tisTrainAlarmAgentEntityData;
		tisTrainAlarmAgentEntityData = 
			boost::dynamic_pointer_cast<TA_Base_Core::TisTrainAlarmAgentEntityData>(entityData);        
        
		// get the primary key of this TisAgent
		m_tisAgentPrimaryKey = tisTrainAlarmAgentEntityData->getKey();

		// get the agent asset name
		m_agentAssetName = tisTrainAlarmAgentEntityData->getAssetName();

        m_analogueDpUpdateInterval = tisTrainAlarmAgentEntityData->getAnalogueDataPointUpdateInterval();

		std::map < unsigned long, TA_Base_Bus::DataPoint *> dataPointList;
		getAllDataPoints(dataPointList);
		
        // get train data configuration        
        TrainInfoManager::getInstance().addTrains(tisTrainAlarmAgentEntityData, dataPointList);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Created trains.");

		// get the configured station name
		m_stationName = tisTrainAlarmAgentEntityData->getStationName();		 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"loadAgentParamValues finished. Set - "
				"m_tisAgentPrimaryKey = %d \n"
				"m_agentAssetName = %s \n"
				"m_stationName = %s \n"				
			, m_tisAgentPrimaryKey, 
              m_agentAssetName.c_str(), 
              m_stationName.c_str());
	}

	void TisAgent::startProcessors()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In TisAgent::startProcessors()");
		createRTUWorkers();
	}

	void TisAgent::stopProcessors()
	{
	}

    void TisAgent::createRTUWorkers()
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In TisAgent::createRTUWorkers()");

		// if does not have the agent's pkey
        if (0 == m_tisAgentPrimaryKey)
        {
            loadAgentParamValues();
            // create all the train data based on train configuration in database                   

            RTUManagerMap::iterator pos;
            for (pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
            {
                pos->second->setAgentAssetName(m_agentAssetName);             
                pos->second->setStationName(m_stationName);
                pos->second->setIsRTUSwitchingDisabled(m_isRTUSwitchingDisabled);
                pos->second->setAnalogueDpUpdateInterval(m_analogueDpUpdateInterval);
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                            "RTUManager for grouId %d set AgentAssetName %s,StationName %s, \
                             RTUSwitchingDisabled %d, AnalogueDpUpdateInterval %d", 
                             pos->first, m_agentAssetName.c_str(), m_stationName.c_str(), 
                             m_isRTUSwitchingDisabled, m_analogueDpUpdateInterval);
            }
        }
        
        // create rtu workers        
        RTUManagerMap::iterator pos;
        for (pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
        {
             pos->second->createRTUWorkers();
             pos->second->setRTUStatusWorker();
             int groupID = pos->first;
             TA_Base_Core::RTUEntityData* rtuEntityData = m_rtuEntityDataMap[groupID];
             unsigned long 	startAddr 	= rtuEntityData->getRTUPollingTableStartAddress();
             unsigned long 	endAddr 	= rtuEntityData->getRTUPollingTableEndAddress();
             int 			timeout 	= rtuEntityData->getPollTimeout();
                                       
             std::vector<const TrainData *> trains = TrainInfoManager::getInstance().getTrains();
             for (std::vector<const TrainData *>::iterator it = trains.begin(); it != trains.end(); it++)
             {
                 unsigned long trainStartAddr = (*it)->getTrainAddr();
                 unsigned long trainEndAddr = (*it)->getTrainEndAddr();
                 TisDpProcessor* dpProcessor = new TisDpProcessor(m_stationName, trainStartAddr, 
                     trainEndAddr, 0, false, m_analogueDpUpdateInterval);                 
                 pos->second->addDpProcessor(dpProcessor);
                 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Add TisDpProcessor for train %d, \
                     trainStartAddr %d, trainEndAddr %d, m_analogueDpUpdateInterval %d", 
                     (*it)->getTrainNum(), trainStartAddr, trainEndAddr, m_analogueDpUpdateInterval);
             }                                       
             pos->second->addPollingScheduler(startAddr, endAddr, timeout, TA_Base_Bus::FAST_POLLING);
             LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Add polling scheduler startAddr %d, endAddr %d, \
                 timeout %d, polling type FAST_POLLING", startAddr, endAddr, timeout);

             LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RTUManager %d create RTU works", pos->first);
        }

		std::map < unsigned long, TA_Base_Bus::DataPoint *> dataPointList;
		getAllDataPoints(dataPointList);
		
        for (std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator itrDp = dataPointList.begin(); itrDp != dataPointList.end(); ++itrDp)
        {
            if (false == itrDp->second->isVirtual())
            {
                RTUManagerMap::iterator pos;
                for (pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
                {                    
                    bool ret = pos->second->addDataPoint(itrDp->second);
                    if (true == ret)
                    {
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "RTUManager %d addDataPoint address %s, entitykey %d successful.  \
                            It's not special data point for trains.", pos->first, itrDp->second->getDataPointAddress().c_str(), itrDp->second->getEntityKey());
                    }                    
                }
            }
        }
    }

	TA_Base_Bus::IEntity* TisAgent::createEntity(TA_Base_Core::IEntityDataPtr EntityData)
	{
		//
		// Note: the objects of interface IEntityData and IEntity are not owned
		// by this specific agent, but by the GenericAgent.  As such no object
		// clean up shall be done with regards to these objects.
		//

		std::string entityDataType = EntityData->getType();

		// if the specified entity is of the RTU type
		if (TA_Base_Core::RTUEntityData::getStaticType() == entityDataType)
		{
			// create and return the RTU entity
			TA_Base_Core::RTUEntityData *rtuEntityData = dynamic_cast<TA_Base_Core::RTUEntityData*>(EntityData.get());
			int groupID = rtuEntityData->getGroupID();            
			RTUManagerMap::iterator it;
			if( (it = m_rtuManagers.find(groupID) ) == m_rtuManagers.end()) // if not found this group, create it.
			{                            
                // m_rtuManagers[groupID] = new TA_Base_Bus::StandardRTUManager(groupID, false, false, TA_Base_Bus::SINGLE_PRIMARY_RTU_PATTERN);
                m_rtuManagers[groupID] = new TisStandardRtuManager(groupID);

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "created RTUManager with groupID = %d",groupID);
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Not created any RTUManager for groupID %d",groupID);
			}

        	m_rtuManagers[groupID]->createRTUEntity(EntityData);
            m_rtuEntityDataMap[groupID] = rtuEntityData;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "create RTUEntity with groupID = %d",groupID);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Flow should not come here, this should be handled in ScadaAgent");
		}
		return NULL;
	}

	bool TisAgent::createAllEntities( TA_Base_Core::IEntityDataPtr agentEntityData,
											  const TA_Base_Core::SharedIEntityDataList& entityDataList,
											  TA_Base_Bus::IEntityList& createdEntities)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In TisAgent::createAllEntities() - Empty!" );
		// return true to indicate not to call createEntity
		return true;
	}


	void TisAgent::entityCreationComplete()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In TisAgent::entityCreationComplete()." );
	}

	void TisAgent::agentTerminate()
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "In TisAgent::agentTerminate()" );

        // make sure the processors are all terminated
		stopProcessors();
		
    }


	void TisAgent::agentSetMonitor()
	{
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "TisAgent is being set to monitor...");

        if (m_operationMode == TA_Base_Core::Monitor)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetMonitor() called when agent is already in monitor");
			return;
        }
        // set the flag to indicate the agent is now in Standby mode
		m_operationMode = TA_Base_Core::Monitor;
        
        // set rtu manager to monitor mode        
        RTUManagerMap::iterator pos;
        for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{
        	pos->second->setToMonitor();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTUManager %d is set to monitor.",pos->first);
		}        
		
        // register this agent for Monitor messages		
		registerForMonitorMessages();

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TisAgent for Station %s has been started up in MONITOR mode",
            m_stationName.c_str());
	}

    void TisAgent::agentSetControl()
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "TisAgent is being set to control...");

        if (m_operationMode == TA_Base_Core::Control)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"agentSetControl() called when agent is already in control");
			return;
        }
		
        // set the flag to indicate the agent is now in Control mode
        m_operationMode = TA_Base_Core::Control;
        
        // register this agent for Control messages        
        registerForControlMessages();

		RTUManagerMap::iterator pos;
        for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
		{	
			pos->second->setToControl();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RTUManager %d is set to control.",pos->first);
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "TisAgent for Station %s has been started up in CONTROL mode", m_stationName.c_str());
    }
    

    void TisAgent::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		unsigned long updateEventPKey = updateEvent.getKey();

		// if the update event is for TisAgent
		if (m_tisAgentPrimaryKey == updateEventPKey)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received config update for TisAgent...");

			// update TisAgent configuration
			updateTisAgentConfiguration(updateEvent);
			return;
		}

		// if the update event is for RTU1
		bool isContinue = true;

		RTUManagerMap::iterator pos;
		for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
        {
			if( pos->second->updateRTUConfiguration(updateEvent) )
				isContinue = false;
		}
		if (isContinue == false )
		{
            // already processed, no need go further
			return;
		}

		// else update data point configuration
		updateDataPointConfiguration (updateEvent);
	}


	void TisAgent::startTisAgent(int argc, char* argv[])
	{
		LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "startTisAgent in..");		

		// start the processors to process data point events
		initialize(argc, argv);
		
		startProcessors();

		startScadaAgent();
	}


	void TisAgent::registerForControlMessages()
	{
		// nothing to do at this stage
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::registerForControlMessages().");
	}


	void TisAgent::registerForMonitorMessages()
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In PMSAgent::registerForMonitorMessages().");
	}



	void TisAgent::updateTisAgentConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		std::string msg ("");

		switch (updateEvent.getModifications())
		{
			// only if the TisAgent has been updated in the database
			case TA_Base_Core::Update:
			{
				// get the list of changed parameter from the config update
				const std::vector< std::string > * changes = &(updateEvent.getChangedParams());

				if ((0 != changes) && (false == changes->empty()))
				{
					// get configuration data for this EMS Agent
					TA_Base_Core::IEntityDataPtr entityData = TA_Base_Bus::ScadaAgentUser::getAgentEntityConfiguration();

					// dynamic cast the entity data to TisTrainAlarmAgentEntityData
					TA_Base_Core::TisTrainAlarmAgentEntityDataPtr tisTrainAlarmAgentEntityData;
					tisTrainAlarmAgentEntityData = 
						boost::dynamic_pointer_cast<TA_Base_Core::TisTrainAlarmAgentEntityData>(entityData);

					// invalidate existing data so that new data is loaded from database
					tisTrainAlarmAgentEntityData->invalidate();

					for (unsigned int i = 0; i < changes->size(); i++)
					{
					    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Configuration update changes is not for TisTrainAlarmAgentEntityData");
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


    void TisAgent::updateDataPointConfiguration (const TA_Base_Core::ConfigUpdateDetails & updateEvent)
	{
		unsigned long updateEventPKey = updateEvent.getKey();

		// find the DataPoint object whose entity key match with the
		// key associated with the received message

		std::map < unsigned long, TA_Base_Bus::DataPoint *> dataPointList;
		getAllDataPoints(dataPointList);
				
		std::map< unsigned long, TA_Base_Bus::DataPoint * >::iterator dpIter = dataPointList.find (updateEventPKey);

		// if found one
		if (dpIter != dataPointList.end())
		{
			TA_Base_Bus::DataPoint * dp = dpIter->second;

			RTUManagerMap::iterator pos;
			// get the type of updating event
			switch (updateEvent.getModifications())
			{
            case TA_Base_Core::Update:
                // pass the config update to the data point for updating
                dp->update(updateEvent);
                break;
            
            case TA_Base_Core::Delete:
        			for(pos = m_rtuManagers.begin(); pos !=  m_rtuManagers.end(); ++pos)
        				pos->second->removeDataPoint(dp);
                break;
                
            case TA_Base_Core::Create:
					for(pos = m_rtuManagers.begin(); pos !=  m_rtuManagers.end(); ++pos)
        				pos->second->addDataPoint(dp);
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
					// update the RTU's with the data point whose update priority
					// has been changed
					//
					RTUManagerMap::iterator pos;
        			for(pos = m_rtuManagers.begin(); pos != m_rtuManagers.end(); ++pos)
					{
						pos->second->removeDataPoint (dp);
						pos->second->addDataPoint (dp);
					}
					
				}
			}

			dp = 0;
		}
	}

	void TisAgent::notifyGroupOffline(const std::string& group)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In TisAgent::notifyGroupOffline().");
		// nothing to do at this stage
	}


	void TisAgent::notifyGroupOnline(const std::string& group)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In TisAgent::notifyGroupOnline().");
		// nothing to do at this stage
	}
		
    void TisAgent::registerForStateUpdates()
    {
 		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In TisAgent::registerForStateUpdates().");
    }
	
    void TisAgent::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "In TisAgent::receiveSpecialisedMessage().");
    }
}

