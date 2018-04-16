//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableFailureSummaryStep.cpp $
// @author:  Lizette Lingo
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "PlanManager.h"
#include "ExecutableFailureSummaryStep.h"
#include "FailureSummaryDlg.h"
#include "InteractiveStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// ExecutableFailureSummaryStep

ExecutableFailureSummaryStep::ExecutableFailureSummaryStep(const TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail& stepDetail) :
    InteractiveStep(stepDetail)
{
    FUNCTION_ENTRY( "ExecutableFailureSummaryStep" );

    TA_ASSERT(stepDetail.data._d() == TA_Base_Core::FAILURE_SUMMARY_STEP, "Invalid step parameter type");

    FUNCTION_EXIT;
}


ExecutableFailureSummaryStep::~ExecutableFailureSummaryStep()
{
    FUNCTION_ENTRY( "~ExecutableFailureSummaryStep" );
    FUNCTION_EXIT;
}


void ExecutableFailureSummaryStep::launch()
{
    FUNCTION_ENTRY( "launch" );

    CFailureSummaryDlg* failureSummaryDlg = new CFailureSummaryDlg(getStepDetail());

    if (failureSummaryDlg->Create(IDD_FAILURE_SUMMARY_DLG) == 0)
    {
        TA_THROW(InteractiveStepException("Failed to create Failure Summary step interaction dialog"));
    }

    FUNCTION_EXIT;
}


