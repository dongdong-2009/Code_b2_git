
#ifndef _GLOBAL_DATA_TYPES_H_
#define _GLOBAL_DATA_TYPES_H_
#include <string>
#include <vector>

#define WM_UPDATE_LOGGER WM_APP + 1000
#define WM_UIUPDATE		WM_APP  + 1001
#define WM_PDSVIEW     WM_APP  + 1002
#define WM_HEARTBEAT_INFO WM_APP + 1003
#define WM_HEARTBEAT WM_APP + 1004

#define PARAM_FILE_NAME			"ClientInfo.ini"

#define SECTIONNAME_IP			"IPAddress"
#define SECTIONNAME_PORT		"PORT"
#define COMMON_KEY_NAME			"1"

#define BEGIN					1
#define REPEAT					2

#define FRAME_RECV				1
#define FRAME_SEND				2

#pragma warning (disable:4786)	

struct ListInfo
{
	std::string Title;
	std::string Message;
};

struct LogInfo
{
	std::string Time;
	std::string LogMessage;
};
struct MessageInfo
{
	unsigned int ID;
	std::string BtnCaption;
	ListInfo ListInfoMessage; 
	LogInfo LogInfoMessage;
};

struct ResourceInfo
{
	HWND HanderWND;
	unsigned int ID;
	HWND HandlerWNDHeartBeat;
};

struct MessageEventInfo
{
	unsigned char CommandValue;
	unsigned char TrainNumber;
	unsigned char Data1;
	std::vector<unsigned char> Data2;

};

struct BeginDownloadData
{
	int trainID;
	std::string StringData;
	std::vector<unsigned char> Data;
};

enum TrainStatus
{
	START = 0,
	STOP,
	REMOVE
};

enum TrainStrategyType
{
	TIMER_SEND = 0,
	TIMER_TIMEOUT,
	TIMER_DEFAULT
	
};

struct TrainStrategyInfo
{
	int TrainID;
	TrainStrategyType StrategyType;
	int TimerSet;
};

// enum eTIMSCommand
// {
// 	END_OF_TTIS_DOWNLOAD = 0,
// 	CHANGEOVER_STATUS,
// 	INVALID_COMMAND
// };

#endif