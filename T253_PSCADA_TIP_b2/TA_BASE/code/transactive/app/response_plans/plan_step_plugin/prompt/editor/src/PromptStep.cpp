/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PromptStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "PlanStepEditPromptPage.h"
#include "PromptStep.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

PromptStep::PromptStep(IPlanNode* parent,const BasicParameter &stepDetail) :
PlanStep(parent,m_params,false),
m_params(stepDetail),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "PromptStep" );
    FUNCTION_EXIT;
}


PromptStep::PromptStep(IPlanNode* parent,const unsigned long stepId, const int stepType) :
PlanStep(parent, m_params,true),
m_params(stepId, stepType),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "PromptStep" );
    FUNCTION_EXIT;
}


PromptStep::PromptStep(const PromptStep& planStep) :
PlanStep(planStep.m_parent, m_params, true),
m_params(planStep.m_params),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "PromptStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


PromptStep::~PromptStep()
{
    FUNCTION_ENTRY( "~PromptStep" );
    FUNCTION_EXIT;
}


PlanStep* PromptStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new PromptStep(*this);
}


void PromptStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* PromptStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditPromptPage(this);
}

bool PromptStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t PromptParameterSize = 2u;

	bool bValid = (PromptParameterSize == parameters.size( ));
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for prompt step [%u].", parameters.size(), PromptParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string PromptStep::getStepTypeName() const
{
	return "Prompt";
}

IActivityParameter& PromptStep::getActivityParameter()
{
    reloadParameters();

	return m_parActivityParameter;
}

bool PromptStep::hasCustomOutgoingStep() const
{
    return false;
}

bool PromptStep::canIgnoreFailure() const
{
    return true;
}

}
