/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/src/IMOSCommunicationsHandler.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #7 $
  *
  * Last modification: $DateTime: 2013/08/30 15:38:55 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the operations for a IMOSCommunicationsHandler
  * using the IMOS protocol.
  * This interface is used by the IMOSCommunicationsHandler class. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 4503)
#endif // _MSC_VER

#define NOT_READY_FOR_DATAPOINT

#include "ace/INET_Addr.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSCommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/src/IAgentRequestCommand.h"
#include "app/cctv/video_switch_agent/protocols/src/IHardwareNotificationCommand.h"
#include "app/cctv/video_switch_agent/src/AddressHelper.h"
#include "app/cctv/video_switch_agent/src/StateUpdateBroadcaster.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "core/alarm/src/AlarmConstants.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/types/VisualAlarms_MessageTypes.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include <algorithm>
#include <boost/smart_ptr.hpp>
#include <iostream>

#if defined(sparc)
#include "app/cctv/video_switch_agent/protocols/imos_sdk_sparc/include/sdk_func_inner.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_sparc/include/xp_func.h"
#elif defined(WIN32)
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/sdk_func_inner.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/xp_func.h"
#endif


#define LOG_ERRORCODE_ERROR(errorCode) \
    if((errorCode) != ERR_COMMON_SUCCEED)\
    {\
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Call imos function return error, errorcode is:%u",(errorCode));\
    }

#define LOG_ERRORCODE_DEBUG(errorCode) \
    if((errorCode) != ERR_COMMON_SUCCEED)\
    {\
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Call imos function return error, errorcode is:%u",(errorCode));\
    }

#define LOG_CALLED_INFO(funcName) \
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function "#funcName" was called")

#define THROW_VSS_EXCEPTION(content) throw TA_Base_Bus_GZL9::VideoSwitchAgentException(content)

#ifdef _UNITEST
#define IMOS_CALL(call) ((TA_Base_Core::RunParams::getInstance().get("ImosReturnValue") == "succeed") ? ERR_COMMON_SUCCEED : ERR_COMMON_FAIL)
#else
#define IMOS_CALL(call) call
#endif


namespace
{
std::string getLocalIpAddress()
{
    ACE_INET_Addr* addrArray;
    size_t count = 0;
    std::string ipAddress;
    if (ACE::get_ip_interfaces(count, addrArray) == 0)
    {
        boost::scoped_array<ACE_INET_Addr> arrayPtr(addrArray);
        while (count--)
        {
            if (false == arrayPtr[count].is_loopback())
            {
                ipAddress = arrayPtr[count].get_host_addr();
                break;
            }
        }
    }
    return ipAddress;
}

// Below struct & func is only available on solaris. For pass build on windows, copy these from solaris sdk
#ifdef WIN32

typedef struct tagSplitScrInfo
{
    ULONG ulSplitScrMode;
    CHAR szSplitScrCode[IMOS_CODE_LEN];
    BOOL_T bSwitchStream;
    CHAR szReserve[128];
} SPLIT_SCR_INFO_S;

typedef enum tagSplitScrMode
{
    SPLIT_SCR_MODE_0    = 0,
    SPLIT_SCR_MODE_1    = 1,
    SPLIT_SCR_MODE_4    = 4,
    SPLIT_SCR_MODE_6    = 6,
    SPLIT_SCR_MODE_8    = 8,
    SPLIT_SCR_MODE_9    = 9,
    SPLIT_SCR_MODE_10   = 10,
    SPLIT_SCR_MODE_13   = 13,
    SPLIT_SCR_MODE_16   = 16,

    SPLIT_SCR_MODE_MAX,
    SPLIT_SCR_MODE_INVALID  = 0xFFFFFFFF
} SPLIT_SCR_MODE_E;

ULONG STDCALL IMOS_ConfigSplitScrInfo
(
    IN USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN CHAR                 szScrCode[IMOS_CODE_LEN],
    IN SPLIT_SCR_INFO_S     *pstSplitScrInfo
)
{
    return 0;
}

#endif
const std::string ERROR_IMOS_NOT_LOGIN = "IMOS not login";
const std::string ERROR_IMOS_NORMAL = "IMOS return error";
}

namespace TA_IRS_App
{
//
// IMOSCommunicationsHandler
//
IMOSCommunicationsHandler::IMOSCommunicationsHandler(TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData)
        : m_videoSwitchAgentData(videoSwitchAgentData)
{
    try
    {
        m_locationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(videoSwitchAgentData->getLocationName());

        loadCommunicationsChannels();
        SwitchManager::getInstance().setAlarmInterface(this);

        SwitchManager::getInstance().setStageModeInterface(this);

        // init IMOS communication
        std::string imosIp = m_videoSwitchAgentData->getCCTVServerIPAddress();

        TA_ASSERT(false == imosIp.empty(), "IMOS ip address should not be empty");

        unsigned long imosPort = m_videoSwitchAgentData->getCCTVServerPort();

        std::string imosUserName = m_videoSwitchAgentData->getCCTVServerUserName();

        TA_ASSERT(false == imosUserName.empty(), "IMOS user name should not be empty");

        std::string imosUserPwd = m_videoSwitchAgentData->getCCTVServerPassword();

        TA_ASSERT(false == imosUserPwd.empty(), "IMOS user password should not be empty");

        m_isAlive = false;
        m_threadShouldRunning = true;
        m_serverPort = imosPort;

        strncpy(m_serverIPAddress, imosIp.c_str(), IMOS_IPADDR_LEN);
        strncpy(m_userName, imosUserName.c_str(), IMOS_NAME_LEN);
        strncpy(m_userPassword, imosUserPwd.c_str(), IMOS_PASSWD_LEN);
        strncpy(m_localIPAddress, getLocalIpAddress().c_str(), IMOS_IPADDR_LEN);
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
        LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "IMOSComunicationHandler initialization.");
        LOG_EXCEPTION_DETAILS(SourceInfo, "UnknownException",
                              "Unknown reason");
        cleanUp();
    }
}


//
// ~IMOSCommunicationsHandler
//
IMOSCommunicationsHandler::~IMOSCommunicationsHandler()
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

        // As this is last IMOS Protocol object to destruct, here we manually destruct
        // the IMOSSwitvhManager singleton also.
        IMOSSwitchManager::getInstance().removeInstance();
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
void IMOSCommunicationsHandler::submitRequestCommand(RequestCommandSharedPtr command, bool isClientRequest)
{
}


//
// submitNotificationCommand
//
void IMOSCommunicationsHandler::submitNotificationCommand(NotificationCommandSharedPtr command)
{
}


void IMOSCommunicationsHandler::processSummaryAlarmsState(bool systemSummaryAlarm, bool transmissionSummaryAlarm, bool cameraSummaryAlarm, bool ancillarySummaryAlarm, bool dvrSummaryAlarm)
{
}

void IMOSCommunicationsHandler::processAlarmStack(TA_IMOS::AlarmStack alarmStack)
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
void IMOSCommunicationsHandler::connect()
{
    FUNCTION_ENTRY("connect");

    if (TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState())
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                    "connect() called while already active. Thread will be restarted.");

        terminateAndWait();
    }

    start();

    FUNCTION_EXIT;
}


