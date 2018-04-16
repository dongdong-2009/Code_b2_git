//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableFailureSummaryStep.cpp $
// @author:  Lizette Lingo
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2013/01/18 17:55:27 $
// Last modified by:  $Author: huangjian $
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

ExecutableFailureSummaryStep::ExecutableFailureSummaryStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
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


