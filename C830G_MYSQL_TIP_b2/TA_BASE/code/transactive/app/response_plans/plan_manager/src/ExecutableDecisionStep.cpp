//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableDecisionStep.cpp $
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

ExecutableDecisionStep::ExecutableDecisionStep(const TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail& stepDetail) :
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


