//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutablePromptStep.cpp $
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
#include "ExecutablePromptStep.h"
#include "PromptDlg.h"
#include "InteractiveStepException.h"
#include "PlanAgentAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// ExecutablePromptStep

ExecutablePromptStep::ExecutablePromptStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
    InteractiveStep(stepDetail)
{
    FUNCTION_ENTRY( "ExecutablePromptStep" );

    TA_ASSERT(stepDetail.data._d() == TA_Base_Core::PROMPT_STEP, "Invalid step parameter type");

    FUNCTION_EXIT;
}


ExecutablePromptStep::~ExecutablePromptStep()
{
    FUNCTION_ENTRY( "~ExecutablePromptStep" );
    FUNCTION_EXIT;
}


void ExecutablePromptStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    CPromptDlg* promptDlg = new CPromptDlg(getStepDetail());

    if (promptDlg->Create(IDD_PROMPT_DLG) == 0)
    {
        TA_THROW(InteractiveStepException("Failed to create Prompt step interaction dialog"));
    }

    FUNCTION_EXIT;
}


