//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableRunPlanStep.cpp $
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
#include "ExecutableRunPlanStep.h"
#include "RunPlanDlg.h"
#include "InteractiveStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// ExecutableRunPlanStep

ExecutableRunPlanStep::ExecutableRunPlanStep(const TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail& stepDetail) :
    InteractiveStep(stepDetail)
{
    FUNCTION_ENTRY( "ExecutableRunPlanStep" );

    TA_ASSERT(stepDetail.data._d() == TA_Base_Core::RUN_PLAN_STEP, "Invalid step parameter type");

    FUNCTION_EXIT;
}


ExecutableRunPlanStep::~ExecutableRunPlanStep()
{
    FUNCTION_ENTRY( "~ExecutableRunPlanStep" );
    FUNCTION_EXIT;
}


void ExecutableRunPlanStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    // Prompt the user to acknowledge the launch of the Plan Controller for child plan.
    CRunPlanDlg* runPlanDlg = new CRunPlanDlg(getStepDetail());

    if (runPlanDlg->Create(IDD_RUN_PLAN_DLG) == 0)
    {
        TA_THROW(InteractiveStepException("Failed to create Run Plan step confirmation dialog"));
    }

    FUNCTION_EXIT;
}


