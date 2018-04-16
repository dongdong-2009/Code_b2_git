//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableLaunchGuiAppStep.cpp $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2013/01/18 17:55:27 $
// Last modified by:  $Author: huangjian $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "PlanManager.h"
#include "ExecutableLaunchGuiAppStep.h"
#include "LaunchGuiAppDlg.h"
#include "InteractiveStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// ExecutableLaunchGuiAppStep

ExecutableLaunchGuiAppStep::ExecutableLaunchGuiAppStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
    InteractiveStep(stepDetail)
{
    FUNCTION_ENTRY( "ExecutableLaunchGuiAppStep" );

    TA_ASSERT(stepDetail.data._d() == TA_Base_Core::LAUNCH_GUI_APP_STEP, "Invalid step parameter type");

    FUNCTION_EXIT;
}


ExecutableLaunchGuiAppStep::~ExecutableLaunchGuiAppStep()
{
    FUNCTION_ENTRY( "~ExecutableLaunchGuiAppStep" );
    FUNCTION_EXIT;
}


void ExecutableLaunchGuiAppStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    // Prompt the user to acknowledge the launch of the app.
    CLaunchGuiAppDlg* launchGuiAppDlg = new CLaunchGuiAppDlg(getStepDetail());

    if (launchGuiAppDlg->Create(IDD_LAUNCH_GUI_APP_DLG) == 0)
    {
        TA_THROW(InteractiveStepException("Failed to create Launch GUI Application step confirmation dialog"));
    }

    FUNCTION_EXIT;
}


