//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/MessageBox.cpp $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2014/07/11 17:19:04 $
// Last modified by:  $Author: huangjian $
//
// <description>

#include "stdafx.h"
//#include "PlanManager.h"
#include "MessageBox.h"
//#include "app/response_plans/plan_agent/src/PlanNotFoundException.h"
#include "bus/response_plans/plan_agent/IDL/src/IPlanAgentCorbaDef.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/exceptions/src/RightsException.h"
#include "core/exceptions/src/PlanStepDetailException.h"
#include "core/exceptions/src/PlanStepException.h"
#include "core/exceptions/src/PlanAgentException.h"
#include "core/exceptions/src/RightsException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/ObjectResolutionException.h"

#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "bus\security\authentication_library\src\AuthenticationLibrary.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
using namespace TA_Base_Core;

/////////////////////////////////////////////////////////////////////////////
// MessageBox

void MessageBox::error(const unsigned long actionId, const TransactiveException& exception)
{
    FUNCTION_ENTRY( "error" );

    CString actionName;
    actionName.LoadString(actionId);

    displayError(actionName, exception);

    FUNCTION_EXIT;
}


void MessageBox::error(const unsigned long actionId, const CString& actionParam, const TransactiveException& exception)
{
    FUNCTION_ENTRY( "error" );

    CString actionName;
    AfxFormatString1(actionName, actionId, actionParam);

    displayError(actionName, exception);

    FUNCTION_EXIT;
}


void MessageBox::error(const unsigned long actionId)
{
    FUNCTION_ENTRY( "error" );

    // some generic "Cannot do x due to error" message.
    TA_Base_Bus::TransActiveMessage userMsg;
    CString actionName;
    actionName.LoadString(actionId);
    userMsg << actionName;
    userMsg.showMsgBox(IDS_UE_210100);

    FUNCTION_EXIT;
}


void MessageBox::displayError(const CString& actionName, const TransactiveException& exception)
{
    FUNCTION_ENTRY( "displayError" );

    // determine which type of error it is and display the error accordingly
    const RightsException* rightsException = dynamic_cast<const RightsException*>(&exception);
    if (rightsException)
    {
        displayPermissionError(actionName, *rightsException);

        FUNCTION_EXIT;
        return;
    }

    //const PlanNotFoundException* planNotFoundException = dynamic_cast<const PlanNotFoundException*>(&exception);
    //if (planNotFoundException)
    //{
    //    displayPlanNotFoundError(actionName, *planNotFoundException);

    //    FUNCTION_EXIT;
    //    return;
    //}

    //const PlanStepException* planStepException = dynamic_cast<const PlanStepException*>(&exception);
    //if (planStepException)
    //{
    //    displayPlanStepError(actionName, *planStepException);

    //    FUNCTION_EXIT;
    //    return;
    //}

    const PlanStepDetailException* planStepDetailException = dynamic_cast<const PlanStepDetailException*>(&exception);
    if (planStepDetailException)
    {
        displayPlanStepDetailError(actionName, *planStepDetailException);

        FUNCTION_EXIT;
        return;
    }

    const PlanAgentException* planAgentException = dynamic_cast<const PlanAgentException*>(&exception);
    if (planAgentException)
    {
        displayPlanAgentError(actionName, *planAgentException);

        FUNCTION_EXIT;
        return;
    }

    const ObjectResolutionException* objectResolutionException = dynamic_cast<const ObjectResolutionException*>(&exception);
    if (objectResolutionException)
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_210011);

        FUNCTION_EXIT;
        return;
    }

    // otherwise, it's not any of above, so just display with some generic error message.
    TA_Base_Bus::TransActiveMessage userMsg;
    userMsg << actionName;
    userMsg.showMsgBox(IDS_UE_210100);

    FUNCTION_EXIT;
}


void MessageBox::displayPermissionError(const CString& actionName, const RightsException& /*exception*/)
{
    FUNCTION_ENTRY( "displayPermissionError" );

    TA_Base_Bus::TransActiveMessage userMsg;
    std::string userName;
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_Base_Bus::AuthenticationLibrary authLibrary(true);
    TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
    std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
    userName = iOperator->getName();
    userMsg << userName;
    userMsg << actionName;
    userMsg.showMsgBox(IDS_UE_010021);

    FUNCTION_EXIT;
}


