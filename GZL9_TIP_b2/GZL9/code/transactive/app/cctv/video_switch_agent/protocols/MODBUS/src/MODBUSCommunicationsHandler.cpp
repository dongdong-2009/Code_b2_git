/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSCommunicationsHandler.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #7 $
  *
  * Last modification: $DateTime: 2013/08/30 15:38:55 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the operations for a MODBUSCommunicationsHandler
  * using the MODBUS protocol.
  * This interface is used by the MODBUSCommunicationsHandler class. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 4503)
#endif // _MSC_VER

#define NOT_READY_FOR_DATAPOINT

//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/PTZCommand.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSCommunicationsHandler.h"
#include "app/cctv/video_switch_agent/src/AddressHelper.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/ConnectionChecker.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/scada/ModbusHelper/src/ModbusHelperFactory.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
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
// TD 15200
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/CommandPackage.h"
#include <algorithm>
#include <iostream>

namespace
{
WORD makeWord(BYTE high, BYTE low)
{
    return ((WORD)high << 8) | low;
}
}

namespace TA_IRS_App
{
//
// MODBUSCommunicationsHandler
//
MODBUSCommunicationsHandler::MODBUSCommunicationsHandler(TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData)
        : m_videoSwitchAgentData(videoSwitchAgentData)
{
    try
    {
        m_locationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(videoSwitchAgentData->getLocationName());

        loadCommunicationsChannels();
        SwitchManager::getInstance().setAlarmInterface(this);

        SwitchManager::getInstance().setStageModeInterface(this);

        // init MODBUS communication

        // boost smart pointer
        m_modbusHelper = TA_Base_Bus::ModbusHelperFactory::createHelper(videoSwitchAgentData->getLocation(), TA_Base_Bus::ModbusHelperFactory::CCTV);
    }
    catch (TA_Base_Bus::NotFoundFEPConfigException& expConfigError)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Bus::NotFoundFEPConfigException", expConfigError.what());
        cleanUp();

        TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR,
                                                                 "Failed to retrieve FEP configuration from the database."));
    }
    catch (const TA_Base_Core::DatabaseException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", e.what());
        LOG_EXCEPTION_DETAILS(SourceInfo, "DatabaseException",
                              "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database.");

        cleanUp();

        TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR,
                                                                 "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database."));
    }
    catch (const TA_Base_Core::DataException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", e.what());
        LOG_EXCEPTION_DETAILS(SourceInfo, "DataException",
                              "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database.");

        cleanUp();

        TA_THROW(TA_Base_Core::VideoSwitchAgentInternalException(TA_Base_Core::VideoSwitchAgentInternalException::CONFIGURATION_ERROR,
                                                                 "Failed to retrieve mandatory VideoSwitchAgent parameters, or addresses of any configured VideoSwitch entities, from the database."));
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "MODBUSComunicationHandler initialization.");
        LOG_EXCEPTION_DETAILS(SourceInfo, "UnknownException",
                              "Unknown reason");
        cleanUp();
    }
}


//
// ~MODBUSCommunicationsHandler
//
MODBUSCommunicationsHandler::~MODBUSCommunicationsHandler()
{
    FUNCTION_ENTRY("Destructor");

    // If an exception is thrown here, there is nothing we can do,
    // but we should make sure no exceptions are thrown out of this destructor.

    try
    {
        // Clean up this thread if still running, and any queued commands.
        disconnect();

        // Clean up heap objects.
        cleanUp();

        // As this is last MODBUS Protocol object to destruct, here we manually destruct
        // the MODBUSSwitvhManager singleton also.
        MODBUSSwitchManager::getInstance().removeInstance();
    }
    catch (const TA_Base_Core::TransactiveException& e)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Reason unknown");
    }

    FUNCTION_EXIT;
}


///
// submitRequestCommand
//
void MODBUSCommunicationsHandler::submitRequestCommand(RequestCommandSharedPtr command, bool isClientRequest)
{
}


