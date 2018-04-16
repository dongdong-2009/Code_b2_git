/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/GlobalConstants.h $
 * @author  HuangQi
 * @version $Revision: #25 $
 * Last modification : $DateTime: 2017/09/05 15:51:24 $
 * Last modified by : $Author: huang.wenbo $
 *
 * 
 */

#ifndef GLOBALCONSTANTS_H
#define GLOBALCONSTANTS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_App
{

const long PAS_POLLING_FIREALARM_INTERVAL = 500;
const long PAS_POLLING_ZONE_COUNTER_INTERVAL = 200;
const long PAS_POLLING_SYSTEM_FAULT_INTERVAL = 500;
const long PAS_POLLING_DVA_LIB_COUNTER_INTERVAL = 2000;
const long PAS_POLLING_MIN_INTERVAL = 50;

const unsigned long DEFAULT_NO_PAS_STATUS_TIMEOUT = 5000u; // milliseconds
const unsigned long DEFAULT_NO_PTT_STATUS_TIMEOUT = 60000u; // milliseconds
const unsigned long DEFAULT_NO_PLAYBACK_STATUS_TIMEOUT = 2000u; // milliseconds
const unsigned long DEFAULT_AD_HOC_RECORDING_TIMEOUT = 30000u; // milliseconds

const unsigned long PAS_MAX_DVA_RECORDING_DURATION = 60u;

const int PAS_POLLING_SYSTEM_FAULT_START = 200;
const int PAS_POLLING_SYSTEM_FAULT_END = 265;
const int PAS_POLLING_SYSTEM_FAULT = 0;
const int PAS_POLLING_ZONE_COUNTER_START = 7;
const int PAS_POLLING_ZONE_COUNTER_END = 8;
const int PAS_POLLING_ZONE_STATE_COUNTER = 7;
const int PAS_POLLING_FAULT_ZONE_COUNTER = 8;
const int PAS_POLLING_DVA_LIB_COUNTER = 10;
const int PAS_POLLING_STN_FIREALARM = 27732;
const int PAS_POLLING_OCC_FIREALARM_START = 26380;
const int PAS_POLLING_OCC_FIREALARM_END = 26445;
const int PAS_POLLING_OCC_CONSOLE_START = 101;
const int PAS_POLLING_OCC_CONSOLE_END = 111;
const int PAS_POLLING_STN_CONSOLE_START = 257;
const int PAS_POLLING_STN_CONSOLE_END = 257;

const int PAS_EXECUTION_ACKNOWLEDGE_TIMEOUT = 5000;
const int BIT_COUNT_IN_BYTE = 8;
const int BIT_COUNT_IN_WORD = 16;
const int BIT_COUNT_IN_DWORD = 32;

const int PAS_ADDRESS_STN_SCHEDULE_DVA_SOURCE_TYPE = 24582;
const int PAS_ADDRESS_OCC_MATRIX_TABLE_START = 25000;
const int PAS_ADDRESS_OCC_MATRIX_TABLE_END = 25263;
const int PAS_ADDRESS_STN_MATRIX_TABLE_START = 24715;
const int PAS_ADDRESS_STN_MATRIX_TABLE_END = 24716;
const int PAS_ADDRESS_DVA_PRIORITY = 24577;
const int PAS_ADDRESS_DVA_BANK_NUMBER = 24583;
const int PAS_ADDRESS_OCC_TRANSACTION_ID = 24697;//lixiaoxia
const int PAS_ADDRESS_STN_TRANSACTION_ID = 24597;
const int PAS_ADDRESS_OCC_CHIME = 24583;
const int PAS_ADDRESS_STN_CHIME = 24579;
const int PAS_ADDRESS_STN_SOURCE_ID = 24582;
const int PAS_ADDRESS_OCC_SCHEDULE_ID = 24583;
const int PAS_ADDRESS_STN_SCHEDULE_ID = 24580;
const int PAS_ADDRESS_OCC_SCHEDULE_START_TIME = 24578;
const int PAS_ADDRESS_STN_SCHEDULE_START_TIME = 24587;
const int PAS_ADDRESS_OCC_SCHEDULE_START_SECOND = 24895;
const int PAS_ADDRESS_STN_SCHEDULE_START_SECOND = 24588;
const int PAS_ADDRESS_OCC_SCHEDULE_END_TIME = 24579;
const int PAS_ADDRESS_STN_SCHEDULE_END_TIME = 24591;
const int PAS_ADDRESS_OCC_SCHEDULE_END_SECOND = 24896;
const int PAS_ADDRESS_STN_SCHEDULE_END_SECOND = 24592;
const int PAS_ADDRESS_OCC_SCHEDULE_PERIOD = 24580;
const int PAS_ADDRESS_STN_SCHEDULE_PERIOD = 24593;
const int PAS_ADDRESS_OCC_SCHEDULE_DAYS = 24581;
const int PAS_ADDRESS_STN_SCHEDULE_DAYS = 24595;
const int PAS_ADDRESS_STN_SCHEDULE_START_YEAR = 24585;
const int PAS_ADDRESS_STN_SCHEDULE_START_DAY = 24586;
const int PAS_ADDRESS_STN_SCHEDULE_STOP_YEAR = 24589;
const int PAS_ADDRESS_STN_SCHEDULE_STOP_DAY = 24590;
const int PAS_ADDRESS_OCC_REMOTE_NUMBER = 24583;
const int PAS_ADDRESS_STN_REMOTE_NUMBER = 24584;
const int PAS_ADDRESS_OCC_MESSAGE_ID = 24769;
const int PAS_ADDRESS_STN_MESSAGE_ID = 24578;
const int PAS_ADDRESS_OCC_PLAYBACK_ID = 24583;
const int PAS_ADDRESS_STN_PLAYBACK_ID = 24578;
const int PAS_ADDRESS_EXECUTION_REGISTER = 9;
const int PAS_ADDRESS_OCC_COMMAND_REGISTER = 24575;
const int PAS_ADDRESS_STN_COMMAND_REGISTER = 24575;
const int PAS_ADDRESS_COMMAND_RESULT = 24576;
const int PAS_ADDRESS_OCC_ZONE_STATE_START = 456;
const int PAS_ADDRESS_OCC_ZONE_STATE_END = 4679;
const int PAS_ADDRESS_STN_ZONE_STATE_START = 27732;
const int PAS_ADDRESS_STN_ZONE_STATE_END = 27763;
const int PAS_ADDRESS_OCC_FAULT_ZONE_START = 456;
const int PAS_ADDRESS_OCC_FAULT_ZONE_END = 4679;
const int PAS_ADDRESS_STN_FAULT_ZONE_START = 27432;
const int PAS_ADDRESS_STN_FAULT_ZONE_END = 27463;
const int PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_START = 24775;
const int PAS_ADDRESS_OCC_DVA_LIB_DOWNLOAD_END = 24894;
const int PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_START = 27200;
const int PAS_ADDRESS_STN_DVA_LIB_DOWNLOAD_END = 27319;
const int PAS_ADDRESS_OCC_DVA_LIB_VERSION_START = 14;
const int PAS_ADDRESS_OCC_DVA_LIB_VERSION_END = 16;
const int PAS_ADDRESS_STN_DVA_LIB_VERSION_START = 14;
const int PAS_ADDRESS_STN_DVA_LIB_VERSION_END = 16;
const int PAS_ADDRESS_OCC_SCHEDULE_ID_STATE = 16841;
const int PAS_ADDRESS_STN_SCHEDULE_ID_STATE = 2001;
const int PAS_ADDRESS_OCC_RECORDING_TIME = 20700;
const int PAS_ADDRESS_STN_RECORDING_TIME = 24591;
//added by lixiaoxia
const int PAS_ADDRESS_OCC_ZONE_TRANSID_STATE_START = 43788;
const int PAS_ADDRESS_OCC_ZONE_TRANSID_STATE_END = 48011;
const int PAS_ADDRESS_STN_ZONE_TRANSID_STATE_START = 27796;
const int PAS_ADDRESS_STN_ZONE_TRANSID_STATE_END = 27827;
const int PAS_ADDRESS_MULTI_MESSAGES_ID_START = 24900;
const int PAS_ADDRESS_MULTI_MESSAGES_ID_END = 24903;
const int PAS_ADDRESS_OCC_SCHEDULE_REPEAT_TIME = 24582;
const int PAS_ADDRESS_STN_SCHEDULE_REPEAT_TIME = 24596;
const int PAS_ADDRESS_OCC_DWELL_PERIOD = 24698;
const int PAS_ADDRESS_STN_DWELL_PERIOD = 24598;

const int PAS_STN_SCHEDULE_DVA_SOURCE_TYPE = 0x0C00;
const int PAS_SCHEDULE_DEFAULT_REPEAT_TIME = 0x0000;
const int PAS_SCHEDULE_ID_STATE_LENGTH = 1;
const int PAS_OCC_SCHEDULE_DVA_BUFF_LENGTH = 8;
const int PAS_STN_SCHEDULE_DVA_BUFF_LENGTH = 16;
const int PAS_EACH_STATION_WORD_COUNT = 4;
const int PAS_ADDRESS_LENGTH = 16;

const unsigned short CUR_STATION_ID = 0u;

const unsigned short INVALID_SCHEDULE_EVENT_ID = 0u;
const unsigned short FAILURE_SCHEDULE_EVENT_ID = 0xFFFFu;
const unsigned short PAS_DVA_MSG_LIB_END_ID = 0u;
const unsigned short MASK_LOW_BYTE = 0x00FFu;
const unsigned short MASK_HIGH_BYTE = 0xFF00u;
const unsigned short MASK_OCC_FIRE_ALARM = 0x03FEu;
const unsigned short MASK_OCC_FIRE_ALARM_WITH_DVA = 0x03EBu;
const unsigned short MASK_STN_FIRE_ALARM = 0x8000u;
const unsigned short MASK_STN_FIRE_ALARM_WITH_DVA = 0x8000u;
const unsigned short MASK_OCC_ZONE_BROADCAST_STATE = 0x0FFFu;
const unsigned short MASK_STN_ZONE_BROADCAST_STATE = 0x1FFFu;
const unsigned short MASK_TRANSACTION_STATE_ID = 0x0FFFu;//lixiaoxia
const unsigned short MASK_ZONE_FAULT_STATE = 0xC000u;//lixiaoxia
const unsigned short MASK_BUSY_TRANSACTION = 0x3FFFu;
const unsigned short PAS_SCHEDULE_ID_AVAILABLE = 0u;
const unsigned short PAS_OCC_MINOR_ALARM = 0x8000u;
const unsigned short PAS_STN_MINOR_ALARM = 0x8000u;
const unsigned short PAS_OCC_MAJOR_ALARM = 0x4000u;
const unsigned short PAS_STN_MAJOR_ALARM = 0x4000u;
const unsigned short PAS_OCC_UNKNOWN_ALARM = 0x3100u;
const unsigned short PAS_OCC_FAULT_ZONE = 0x4000u;
const unsigned short PAS_OCC_UNKNOWN_ZONE = 0xFFFFu;
const unsigned short PAS_STN_FAULT_ZONE = 0x4000u;
const unsigned short PAS_CONSOLE_PLAYBACKING = 0x0800u;
const unsigned short PAS_CONSOLE_RECORDING = 0x0400u;
const unsigned short PAS_CONSOLE_LIVING_MASK = 0x0100u | 0x0200u;
const unsigned short PAS_OCC_MUSIC_STATION_SELECTED = 0x8000u;
const unsigned short PAS_REMOTE_NUMBER_FIRE_ALARM = 0x0001u;
const unsigned short PAS_MESSAGE_NUMBER_FIRE_ALARM = 0x0001u;
const unsigned short PAS_ZONE_STATE_FREE = 0u;
const unsigned short PAS_FREE_TRANSACTION = 0u;//added by lixiaoxia
const unsigned short PAS_ZONE_STATE_OCC_LIVE_SRC_BASE = 0u;
const unsigned short PAS_ZONE_STATE_STN_LIVE_SRC_BASE = 0u;
const unsigned short PAS_ZONE_STATE_OCC_CENTRE_MUSIC = 0x03EEu;
const unsigned short PAS_ZONE_STATE_OCC_LOCAL_LIVE = 0x03EAu;
const unsigned short PAS_ZONE_STATE_OCC_LOCAL_DVA = 0x03ECu;
const unsigned short PAS_ZONE_STATE_OCC_LOCAL_MUSIC = 0x03EDu;
const unsigned short PAS_ZONE_STATE_OCC_FIRE_DVA_BROADCAST = 0x0C01u;
const unsigned short PAS_ZONE_STATE_STN_CENTRE_MUSIC = 0x0802u;
const unsigned short PAS_ZONE_STATE_STN_CENTRE_DVA = 0x1402u;
const unsigned short PAS_ZONE_STATE_STN_CENTRE_LIVE = 0x1401u;
const unsigned short PAS_ZONE_STATE_STN_LOCAL_MUSIC = 0x0801u;
const unsigned short PAS_ZONE_STATE_STN_FIRE_DVA_BROADCAST = 0x03EBu;
const unsigned short PAS_ZONE_STATE_OCC_DVA_ID_BASE = 100u;
const unsigned short PAS_ZONE_STATE_STN_DVA_ID_BASE = 0x0C00u;
const unsigned short PAS_STN_SCHEDULE_MSG_ID_BASE = 0x0000u;
const unsigned short PAS_ZONE_STATE_OCC_FAULT = 0x1000u;
const unsigned short PAS_ZONE_STATE_STN_FAULT = 0x2000u;
const unsigned short PAS_ZONE_STATE_OCC_OVERRIDDEN = 0x2000u;
const unsigned short PAS_ZONE_STATE_STN_OVERRIDDEN = 0x4000u;
const unsigned short PAS_PAGING_CONSOLE_PSC = 0x0402u;
const unsigned short PAS_ZONE_FAULT_STATE = 0x4000u;//lixiaoxia
const unsigned short PAS_MUSIC_SOURCE_FROM_OCC = 0x0802u;
const unsigned short PAS_MUSIC_SOURCE_FROM_STN = 0x0801u;
const unsigned short PAS_COMMAND_OCC_START_DVA = 0x0900u;
const unsigned short PAS_COMMAND_STN_START_DVA = 0x0900u;
const unsigned short PAS_COMMAND_OCC_START_CHIME_DVA = 0x2900u;
const unsigned short PAS_COMMAND_STN_START_CHIME_DVA = 0x2900u;
const unsigned short PAS_COMMAND_OCC_CREATE_SCHEDULE = 0x4900u;
const unsigned short PAS_COMMAND_STN_CREATE_SCHEDULE = 0x4900u;
const unsigned short PAS_COMMAND_OCC_CREATE_CHIME_SCHEDULE = 0x6900u;
const unsigned short PAS_COMMAND_STN_CREATE_CHIME_SCHEDULE = 0x6900u;
const unsigned short PAS_COMMAND_OCC_DELETE_SCHEDULE_BASE = 0x4A00u; // Command differ with schedule ID
const unsigned short PAS_COMMAND_STN_DELETE_SCHEDULE = 0x4A00u;
const unsigned short PAS_COMMAND_OCC_STOP_DVA = 0x0A00u;
const unsigned short PAS_COMMAND_STN_STOP_DVA = 0x0A00u;
const unsigned short PAS_COMMAND_DVA_LIB_NEW_DOWNLOAD = 0x1D00u;
const unsigned short PAS_COMMAND_DVA_LIB_NEXT_DOWNLOAD = 0x1E00u;
const unsigned short PAS_COMMAND_OCC_CHIME_LIVE_BASE = 0x1400u; // Command differ with Source ID
const unsigned short PAS_COMMAND_STN_CHIME_LIVE = 0x1400u;
const unsigned short PAS_COMMAND_OCC_LIVE_BASE = 0x1300u; // Command differ with Source ID
const unsigned short PAS_COMMAND_STN_LIVE= 0x1300u;
const unsigned short PAS_COMMAND_OCC_ACKNOWLEDEG_FIRE_ALARM_BASE = 0x6200u;
const unsigned short PAS_COMMAND_STN_ACKNOWLEDEG_FIRE_ALARM = 0x0203u;
const unsigned short PAS_COMMAND_OCC_START_MUSIC_BROADCAST= 0x050Du;
const unsigned short PAS_COMMAND_STN_START_MUSIC_BROADCAST= 0x0500u;
const unsigned short PAS_COMMAND_OCC_STOP_MUSIC_BROADCAST= 0x060Du;
const unsigned short PAS_COMMAND_STN_STOP_MUSIC_BROADCAST= 0x0600u;
const unsigned short PAS_COMMAND_OCC_CHANGE_DVA_TYPE = 0x1C00u;
const unsigned short PAS_COMMAND_OCC_DELETE_DVA = 0x1B00u;
const unsigned short PAS_COMMAND_STN_CHANGE_DVA_TYPE = 0x1C00u;
const unsigned short PAS_COMMAND_STN_DELETE_DVA = 0x1B00u;
const unsigned short PAS_COMMAND_OCC_DVA_PLAYBACK_BASE = 0x2100u; // Command differ with Source ID
const unsigned short PAS_COMMAND_STN_DVA_PLAYBACK = 0x2100u;
const unsigned short PAS_COMMAND_OCC_START_DVA_RECORDING_BASE = 0x1900u; // Command differ with Source ID
const unsigned short PAS_COMMAND_OCC_STOP_DVA_RECORDING_BASE = 0x1A00u; // Command differ with Source ID
const unsigned short PAS_COMMAND_STN_START_DVA_RECORDING = 0x1900u;
const unsigned short PAS_COMMAND_STN_STOP_DVA_RECORDING = 0x1A00u;


const char* const EMPTY_STRING = "";
const char END_OF_STRING = '\0';

enum TRANS_EXECUTION_RESULT
{
    INVALID_NULL_TRANSACTION = -2,
    TRANSACTION_NOT_EXECUTED = -1,
    TRANSACTION_SUCCESS = 0,
    TRANSACTION_QUEUED,
    TRANSACTION_IN_MONITOR_MODE,
    TRANSACTION_FAILED,
    TRANSACTION_CONNECTION_FAILURE
};

} // namespace TA_IRS_App

#endif // GLOBALCONSTANTS_H