//void MessageBox::displayPlanNotFoundError(const CString& actionName, const PlanNotFoundException& exception)
//{
//    FUNCTION_ENTRY( "displayPlanNotFoundError" );
//
//    unsigned long messageId;
//
//    switch (exception.getErrorType())
//    {
//        case PNFE_FAIL_FETCH_ACTIVE_LIST_STEP:
//            messageId = IDS_UE_210047;
//            break;
//        case PNFE_INSTANCE_NOT_IN_BULLETIN_BOARD:
//            messageId = IDS_UE_210048;
//            break;
//        case PNFE_INSTANCE_NOT_FOUND:
//            messageId = IDS_UE_210049;
//            break;
//        case PNFE_NODE_NOT_EXIST:
//        default:
//            messageId = IDS_UE_210050;
//            break;
//    }
//
//    TA_Base_Bus::TransActiveMessage userMsg;
//    userMsg << actionName << exception.getPlanId() << exception.getInstanceId();
//    userMsg.showMsgBox(messageId);
//
//    FUNCTION_EXIT;
//}


//void MessageBox::displayPlanStepError(const CString& actionName, const PlanStepException& exception)
//{
//    FUNCTION_ENTRY( "displayPlanStepError" );
//
//    unsigned long resourceId;
//
//    switch (exception.getStepType())
//    {
//        case END_STEP:
//            resourceId = IDS_STEP_TYPE_END;
//            break;
//        case ABORT_STEP:
//            resourceId = IDS_STEP_TYPE_ABORT;
//            break;
//        case ACTIVATE_CAMERA_PRESET_STEP:
//            resourceId = IDS_STEP_TYPE_ACTIVATE_CAMERA_PRESET;
//            break;
//        case ACTIVE_PLAN_CHECK_STEP:
//            resourceId = IDS_STEP_TYPE_ACTIVE_PLAN_CHECK;
//            break;
//        case ASSIGN_CAMERA_TO_MONITOR_STEP:
//            resourceId = IDS_STEP_TYPE_ASSIGN_CAMERA_TO_MONITOR;
//            break;
//        case DATA_POINT_CHECK_STEP:
//            resourceId = IDS_STEP_TYPE_DATA_POINT_CHECK;
//            break;
//        case DATA_POINT_SET_STEP:
//            resourceId = IDS_STEP_TYPE_DATA_POINT_SET;
//            break;
//        case DECISION_STEP:
//            resourceId = IDS_STEP_TYPE_DECISION;
//            break;
//        case JUMP_STEP:
//            resourceId = IDS_STEP_TYPE_JUMP;
//            break;
//        case LAUNCH_GUI_APP_STEP:
//            resourceId = IDS_STEP_TYPE_LAUNCH_GUI_APP;
//            break;
//        case LAUNCH_SCHEMATIC_STEP:
//            resourceId = IDS_STEP_TYPE_LAUNCH_SCHEMATIC;
//            break;
//        case LOG_STEP:
//            resourceId = IDS_STEP_TYPE_LOG;
//            break;
//        case PROMPT_STEP:
//            resourceId = IDS_STEP_TYPE_PROMPT;
//            break;
//        case RUN_PLAN_STEP:
//            resourceId = IDS_STEP_TYPE_RUN_PLAN;
//            break;
//        case SEND_SMS_MESSAGE_STEP:
//            resourceId = IDS_STEP_TYPE_SEND_SMS_MESSAGE;
//            break;
//        case STATION_TRAVELLER_INFO_SYSTEM_STEP:
//            resourceId = IDS_STEP_TYPE_STATION_TRAVELLER_INFO_SYSTEM;
//            break;
//        case TERMINATE_PLAN_STEP:
//            resourceId = IDS_STEP_TYPE_TERMINATE_PLAN;
//            break;
//          case STATION_PUBLIC_ADDRESS_STEP:
//            resourceId = IDS_STEP_TYPE_STATION_PUBLIC_ADDRESS;
//            break;
//          case TRAIN_PUBLIC_ADDRESS_STEP:
//            resourceId = IDS_STEP_TYPE_TRAIN_PUBLIC_ADDRESS;
//            break;
//        case TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
//            resourceId = IDS_STEP_TYPE_TRAIN_TRAVELLER_INFO_SYSTEM;
//            break;
//          case VMS_RESTORE_SCHEDULE_STEP:
//            resourceId = IDS_STEP_TYPE_VMS_RESTORE_SCHEDULE;
//            break;
//        case VIDEO_WALL_STEP:
//            resourceId = IDS_STEP_TYPE_VIDEO_WALL;
//            break;
//        case VMS_SET_SCENE_STEP:
//            resourceId = IDS_STEP_TYPE_VMS_SET_SCENE;
//            break;
//        case VMS_BLANKOUT_STEP:
//            resourceId = IDS_STEP_TYPE_VMS_BLANKOUT;
//            break;
//        // lizettejl++ (DP-Changes)
//        case FAILURE_SUMMARY_STEP:
//            resourceId = IDS_STEP_TYPE_FAILURE_SUMMARY;
//            break;
//        // ++lizettejl
//        case PLAN_CHECK_TIMER_STEP:
//            resourceId = IDS_STEP_TYPE_PLAN_CHECK_TIMER;
//            break;
//        case PLAN_SET_TIMER_STEP:
//            resourceId = IDS_STEP_TYPE_PLAN_SET_TIMER;
//            break;
//        case  DATA_POINT_TIME_CHECK_STEP:
//            resourceId = IDS_STEP_TYPE_DATA_POINT_TIME_CHECK;
//            break;
//		case INCLUSIVE_GATEWAY_STEP:
//			resourceId = IDS_STEP_TYPE_INCLUSIVE_GATEWAY;
//			break;
//		case EXCLUSIVE_GATEWAY_STEP:
//			resourceId = IDS_STEP_TYPE_EXCLUSIVE_GATEWAY;
//			break;
//                /*
//        case MITSIM_GENERATE_INCIDENT_IMAGES_STEP:
//            resourceId = IDS_STEP_TYPE_MITSIM_GENERATE_INCIDENT_IMAGES;
//            break;
//        case GENERATE_REPORT_STEP:
//            resourceId = IDS_STEP_TYPE_GENERATE_REPORT;
//            break;
//        case LIVE_PROPERTY_CHECK_STEP:
//            resourceId = IDS_STEP_TYPE_LIVE_PROPERTY_CHECK;
//            break;
//        case LAUNCH_MAP_VIEWER_STEP:
//            resourceId = IDS_STEP_TYPE_LAUNCH_MAP_VIEWER;
//            break;
//        case START_RECORDING_STEP:
//            resourceId = IDS_STEP_TYPE_START_RECORDING;
//            break;
//        case STOP_RECORDING_STEP:
//            resourceId = IDS_STEP_TYPE_STOP_RECORDING;
//            break;
//        case ASSIGN_VIDEO_INPUT_TO_RECORDING_UNIT_STEP:
//            resourceId = IDS_STEP_TYPE_ASSIGN_VIDEO_INPUT_TO_RECORDING_UNIT;
//            break;
//        case CLEAR_VIDEO_WALL_STEP:
//            resourceId = IDS_STEP_TYPE_CLEAR_VIDEO_WALL;
//            break;
//            */
//        case UNDEFINED_STEP:
//        default:
//            resourceId = IDS_STEP_TYPE_UNDEFINED;
//            break;
//    }
//
//    TA_Base_Bus::TransActiveMessage userMsg;
//    if (resourceId == IDS_STEP_TYPE_UNDEFINED)
//    {
//        userMsg.showMsgBox(IDS_UE_210084);
//
//        FUNCTION_EXIT;
//        return;
//    }
//
//    CString stepType;
//    stepType.LoadString(resourceId);
//    userMsg << stepType;
//    userMsg.showMsgBox(IDS_UE_210082);
//
//    FUNCTION_EXIT;
//}


