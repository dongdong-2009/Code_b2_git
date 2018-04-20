//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/common/src/ProjectDefinition.h $
// @author:  Mahaveer Pareek
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
//
// This file contains project-specific preprocessor definitions used 
// for conditional compilation of Plan Agent and Plan Manager.
//
// Each project should uncomment the relevant PROJECT_XXXX definition.
//
// STEPTYPE_* definitions control the types of plan step that are 
// supported within each project.
//

#ifndef PROJECT_DEFINITION_HEADER_INCLUDED
#define PROJECT_DEFINITION_HEADER_INCLUDED


// ----------------------------------------------
// Project definitions - uncomment as appropriate
// ----------------------------------------------
//#define  PROJECT_4619
//#define  PROJECT_4661
#define  PROJECT_4669
//#define  PROJECT_4680
// ----------------------------------------------


//Comment out the definition of DEVELOPMENT_BUILD for RELEASE BUILD
//#define DEVELOPMENT_BUILD

//
#define PLANEDITLOCK_TIMEOUT_SECONDS    60

#if defined ( PROJECT_4619 )
    #define STEPTYPE_ABORT
    #define STEPTYPE_ACTIVE_PLAN_CHECK
    #define STEPTYPE_DECISION
    #define STEPTYPE_JUMP
    #define STEPTYPE_LAUNCH_GUI_APP
    #define STEPTYPE_LOG
    #define STEPTYPE_PROMPT
    #define STEPTYPE_RUN_PLAN
    #define STEPTYPE_TERMINATE_PLAN

    //External Equipment Steps
    #define STEPTYPE_DATA_POINT_CHECK
    #define STEPTYPE_DATA_POINT_SET
    #define STEPTYPE_ASSIGN_CAMERA_TO_MONITOR    
    #define STEPTYPE_TIS_SEND_MESSAGE_TO_SIGN
    #define STEPTYPE_STATION_PUBLIC_ADDRESS
    #define STEPTYPE_TRAIN_PUBLIC_ADDRESS
    #define STEPTYPE_SEND_SMS_MESSAGE
#endif

//Steps included for Project 4661
#if defined ( PROJECT_4661 )
    #define STEPTYPE_ABORT
    #define STEPTYPE_ACTIVE_PLAN_CHECK
    #define STEPTYPE_DATA_POINT_CHECK
    #define STEPTYPE_DATA_POINT_SET
    #define STEPTYPE_DECISION
    #define STEPTYPE_JUMP
    #define STEPTYPE_LAUNCH_GUI_APP
    #define STEPTYPE_LOG
    #define STEPTYPE_PROMPT
    #define STEPTYPE_RUN_PLAN
    #define STEPTYPE_TERMINATE_PLAN
    #define STEPTYPE_VMS_SET_SCENE
#endif


//Steps included for Project 4669
#if defined ( PROJECT_4669 )
    #define STEPTYPE_ABORT
    #define STEPTYPE_ACTIVE_PLAN_CHECK
    #define STEPTYPE_DECISION
    #define STEPTYPE_JUMP
    #define STEPTYPE_LAUNCH_GUI_APP
    #define STEPTYPE_LAUNCH_SCHEMATIC
    #define STEPTYPE_LOG
    #define STEPTYPE_PROMPT
    #define STEPTYPE_RUN_PLAN
    #define STEPTYPE_TERMINATE_PLAN
	 #define STEPTYPE_EVALUATE_EXPRESSION
	//#define STEPTYPE_FAILURE_SUMMARY_STEP

    //External Equipment Steps
    #define STEPTYPE_DATA_POINT_CHECK
    #define STEPTYPE_DATA_POINT_SET
    #define STEPTYPE_ASSIGN_CAMERA_TO_MONITOR    
    #define STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM
    #define STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM
    #define STEPTYPE_STATION_PUBLIC_ADDRESS
    #define STEPTYPE_TRAIN_PUBLIC_ADDRESS
#endif


//Steps included for Project 4680
#if defined ( PROJECT_4680 )
    #define STEPTYPE_ABORT
    #define STEPTYPE_ACTIVE_PLAN_CHECK
    #define STEPTYPE_DECISION
    #define STEPTYPE_JUMP
    #define STEPTYPE_LAUNCH_GUI_APP
    #define STEPTYPE_LOG
    #define STEPTYPE_PROMPT
    #define STEPTYPE_RUN_PLAN
    #define STEPTYPE_TERMINATE_PLAN

    //External Equipment Steps
    #define STEPTYPE_DATA_POINT_CHECK
    #define STEPTYPE_DATA_POINT_SET
    //
    #define STEPTYPE_VMS_SET_SCENE
    #define STEPTYPE_VMS_BLANKOUT
    #define STEPTYPE_VMS_RESTORE_SCHEDULE
    #define STEPTYPE_ASSIGN_CAMERA_TO_MONITOR
    #define STEPTYPE_ACTIVATE_CAMERA_PRESET
    #define STEPTYPE_VIDEOWALL
#endif


#endif // PROJECT_DEFINITION_HEADER_INCLUDED
