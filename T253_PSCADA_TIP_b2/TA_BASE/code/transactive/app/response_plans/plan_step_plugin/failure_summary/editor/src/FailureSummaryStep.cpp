/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/FailureSummaryStep.cpp $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#include "StdAfx.h"
#include <memory>
#include "FailureSummaryStep.h"
#include "resource.h"
#include "FailureCategoryStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"
#include "PlanStepEditFailureSummaryPage.h"
#include "core/exceptions/src/PlanStepException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

FailureSummaryStep::FailureSummaryStep(IPlanNode* parent, const BasicParameter &stepDetail) :
    PlanStep(parent, m_params, false),
    m_failureCategoryParameter(NULL),
	m_params(stepDetail),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "FailureSummaryStep" );
    FUNCTION_EXIT;
}


FailureSummaryStep::FailureSummaryStep(IPlanNode* parent, const unsigned long stepId, const int stepType) :
    PlanStep(parent,m_params,true),
    m_failureCategoryParameter(NULL),
	m_params(stepId, stepType),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "FailureSummaryStep" );
    FUNCTION_EXIT;
}


FailureSummaryStep::FailureSummaryStep(const FailureSummaryStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
    m_failureCategoryParameter(NULL),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "FailureSummaryStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


FailureSummaryStep::~FailureSummaryStep()
{
    FUNCTION_ENTRY( "~FailureSummaryStep" );

    delete m_failureCategoryParameter;

    FUNCTION_EXIT;
}


PlanStep* FailureSummaryStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new FailureSummaryStep(*this);
}


void FailureSummaryStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_failureCategoryParameter;

	m_failureCategoryParameter= new FailureCategoryStepParameter(*this, ParamFailureCategory, m_params.failureCategory);

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* FailureSummaryStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditFailureSummaryParameterPage(this);
}

bool FailureSummaryStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t FailureSummaryParameterSize = 1u;

	bool bValid = ( FailureSummaryParameterSize == parameters.size() );
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for failure summary step [%u].", parameters.size(), FailureSummaryParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string FailureSummaryStep::getStepTypeName() const
{
	return "Failure summary";
}

IActivityParameter& TA_Base_App::FailureSummaryStep::getActivityParameter()
{
    reloadParameters();

	return m_parActivityParameter;
}

}
