#include <map>
#include <string>
#include <vector>
#include "AQCommonDef.h"

#ifndef __DB_SYNC_SIM_COMMON_H_INCLUDED__
#define __DB_SYNC_SIM_COMMON_H_INCLUDED__

#define		WMU_SETUP_CLIENT			WM_USER + 500
#define		WMU_CLIENT_STATUS_UPDATE	WM_USER + 501	// Not in use
#define		WMU_CLIENT_MSGID_UPDATE		WM_USER + 502	// Not in use
#define		WMU_CLIENT_UPDATE			WM_USER + 503

#define TEST_RUNNING_TIMER 99
#define TIME_INTERVAL 1000

//define colors
const COLORREF g_clrRef[] =
{
	RGB(0,255,0),		//Green
	RGB(255,255,0),		//Yellow
	RGB(255,0,0),		//Red	
	RGB(0,255,0)		//Green	//nSimStatus
};

enum CLIENT_STATUS
{
	STATUS_GOOD,
	STATUS_PAUSED,
	STATUS_ERROR,
	STATUS_FINISHED
};

const std::string g_strStatus [] =
{
	"NORMAL",
	"PAUSED",
	"ERROR",
	"FINISHED"
};

enum CLIENT_UPDATE_TYPE
{
	DEQUEUE_UPDATE,
	ENQUEUE_UPDATE,
	CONNECTION_UPDATE
};

struct ClientUpdate 
{
	std::string	strClientName;
	CLIENT_UPDATE_TYPE nUpdateType;
	CLIENT_STATUS nStatus;
	TA_AQ_Lib::ulong64 ulMgsID;
};

struct ClientInfo
{
	ClientInfo::ClientInfo()
	{
		strClientName = "";
		nEnqueueStatus = STATUS_GOOD;
		nDequeueStatus = STATUS_GOOD;
		nConnStatus = STATUS_GOOD;
		nSimStatus = STATUS_GOOD;

		ulEnqueueMsgID = 0;
		ulDequeueMsgID = 0;
		ulEnqueueErrCount = 0;
		ulDequeueErrCount = 0;
		ulEnqTotalMgs = 0;
		ulDeqTotalMgs = 0;

	}

	std::string		strClientName;
	CLIENT_STATUS	nEnqueueStatus;
	CLIENT_STATUS	nDequeueStatus;
	CLIENT_STATUS	nConnStatus;
	CLIENT_STATUS	nSimStatus;

	TA_AQ_Lib::ulong64	ulEnqueueMsgID;
	TA_AQ_Lib::ulong64	ulDequeueMsgID;
	TA_AQ_Lib::ulong64	ulEnqTotalMgs;
	TA_AQ_Lib::ulong64	ulDeqTotalMgs;
	ULONG ulEnqueueErrCount;
	ULONG ulDequeueErrCount;

};

typedef std::vector< ClientInfo* > T_ClientInfo_Vec;
typedef std::map<int, std::string > T_CLIENT_MAP;
typedef std::map<std::string, ClientInfo*> T_ClientInfo_Map;

#endif //__DB_SYNC_SIM_COMMON_H_INCLUDED__