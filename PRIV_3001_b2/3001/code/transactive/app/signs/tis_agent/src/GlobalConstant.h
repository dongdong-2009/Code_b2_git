#ifndef GLOBALCONSTANT_H
#define GLOBALCONSTANT_H

#include <string>

const std::string CONNECTION_LINK_SUMMARY_ALARM_DP_NAME = "diiSEV-LinkNormalAlarm";
const std::string OCC_CONNECTION_LINK_SUMMARY_ALARM_DP_NAME = "diiSEVOCC-LinkNormalAlarm";

const std::string TIS_OCC_SERVER_STATUS_ALARM_DP_NAME = "diiSEVOCC-ServerNormalAlarm"; // OCC only

const std::string CURRENT_STATION_LIBRARY_VERSION_DP_NAME = "aiiTISC-CurrentSTISLibraryVersion";
const std::string CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME = "aiiTISC-CurrentTTISLibraryVersion";
const std::string NEXT_STATION_LIBRARY_VERSION_DP_NAME = "aiiTISC-NextSTISLibraryVersion";
const std::string NEXT_TRAIN_LIBRARY_VERSION_DP_NAME = "aiiTISC-NextTTISLibraryVersion";

const std::string CENTRAL_CURRENT_STATION_LIBRARY_VERSION_DP_NAME = "aiiTISC-CurrentSTISLibraryVersion-CDB";
const std::string CENTRAL_CURRENT_TRAIN_LIBRARY_VERSION_DP_NAME = "aiiTISC-CurrentTTISLibraryVersion-CDB";
const std::string CENTRAL_NEXT_STATION_LIBRARY_VERSION_DP_NAME = "aiiTISC-NextSTISLibraryVersion-CDB";
const std::string CENTRAL_NEXT_TRAIN_LIBRARY_VERSION_DP_NAME = "aiiTISC-NextTTISLibraryVersion-CDB";

const std::string STATION_LIBRARIES_SYNCHRONISED_DP_NAME = "diiTISC-StationLibrariesSynchronised";
const std::string TRAIN_LIBRARIES_SYNCHRONISED_DP_NAME = "diiTISC-TrainLibrariesSynchronised";

const std::string STIS_SERVER_EQUIPMENT_NAME = "SEV";
const std::string OCC_STIS_SERVER_EQUIPMENT_NAME = "SEV_OCC";
const std::string ISCS_SERVER_EQUIPMENT_NAME = "ISCS";


const std::string SFTP_HOME = "";
// const std::string STISManager::SFTP_HOME = "/u01/transactive"; // for test
const std::string TIME_SCHEDULE_UPLOAD_FOLDER = "/ISCS/TIME_SCHEDULE/";
const std::string TIME_SCHEDULE_FILE_NAME = "TS_FILE.txt";
const std::string STIS_LIBRARY_DOWNLOAD_FOLDER = "/ISCS/PMLIBRARY/";

const std::string LOCATION_AUDIT_MESSAGE_PARAMETER_NAME = "Location";
const std::string PIDCONTROL_LOCATION_AUDIT_MESSAGE_PARAMETER_NAME = "location";
const std::string MESSAGE_TAG_AUDIT_MESSAGE_PARAMETER_NAME = "ID";
const std::string ADHOC_MESSAGE_AUDIT_MESSAGE_PARAMETER_NAME = "Message";
const std::string LIBRARY_VERSION_AUDIT_MESSAGE_PARAMETER_NAME = "Version";
const std::string DESTINATION_AUDIT_MESSAGE_PARAMETER_NAME = "Destination";
const std::string TYPE_AUDIT_MESSAGE_PARAMETER_NAME = "type";
const std::string MESSAGE_AUDIT_MESSAGE_PARAMETER_NAME = "Message";
const std::string RATISTAG_AUDIT_MESSAGE_PARAMETER_NAME = "Ratis Tag";
const std::string ENABLE_STATUS_AUDIT_MESSAGE_PARAMETER_NAME = "Enabled/Disabled";
const std::string STATUS_AUDIT_MESSAGE_PARAMETER_NAME = "on/off";
const std::string RATISDESTINATION_AUDIT_MESSAGE_PARAMETER_NAME = "Ratis destination";
const std::string APPROVAL_STATUS_AUDIT_MESSAGE_PARAMETER_NAME = "Approved/Rejected";
const std::string PID_AUDIT_MESSAGE_PARAMETER_NAME = "PID";
const std::string PRIORITY_AUDIT_MESSAGE_PARAMETER_NAME = "Priority";
const std::string STARTTIME_AUDIT_MESSAGE_PARAMETER_NAME = "Start time";
const std::string ENDTIME_AUDIT_MESSAGE_PARAMETER_NAME = "End time";
const std::string OVERRIDE_AUDIT_MESSAGE_PARAMETER_NAME = "Override";
const std::string VETTED_AUDIT_MESSAGE_PARAMETER_NAME = "Vetted";
const std::string REPEAT_INTERVAL_AUDIT_MESSAGE_PARAMETER_NAME = "Repeat interval";
const std::string DISPLAY_ATTRIBUTE_AUDIT_MESSAGE_PARAMETER_NAME = "Display attribute";
const std::string VIRTUAL_DATAPOINT_ADDRESS_NAME = "VIRTUAL";

const std::string EVENT_PARAMETER_TITLE_ID = "TitleID";
const std::string EVENT_PARAMETER_START_DATE_TIME = "StartDateTime";
const std::string EVENT_PARAMETER_END_DATE_TIME = "EndDateTime";
const std::string EVENT_PARAMETER_PRIORITY = "Priority";
const std::string EVENT_PARAMETER_STN_PID_INFO = "STN_PID_INFO";
const std::string EVENT_PARAMETER_MESSAGE_CONTENT = "MessageContent";
const std::string EVENT_PARAMETER_CLEAR_TYPE = "ClearType";
const std::string EVENT_PARAMETER_PID_STATUS = "State";

#endif