//
// submitNotificationCommand
//
void MODBUSCommunicationsHandler::submitNotificationCommand(NotificationCommandSharedPtr command)
{
}


void MODBUSCommunicationsHandler::processSummaryAlarmsState(bool systemSummaryAlarm, bool transmissionSummaryAlarm, bool cameraSummaryAlarm, bool ancillarySummaryAlarm, bool dvrSummaryAlarm)
{
}

void MODBUSCommunicationsHandler::processAlarmStack(TA_MODBUS::AlarmStack alarmStack)
{
}


///////////////////////////////////////////////////////////////////////
//
// IPDCommunicationsHandler Methods to be overridden
//
///////////////////////////////////////////////////////////////////////


//
// connect
//
void MODBUSCommunicationsHandler::connect()
{
    FUNCTION_ENTRY("connect");
    FUNCTION_EXIT;
}


//
// disconnect
//
void MODBUSCommunicationsHandler::disconnect()
{
    FUNCTION_ENTRY("disconnect");
    FUNCTION_EXIT;
}



///////////////////////////////////////////////////////////////////////
//
// Private Methods
//
///////////////////////////////////////////////////////////////////////


//
// loadCommunicationChannels
//
void MODBUSCommunicationsHandler::loadCommunicationsChannels()
{
}


//
// activateCommunicationsChannels
//
void MODBUSCommunicationsHandler::activateCommunicationsChannels()
{
}


//
// deactivateCommuncationsChannels
//
void MODBUSCommunicationsHandler::deactivateCommunicationsChannels()
{
}


//
// cleanUp
//
void MODBUSCommunicationsHandler::cleanUp()
{
    FUNCTION_ENTRY("cleanUpVideoSwitchData");
    FUNCTION_EXIT;
}


//
// processRequestCommand
//
void MODBUSCommunicationsHandler::processRequestCommand(RequestCommandSharedPtr command)
{
}


//
// receivedChannelStateCommsFailure
//
void MODBUSCommunicationsHandler::receivedChannelStateCommsFailure(unsigned int channelIndex)
{
}


//
// receivedChannelStateNotCommsFailure
//
void MODBUSCommunicationsHandler::receivedChannelStateNotCommsFailure(unsigned int channelIndex)
{
}


//
// receivedChannelStateActive
//
void MODBUSCommunicationsHandler::receivedChannelStateActive(unsigned int channelIndex)
{
}


//
// receivedChannelStateNotActive
//
void MODBUSCommunicationsHandler::receivedChannelStateNotActive(unsigned int channelIndex)
{
}


//
// setDataPointsAsBadValue
//
void MODBUSCommunicationsHandler::setDataPointsAsBadValue()
{
}


//
// commandNotToBeProcessed
//
void  MODBUSCommunicationsHandler::commandNotToBeProcessed(RequestCommandSharedPtr command)
{
}


//
// commandNotToBeProcessed
//
void MODBUSCommunicationsHandler::commandNotToBeProcessed(NotificationCommandSharedPtr command)
{
}


//
// clearCommandQueues
//
void MODBUSCommunicationsHandler::clearCommandQueues()
{
}


void MODBUSCommunicationsHandler::openTriggeringEventAlarms(const TA_MODBUS::AlarmStack& alarmsToBeOpened)
{
}

void MODBUSCommunicationsHandler::closeTriggeringEventAlarms(const TA_MODBUS::AlarmStack& alarmsToBeClosed)
{
}

void MODBUSCommunicationsHandler::acknowledgeAlarm(const std::string& alarmKey,/* unsigned long entityKey,*/ unsigned long consoleId)
{
}

void MODBUSCommunicationsHandler::acknowledgeAlarm(unsigned long stationId, unsigned long eventId, unsigned long consoleId)
{
}

void MODBUSCommunicationsHandler::acknowledgeTrainCameraAlarm(unsigned long locationId, unsigned long trainId, const std::vector<unsigned long>& cameras, unsigned long consoleId)
{
}