void MessageBox::displayPlanStepDetailError(const CString& actionName, const PlanStepDetailException& exception)
{
    FUNCTION_ENTRY( "displayPlanStepDetailError" );

    TA_Base_Bus::TransActiveMessage userMsg;
    userMsg << actionName << exception.getStepPosition() << exception.getDetailType() << std::string("invalid");
    userMsg.showMsgBox(IDS_UE_210051);

    FUNCTION_EXIT;
}


void MessageBox::displayPlanAgentError(const CString& actionName, const PlanAgentException& exception)
{
    FUNCTION_ENTRY( "displayPlanAgentError" );

    TA_Base_Bus::TransActiveMessage userMsg;

    if (!actionName.IsEmpty())
    {
        userMsg << actionName;
    }

    unsigned long resourceId = 0;

    switch (exception.getErrorType())
    {
        case PAE_EDIT_LOCKED_BY_OPERATOR:
            {
                try
                {
                    IOperator* op = OperatorAccessFactory::getInstance().getOperator(exception.getItemId(), false);
                    if (op != NULL)
                    {
                        userMsg << op->getName();
                    }
                    resourceId = IDS_UE_210052;
                }
                catch (...)
                {
                    // if there's any error, then just
                    resourceId = IDS_UE_210052;
                }
            }
            break;
        case PAE_CANNOT_RELEASE_EDIT_LOCK:
            resourceId = IDS_UE_210053;
            break;
        case PAE_NO_PLAN_NAME:
            resourceId = IDS_UE_210054;
            break;
          case PAE_INVALID_PLAN_NAME:
              resourceId = IDS_UE_210121;
              break;
        case PAE_NAME_NOT_UNIQUE:
            resourceId = IDS_UE_210055;
            break;
        case PAE_CANNOT_SAVE_APPROVED_PLAN:
            resourceId = IDS_UE_210056;
            break;
        case PAE_CANNOT_DELETE_APPROVED_PLAN:
            resourceId = IDS_UE_210057;
            break;
        case PAE_NOT_APPROVED:
            resourceId = IDS_UE_210058;
            break;
        case PAE_CANNOT_SKIP_UNSKIPPABLE:
            resourceId = IDS_UE_210059;
            break;
        case PAE_NOT_PLAN:
            resourceId = IDS_UE_210061;
            break;
        case PAE_NOT_CATEGORY:
            resourceId = IDS_UE_210062;
            break;
        case PAE_PARENT_NOT_CATEGORY:
            resourceId = IDS_UE_210063;
            break;
        case PAE_EXCEED_MAX_CATAGORY_LEVEL:
            resourceId = IDS_UE_210064;
            break;
        case PAE_CANNOT_DELETE_NON_EMPTY_CATAGORY:
            resourceId = IDS_UE_210065;
            break;
        case PAE_PLAN_HAS_ACTIVE_INSTANCE:
            resourceId = IDS_UE_210066;
            break;
        case PAE_CANNOT_CREATE_PLAN:
            resourceId = IDS_UE_210067;
            break;
        case PAE_DATABASE_ERROR:
            resourceId = IDS_UE_010001;
            break;
        case PAE_DATA_ERROR:
            resourceId = IDS_UE_010028;
            break;
        case PAE_WRONG_STATE_TO_CUSTOMISE_ACTIVE_PLAN:
            resourceId = IDS_UE_210069;
            break;
        case PAE_NO_RESOURCE:
            resourceId = IDS_UE_210070;
            break;
        case PAE_STEP_TYPE_AND_PARAMETER_MISMATCH:
            resourceId = IDS_UE_210071;
            break;
          case PAE_PLAN_UNAPPROVE_ERROR:
              resourceId = IDS_UE_210126;
              break;
          case PAE_PLAN_EXEC_ERROR:
              resourceId = IDS_UE_210126;
              break;
          case PAE_PLAN_STOP_ERROR:
              resourceId = IDS_UE_210125;
              break;
          case PAE_PLAN_RESUME_ERROR:
              resourceId = IDS_UE_210124;
              break;
          case PAE_PLAN_PAUSE_ERROR:
              resourceId = IDS_UE_210123;
              break;
        case PAE_INITIALISATION_FAILURE:    // this is agent initialisation problem, so probably shouldn't affect us
        case PAE_UNKNOWN_ERROR:
        default:
            resourceId = IDS_UE_250001;
            break;
    }

    userMsg.showMsgBox(resourceId);

    FUNCTION_EXIT;
}


