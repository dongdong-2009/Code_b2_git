/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveStepsHeader.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef ACTIVE_STEPS_HEADER_INCLUDED
#define ACTIVE_STEPS_HEADER_INCLUDED

#include "app/response_plans/common/src/ProjectDefinition.h"


#if defined ( STEPTYPE_ABORT )
#include "app/response_plans/plan_agent/src/ActiveAbortStep.h"
#endif //STEPTYPE_ABORT

#if defined ( STEPTYPE_ACTIVE_PLAN_CHECK )
#include "app/response_plans/plan_agent/src/ActiveActivePlanCheckStep.h"
#endif //STEPTYPE_ACTIVE_PLAN_CHECK

#if defined ( STEPTYPE_DATA_POINT_CHECK )
#include "app/response_plans/plan_agent/src/ActiveDataPointCheckStep.h"
#endif //STEPTYPE_DATA_POINT_CHECK

#if defined ( STEPTYPE_DATA_POINT_SET )
#include "app/response_plans/plan_agent/src/ActiveDataPointSetStep.h"
#endif //STEPTYPE_DATA_POINT_SET

#if defined ( STEPTYPE_DECISION )
#include "app/response_plans/plan_agent/src/ActiveDecisionStep.h"
#endif //STEPTYPE_DECISION

#if defined ( STEPTYPE_JUMP )
#include "app/response_plans/plan_agent/src/ActiveJumpStep.h"
#endif //STEPTYPE_JUMP

#if defined ( STEPTYPE_LAUNCH_GUI_APP )
#include "app/response_plans/plan_agent/src/ActiveLaunchGuiAppStep.h"
#endif //STEPTYPE_LAUNCH_GUI_APP

#if defined ( STEPTYPE_LAUNCH_SCHEMATIC )
#include "app/response_plans/plan_agent/src/ActiveLaunchSchematicStep.h"
#endif //STEPTYPE_LAUNCH_SCHEMATIC

#if defined ( STEPTYPE_LOG )
#include "app/response_plans/plan_agent/src/ActiveLogStep.h"
#endif //STEPTYPE_LOG

#if defined ( STEPTYPE_PROMPT )
#include "app/response_plans/plan_agent/src/ActivePromptStep.h"
#endif //STEPTYPE_PROMPT

#if defined ( STEPTYPE_RUN_PLAN )
#include "app/response_plans/plan_agent/src/ActiveRunPlanStep.h"
#endif //STEPTYPE_RUN_PLAN

#if defined ( STEPTYPE_TERMINATE_PLAN )
#include "app/response_plans/plan_agent/src/ActiveTerminatePlanStep.h"
#endif //STEPTYPE_TERMINATE_PLAN

#if defined ( STEPTYPE_EVALUATE_EXPRESSION )
#include "app/response_plans/plan_agent/src/ActiveEvaluateExpressionStep.h"
#endif //STEPTYPE_EVALUATE_EXPRESSION

#if defined ( STEPTYPE_VMS_SET_SCENE )
#include "app/response_plans/plan_agent/src/ActiveVmsSetSceneStep.h"
#endif //STEPTYPE_VMS_SET_SCENE

#if defined ( STEPTYPE_VMS_RESTORE_SCHEDULE )
#include "app/response_plans/plan_agent/src/ActiveVmsRestoreScheduleStep.h"
#endif //STEPTYPE_VMS_RESTORE_SCHEDULE

#if defined ( STEPTYPE_VMS_BLANKOUT )
#include "app/response_plans/plan_agent/src/ActiveVmsBlankoutStep.h"
#endif //STEPTYPE_VMS_BLANKOUT

#if defined ( STEPTYPE_ASSIGN_CAMERA_TO_MONITOR )
#include "app/response_plans/plan_agent/src/ActiveAssignCameraToMonitorStep.h"
#endif //STEPTYPE_ASSIGN_CAMERA_TO_MONITOR

#if defined ( STEPTYPE_ACTIVATE_CAMERA_PRESET )
#include "app/response_plans/plan_agent/src/ActiveAssignCameraToPresetStep.h"
#endif //STEPTYPE_ACTIVATE_CAMERA_PRESET

#if defined ( STEPTYPE_VIDEOWALL )
#include "app/response_plans/plan_agent/src/ActiveVideoWallStep.h"
#endif //STEPTYPE_VIDEOWALL

#if defined ( STEPTYPE_STATION_PUBLIC_ADDRESS )
#include "app/response_plans/plan_agent/src/ActiveStationPaStep.h"
#endif //STEPTYPE_STATION_PUBLIC_ADDRESS

#if defined ( STEPTYPE_TRAIN_PUBLIC_ADDRESS )
#include "app/response_plans/plan_agent/src/ActiveTrainPaStep.h"
#endif //STEPTYPE_TRAIN_PUBLIC_ADDRESS

#if defined ( STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM )
#include "app/response_plans/plan_agent/src/ActiveStationTisStep.h"
#endif //STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM

#if defined ( STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM )
#include "app/response_plans/plan_agent/src/ActiveTrainTisStep.h"
#endif //STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM

#if defined ( STEPTYPE_FAILURE_SUMMARY_STEP)
#include "app/response_plans/plan_agent/src/ActiveFailureSummaryStep.h"
#endif //STEPTYPE_FAILURE_SUMMARY_STEP
#endif // ACTIVE_STEPS_HEADER_INCLUDED