void MODBUSCommunicationsHandler::closeAlarm(const std::string& alarmKey/*, unsigned long entityKey*/)
{
}

void MODBUSCommunicationsHandler::closeAlarm(unsigned long stationId, unsigned long eventId)
{
}

TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack MODBUSCommunicationsHandler::getAlarmStack()
{
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack stack;
    return stack;
}

void MODBUSCommunicationsHandler::updateAlarmStack(TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack& stack)
{
}

unsigned long MODBUSCommunicationsHandler::getEntityKey()
{
    return m_videoSwitchAgentData->getKey();
}


bool MODBUSCommunicationsHandler::canbeManuallyClosed(const TA_MODBUS::AlarmState& alarmState)
{
    return false;
}

void MODBUSCommunicationsHandler::setStageSwitchMode(unsigned long originatingStage, unsigned long destinationStage, bool allowSwitch)
{
}

void MODBUSCommunicationsHandler::setControlPrivilege(int privilege)
{
    m_controlPrivilege = static_cast<unsigned char>(privilege);
}

void MODBUSCommunicationsHandler::assignCameraToMonitor(unsigned short monitorID, unsigned char stationID, unsigned char cameraID)
{
    TA_Base_Bus::DataBlock<WORD> data(1, 9);
	data.setAll(0);
    int cmdType = 1;
    data.set(1, makeWord(0, cmdType));
    data.set(3, makeWord(m_controlPrivilege, 0));
    data.set(4, monitorID);
    data.set(5, makeWord(stationID, cameraID));

    m_modbusHelper->SendData(data);
}

void MODBUSCommunicationsHandler::assignCameraToQuad(unsigned short monitorID,
													 unsigned char stationID1,
													 unsigned char topRightCameraID,
													 unsigned char stationID2,
													 unsigned char topLeftCameraID,
													 unsigned char stationID3,
													 unsigned char bottomRightCameraID,
													 unsigned char stationID4,
													 unsigned char bottomLeftCameraID)
{
    TA_Base_Bus::DataBlock<WORD> data(1, 9);
	data.setAll(0);
    int cmdType = 2;
    data.set(1, makeWord(0, cmdType));
    data.set(3, makeWord(m_controlPrivilege, 0));
    data.set(4, monitorID);
    data.set(5, 0);
    data.set(6, makeWord(stationID1, topRightCameraID));
    data.set(7, makeWord(stationID2, topLeftCameraID));
    data.set(8, makeWord(stationID3, bottomRightCameraID));
    data.set(9, makeWord(stationID4, bottomLeftCameraID));

    m_modbusHelper->SendData(data);
}

void MODBUSCommunicationsHandler::startAutoSwitch(unsigned char sequenceID, unsigned short monitorID)
{
    TA_Base_Bus::DataBlock<WORD> data(1, 9);
	data.setAll(0);
    int cmdType = 3;
    data.set(1, makeWord(sequenceID, cmdType));
    data.set(3, makeWord(m_controlPrivilege, 0));
    data.set(4, monitorID);
    data.set(5, 0);

    m_modbusHelper->SendData(data);
}

void MODBUSCommunicationsHandler::stopAutoSwitchMode(unsigned short monitorID)
{
    TA_Base_Bus::DataBlock<WORD> data(1, 9);
	data.setAll(0);
    int cmdType = 4;
    data.set(1, makeWord(0, cmdType));
    data.set(3, makeWord(m_controlPrivilege, 0));
    data.set(4, monitorID);
    data.set(5, 0);

    m_modbusHelper->SendData(data);
}