//
// disconnect
//
void IMOSCommunicationsHandler::disconnect()
{
    FUNCTION_ENTRY("disconnect");

    //
    // IMPORTANT - Must deactivate State Validation Thread before we obtain
    // m_threadStateLock. Otherwise deadlock may occur because the validation
    // thread acquires this lock during its run method (via calling
    // submitRequestCommand).
    //

    if (TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != getCurrentState())
    {
        terminateAndWait();
    }

    // Empty anything left in the queues as these may never be processed now,
    //clearCommandQueues();

    // Ensure flag reset so thread will run correctly again when reactivated.
    //m_doTerminate = false;

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
void IMOSCommunicationsHandler::loadCommunicationsChannels()
{
}


//
// activateCommunicationsChannels
//
void IMOSCommunicationsHandler::activateCommunicationsChannels()
{
}


//
// deactivateCommuncationsChannels
//
void IMOSCommunicationsHandler::deactivateCommunicationsChannels()
{
}


//
// cleanUp
//
void IMOSCommunicationsHandler::cleanUp()
{
    FUNCTION_ENTRY("cleanUpVideoSwitchData");
    FUNCTION_EXIT;
}


//
// processRequestCommand
//
void IMOSCommunicationsHandler::processRequestCommand(RequestCommandSharedPtr command)
{
}


//
// receivedChannelStateCommsFailure
//
void IMOSCommunicationsHandler::receivedChannelStateCommsFailure(unsigned int channelIndex)
{
}


//
// receivedChannelStateNotCommsFailure
//
void IMOSCommunicationsHandler::receivedChannelStateNotCommsFailure(unsigned int channelIndex)
{
}


//
// receivedChannelStateActive
//
void IMOSCommunicationsHandler::receivedChannelStateActive(unsigned int channelIndex)
{
}


//
// receivedChannelStateNotActive
//
void IMOSCommunicationsHandler::receivedChannelStateNotActive(unsigned int channelIndex)
{
}


//
// setDataPointsAsBadValue
//
void IMOSCommunicationsHandler::setDataPointsAsBadValue()
{
}


//
// isConnected
//
bool IMOSCommunicationsHandler::isConnected()
{
    return ((TA_Base_Core::Thread::THREAD_STATE_RUNNING == getCurrentState())
            || (TA_Base_Core::Thread::THREAD_STATE_NEW == getCurrentState()));
}


//
// isThereActiveSwitchChannel
//
bool IMOSCommunicationsHandler::isThereActiveSwitchChannel(unsigned int& activeChannelIndex)
{
    return true;
}


//
// commandNotToBeProcessed
//
void  IMOSCommunicationsHandler::commandNotToBeProcessed(RequestCommandSharedPtr command)
{
}


//
// commandNotToBeProcessed
//
void IMOSCommunicationsHandler::commandNotToBeProcessed(NotificationCommandSharedPtr command)
{
}


//
// clearCommandQueues
//
void IMOSCommunicationsHandler::clearCommandQueues()
{
}


void IMOSCommunicationsHandler::openTriggeringEventAlarms(const TA_IMOS::AlarmStack& alarmsToBeOpened)
{
}

void IMOSCommunicationsHandler::closeTriggeringEventAlarms(const TA_IMOS::AlarmStack& alarmsToBeClosed)
{
}

void IMOSCommunicationsHandler::acknowledgeAlarm(const std::string& alarmKey,/* unsigned long entityKey,*/ unsigned long consoleId)
{
}

void IMOSCommunicationsHandler::acknowledgeAlarm(unsigned long stationId, unsigned long eventId, unsigned long consoleId)
{
}

void IMOSCommunicationsHandler::acknowledgeTrainCameraAlarm(unsigned long locationId, unsigned long trainId, const std::vector<unsigned long>& cameras, unsigned long consoleId)
{
}

void IMOSCommunicationsHandler::closeAlarm(const std::string& alarmKey/*, unsigned long entityKey*/)
{
}

void IMOSCommunicationsHandler::closeAlarm(unsigned long stationId, unsigned long eventId)
{
}

TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack IMOSCommunicationsHandler::getAlarmStack()
{
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack stack;
    return stack;
}

void IMOSCommunicationsHandler::updateAlarmStack(TA_Base_Bus_GZL9::SwitchManagerCorbaDef::AlarmStack& stack)
{
}

unsigned long IMOSCommunicationsHandler::getEntityKey()
{
    return m_videoSwitchAgentData->getKey();
}


bool IMOSCommunicationsHandler::canbeManuallyClosed(const TA_IMOS::AlarmState& alarmState)
{
    return false;
}

void IMOSCommunicationsHandler::setStageSwitchMode(unsigned long originatingStage, unsigned long destinationStage, bool allowSwitch)
{
}

void IMOSCommunicationsHandler::requestSingleCamToOutput(const std::string& camAddress, const std::string& monAddress)
{
    CHAR cameraCode[IMOS_RES_CODE_LEN], monitorCode[IMOS_RES_CODE_LEN];
    strncpy(cameraCode, camAddress.c_str(), IMOS_RES_CODE_LEN);
    strncpy(monitorCode, monAddress.c_str(), IMOS_RES_CODE_LEN);

    LOG_CALLED_INFO(requestSingleCamToOutput);
    TA_Base_Core::ThreadGuard guard(m_lock);
    std::string error = ERROR_IMOS_NOT_LOGIN;
    if(true == m_isAlive)
    {
        error = ERROR_IMOS_NORMAL;
        ULONG ret = IMOS_CALL(IMOS_StartMonitor(&m_userLoginInfo, cameraCode, monitorCode, IMOS_FAVORITE_STREAM_ANY, USER_OPERATE_SERVICE));
        LOG_ERRORCODE_ERROR(ret);
        if(ERR_COMMON_SUCCEED == ret)
        {
            return;
        }
    }
    THROW_VSS_EXCEPTION(error.c_str());
}

void IMOSCommunicationsHandler::requestSequenceToOutput(const std::string& seqAddress, const std::string& monAddress)
{
    CHAR seqCode[IMOS_RES_CODE_LEN], monitorCode[IMOS_RES_CODE_LEN];
    strncpy(seqCode, seqAddress.c_str(), IMOS_RES_CODE_LEN);
    strncpy(monitorCode, monAddress.c_str(), IMOS_RES_CODE_LEN);
    LOG_CALLED_INFO(requestSequenceToOutput);
    TA_Base_Core::ThreadGuard guard(m_lock);
    std::string error = ERROR_IMOS_NOT_LOGIN;
    if(true == m_isAlive)
    {
        error = ERROR_IMOS_NORMAL;
        ULONG ret = IMOS_CALL(IMOS_StartSwitchManual(&m_userLoginInfo, seqCode, monitorCode, USER_OPERATE_SERVICE));
        LOG_ERRORCODE_ERROR(ret);
        if(ERR_COMMON_SUCCEED == ret)
        {
            return;
        }
    }
    THROW_VSS_EXCEPTION(error.c_str());
}

void IMOSCommunicationsHandler::deactiveSequence(const std::string& seqAddress, const std::string& monAddress)
{
    CHAR monitorCode[IMOS_RES_CODE_LEN];
    strncpy(monitorCode, monAddress.c_str(), IMOS_RES_CODE_LEN);
    LOG_CALLED_INFO(deactiveSequence);
    TA_Base_Core::ThreadGuard guard(m_lock);
    std::string error = ERROR_IMOS_NOT_LOGIN;
    if(true == m_isAlive)
    {
        error = ERROR_IMOS_NORMAL;
        ULONG ret = IMOS_CALL(IMOS_StopSwitchManual(&m_userLoginInfo, monitorCode, USER_OPERATE_SERVICE));
        LOG_ERRORCODE_ERROR(ret);
        if(ERR_COMMON_SUCCEED == ret)
        {
            return;
        }
    }
    THROW_VSS_EXCEPTION(error.c_str());
}

void IMOSCommunicationsHandler::requestQuadToOutput(const std::string& quadAddress, const std::string& monAddress)
{
    CHAR quadCode[IMOS_RES_CODE_LEN], monitorCode[IMOS_RES_CODE_LEN];
    strncpy(quadCode, quadAddress.c_str(), IMOS_RES_CODE_LEN);
    strncpy(monitorCode, monAddress.c_str(), IMOS_RES_CODE_LEN);
    TA_Base_Core::ThreadGuard guard(m_lock);
    std::string error = ERROR_IMOS_NOT_LOGIN;
    if(true == m_isAlive)
    {
        error = ERROR_IMOS_NORMAL;
        SPLIT_SCR_INFO_S s;
        s.ulSplitScrMode = SPLIT_SCR_MODE_4;
        strncpy(s.szSplitScrCode, quadCode, IMOS_RES_CODE_LEN);
        s.bSwitchStream = BOOL_FALSE;
        ULONG ret = IMOS_CALL(IMOS_ConfigSplitScrInfo(&m_userLoginInfo, monitorCode, &s));
        LOG_ERRORCODE_ERROR(ret);
        if(ERR_COMMON_SUCCEED == ret)
        {
            return;
        }
    }
    THROW_VSS_EXCEPTION(error.c_str());
}

void IMOSCommunicationsHandler::stopMonitor(const std::string& monAddress)
{
    LOG_CALLED_INFO(stopMonitor);
    CHAR monitorCode[IMOS_RES_CODE_LEN];
    strncpy(monitorCode, monAddress.c_str(), IMOS_RES_CODE_LEN);
    TA_Base_Core::ThreadGuard guard(m_lock);
    std::string error = ERROR_IMOS_NOT_LOGIN;
    if(true == m_isAlive)
    {
        error = ERROR_IMOS_NORMAL;
        ULONG ret = IMOS_CALL(IMOS_StopMonitor(&m_userLoginInfo, monitorCode, USER_OPERATE_SERVICE));
        LOG_ERRORCODE_ERROR(ret);
        if(ERR_COMMON_SUCCEED == ret)
        {
            return;
        }
    }
    THROW_VSS_EXCEPTION(error.c_str());
}

void IMOSCommunicationsHandler::ptzCtrl(const std::string& camAddress, PTZ_CTRL_COMMAND_S* s)
{
    LOG_CALLED_INFO(ptzCtrl);
    CHAR cameraCode[IMOS_RES_CODE_LEN];
    strncpy(cameraCode, camAddress.c_str(), IMOS_RES_CODE_LEN);
    TA_Base_Core::ThreadGuard guard(m_lock);
    std::string error = ERROR_IMOS_NOT_LOGIN;
    if(true == m_isAlive)
    {
        // start ptz ctrl
        error = ERROR_IMOS_NORMAL;
        ULONG ret = IMOS_CALL(IMOS_StartPtzCtrl(&m_userLoginInfo, cameraCode));
        LOG_ERRORCODE_ERROR(ret);
        if(ERR_COMMON_SUCCEED == ret)
        {
            // then run ctrl command
            ret = IMOS_CALL(IMOS_PtzCtrlCommand(&m_userLoginInfo, cameraCode, s));
            LOG_ERRORCODE_ERROR(ret);
            if(ERR_COMMON_SUCCEED == ret)
            {
                ret = IMOS_CALL(IMOS_ReleasePtzCtrl(&m_userLoginInfo, cameraCode, true));
                LOG_ERRORCODE_ERROR(ret);
                return;
            }
        }
    }
    THROW_VSS_EXCEPTION(error.c_str());
}

void IMOSCommunicationsHandler::modifySequenceConfig(const std::string& seqAddress, SWITCH_RESOURE_S* s)
{
    LOG_CALLED_INFO(modifySequenceConfig);
    CHAR seqCode[IMOS_RES_CODE_LEN];
    strncpy(seqCode, seqAddress.c_str(), IMOS_RES_CODE_LEN);
    TA_Base_Core::ThreadGuard guard(m_lock);
    std::string error = ERROR_IMOS_NOT_LOGIN;
    if(true == m_isAlive)
    {
        error = ERROR_IMOS_NORMAL;
        ULONG ret = IMOS_CALL(IMOS_ModifySwitchResource(&m_userLoginInfo, seqCode, s));
        LOG_ERRORCODE_ERROR(ret);
        if(ERR_COMMON_SUCCEED == ret)
        {
            return;
        }
    }
    THROW_VSS_EXCEPTION(error.c_str());
}

void IMOSCommunicationsHandler::querySequenceInfo(const std::string& seqAddress, SWITCH_RESOURE_S* s)
{
    LOG_CALLED_INFO(querySequenceInfo);
    CHAR seqCode[IMOS_RES_CODE_LEN];
    strncpy(seqCode, seqAddress.c_str(), IMOS_RES_CODE_LEN);
    TA_Base_Core::ThreadGuard guard(m_lock);
    std::string error = ERROR_IMOS_NOT_LOGIN;
    if(true == m_isAlive)
    {
        error = ERROR_IMOS_NORMAL;
        ULONG ret = IMOS_CALL(IMOS_QuerySwitchResource(&m_userLoginInfo, seqCode, s));
        LOG_ERRORCODE_ERROR(ret);
        if(ERR_COMMON_SUCCEED == ret)
        {
            return;
        }
    }
    THROW_VSS_EXCEPTION(error.c_str());
}

void IMOSCommunicationsHandler::connectionEstablished()
{
    // TODO, need modify
    processSummaryAlarmsState(false, false, false, false, false);
}
void IMOSCommunicationsHandler::connectionLost()
{
    setDataPointsAsBadValue();
}

int IMOSCommunicationsHandler::handle_timeout(const ACE_Time_Value &tv, const void *arg)
{

    static int sKeepAliveFailTimes = 0;
    if (isAlive())
    {
        ULONG ret = 0;
        {
            TA_Base_Core::ThreadGuard guard(m_lock);
            ret = IMOS_CALL(IMOS_UserKeepAlive(&m_userLoginInfo));
        }
        LOG_ERRORCODE_DEBUG(ret);
        if (ERR_COMMON_SUCCEED != ret)
        {
            ++sKeepAliveFailTimes;
            if (sKeepAliveFailTimes >= 3)
            {
                sKeepAliveFailTimes = 0;
                logout();
            }
        }
        else
        {
            sKeepAliveFailTimes = 0;
        }
    }
    return 0;
}

void IMOSCommunicationsHandler::run()
{

    m_threadShouldRunning = true;
    while (m_threadShouldRunning)
    {
        if (false == isAlive())
        {
            login();
        }
        TA_Base_Core::Thread::sleep(500);
    }
    if (true == isAlive())
    {
        logout();
    }
}

void IMOSCommunicationsHandler::terminate()
{

    m_threadShouldRunning = false;
}

void IMOSCommunicationsHandler::login()
{
    bool connected = false;
    {
        TA_Base_Core::ThreadGuard guard(m_lock);
        if (false == m_isAlive)
        {
            ULONG ret;
            // Initiate
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "IMOS server ip:%s, port:%d", m_serverIPAddress, m_serverPort);
            ret = IMOS_CALL(IMOS_Initiate(m_serverIPAddress, m_serverPort, BOOL_TRUE, BOOL_FALSE));
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "IMOS_Initiate returned");
            LOG_ERRORCODE_ERROR(ret);
            if (ERR_COMMON_SUCCEED == ret)
            {
                CHAR encryptPwd[IMOS_PASSWD_ENCRYPT_LEN];
                // Encrypt password
                ret = IMOS_CALL(IMOS_Encrypt(m_userPassword, strlen(m_userPassword), encryptPwd));
                LOG_ERRORCODE_DEBUG(ret);
                if (ERR_COMMON_SUCCEED == ret)
                {
                    LOGIN_INFO_S stLoginInfo;
                    // Login
                    ret = IMOS_CALL(IMOS_Login(m_userName, encryptPwd, m_localIPAddress, &stLoginInfo));
                    LOG_ERRORCODE_DEBUG(ret);
                    if (ERR_COMMON_SUCCEED == ret)
                    {
                        connected = true;
                        m_userLoginInfo = stLoginInfo.stUserLoginIDInfo;
                        m_isAlive = true;
                        // keep alive
                        // initialize timer
                        m_timerMgr.activate();
                        // Add event handle 1 second later, every 30s call userkeepalive
                        m_timerId = m_timerMgr.schedule(this, NULL, ACE_OS::gettimeofday() + ACE_Time_Value(1), ACE_Time_Value(30));
                    }
                }
            }
        }
    }
    if (true == connected)
    {
        connectionEstablished();
    }
}

void IMOSCommunicationsHandler::logout()
{
    {
        LOG_CALLED_INFO(logout);
        TA_Base_Core::ThreadGuard guard(m_lock);
        if (false == m_isAlive) return;
        // Cancel timer
        m_timerMgr.cancel(m_timerId);
        m_timerMgr.deactivate();
        m_isAlive = false;
        ULONG ret;
        ret = IMOS_CALL(IMOS_Logout(&m_userLoginInfo));
        LOG_ERRORCODE_DEBUG(ret);
        ret = IMOS_CALL(IMOS_CleanUp(&m_userLoginInfo));
        LOG_ERRORCODE_DEBUG(ret);
    }
    connectionLost();
}

bool IMOSCommunicationsHandler::isAlive()
{

    TA_Base_Core::ThreadGuard guard(m_lock);
    return m_isAlive;
}

} // TA_IRS_App
