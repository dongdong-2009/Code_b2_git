#ifndef _GENA_ATS_CONSTANTS_H_
#define _GENA_ATS_CONSTANTS_H_

#include "bus/protocol/Gena/src/CommonDef.h"

namespace TA_IRS_App
{
	//Train Attributes
	#define TRAINCONGESTION		"TravelTimeExceeded/TravelTimeExceeded/"
	#define TRAINSERVICENUM		"TrainLabel/"
	#define TRAINDIRECTION		"CurrentDir/"
	#define DRIVEMODE			"OperationMode/"
	#define TRAINEDGEID			"FrontPos/parentEdge/"
	#define BRANCHOFFSET		"FrontPos/meter/"
	//Platform Attributes
	#define FIRSTTRAINARRTIME	"ArrivalTime/"
	#define OPENSTATUS			"OpenStatus/"
	#define HOLDSTATUS			"Hold/"
	//PA Attributes
	#define ARRIVALTIME			"ArrivalTime"
	#define DEPARTURETIME		"DepartureTime"
	#define FINALDESTINATION	"FinalDestination"
	//PIDLOCATION PIS Attributes
	#define TRAINLIST			"TrainList"	
	#define PIDDATA				"PIDData"
	#define TRAINLIST_PIDDATA				"TrainList/PIDData"
	#define TRAINID				TRAINLIST_PIDDATA##"/TrainID"
	//Signal Attributes
	#define STATUS				"Status/"
	#define HEALTHSTATUS		"HealthStatus/"
	//Block Attributes
	#define BLKSTATUS			"Status/"
	//Switch Attributes
	#define SWPOSITION			"Position/"
	#define FAILEDSTATUS		"FailedStatus/" 
	#define RESSTATUS			"ResStatus/"
	//EmergencyStopButton Attributes
	#define ESBSTATUS			"Status/"
	//FloodGate Attributes
	#define REQUESTCLOSE		"RequestedStatus/"
	#define PERMISSIONCLOSE		"PermissionToCloseStatus/"
	//PowerStatus
	#define POWER_STATUS_ATTR			"PowerStatus/"
	#define POWER_SECTION_TOPIC_NAME	"PowerSection"
	#define POWER_TOPIC_URL				"/PowerSection?attr=PowerStatus"
	//PlatformDoor
	#define OVERRIDE_STATUS		"OverrideStatus/"

	struct time {
		int tm_sec;     /* seconds after the minute - [0,59] */
		int tm_min;     /* minutes after the hour - [0,59] */
		int tm_hour;    /* hours since midnight - [0,23] */
		int tm_mday;    /* day of the month - [1,31] */
		int tm_mon;     /* months since January - [0,11] */
		int tm_year;    /* years since 1900 */
		int	tm_msec;	/* msec */
	};

	enum ServerConnStatus
	{
		ConnStat_BAD,
		ConnStat_PASSIVE,
		ConnStat_GOOD
	};

}

#endif