void MODBUSCommunicationsHandler::startManualSequence(unsigned short monitorID, const std::vector<ManualSequenceConfig>& sequence)
{
    TA_ASSERT(sequence.size() <= 40, "");
    TA_Base_Bus::DataBlock<WORD> data(10, 91);
    data.setAll(0);
    unsigned char singleScreenMark = 1;
    singleScreenMark = (singleScreenMark << 2) | 1;
    data.set(10, makeWord(0, singleScreenMark));
    data.set(11, monitorID);
    int i = 12;
    for (std::vector<ManualSequenceConfig>::const_iterator it = sequence.begin(); it < sequence.end(); ++it)
    {
        const ManualSequenceConfig& config = *it;
        data.set(i, makeWord(config.stationID, config.cameraID));
        data.set(i + 1, config.stayTime);
        i += 2;
    }

    m_modbusHelper->SendData(data);
}

void MODBUSCommunicationsHandler::stopManualSequence(unsigned short monitorID)
{
    TA_Base_Bus::DataBlock<WORD> data(10, 91);
    data.setAll(0);
    unsigned char singleScreenMark = 1;
    singleScreenMark = (singleScreenMark << 2) | 2;
    data.set(10, makeWord(0, singleScreenMark));
    data.set(11, monitorID);
    m_modbusHelper->SendData(data);
}

void MODBUSCommunicationsHandler::startQuadManualSequence(unsigned short monitorID,
														  const std::vector<ManualSequenceConfig>& topRightSequence,
														  const std::vector<ManualSequenceConfig>& topLeftSequence,
														  const std::vector<ManualSequenceConfig>& bottomRightSequence,
														  const std::vector<ManualSequenceConfig>& bottomLeftSequence)
{
    TA_ASSERT(topRightSequence.size() <= 10, "");
    TA_ASSERT(topLeftSequence.size() <= 10, "");
    TA_ASSERT(bottomRightSequence.size() <= 10, "");
    TA_ASSERT(bottomLeftSequence.size() <= 10, "");
    TA_Base_Bus::DataBlock<WORD> data(10, 91);
    data.setAll(0);
    unsigned char singleScreenMark = 2;
    singleScreenMark = (singleScreenMark << 2) | 1;
    data.set(10, makeWord(0, singleScreenMark));
    data.set(11, monitorID);
    int i1 = 12;
    int i2 = 32;
    int i3 = 52;
    int i4 = 72;
    for (std::vector<ManualSequenceConfig>::const_iterator it = topRightSequence.begin(); it < topRightSequence.end(); ++it)
    {
        const ManualSequenceConfig& config = *it;
        data.set(i1, makeWord(config.stationID, config.cameraID));
        data.set(i1 + 1, config.stayTime);
        i1 += 2;
    }
    for (std::vector<ManualSequenceConfig>::const_iterator it = topLeftSequence.begin(); it < topLeftSequence.end(); ++it)
    {
        const ManualSequenceConfig& config = *it;
        data.set(i2, makeWord(config.stationID, config.cameraID));
        data.set(i2 + 1, config.stayTime);
        i2 += 2;
    }
    for (std::vector<ManualSequenceConfig>::const_iterator it = bottomRightSequence.begin(); it < bottomRightSequence.end(); ++it)
    {
        const ManualSequenceConfig& config = *it;
        data.set(i3, makeWord(config.stationID, config.cameraID));
        data.set(i3 + 1, config.stayTime);
        i3 += 2;
    }
    for (std::vector<ManualSequenceConfig>::const_iterator it = bottomLeftSequence.begin(); it < bottomLeftSequence.end(); ++it)
    {
        const ManualSequenceConfig& config = *it;
        data.set(i4, makeWord(config.stationID, config.cameraID));
        data.set(i4 + 1, config.stayTime);
        i4 += 2;
    }

    m_modbusHelper->SendData(data);
}

void MODBUSCommunicationsHandler::stopQuadManualSequence(unsigned short monitorID)
{
    TA_Base_Bus::DataBlock<WORD> data(10, 91);
    data.setAll(0);
    unsigned char singleScreenMark = 2;
    singleScreenMark = (singleScreenMark << 2) | 2;
    data.set(10, makeWord(0, singleScreenMark));
    data.set(11, monitorID);

    m_modbusHelper->SendData(data);
}

} // TA_IRS_App
