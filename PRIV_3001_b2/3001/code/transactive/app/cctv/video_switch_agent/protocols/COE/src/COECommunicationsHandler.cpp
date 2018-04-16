/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Handles the protocol dependent sections of the operations for a COECommunicationsHandler
  * using the COE protocol.
  * This interface is used by the COECommunicationsHandler class. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 4503)
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsChannel.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/AddressHelper.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/ReceivedDataProcessor.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/StateValidationCycle.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/TriggeringEventAckCommand.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/cctv/video_switch_agent/IDL/src/SwitchManagerCorbaDef.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/data_access_interface/entity_access/src/BVSStage.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/types/VisualAlarms_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmConstants.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"


#include <algorithm>
#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>

namespace TA_IRS_App
{
    using TA_Base_Bus::DataPoint;
    using TA_Base_Bus::DpValue;
    using TA_Base_Bus::DataPointState;


    const unsigned short COECommunicationsHandler::NO_ACTIVE_SWITCH_CYCLE_ALLOWANCE = 3;
    const std::string COECommunicationsHandler::VIDEO_SWITCH_TYPE = "VideoSwitch";


    //TD19444

    // TD 15200
    static const std::string INTRUDER_ALM = "CET - Intruder Alm";
    static const std::string INTERCOM_ACT_ALM = "CET - Intercom Act Alm";
    static const std::string BLS_ACT_ALM = "CET - BLS Act Alm";
    static const std::string ESP_ALM = "CET - ESP";
    static const std::string PAT_ALM = "CET - PAT";
    // TD 15200

