//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableLaunchGuiAppStep.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
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

ExecutableLaunchGuiAppStep::ExecutableLaunchGuiAppStep(const TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail& stepDetail) :
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


