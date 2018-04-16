#pragma once

#include <string>
#include <map>
#include "bus/cctv/video_switch_agent/idl/src/CameraCorbaDef.h"
#include "bus/cctv/video_switch_agent/idl/src/VideoOutputCorbaDef.h"

const int LOCAL_MONITOR_INDEX = 0;
const int MAX_STATION_MONITORS = 5;
const int MAX_OCC_MONITORS = 29;
const int QUAD_START_INDEX = 1000;		//OCC quad start from 1000. station can start from 1 or 1000, it is also for DisplayOrder
const int SINGLEOPS_START_INDEX = 2000;		//OCC quad start from 1000. station can start from 1 or 1000, it is also for DisplayOrder
const int INDEX_INTERVAL = 1000;		//index interval for different monitor types
const int NUMBER_WALL_MONITORS = 24;
const int NUMBER_SINGLE_WALL_MONITORS = 6;

const std::string DESC_SINGLE_WALL_MONITOR = "SINGLE";
const std::string DESC_SPLIT_WALL_MONITOR = "SPLIT";

#define RPARAM_DISPLAYMONITOR "DisplayMonitor"
#define RPARAM_MONITOR_ID "MonitorId" // 0 to 4 in station, 0 to 6 in occ
#define RPARAM_CLEAR_MONITOR "ClearMonitor"
#define RPARAM_ENTITY "Entity"
#define RPARAM_PLAYBACK "Playback"
#define DISPLAY_MANAGER_NAME "闭路电视系统管理器"
#define OCC_DISPLAY_MANAGER_NAME "闭路电视系统管理器(控制中心)"
#define STATION_DISPLAY_MANAGER_NAME "闭路电视系统管理器(车站)"
#define PTZREQUESTINTERVALEPRAMETERNAME "PtzRequestInterval"
#define VIDEOASPECTRATIOPARAMETERNAME "VideoAspectRatio"

namespace TA_IRS_App
{

	//const COLORREF MONITOR_INDEX_TEXT_COLOR = 0x000000FF;
	static char CAPTURE_CARD_DEVICE_NAME[] = "SA7160 PCI";

    struct RunParamStrut
    {
        std::string name;
        std::string value;
    };

	class Camera;
	class Sequence;
	class Monitor;
    typedef std::map<std::string, Camera*> NameToCameraMap;
    typedef std::map<unsigned long, Camera*> KeyToCameraMap;
    typedef NameToCameraMap AddressToCameraMap;
    typedef std::map<std::string, Sequence*> NameToSequenceMap;
    typedef std::map<unsigned long, Sequence*> KeyToSequenceMap;
    typedef std::map<std::string, Monitor*> NameToMonitorMap;
    typedef std::map<unsigned long, Monitor*> KeyToMonitorMap;

    // these are the post messages that will be sent when items are changed
#define WM_MONITOR_UPDATED  (WM_USER + 101)
#define WM_SEQUENCE_UPDATED (WM_USER + 102)
#define WM_QUAD_UPDATED     (WM_USER + 103)
#define WM_CAMERA_UPDATED   (WM_USER + 104)
#define WM_STAGE_UPDATED    (WM_USER + 105)

    // sent as things are loaded on initialisation
#define TA_INIT_CAMERAS     (WM_USER + 106)
#define TA_INIT_QUADS       (WM_USER + 107)
#define TA_INIT_SEQUENCES   (WM_USER + 108)
#define TA_INIT_STAGES      (WM_USER + 109)
#define TA_INIT_MONITORS    (WM_USER + 110)
#define TA_INIT_CCTV        (WM_USER + 111)
#define TA_INIT_STATION     (WM_USER + 112)
#define TA_RUN_PARAM        (WM_USER + 113)

    // Lock status change message
#define LOCK_STATUS_CHANGE (WM_USER + 114)

#define REFRESH_SCHEMATIC_MESSAGE (WM_USER + 115)

	enum PTZDirection { PTZ_LEFT, PTZ_RIGHT, PTZ_UP, PTZ_DOWN, PTZ_LEFTUP, PTZ_RIGHTUP, PTZ_LEFTDOWN, PTZ_RIGHTDOWN, PTZ_ZOOMIN, PTZ_ZOOMOUT, PTZ_ALLSTOP /*, __max_PTZCtrlCmd=0xffffffff */ };

    // pan
    /*const TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd G_PAN_LEFT = TA_Base_Bus::CameraCorbaDef::PAN_LEFT_WX;
    const TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd G_PAN_RIGHT = TA_Base_Bus::CameraCorbaDef::PAN_RIGHT_WX;

    // tilt
    const TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd G_TILT_UP = TA_Base_Bus::CameraCorbaDef::TILT_UP_WX;
    const TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd G_TILT_DOWN = TA_Base_Bus::CameraCorbaDef::TILT_DOWN_WX;

    // zoom
    const TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd G_ZOOM_IN = TA_Base_Bus::CameraCorbaDef::ZOOM_IN_WX;
    const TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd G_ZOOM_OUT = TA_Base_Bus::CameraCorbaDef::ZOOM_OUT_WX;
*/
	struct PlayBackTimePair
	{
		TA_Base_Bus::VideoOutputCorbaDef::Time startTime;
		TA_Base_Bus::VideoOutputCorbaDef::Time endTime;
	};
}