/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/application_types/src/apptypes.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef APP_TYPES_H
#define APP_TYPES_H


enum AppTypes
{
    ADMIN_GUI_APPTYPE                               =  1,
    ALARM_GUI_APPTYPE                               =  2,
    CCTV_GUI_APPTYPE                                =  3,
    CONTROLROOM_GUI_APPTYPE,                      //=  4,
    EVENT_GUI_APPTYPE,                            //=  5,
    HISTORIAN_GUI_APPTYPE,                        //=  6,
    MAP_GUI_APPTYPE,                              //=  7,
    NETMON_GUI_APPTYPE,                           //=  8,
    OPLOGS_GUI_APPTYPE,                           //=  9,
    SIGNS_GUI_APPTYPE,                            //= 10,
    VIDIWALL_GUI_APPTYPE,                         //= 11,
    IRPLANS_GUI_APPTYPE,                          //= 12,
    AREAALLOC_GUI_APPTYPE,                        //= 13,
    GFF_GUI_APPTYPE,                              //= 14,
    IRPCONFIG_GUI_APPTYPE,                        //= 15,
    BACKUP_GUI_APPTYPE,                           //= 16,
    SIGN_VIEWER_GUI_APPTYPE,                      //= 17,
    SCENES_EDITOR_GUI_APPTYPE,                    //= 18,
    ALARM_SCHED_GUI_APPTYPE,                      //= 19,
    SEND_EMSERVMESG_GUI_APPTYPE                     = 22,
    TREND_VIEWER_GUI_APPTYPE                        = 23,
    DATA_POINT_PANEL_GUI_APPTYPE                    = 24,
    OPC_BRIDGE_APPTYPE                              = 27,
    PLAN_MANAGER_GUI_APPTYPE                        = 29,
    BANNER_GUI_APPTYPE                              = 30,
    INSPECTOR_PANEL_GUI_APPTYPE                     = 31,
    RADIO_CONTROLLER_GUI_APPTYPE                    = 32,
    CONFIG_EDITOR_GUI_APPTYPE                       = 33,
    TELEPHONE_CONTROLLER_GUI_APPTYPE                = 34,
    STIS_MANAGER_GUI_APPTYPE                        = 35,
    PLAN_SCHEDULER_GUI_APPTYPE                      = 36,
    DUTY_MANAGER_GUI_APPTYPE                        = 37,
    TIS_LOG_VIEWER_GUI_APPTYPE                      = 38,
    TTIS_MANAGER_GUI_APPTYPE                        = 39,
    ECS_STATION_MODES_GUI_APPTYPE                   = 40,				
    ECS_STATION_MODES_OVERVIEW_GUI_APPTYPE          = 41,				
    ECS_NORMAL_MASTER_MODES_GUI_APPTYPE             = 42,				
    ECS_EMERGENCY_MASTER_MODES_GUI_APPTYPE          = 43,
    ECS_EMERGENCY_MASTER_MODES_FOR_ZONE_GUI_APPTYPE = 44,
    ATS_ECS_AGENT_SIMULATOR_GUI_APPTYPE             = 45,
    EQUIP_STATUS_VIEWER_GUI_APPTYPE                 = 46,
    PA_MANAGER_GUI_APPTYPE                          = 47,
    REPORT_MANAGER_GUI_APPTYPE	                    = 48,
    ARCHIVE_MANAGER_APPTYPE                         = 49,
    INCIDENT_MANAGER_GUI_APPTYPE                    = 50,
    ECS_CONGESTION_PARAMETERS_APPTYPE               = 51,
    ECS_STATION_TIMETABLE_APPTYPE                   = 52,
    JOB_REQUEST_MANAGER                             = 53,
    CONTROL_STATION                                 = 54,
	MMS_CONTROLLER_GUI_APPTYPE						= 56,
    SCHEDULING_MANAGER                              = 57,
    REGION_MANAGER_GUI_APPTYPE                      = 58,
    INTEGRATED_ALARM_GUI_APPTYPE                    = 59,
    DVR_GUI_APPTYPE                                 = 60,
    ECS_OVERALL_TIMETABLE_SETTING_GUI_APPTYPE       = 61,
    TRAIN_PA_MANAGER_GUI_APPTYPE                    = 63,
    TRAIN_PID_MANAGER_GUI_APPTYPE                   = 64,
	TELEPHONE_MANAGER_GUI_APPTYPE                   = 65,
    PID_MANAGER_GUI_APPTYPE                         = 75,
    RADIO_MANAGER_GUI_APPTYPE                       = 76,
    INTERNET_EXPLORER_APPTYPE                       = 77,
	PID_LOG_VIEWER_GUI_APPTYPE                      = 78,
	POWER_DEMAND_EDITOR_GUI_APPTYPE					= 79, //14000
	TRN_MANAGER_GUI_APPTYPE							= 80,	
    TRAIN_MANAGER_GUI_APPTYPE					    = 81,
	HIERARCHICAL_ALARM_VIEWER_GUI_APPTYPE           = 82,
	PID_PREVIEW_APPTYPE								= 83 //TD17160
};

#endif
