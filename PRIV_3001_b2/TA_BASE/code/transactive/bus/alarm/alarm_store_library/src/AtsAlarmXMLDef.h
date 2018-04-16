
//This header file contains all macro definition for Ats XML tags

#include "boost/shared_ptr.hpp"
#include <string>

#define XML_VERSION				"1.0"
#define XML_ENCODING			"UTF-8"

#define ATTRIB_VERSION			"version"
#define ATTRIB_ENCODING			"encoding"

#define ELEM_BODY				"body"
#define ELEM_ALARM				"alarm"

#define ELEM_SOURCE_TIME		"sourceTime"
#define ELEM_TIME_PART			"time"
#define ELEM_MILI_PART			"mili"

#define ELEM_ALARMID		"alarmID"
#define ELEM_ACK_TIME		"ackTime"
#define ELEM_CLOSE_TIME		"closeTime"
#define ELEM_ASSET_NAME		"assetName"
#define ELEM_SEVERITY		"alarmSeverity"
#define ELEM_DESCRIPTION	"alarmDescription"
#define ELEM_ACK_BY			"alarmAcknowledgeBy"
#define ELEM_STATE			"state"
#define ELEM_LOCATIONID		"locationId"
#define ELEM_PARENT_ALARMID	"parentAlarmID"
#define ELEM_AV_HEADID		"avalancheHeadID"
#define ELEM_IS_AV_HEAD		"isHeadOfAvalanche"
#define ELEM_IS_CHILD_AV	"isChildOfAvalanche"
#define ELEM_MMS_STATE		"mmsState"
#define ELEM_ALARM_VALUE	"alarmValue"
#define ELEM_OM_ALARM		"omAlarm"
#define ELEM_ALARM_TYPE		"alarmType"
#define ELEM_SUBSYSTEM_KEY	"subsytemkey"
#define ELEM_SYSTEM_KEY		"systemkey"
#define ELEM_COMMENTS		"alarmComments"
#define ELEM_STR_ALARM_TYPE	"strAlarmType"
#define ELEM_SUBSYSTEM_TYPE	"subsytemType"
#define ELEM_SYSTEMKEY_TYPE	"systemkeyType"
#define ELEM_LOCATION_KEY	"locationKey"

struct AtsXmlData
{
	std::string sourceTime_time;  // The alarm generated time
	std::string sourceTime_mili;  // The alarm generated time
	std::string alarmID;         // An unique alarm id to differentiate each alarm generated, max 64 characters.
	std::string ackTime;       // the acknowledge time if any
	std::string closeTime;     // the close time if any
	std::string assetName;       // the asset name(equipment ID), max 40 characters.
	std::string alarmSeverity; // the severity of the alarm
	std::string alarmDescription;// the description of the alarm, max 200 characters.
	std::string alarmAcknowledgeBy; // the operator who acknowledge this alarm, max 10 characters
	std::string state;           // the alarm state
	std::string locationId;     // the location (Sect D.5 Station Code Name) that alarm is generated, max 6 characters.
	std::string parentAlarmID;  // the parent alarm ID
	std::string avalancheHeadID;// related to the avalanche head if applicable
	std::string isHeadOfAvalanche;     // whether the alarm is the head of avalanche
	std::string isChildOfAvalanche;    // whether the alarm is the child of avalanche
	std::string mmsState;      // the MMS state
	std::string alarmValue;     // the value of the data point related to the alarm, max 16 characters.
	std::string omAlarm;               // ¡®O¡¯ for Operation Alarm, ¡®M¡¯ for maintenance alarm, or ¡®B¡¯ for both.
	std::string alarmType;    // the PKey of alarm type
	std::string subsytemkey;  // the PKey of subsystem
	std::string systemkey;    // the PKey of system		
	std::string alarmComments;  // comments of alarm, max 200 charactrs.
	std::string strAlarmType;   // the PKey of alarm type
	std::string subsytemType;	// the PKey of subsystem
	std::string systemkeyType;  // the PKey of system	
	std::string locationKey;  // location key // Added by Noel 
	AtsXmlData():alarmID(""),assetName(""),alarmDescription(""),
		alarmAcknowledgeBy(""),locationId(""),parentAlarmID(""),
		avalancheHeadID(""),alarmValue(""),alarmComments(""),
		strAlarmType(""),subsytemType(""),systemkeyType(""),
		sourceTime_time(""),sourceTime_mili(""),ackTime(""),
		closeTime(""),alarmSeverity(""),state(""),
		isHeadOfAvalanche(""),mmsState(""),omAlarm(""),alarmType(""),
		subsytemkey(""),systemkey(""),locationKey("")
	{
	}

};

typedef boost::shared_ptr<AtsXmlData> AtsXmlDataPtr;