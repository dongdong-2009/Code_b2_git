/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DecisionStep.cpp $
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
#include "app/response_plans/plan_step_plugin/decision/editor/src/DecisionStep.h"
#include "app/response_plans/plan_step_plugin/decision/editor/src/PlanStepEditDecisionParameterPage.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

DecisionStep::DecisionStep(IPlanNode* parent, BasicParameter& stepDetail) :
    PlanStep(parent, m_params, false),
	m_params(stepDetail),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "DecisionStep" );

    FUNCTION_EXIT;
}

DecisionStep::DecisionStep(IPlanNode* parent, const unsigned long stepId, const int stepType) :
    PlanStep(parent, m_params, true),
	m_params(stepId, stepType),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "DecisionStep" );

    FUNCTION_EXIT;
}

DecisionStep::DecisionStep(const DecisionStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "DecisionStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}

DecisionStep::~DecisionStep()
{
    FUNCTION_ENTRY( "~DecisionStep" );
    FUNCTION_EXIT;
}

PlanStep* DecisionStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new DecisionStep(*this);
}

void DecisionStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

bool DecisionStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t DecisionParameterSize = 9u;

    bool bValid = ( DecisionParameterSize == parameters.size() );
    if ( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for decision step [%u].", parameters.size(), DecisionParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

CPlanStepEditParameterPage* DecisionStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditDecisionParameterPage(this);
}

std::string DecisionStep::getStepTypeName() const
{
    return "Decision";
}

IActivityParameter& DecisionStep::getActivityParameter()
{
    reloadParameters();

	return m_parActivityParameter;
}

void DecisionStep::outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing )
{
    if ( unPrevOutgoing == m_params.yesStep )
    {
        m_params.yesStep = unNewOutgoing;
    }
    else if ( unPrevOutgoing == m_params.noStep )
    {
        m_params.noStep = unNewOutgoing;
    }
}

bool DecisionStep::retrieveOutgoings( std::vector<unsigned int>& vecOutgoings )
{
    if ( 0u != m_params.yesStep )
    {
        vecOutgoings.push_back( m_params.yesStep );
    }

    if ( 0u != m_params.noStep )
    {
        vecOutgoings.push_back( m_params.noStep );
    }

    return true;
}

bool DecisionStep::allowMultipleIncoming()
{
    return false;
}

bool DecisionStep::allowMultipleOutgoing()
{
    return true;
}

void DecisionStep::clearFlowParameters()
{
    m_params.yesStep = 0u;
    m_params.noStep = 0u;
}

}
