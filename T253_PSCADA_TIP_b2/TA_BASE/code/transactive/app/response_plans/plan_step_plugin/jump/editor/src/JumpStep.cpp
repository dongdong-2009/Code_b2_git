/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/JumpStep.cpp $
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
#include "app/response_plans/plan_step_plugin/jump/editor/src/JumpStep.h"
#include "app/response_plans/plan_step_plugin/jump/editor/src/PlanStepEditJumpPage.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

JumpStep::JumpStep(IPlanNode* parent, const BasicParameter &stepDetail) :
PlanStep(parent, m_params, false),
m_params(stepDetail),
m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "JumpStep" );
    FUNCTION_EXIT;
}


JumpStep::JumpStep(IPlanNode* parent, const unsigned long stepId, const int stepType) :
PlanStep(parent, m_params,true),
m_params(stepId, stepType),
m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "JumpStep" );
    FUNCTION_EXIT;
}


JumpStep::JumpStep(const JumpStep& planStep) :
PlanStep(planStep.m_parent, m_params, true),
m_params(planStep.m_params),
m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "JumpStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


JumpStep::~JumpStep()
{
    FUNCTION_ENTRY( "~JumpStep" );
    FUNCTION_EXIT;
}


PlanStep* JumpStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new JumpStep(*this);
}


void JumpStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

bool JumpStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t JumpParameterSize = 1u;

	bool bValid = ( JumpParameterSize == parameters.size() );
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for jump step [%u].", parameters.size(), JumpParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

CPlanStepEditParameterPage* JumpStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditJumpPage(this);
}

std::string JumpStep::getStepTypeName() const
{
    return "Jump";
}

IActivityParameter& JumpStep::getActivityParameter()
{
    reloadParameters();

	return m_parActivityParameter;
}

void JumpStep::outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing )
{
    if ( unPrevOutgoing == m_params.stepId )
    {
        m_params.stepId = unNewOutgoing;
    }
}

bool JumpStep::retrieveOutgoings( std::vector<unsigned int>& vecOutgoings )
{
    if ( 0u != m_params.stepId )
    {
        vecOutgoings.push_back( m_params.stepId );
    }

    return true;
}

void JumpStep::clearFlowParameters()
{
    m_params.stepId = 0u;
}

}
