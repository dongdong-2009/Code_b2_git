#pragma once

#include <boost/array.hpp>
#include <string>

const int LOCAL_MONITOR_INDEX = 0;
const int MAX_STATION_MONITORS = 5;
const int MAX_OCC_MONITORS = 7;
#define RPARAM_DISPLAYMONITOR "DisplayMonitor"
#define RPARAM_MONITOR_ID "MonitorId" // 0 to 4 in station, 0 to 6 in occ
#define RPARAM_CLEAR_MONITOR "ClearMonitor"

struct RunParamStrut 
{
	std::string name;
	std::string value;
};

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

#define FULLSCREEN_REQUEST (WM_USER + 116)


// pan
const unsigned short G_PAN_LEFT = 1;
const unsigned short G_PAN_RIGHT = 2;

// tilt
const unsigned short G_TILT_UP = 1;
const unsigned short G_TILT_DOWN = 2;

// zoom
const unsigned short G_ZOOM_IN = 1;
const unsigned short G_ZOOM_OUT = 2;

// pan and tilt
const unsigned short G_PT_LEFTUP = 1;
const unsigned short G_PT_RIGHTUP = 2;
const unsigned short G_PT_LEFTDOWN = 3;
const unsigned short G_PT_RIGHTDOWN = 4;

// typedef
namespace TA_IRS_App
{
	class Monitor;
}

typedef boost::array<TA_IRS_App::Monitor*, 4> VirtualQuadArray;
typedef boost::array<TA_IRS_App::Monitor*, 25> AllMonitorsArray;
