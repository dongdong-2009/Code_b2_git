//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableDecisionStep.cpp $
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
#include "ExecutableDecisionStep.h"
#include "DecisionDlg.h"
#include "InteractiveStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// ExecutableDecisionStep

ExecutableDecisionStep::ExecutableDecisionStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
    InteractiveStep(stepDetail)
{
    FUNCTION_ENTRY( "ExecutableDecisionStep" );

    TA_ASSERT(stepDetail.data._d() == TA_Base_Core::DECISION_STEP, "Invalid step parameter type");

    FUNCTION_EXIT;
}


ExecutableDecisionStep::~ExecutableDecisionStep()
{
    FUNCTION_ENTRY( "~ExecutableDecisionStep" );
    FUNCTION_EXIT;
}


void ExecutableDecisionStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    CDecisionDlg* decisionDlg = new CDecisionDlg(getStepDetail());

    if (decisionDlg->Create(IDD_DECISION_DLG) == 0)
    {
        TA_THROW(InteractiveStepException("Failed to create Decision step interaction dialog"));
    }

    FUNCTION_EXIT;
}


