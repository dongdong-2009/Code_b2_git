/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ExpressionStep.cpp $
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
#include "core/exceptions/src/PlanStepException.h"
#include "app/response_plans/plan_step_plugin/evaluate_expression/editor/src/ExpressionStep.h"
#include "app/response_plans/plan_step_plugin/evaluate_expression/editor/src/PlanStepEditExpressionPage.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

ExpressionStep::ExpressionStep(IPlanNode* parent,const BasicParameter &stepDetail) :
PlanStep(parent,m_params,false),
m_params(stepDetail),
m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "ExpressionStep" );
    FUNCTION_EXIT;
}


ExpressionStep::ExpressionStep(IPlanNode* parent,const unsigned long stepId, const int stepType) :
PlanStep(parent,m_params,true),
m_params(stepId, stepType),
m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "ExpressionStep" );
    FUNCTION_EXIT;
}


ExpressionStep::ExpressionStep(const ExpressionStep& planStep) :
PlanStep(planStep.m_parent, m_params, true),
m_params(planStep.m_params),
m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "ExpressionStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


ExpressionStep::~ExpressionStep()
{
    FUNCTION_ENTRY( "~ExpressionStep" );
    FUNCTION_EXIT;
}


PlanStep* ExpressionStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new ExpressionStep(*this);
}


void ExpressionStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* ExpressionStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditExpressionPage(this);
}

bool ExpressionStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t EvaluateExpressionParameterSize = 3u;

	bool bValid = ( EvaluateExpressionParameterSize == parameters.size() );
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for evaluate expression step [%u].", parameters.size(), EvaluateExpressionParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string ExpressionStep::getStepTypeName() const
{
	return "Evaluate expression";
}

IActivityParameter& ExpressionStep::getActivityParameter()
{
    reloadParameters();
	return m_parActivityParameter;
}

void ExpressionStep::outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing )
{
    if ( unPrevOutgoing == m_params.successStep )
    {
        m_params.successStep = unNewOutgoing;
    }
    else if ( unPrevOutgoing == m_params.failureStep )
    {
        m_params.failureStep = unNewOutgoing;
    }
}

bool ExpressionStep::allowMultipleIncoming()
{
	return false;
}

bool ExpressionStep::allowMultipleOutgoing()
{
	return true;
}

bool ExpressionStep::retrieveOutgoings( std::vector<unsigned int>& vecOutgoings )
{
	if ( 0u != m_params.successStep )
	{
		vecOutgoings.push_back( m_params.successStep );
	}

	if ( 0u != m_params.failureStep )
	{
		vecOutgoings.push_back( m_params.failureStep );
	}

	return true;
}

bool ExpressionStep::hasCustomOutgoingStep() const
{
	return true;
}

bool ExpressionStep::canIgnoreFailure() const
{
	return false;
}

void ExpressionStep::clearFlowParameters()
{
    m_params.successStep = 0u;
    m_params.failureStep = 0u;
}
}