    //
    // COECommunicationsHandler
    //
    COECommunicationsHandler::COECommunicationsHandler(TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData)
        : m_videoSwitchAgentData(videoSwitchAgentData),
          m_switchConnectTimeout(0),
          m_switchPollInterval(0),
          m_maxSwitchResponseTime(0),
		  m_summaryPollingInterval(0),
		  m_maxSummaryPollingResponseTime(0),
          m_lowPriorityStateValidationCycle(0),
          m_highPriorityStateValidationCycle(0),
          m_activeSwitchChannelIndex(0),
          m_exceededNoActiveSwitchCycleAllowance(false),
          m_receivedDataProcessor(std::auto_ptr< ReceivedDataProcessor >(new ReceivedDataProcessor())),
		  m_alarmCounter(0)
          //m_alarmHelper( std::auto_ptr< TA_Base_Core::AlarmHelper >( TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper() ) )
    {
        try
        {
            // Gather comms related parameters from Agent Entity Config
            std::string location = m_videoSwitchAgentData->getLocationName();
            m_switchConnectTimeout = m_videoSwitchAgentData->getSwitchConnectTimeout();
            m_maxSwitchResponseTime = m_videoSwitchAgentData->getMaxSwitchResponseTime();
            m_switchPollInterval = m_videoSwitchAgentData->getSwitchPollInterval();
			m_maxSummaryPollingResponseTime = m_videoSwitchAgentData->getMaxSummaryPollingResponseTime();
			m_summaryPollingInterval = m_videoSwitchAgentData->getSummaryPollingInterval();
            m_lowPriorityStateValidationCycle = m_videoSwitchAgentData->getLowPriorityStateValidationCycle();
            m_highPriorityStateValidationCycle = m_videoSwitchAgentData->getHighPriorityStateValidationCycle();
            //need to create a map of console keys to videoMonitor Ids if we are the OCC
            m_locationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(videoSwitchAgentData->getLocationName());
			// Load alarm entities
			m_cctvAlarmEntities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation("CctvAlarm", videoSwitchAgentData->getLocation());
			for(TA_Base_Core::IEntityDataList::iterator it = m_cctvAlarmEntities.begin(); it != m_cctvAlarmEntities.end(); ++it)
			{
				m_entityKeyToCCTVAlarm.insert(std::make_pair((*it)->getKey(), *it));
			}
			// Alarm & ack alarm monitor data init
            m_videoMonitorEntities.clear();
            m_videoMonitorEntities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::VideoMonitor::getStaticType(), videoSwitchAgentData->getLocation());
            TA_Base_Core::IEntityDataList::iterator videoMonitorEntitiesIt;
            m_stationAlarmMonitorSet.clear();
            m_occAlarmMonitorSet.clear();
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Loaded %d monitors", m_videoMonitorEntities.size());
            const std::string locationList("AlarmLocationList");
            for(videoMonitorEntitiesIt = m_videoMonitorEntities.begin(); videoMonitorEntitiesIt != m_videoMonitorEntities.end(); videoMonitorEntitiesIt++)
            {
                TA_Base_Core::VideoMonitor* monitor = dynamic_cast<TA_Base_Core::VideoMonitor*>(*videoMonitorEntitiesIt);
                if(monitor != NULL)
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Add Monitor key:%ld,consoleKey:%ld,isAlarm:%d",
                                monitor->getKey(), monitor->getConsoleKey(), monitor->isAlarmMonitor());
                if(monitor != NULL && m_consoleKeyToMonitorKeyId.find(monitor->getConsoleKey()) == m_consoleKeyToMonitorKeyId.end())
                {
                    unsigned long monitorId = atoi(monitor->getAddress().c_str());
                    if(monitorId > 0)
                    {
                        if(monitor->getConsoleKey() > 0)
                        {
                            m_consoleKeyToMonitorKeyId[monitor->getConsoleKey()] = std::make_pair(monitor->getKey(), monitorId);
                        }
                        if(monitor->getAlarmType() == VM_ALARM_TYPE_VSS_TRIGERING)
                        {
                            m_stationAlarmMonitorSet.insert(monitor->getKey());
                            if("OCC" == location || "Occ" == location)
                            {
								std::string locationListStr = monitor->getHelper()->getParameter(locationList);
                                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Monitor:%s, StationId:%s", monitor->getName().c_str(), locationListStr.c_str());
                                typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
                                boost::char_separator<char> sep(",");
                                tokenizer tokens(locationListStr, sep);
								TA_IRS_Core::VideoStationIdAccessFactory::LocationKeyToStationIdMap locationKeyToStaionIdMap;
								locationKeyToStaionIdMap = TA_IRS_Core::VideoStationIdAccessFactory::getInstance().getLocationKeyToStationIdMap();
                                for(tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
                                {
                                    unsigned long locationKey = TA_Base_Core::LocationAccessFactory::getInstance().getLocationKeyFromName(*tok_iter);
                                    if(locationKeyToStaionIdMap.end() != locationKeyToStaionIdMap.find(locationKey))
                                    {
										m_occAlarmMonitorSet.insert(monitor->getKey());
                                        unsigned long stationId = locationKeyToStaionIdMap[locationKey];
                                        m_stationIdAlarmMonitorKeyMap.insert(std::make_pair(stationId, monitor->getKey()));
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        std::stringstream message;
                        message << "Monitor addresses must be a valid number: " << monitor->getName();
                        throw(TA_Base_Core::DataException(message.str().c_str(), TA_Base_Core::DataException::INVALID_VALUE, monitor->getName()));
                    }
                }
                else if(monitor != NULL && (location == "OCC" || location == "KOCC" || location == "KCD"  ||  location == "DTS" || location == "TMF")) // monitor->isAlarmMonitor())
                {
                    std::stringstream message;
                    message << "Console keys must be unique: " << monitor->getName();
                    throw(TA_Base_Core::DataException(message.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, monitor->getName()));
                }
            }
            TA_Base_Bus::VideoSwitchAgentFactory::getInstance().setLocationName(location);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CCTV Configuration Loaded");
            TA_IRS_Core::IVideoTriggeringEvents allEvents = TA_IRS_Core::VideoTriggeringEventAccessFactory::getInstance().getAllVideoTriggeringEvents(false);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Event config Loaded:% config", allEvents.size());
            for(TA_IRS_Core::IVideoTriggeringEvents::iterator it = allEvents.begin(); it != allEvents.end(); ++it)
            {
                unsigned long alarmEntityKey = (*it)->getEntityKey();
                if(m_entityKeyToCCTVAlarm.find(alarmEntityKey) == m_entityKeyToCCTVAlarm.end())
                {
                    // The triggering event config not belong to this location
					delete (*it);
                    continue;
                }
				m_videoTriggeringEvents.push_back(*it);
                TA_COE::AlarmState alarm;
                alarm.eventId = (*it)->getEventId();
                alarm.stationId = (*it)->getStationId();
				AlarmEntityVideoTriggeringEventPair pair(m_entityKeyToCCTVAlarm[alarmEntityKey], *it);
				m_alarmToEntityAndVideoTriggeringEvent.insert(std::make_pair(alarm, pair));
            }
            m_alarmToWallMonitorKeyMap.clear();
            COESwitchManager::getInstance().getStateValidationCycle().setValidationCyclePeriods(m_lowPriorityStateValidationCycle, m_highPriorityStateValidationCycle);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "setValidationCyclePeriods done");
            // Get the VideoSwitch data, so we can get the addresses for the switching hardware.
            unsigned long locationKey = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str() , NULL, 10);
            m_videoSwitchData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(VIDEO_SWITCH_TYPE, locationKey);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "m_videoSwitchData set");
            loadCommunicationsChannels();
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "loadCommunicationsChannels created");
            SwitchManager::getInstance().setAlarmInterface(this);
            m_alarmStack.clear();
            // init command processor
            m_clientRequestProcessor = new CommandsProcessor<RequestCommandSharedPtr>
            (m_clientRequestCommandQueue, boost::bind(&COECommunicationsHandler::processRequestCommand, this, _1));
            m_internalRequestProcessor = new CommandsProcessor<RequestCommandSharedPtr>
            (m_internalRequestCommandQueue, boost::bind(&COECommunicationsHandler::processRequestCommand, this, _1));
            m_notificationProcessor = new CommandsProcessor<NotificationCommandSharedPtr>
            (m_notificationCommandQueue, boost::bind(&COECommunicationsHandler::processNotification, this, _1));
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                                  "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database.");
            cleanUp();
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR,
                                                                     "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database."));
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DataException",
                                  "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database.");
            cleanUp();
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR,
                                                                     "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database."));
        }
    }


    //
    // ~COECommunicationsHandler
    //
    COECommunicationsHandler::~COECommunicationsHandler()
    {
        FUNCTION_ENTRY("Destructor");
        // If an exception is thrown here, there is nothing we can do,
        // but we should make sure no exceptions are thrown out of this destructor.
        try
        {
            // Clean up this thread if still running, and any queued commands.
            disconnect();
            delete m_notificationProcessor;
            delete m_clientRequestProcessor;
            delete m_internalRequestProcessor;
            // Clean up heap objects.
            cleanUp();
            // As this is last COE Protocol object to destruct, here we manually destruct
            // the COESwitvhManager singleton also.
            COESwitchManager::getInstance().removeInstance();
        }
        catch(const TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Reason unknown");
        }
        FUNCTION_EXIT;
    }


    bool COECommunicationsHandler::locationIsStation(std::string locationName)
    {
        if(locationName == "OCC" || locationName == "KOCC" || locationName == "KCD" ||  locationName == "DTS" || locationName == "TMF" || locationName == "GBD")
		{
            return false;
		}
        return true;
    }

    ///
    // submitRequestCommand
    //
    void COECommunicationsHandler::submitRequestCommand(RequestCommandSharedPtr command, bool isClientRequest)
    {
        FUNCTION_ENTRY("submitRequestCommand");
        TA_Base_Core::ThreadGuard threadStateGuard(m_threadStateLock);
        //
        // Make sure the communications handler is connected (i.e. thread active), else
        // do not queue the command, otherwise it may never be processed.
        //
        if(false == isConnected())
        {
            commandNotToBeProcessed(command);
            FUNCTION_EXIT;
            return;
        }
        //
        // Since we are connected, queue the command as specified by isClientRequest,
        // and post the semaphore to indicate there is work to do.
        //
        if(isClientRequest)
        {
            m_clientRequestCommandQueue.push(command);
        }
        else
        {
            m_internalRequestCommandQueue.push(command);
        }
        FUNCTION_EXIT;
    }


    //
    // submitNotificationCommand
    //
    void COECommunicationsHandler::submitNotificationCommand(NotificationCommandSharedPtr command)
    {
        FUNCTION_ENTRY("submitRequestCommand");
        //
        // Important not to acquire the m_threadStateLock here, as may result in
        // deadlock if a channel is trying to submit a NotificationCommand while we
        // are trying to deactivate it. Incidentally, its of no consequence if we
        // queue a notification command while this thread in process of terminating
        // (because no thread of execution is waiting for a notification command to
        // be processed, and client code isn't interested in hardware notifcations
        // after calling disconnect).
        //
        m_notificationCommandQueue.push(command);
        FUNCTION_EXIT;
    }


    //
    // processCommunicationsChannelState
    //
    void COECommunicationsHandler::processCommunicationsChannelState(unsigned int channelIndex, ECommunicationChannelState state)
    {
        FUNCTION_ENTRY("processCommunicationsChannelStateUpdate");
        TA_ASSERT(channelIndex < m_communicationsChannelsInfo.size(), "Invalid channelIndex passed to processCommunicationsChannelStateUpdate!");
        TA_Base_Core::ThreadGuard commsChannelsStateGuard(m_commsChannelsStateLock);
        switch(state)
        {
            case COMMUNICATIONS_FAILURE:
                receivedChannelStateCommsFailure(channelIndex);
                receivedChannelStateNotActive(channelIndex);
                m_communicationsChannelsInfo[ channelIndex ].state = COMMUNICATIONS_FAILURE;
                break;
            case IDLE_SWITCH_CHANNEL:
                receivedChannelStateNotCommsFailure(channelIndex);
                receivedChannelStateNotActive(channelIndex);
                m_communicationsChannelsInfo[ channelIndex ].state = IDLE_SWITCH_CHANNEL;
                break;
            case ACTIVE_SWITCH_CHANNEL:
                receivedChannelStateNotCommsFailure(channelIndex);
                receivedChannelStateActive(channelIndex);
                m_communicationsChannelsInfo[ channelIndex ].state = ACTIVE_SWITCH_CHANNEL;
                break;
            default:
                break;
        }
        FUNCTION_EXIT;
    }


    //
    // processSummaryAlarmsState
    //
    void COECommunicationsHandler::processSummaryAlarmsState(bool systemSummaryAlarm, bool cameraSummaryAlarm, bool ancillarySummaryAlarm, bool dvrSummaryAlarm)
    {
        FUNCTION_ENTRY("processSummaryAlarmsStateUpdate");
        DataPoint* systemDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::SYSTEM_DATAPOINT_ID);
        DataPoint* cameraDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::CAMERA_DATAPOINT_ID);
        DataPoint* ancillaryDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::ANCILLARY_DATAPOINT_ID);
        DataPoint* dvrDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::DVR_DATAPOINT_ID);
        time_t now = time(0);
        if(0 != ancillaryDP)
        {
            DpValue dpValue(ancillaryDP->getDataPointDataType(), ancillaryDP->getEnumLabelsMap(), ancillaryDP->getBooleanLabels());
            dpValue.setBoolean(ancillarySummaryAlarm);
            DataPointState dpState(dpValue, now, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
            dpState.setValue(dpValue);
            ancillaryDP->updateFieldState(dpState);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Set DataPoint Ancillary alarm %ld,status:%d", ancillaryDP->getEntityKey(), ancillarySummaryAlarm);
        }
        if(0 != cameraDP)
        {
            DpValue dpValue(cameraDP->getDataPointDataType(), cameraDP->getEnumLabelsMap(), cameraDP->getBooleanLabels());
            dpValue.setBoolean(cameraSummaryAlarm);
            DataPointState dpState(dpValue, now, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
            dpState.setValue(dpValue);
            cameraDP->updateFieldState(dpState);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Set DataPoint Camera alarm %ld,status:%d", cameraDP->getEntityKey(), cameraSummaryAlarm);
        }
        if(0 != dvrDP)
        {
            DpValue dpValue(dvrDP->getDataPointDataType(), dvrDP->getEnumLabelsMap(), dvrDP->getBooleanLabels());
            dpValue.setBoolean(dvrSummaryAlarm);
            DataPointState dpState(dpValue, now, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
            dpState.setValue(dpValue);
            dvrDP->updateFieldState(dpState);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Set DataPoint Driver alarm %ld,status:%d", dvrDP->getEntityKey(), dvrSummaryAlarm);
        }
        if(0 != systemDP)
        {
            DpValue dpValue(systemDP->getDataPointDataType(), systemDP->getEnumLabelsMap(), systemDP->getBooleanLabels());
            dpValue.setBoolean(systemSummaryAlarm);
            DataPointState dpState(dpValue, now, TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
            dpState.setValue(dpValue);
            systemDP->updateFieldState(dpState);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Set DataPoint System alarm %ld,status:%d", systemDP->getEntityKey(), systemSummaryAlarm);
        }
        FUNCTION_EXIT;
    }


	void COECommunicationsHandler::processAlarmStack(TA_COE::AlarmStack alarmStack)
	{
		TA_COE::AlarmStack alarmsToOpen, alarmsToClose;
		//this is the only place that m_alarmStack is altered so we know its already sorted
		//so we only need to sort the new stack
		std::set_difference(m_alarmStack.begin(), m_alarmStack.end(),
			alarmStack.begin(), alarmStack.end(),
			std::insert_iterator<TA_COE::AlarmStack>
			(alarmsToClose, alarmsToClose.begin()));
		if(alarmsToClose.size() != 0)
		{
			try
			{
				closeTriggeringEventAlarms(alarmsToClose);
				if(m_alarmCounter < 10)
					infoAlarmChanged(alarmStack);
			}
			catch(...)
			{
			}
		}

		std::set_difference(alarmStack.begin(), alarmStack.end(),
			m_alarmStack.begin(), m_alarmStack.end(),
			std::insert_iterator<TA_COE::AlarmStack>
			(alarmsToOpen, alarmsToOpen.begin()));
		if(alarmsToOpen.size() != 0)
		{
			try
			{
				openTriggeringEventAlarms(alarmsToOpen);
				if(m_alarmCounter < 10)
					infoAlarmChanged(alarmStack);
			}
			catch(...)
			{
			}
		}
		m_alarmStack = alarmStack;
		if (m_alarmCounter >= 10)
		{
			infoAlarmChanged(m_alarmStack);
			m_alarmCounter = 0;
		}
		else
			m_alarmCounter++;
		StateUpdateBroadcaster::getInstance().notifyAlarmStackUpdate();
	}


    ///////////////////////////////////////////////////////////////////////
    //
    // IPDCommunicationsHandler Methods to be overridden
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // connect
    //
    void COECommunicationsHandler::connect()
    {
        FUNCTION_ENTRY("connect");
        TA_Base_Core::ThreadGuard threadStateGuard(m_threadStateLock);
        activateCommunicationsChannels();
        startProcessors();
        // Activate State Validation Thread.
        COESwitchManager::getInstance().getStateValidationCycle().startValidationCycle();
        FUNCTION_EXIT;
    }


    //
    // disconnect
    //
    void COECommunicationsHandler::disconnect()
    {
        FUNCTION_ENTRY("disconnect");
        //
        // IMPORTANT - Must deactivate State Validation Thread before we obtain
        // m_threadStateLock. Otherwise deadlock may occur because the validation
        // thread acquires this lock during its run method (via calling
        // submitRequestCommand).
        //
        COESwitchManager::getInstance().getStateValidationCycle().stopValidationCycle();
        TA_Base_Core::ThreadGuard threadStateGuard(m_threadStateLock);
        terminateProcessors();
        deactivateCommunicationsChannels();
        // Empty anything left in the queues as these may never be processed now,
        clearCommandQueues();
        FUNCTION_EXIT;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Thread Methods
    //
    ///////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////////////////////////////
    //
    // Private Methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // loadCommunicationChannels
    //
    void COECommunicationsHandler::loadCommunicationsChannels()
    {
        FUNCTION_ENTRY("loadCommunicationsChannels");
        // Get the addresses of all the VideoSwitches and create CommsChannel for each.  This may throw database related exceptions which are documented
        // in the method comment.
        CommunicationsChannelInfo defaultChannelInfo;
        defaultChannelInfo.state = NOT_CURRENTLY_ESTABLISHED;
        defaultChannelInfo.numberCyclesWithoutActiveChannel = 0;
        VideoSwitchData::iterator it = m_videoSwitchData.begin();
        for(; it != m_videoSwitchData.end(); it++)
        {
            // obtain the asset name
            std::auto_ptr<TA_Base_Core::IData> entityParam = std::auto_ptr<TA_Base_Core::IData>(TA_Base_Core::EntityAccessFactory::getInstance().getParameter((*it)->getName(), "AssetName"));
            std::string assetName("");
            if(entityParam.get() != 0 && entityParam->getNumRows() > 0 && entityParam->isNull(0, "VALUE") == false)
            {
                assetName = entityParam->getStringData(0, "VALUE");
            }
            AddressHelper::Address switchAddress = AddressHelper::splitAddress((*it)->getAddress());
            m_communicationsChannels.push_back(new COECommunicationsChannel((it - m_videoSwitchData.begin()),
                                                                            switchAddress.host, switchAddress.port, m_switchConnectTimeout, m_maxSwitchResponseTime, m_switchPollInterval, m_maxSummaryPollingResponseTime, m_summaryPollingInterval, *this, *m_receivedDataProcessor.get(), (*it)->getName(), (*it)->getKey(), (* it)->getLocation(), (*it)->getTypeKey(), (*it)->getSubsystem(), assetName));
            m_communicationsChannelsInfo.push_back(defaultChannelInfo);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "COECommunications channel created for VideoSwitch at '%s:%s'", switchAddress.host.c_str(), switchAddress.port.c_str());
        }
        m_activeSwitchChannelIndex = m_communicationsChannels.size(); // invalid index indicates no current active switch channel
        TA_ASSERT(m_communicationsChannelsInfo.size() == m_communicationsChannels.size(),
                  "The number of CommunicationsChannels and CommunicationsChannelsInfo structs is not equal!");
        FUNCTION_EXIT;
    }


    //
    // activateCommunicationsChannels
    //
    void COECommunicationsHandler::activateCommunicationsChannels()
    {
        FUNCTION_ENTRY("activateCommunicationsChannels");
        CommunicationsChannels::iterator commsChannelIt = m_communicationsChannels.begin();
        for(; commsChannelIt != m_communicationsChannels.end(); commsChannelIt++)
        {
            (*commsChannelIt)->activateChannel();
        }
        FUNCTION_EXIT;
    }


    //
    // deactivateCommuncationsChannels
    //
    void COECommunicationsHandler::deactivateCommunicationsChannels()
    {
        FUNCTION_ENTRY("deactivateCommuncationsChannels");
        TA_ASSERT(m_communicationsChannelsInfo.size() == m_communicationsChannels.size(),
                  "The number of CommunicationsChannels and CommunicationsChannelsInfo structs is not equal!");
        CommunicationsChannels::iterator commsChannelIt = m_communicationsChannels.begin();
        CommunicationsChannelsInfo::iterator commsChannelInfoIt = m_communicationsChannelsInfo.begin();
        for(; commsChannelIt != m_communicationsChannels.end(); commsChannelIt++)
        {
            (*commsChannelIt)->deactivateChannel();
            //
            // Important to only acquire lock while channel not in process of deactivating
            // else potential for deadlock.
            //
            TA_Base_Core::ThreadGuard commsChannelsStateGuard(m_commsChannelsStateLock);
            (*commsChannelInfoIt).state = NOT_CURRENTLY_ESTABLISHED;
            (*commsChannelInfoIt).numberCyclesWithoutActiveChannel = 0;
            commsChannelInfoIt++;
        }
        m_activeSwitchChannelIndex = m_communicationsChannels.size(); // invalid index indicates no current active switch channel
        m_exceededNoActiveSwitchCycleAllowance = false;
        FUNCTION_EXIT;
    }


    //
    // cleanUp
    //
    void COECommunicationsHandler::cleanUp()
    {
        FUNCTION_ENTRY("cleanUpVideoSwitchData");
        for(VideoSwitchData::iterator it = m_videoSwitchData.begin(); it != m_videoSwitchData.end(); ++it)
        {
            delete(*it);
        }
        m_videoSwitchData.clear();
        for(CommunicationsChannels::iterator it = m_communicationsChannels.begin(); it != m_communicationsChannels.end(); ++it)
        {
            delete(*it);
        }
        m_communicationsChannels.clear();
        m_communicationsChannelsInfo.clear();
        m_alarmToWallMonitorKeyMap.clear();
        m_videoMonitorEntities.clear();
		m_alarmToEntityAndVideoTriggeringEvent.clear();
		m_entityKeyToCCTVAlarm.clear();
		for(TA_IRS_Core::IVideoTriggeringEvents::iterator it = m_videoTriggeringEvents.begin(); it != m_videoTriggeringEvents.end(); ++it)
		{
			delete *it;
		}
		for(TA_Base_Core::IEntityDataList::iterator it = m_cctvAlarmEntities.begin(); it != m_cctvAlarmEntities.end(); ++it)
		{
			delete *it;
		}
        FUNCTION_EXIT;
    }


    //
    // processRequestCommand
    //
    void COECommunicationsHandler::processRequestCommand(RequestCommandSharedPtr command)
    {
        FUNCTION_ENTRY("processRequestCommand");
        //
        // Important that this thread of execution should not acquire the m_commsChannelsStateLock
        // while calling "sendCommand" on the channel. Otherwise deadlock can occur because the
        // channel's thread may be trying to acquire thwe same lock when processing it's polling
        // commands. This is why this method uses "isThereActiveSwitchChannel" to determine this
        // information.
        //
        unsigned int activeChannelIndex(0);
        if(isThereActiveSwitchChannel(activeChannelIndex))
        {
            TA_ASSERT(0 != command.get(), "Request command shared ptr is null!");
            m_communicationsChannels[ m_activeSwitchChannelIndex ]->
            sendCommand(*command.get());
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                        "Request command could not be sent as no connection to active switch!");
            command->signalCouldNotBeSent();
        }
        FUNCTION_EXIT;
    }


    void COECommunicationsHandler::processNotification(NotificationCommandSharedPtr command)
    {
        FUNCTION_ENTRY("processNotification");
        try
        {
            command->processNotification();
        }
        catch(...)
        {
        }
        FUNCTION_EXIT;
    }
    //
    // receivedChannelStateCommsFailure
    //
    void COECommunicationsHandler::receivedChannelStateCommsFailure(unsigned int channelIndex)
    {
        if(m_communicationsChannelsInfo[ channelIndex ].state != COMMUNICATIONS_FAILURE)
        {
            // TODO Raise Alarm
        }
    }


    //
    // receivedChannelStateNotCommsFailure
    //
    void COECommunicationsHandler::receivedChannelStateNotCommsFailure(unsigned int channelIndex)
    {
        if((m_communicationsChannelsInfo[ channelIndex ].state == COMMUNICATIONS_FAILURE)
                || (m_communicationsChannelsInfo[ channelIndex ].state == NOT_CURRENTLY_ESTABLISHED))
        {
            // TODO Ensure Alarm Closed
        }
    }


    //
    // receivedChannelStateActive
    //
    void COECommunicationsHandler::receivedChannelStateActive(unsigned int channelIndex)
    {
        if(m_activeSwitchChannelIndex != channelIndex)
        {
            // TODO Ensure No Active Switch Alarm Closed
			setDataPointsAsGoodValue();

            m_activeSwitchChannelIndex = channelIndex;
            CommunicationsChannelsInfo::iterator it = m_communicationsChannelsInfo.begin();
            for(; it != m_communicationsChannelsInfo.end(); it++)
            {
                (*it).numberCyclesWithoutActiveChannel = 0;
            }
            m_exceededNoActiveSwitchCycleAllowance = false;
        }
    }


    //
    // receivedChannelStateNotActive
    //
    void COECommunicationsHandler::receivedChannelStateNotActive(unsigned int channelIndex)
    {
        if(m_activeSwitchChannelIndex == channelIndex)
        {
            // As no longer connected to active switch, ensure DataPoints reflect bad quality
            // due to comms failure.
            setDataPointsAsBadValue();
            m_activeSwitchChannelIndex = m_communicationsChannels.size();
        }
        if((m_activeSwitchChannelIndex == m_communicationsChannels.size())
                && (false == m_exceededNoActiveSwitchCycleAllowance))
        {
            m_communicationsChannelsInfo[ channelIndex ].numberCyclesWithoutActiveChannel++;
            CommunicationsChannelsInfo::iterator it = m_communicationsChannelsInfo.begin();
            unsigned short minNumberCyclesWithoutActiveChannel = (*it).numberCyclesWithoutActiveChannel;
            for(; it != m_communicationsChannelsInfo.end(); it++)
            {
                if((*it).numberCyclesWithoutActiveChannel < minNumberCyclesWithoutActiveChannel)
                {
                    minNumberCyclesWithoutActiveChannel = (*it).numberCyclesWithoutActiveChannel;
                }
            }
            if(minNumberCyclesWithoutActiveChannel > NO_ACTIVE_SWITCH_CYCLE_ALLOWANCE)
            {
                // As could not connect to active switch, ensure DataPoints reflect bad quality
                // due to comms failure.
                setDataPointsAsBadValue();
                // TODO Raise No Active Switch Alarm
                m_exceededNoActiveSwitchCycleAllowance = true;
            }
        }
    }


    //
    // setDataPointsAsBadValue
    //
    void COECommunicationsHandler::setDataPointsAsBadValue()
    {
        DataPoint* systemDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::SYSTEM_DATAPOINT_ID);
        // DataPoint* transmissionDP = COESwitchManager::getInstance().getDataPointFromId( TRANSMISSION_DATAPOINT_ID );
        DataPoint* cameraDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::CAMERA_DATAPOINT_ID);
        DataPoint* ancillaryDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::ANCILLARY_DATAPOINT_ID);
        //TD19444
        DataPoint* dvrDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::DVR_DATAPOINT_ID);
        time_t now = time(0);
        if(0 != systemDP)
        {
            DpValue dpValue(systemDP->getDataPointDataType(), systemDP->getEnumLabelsMap(), systemDP->getBooleanLabels());
            DataPointState badState(dpValue, time(0), TA_Base_Bus::QUALITY_BAD_COMM_FAILURE);
            systemDP->updateFieldState(badState);
        }
        /* if ( 0 != transmissionDP )
         {
             DpValue dpValue( transmissionDP->getDataPointDataType(), transmissionDP->getEnumLabelsMap(), transmissionDP->getBooleanLabels() );
             DataPointState badState( dpValue, time( 0 ), TA_Base_Bus::QUALITY_BAD_COMM_FAILURE );
             transmissionDP->updateFieldState( badState );
         }*/
        if(0 != cameraDP)
        {
            DpValue dpValue(cameraDP->getDataPointDataType(), cameraDP->getEnumLabelsMap(), cameraDP->getBooleanLabels());
            DataPointState badState(dpValue, time(0), TA_Base_Bus::QUALITY_BAD_COMM_FAILURE);
            cameraDP->updateFieldState(badState);
        }
        if(0 != ancillaryDP)
        {
            DpValue dpValue(ancillaryDP->getDataPointDataType(), ancillaryDP->getEnumLabelsMap(), ancillaryDP->getBooleanLabels());
            DataPointState badState(dpValue, time(0), TA_Base_Bus::QUALITY_BAD_COMM_FAILURE);
            ancillaryDP->updateFieldState(badState);
        }
        if(0 != dvrDP)
        {
            DpValue dpValue(dvrDP->getDataPointDataType(), dvrDP->getEnumLabelsMap(), dvrDP->getBooleanLabels());
            DataPointState badState(dpValue, time(0), TA_Base_Bus::QUALITY_BAD_COMM_FAILURE);
            dvrDP->updateFieldState(badState);
        }
        //TD19444
    }


    void COECommunicationsHandler::setDataPointsAsGoodValue()
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Set datapoints as good quality");
        DataPoint* systemDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::SYSTEM_DATAPOINT_ID);
        DataPoint* cameraDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::CAMERA_DATAPOINT_ID);
        DataPoint* ancillaryDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::ANCILLARY_DATAPOINT_ID);
        DataPoint* dvrDP = COESwitchManager::getInstance().getDataPointFromId(COESwitchManager::DVR_DATAPOINT_ID);
        time_t now = time(0);
        if(0 != systemDP)
        {
            DpValue dpValue(systemDP->getDataPointDataType(), systemDP->getEnumLabelsMap(), systemDP->getBooleanLabels());
            DataPointState state(dpValue, time(0), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
            systemDP->updateFieldState(state);
        }
        if(0 != cameraDP)
        {
            DpValue dpValue(cameraDP->getDataPointDataType(), cameraDP->getEnumLabelsMap(), cameraDP->getBooleanLabels());
            DataPointState state(dpValue, time(0), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
            cameraDP->updateFieldState(state);
        }
        if(0 != ancillaryDP)
        {
            DpValue dpValue(ancillaryDP->getDataPointDataType(), ancillaryDP->getEnumLabelsMap(), ancillaryDP->getBooleanLabels());
            DataPointState state(dpValue, time(0), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
            ancillaryDP->updateFieldState(state);
        }
        if(0 != dvrDP)
        {
            DpValue dpValue(dvrDP->getDataPointDataType(), dvrDP->getEnumLabelsMap(), dvrDP->getBooleanLabels());
            DataPointState state(dpValue, time(0), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON);
            dvrDP->updateFieldState(state);
        }
	}
    //
    // isConnected
    //
    bool COECommunicationsHandler::isConnected()
    {
        unsigned int index = 0;
        return isThereActiveSwitchChannel(index);
    }


    //
    // isThereActiveSwitchChannel
    //
    bool COECommunicationsHandler::isThereActiveSwitchChannel(unsigned int& activeChannelIndex)
    {
        FUNCTION_ENTRY("isThereActiveSwitchChannel");
        TA_Base_Core::ThreadGuard commsChannelsStateGuard(m_commsChannelsStateLock);
        activeChannelIndex = m_activeSwitchChannelIndex;
        FUNCTION_EXIT;
        return (m_activeSwitchChannelIndex < m_communicationsChannels.size());
    }


    //
    // commandNotToBeProcessed
    //
    void  COECommunicationsHandler::commandNotToBeProcessed(RequestCommandSharedPtr command)
    {
        FUNCTION_ENTRY("commandNotProcessed");
        //
        // Log the fact the request command was never processed by CommsHandler (as disconnected)
        // and signal that it could not be sent so the process waiting for completion will wake
        // up. This should not happen as all servant bases are deactivated before the comms handler
        // is disconnected, so log as warning.
        //
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                    "Request command could not be processed as COECommunications handler was disconnected.");
        command->signalCouldNotBeSent();
        FUNCTION_EXIT;
    }


    //
    // commandNotToBeProcessed
    //
    void COECommunicationsHandler::commandNotToBeProcessed(NotificationCommandSharedPtr command)
    {
        FUNCTION_ENTRY("commandNotProcessed");
        //
        // A notification command may be received by one of our channels during
        // the process of disconnecting. Can't do anything but this logs the
        // fact the command was received but never processed. This is not an error.
        //
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Notification command could not be processed as COECommunications handler was disconnected.");
        FUNCTION_EXIT;
    }


    //
    // clearCommandQueues
    //
    void COECommunicationsHandler::clearCommandQueues()
    {
        FUNCTION_ENTRY("clearCommandQueues");
        while(false == m_notificationCommandQueue.empty())
        {
            commandNotToBeProcessed(m_notificationCommandQueue.front());
            m_notificationCommandQueue.pop();
        }
        while(false == m_clientRequestCommandQueue.empty())
        {
            commandNotToBeProcessed(m_clientRequestCommandQueue.front());
            m_clientRequestCommandQueue.pop();
        }
        while(false == m_internalRequestCommandQueue.empty())
        {
            commandNotToBeProcessed(m_internalRequestCommandQueue.front());
            m_internalRequestCommandQueue.pop();
        }
        FUNCTION_EXIT;
    }

    void COECommunicationsHandler::openTriggeringEventAlarms(const TA_COE::AlarmStack& alarmsToBeOpened)
    {
        for(TA_COE::AlarmStack::const_iterator it = alarmsToBeOpened.begin(); it != alarmsToBeOpened.end(); ++it)
        {
            if((*it).eventId == 0) continue;
			std::map<TA_COE::AlarmState, AlarmEntityVideoTriggeringEventPair>::iterator it2 = m_alarmToEntityAndVideoTriggeringEvent.find(*it);
            if(it2 == m_alarmToEntityAndVideoTriggeringEvent.end())
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find the entity to raise the alarms. eventId %d", (*it).eventId);
                continue;
            }
            AlarmEntityVideoTriggeringEventPair& pair = (*it2).second;
            TA_Base_Core::IEntityData* entityData = pair.first;
			TA_IRS_Core::IVideoTriggeringEvent* triggeringEventData = pair.second;
			// submit alarm
			TA_Base_Core::EntityHelper helper(entityData->getKey(), entityData->getType());
            TA_Base_Core::DescriptionParameters dp;
            TA_Base_Core::NameValuePair eventDescription("Event", triggeringEventData->getCctvTriggering());
            TA_Base_Core::NameValuePair omFlag("OMFlag", helper.getParameter("OMFlag"));
            dp.push_back(&eventDescription);
            dp.push_back(&omFlag);
            timeb timestamp = TA_Base_Core::AlarmConstants::defaultTime;
			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(
                             TA_Base_Core::VisualAlarms::CctvTriggeringAlarm,
                             entityData->getKey(),
                             entityData->getTypeKey(),
                             dp,
                             entityData->getName(),
                             entityData->getLocation(),
                             entityData->getSubsystem(),
                             helper.getParameter("AssetName"),
                             timestamp);
            // add to open alarms cache
            AlarmID alarm_id;
			alarm_id.alarmType = TA_Base_Core::VisualAlarms::CctvTriggeringAlarm.getTypeKey();
            alarm_id.entityKey = entityData->getKey();
            alarm_id.alarmID   = "";
            alarm_id.alarmLocationKey = entityData->getLocation();
			// log and info alarm changed
            std::ostringstream data;
            data << "[Add to OpenAlarms] "
                 << " EventID = " << (*it).eventId
                 << " StationID = " << (*it).stationId
                 << " Alarm Type = " << alarm_id.alarmType
                 << " Entity Key = " << alarm_id.entityKey
                 << " Alarm Location = " << alarm_id.alarmLocationKey;
            m_openAlarms[*it] = alarm_id;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, data.str().c_str());
        }
    }

    void COECommunicationsHandler::closeTriggeringEventAlarms(const TA_COE::AlarmStack& alarmsToBeClosed)
    {
        TA_COE::AlarmStack::const_iterator closeIt;
        for(closeIt = alarmsToBeClosed.begin(); closeIt != alarmsToBeClosed.end(); ++closeIt)
        {
            TA_Base_Core::ThreadGuard guard(m_openAlarmsLock);
            std::map<TA_COE::AlarmState, AlarmID>::iterator openIt= m_openAlarms.find(*closeIt);
            if(openIt != m_openAlarms.end())
            {
				const AlarmID& alarmId = (*openIt).second;
	            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Close alarm %lu at location %lu", alarmId.entityKey, alarmId.alarmLocationKey);
                TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(
					TA_Base_Core::VisualAlarms::CctvTriggeringAlarm,
					alarmId.entityKey,
					alarmId.alarmLocationKey);
				const TA_COE::AlarmState& alarmState = (*openIt).first;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Remove from OpenAlarms cache EventID = %lu StationID =%lu", alarmState.eventId, alarmState.stationId);
                m_openAlarms.erase(openIt);
            }
        }
    }

	void COECommunicationsHandler::infoAlarmChanged(const TA_COE::AlarmStack& alarmStack)
    {
        try
        {
            std::string locationName = m_videoSwitchAgentData->getLocationName();
            if(locationName == "OCC" || locationName == "Occ")
            {
				std::map<unsigned long,TA_COE::AlarmStack> inforMonitorAlarmStackMap;
				for(std::set<unsigned long>::iterator it  = m_occAlarmMonitorSet.begin(); it != m_occAlarmMonitorSet.end(); ++it)
				{
					inforMonitorAlarmStackMap.insert(std::make_pair(*it, TA_COE::AlarmStack()));
				}
				for(TA_COE::AlarmStack::const_iterator it = alarmStack.begin(); it != alarmStack.end(); ++it)
				{
					if(m_stationIdAlarmMonitorKeyMap.find(it->stationId) != m_stationIdAlarmMonitorKeyMap.end())
					{
						inforMonitorAlarmStackMap[m_stationIdAlarmMonitorKeyMap[it->stationId]].insert(*it);
					}
					else
					{
			            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not find monitor to show alarm[stationId = %lu]", it->stationId);
					}
				}
				for(std::map<unsigned long,TA_COE::AlarmStack>::iterator it = inforMonitorAlarmStackMap.begin(); it != inforMonitorAlarmStackMap.end(); ++it)
				{
					COEVideoOutput* monitor = COESwitchManager::getInstance().getVideoOutput(it->first);
					TA_ASSERT(NULL != monitor, "Alarm monitor configured error");
					monitor->notifyOutputAlarm(it->second);
				}
            }
            else
            {
                for(std::set<unsigned long>::iterator it = m_stationAlarmMonitorSet.begin(); it != m_stationAlarmMonitorSet.end(); ++it)
                {
					COEVideoOutput* monitor = COESwitchManager::getInstance().getVideoOutput(*it);
					TA_ASSERT(NULL != monitor, "Alarm monitor configured error");
					monitor->notifyOutputAlarm(alarmStack);
                }
            }
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                                  "Failed to notify the agent's clients that a switch of input has occurred.");
            throw TA_Base_Bus::VideoSwitchAgentException("DatabaseException:Failed to notify the agent's clients that a switch of input has occurred.");
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                                  "Failed to notify the agent's clients that a switch of input has occurred.");
            throw TA_Base_Bus::VideoSwitchAgentException("DataException:Failed to notify the agent's clients that a switch of input has occurred.");
        }
    }

    void COECommunicationsHandler::acknowledgeAlarm( unsigned long entityKey, unsigned long consoleId)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Console to Monitor = %lu ", consoleId);
        std::map<unsigned long, MonitorKeyIdPair>::iterator it = m_consoleKeyToMonitorKeyId.find(consoleId);
        if(it != m_consoleKeyToMonitorKeyId.end())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MonitorID = %lu ", (*it).second.second);
            TA_COE::AlarmStack stack = getOpenAlarmsByAlarmEntityKey(entityKey);
            for(TA_COE::AlarmStack::iterator it = stack.begin(); it != stack.end(); ++it)
            {
                if(false == verifyIsOpenAlarm(*it)) continue;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Sending TriggeringEventAckCommand...");
                acknowledgeAlarm((*it).stationId , (*it).eventId , consoleId);
            }
        }
        else
        {
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                                                                     "Console does not exsist"));
        }
	}

    void COECommunicationsHandler::acknowledgeAlarm(unsigned long stationId, unsigned long eventId, unsigned long consoleId)
    {
        //acknowledging alarm on a switch is only valid at the occ
        //where the default alarm monitors are on the wall not on a console
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "acknowledgeAlarm(%lu,%lu, %lu)", stationId, eventId, consoleId);
        std::string location = m_videoSwitchAgentData->getLocationName();
        unsigned long monitorKey = 0;
        unsigned long monitorID = 0;
        if(!locationIsStation(location))
        {
            std::map<unsigned long, MonitorKeyIdPair>::iterator it = m_consoleKeyToMonitorKeyId.find(consoleId);
            if(it != m_consoleKeyToMonitorKeyId.end())
            {
                MonitorKeyIdPair& pair = (*it).second;
                monitorKey = pair.first;
                monitorID = pair.second;
            }
        }
        else
        {
            for(TA_Base_Core::IEntityDataList::iterator it = m_videoMonitorEntities.begin(); it != m_videoMonitorEntities.end(); ++it)
            {
                TA_Base_Core::VideoMonitor* monitor = dynamic_cast<TA_Base_Core::VideoMonitor*>(*it);
                if(monitor->getAlarmType() == VM_ALARM_TYPE_NONE)
                {
                    monitorKey = monitor->getKey();
                    monitorID = atol(monitor->getAddress().c_str());
                    break;
                }
            }
        }
        if(monitorKey > 0 && monitorID > 0)
        {
            try
            {
                TA_COE::AlarmState state;
                state.stationId = stationId;
                state.eventId = eventId;
                RequestCommandSharedPtr command = COECommunicationsHandler::RequestCommandSharedPtr(new TriggeringEventAckCommand(state, monitorID));
                submitRequestCommand(command, false);
                command->waitForCompletion();
                if(IAgentRequestCommand::SUCCEEDED != command->getCompletionState())
                {
                    std::string error;
                    if(IAgentRequestCommand::COULD_NOT_BE_SENT == command->getCompletionState())
                    {
                        error = "Unable to establish Connection with the VSS System";
                    }
                    else
                    {
                        error = "Error encounter when performing the requested operation";
                    }
                    throw TA_Base_Bus::VideoSwitchAgentException(error.c_str());
                }
				COEVideoOutput* monitor = COESwitchManager::getInstance().getVideoOutput(monitorKey);
				TA_ASSERT(NULL != monitor, "Monitor not config correct");
				monitor->notifyShowingAckAlarm(eventId, stationId, true);
            }
            catch(const TA_Base_Core::DatabaseException& e)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
                LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                                      "Failed to notify the agent's clients that a switch of input has occurred.");
                throw TA_Base_Bus::VideoSwitchAgentException("DatabaseException:Failed to notify the agent's clients that a switch of input has occurred.");
            }
            catch(const TA_Base_Core::DataException& e)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
                LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                                      "Failed to notify the agent's clients that a switch of input has occurred.");
                throw TA_Base_Bus::VideoSwitchAgentException("DataException:Failed to notify the agent's clients that a switch of input has occurred.");
            }
        }
        else
        {
            TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                                                                     "Correct triggering alarm monitor does not exsist"));
        }
    }

    TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack COECommunicationsHandler::getAlarmStack()
    {
        TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack stack;
        TA_Base_Core::ThreadGuard threadStateGuard(m_openAlarmsLock);
        stack.length(m_openAlarms.size());
        unsigned int index(0);
        for(std::map<TA_COE::AlarmState, AlarmID>::iterator it = m_openAlarms.begin(); it != m_openAlarms.end(); it++)
        {
            TA_Base_Bus::SwitchManagerCorbaDef::AlarmState state;
            state.eventId = (*it).first.eventId;
            state.stationId = (*it).first.stationId;
            state.alarmType = CORBA::string_dup((*it).second.alarmType.c_str());
            state.eventId   = (*it).second.entityKey;
            state.alarmID   = CORBA::string_dup((*it).second.alarmID.c_str()); // TD16164
            state.alarmLocationKey = (*it).second.alarmLocationKey;
            stack[index++] = state;
        }
        return stack;
    }

    void COECommunicationsHandler::updateAlarmStack(TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack& stack)
    {
        TA_Base_Core::ThreadGuard threadStateGuard(m_openAlarmsLock);
        m_alarmStack.clear();
        m_openAlarms.clear();
        std::string strInfo = "updateAlarmStack:";
        char buf[1024] = {0};
        for(unsigned int i = 0; i < stack.length(); i++)
        {
            TA_COE::AlarmState state;
            state.eventId = stack[i].eventId;
            state.stationId = stack[i].stationId;
            m_alarmStack.insert(state);
            AlarmID alarm_id;
            alarm_id.alarmType = stack[i].alarmType;
            alarm_id.entityKey = stack[i].entityKey;
            alarm_id.alarmID = stack[i].alarmID.in();
            alarm_id.alarmLocationKey = stack[i].alarmLocationKey;
            m_openAlarms[state] = alarm_id;
            sprintf(buf, "(%d-eventId:%ld,stationId:%ld)", i, stack[i].eventId , stack[i].stationId);
            strInfo.append(buf);
        }
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s ", strInfo.c_str());;
    }

    unsigned long COECommunicationsHandler::getEntityKey()
    {
        return m_videoSwitchAgentData->getKey();
    }

    bool COECommunicationsHandler::canbeManuallyClosed(const TA_COE::AlarmState& alarmState)
    {
        std::map<TA_COE::AlarmState, AlarmEntityVideoTriggeringEventPair>::iterator it = m_alarmToEntityAndVideoTriggeringEvent.find(alarmState);
        if(it != m_alarmToEntityAndVideoTriggeringEvent.end())
        {
            AlarmEntityVideoTriggeringEventPair& pair = (*it).second;
            return pair.second->getManualClosing();
        }
        return false;
    }

    void COECommunicationsHandler::startProcessors()
    {
        FUNCTION_ENTRY("startProcessors");
        m_notificationProcessor->start();
        m_clientRequestProcessor->start();
        m_internalRequestProcessor->start();
        FUNCTION_EXIT;
    }

    void COECommunicationsHandler::terminateProcessors()
    {
        FUNCTION_ENTRY("terminateProcessors");
        m_notificationProcessor->terminateAndWait();
        m_clientRequestProcessor->terminateAndWait();
        m_internalRequestProcessor->terminateAndWait();
        FUNCTION_EXIT;
    }

	TA_COE::AlarmStack COECommunicationsHandler::getOpenAlarmsByAlarmEntityKey(const unsigned long alarmEntityKey)
	{
        FUNCTION_ENTRY("getOpenAlarmsByAlarmEntityKey");
        TA_COE::AlarmStack stack;
        TA_Base_Core::ThreadGuard threadStateGuard(m_openAlarmsLock);
        for(std::map<TA_COE::AlarmState, AlarmID>::iterator it = m_openAlarms.begin(); it != m_openAlarms.end(); ++it)
        {
			if(alarmEntityKey == (*it).second.entityKey)
			{
				stack.insert((*it).first);
				break;
			}
        }
        FUNCTION_EXIT;
        return stack;
	}

    bool COECommunicationsHandler::verifyIsOpenAlarm(const TA_COE::AlarmState& alarm)
    {
        TA_Base_Core::ThreadGuard guard(m_openAlarmsLock);
        return m_openAlarms.find(alarm) != m_openAlarms.end();
    }

	void COECommunicationsHandler::setToControl()
	{
		// Close all opened alarm
		std::stringstream ss(TA_Base_Core::VisualAlarms::CctvTriggeringAlarm.getTypeKey());
		unsigned long cctvTriggeringAlarmTypekey;
		ss >> cctvTriggeringAlarmTypekey;
		TA_Base_Core::AlarmHelper& alarmHelper = TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper();
		try
		{
			TA_Base_Core::AlarmSequence_var locationAllActiveAlarmSeq = alarmHelper.getActiveAlarms(m_videoSwitchAgentData->getLocation());
			for(unsigned long i = 0, sz = locationAllActiveAlarmSeq->length(); i < sz; ++i)
			{
				TA_Base_Core::AlarmDetailCorbaDef& alarmDetail = locationAllActiveAlarmSeq[i];
				if(alarmDetail.messageTypeKey == cctvTriggeringAlarmTypekey)
				{
					alarmHelper.closeAlarmAtLocation(TA_Base_Core::VisualAlarms::CctvTriggeringAlarm, alarmDetail.assocEntityKey, m_videoSwitchAgentData->getLocation());
				}
			}
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed Get cctv triggering alarm at location:%lu", m_videoSwitchAgentData->getLocation());
		}

        m_alarmToWallMonitorKeyMap.clear();
        m_alarmStack.clear();
	}

} // TA_IRS_App
