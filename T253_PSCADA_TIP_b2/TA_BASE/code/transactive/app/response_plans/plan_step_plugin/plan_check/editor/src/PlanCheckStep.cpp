/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanCheckStep.cpp $
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
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanPathStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/StepNumberStepParameter.h"
#include "app/response_plans/plan_step_plugin/plan_check/editor/src/PlanCheckStep.h"
#include "app/response_plans/plan_step_plugin/plan_check/editor/src/PlanStepEditActivePlanCheckPage.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

PlanCheckStep::PlanCheckStep(IPlanNode* parent,const BasicParameter &stepDetail) :
    PlanStep(parent,m_params,false),
    m_planPathParameter(NULL),
    m_yesStepParameter(NULL),
    m_noStepParameter(NULL),
	m_params(stepDetail),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "PlanCheckStep" );
    FUNCTION_EXIT;
}


PlanCheckStep::PlanCheckStep(IPlanNode* parent,const unsigned long stepId, const int stepType) :
    PlanStep(parent,m_params,true),
    m_planPathParameter(NULL),
    m_yesStepParameter(NULL),
    m_noStepParameter(NULL),
	m_params(stepId, stepType),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "PlanCheckStep" );
    FUNCTION_EXIT;
}


PlanCheckStep::PlanCheckStep(const PlanCheckStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
    m_planPathParameter(NULL),
    m_yesStepParameter(NULL),
    m_noStepParameter(NULL),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "PlanCheckStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


PlanCheckStep::~PlanCheckStep()
{
    FUNCTION_ENTRY( "~PlanCheckStep" );

	delete m_planPathParameter;
	delete m_yesStepParameter;
	delete m_noStepParameter;

    FUNCTION_EXIT;
}


PlanStep* PlanCheckStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new PlanCheckStep(*this);
}


void PlanCheckStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_planPathParameter;
    delete m_yesStepParameter;
    delete m_noStepParameter;

    m_planPathParameter   = new PlanPathStepParameter(*this, ParamPlan, m_params.plan);
    m_yesStepParameter    = new StepNumberStepParameter(*this, ParamYesStep, m_params.yesStep);
    m_noStepParameter = new StepNumberStepParameter(*this, ParamNoStep, m_params.noStep);

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* PlanCheckStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditActivePlanCheckPage(this);
}

bool PlanCheckStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t ActivePlanCheckParameterSize = 3u;

	bool bValid = (ActivePlanCheckParameterSize == parameters.size( ));
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for active plan check step [%u].", parameters.size(), ActivePlanCheckParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string PlanCheckStep::getStepTypeName() const
{
	return "Active plan check";
}

IActivityParameter& PlanCheckStep::getActivityParameter()
{
    reloadParameters();
	return m_parActivityParameter;
}

void PlanCheckStep::outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing )
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

bool PlanCheckStep::allowMultipleIncoming()
{
	return false;
}

bool PlanCheckStep::allowMultipleOutgoing()
{
	return true;
}

bool PlanCheckStep::retrieveOutgoings( std::vector<unsigned int>& vecOutgoings )
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

bool PlanCheckStep::hasCustomOutgoingStep() const
{
	return true;
}

bool PlanCheckStep::canIgnoreFailure() const
{
	return false;
}

void PlanCheckStep::clearFlowParameters()
{
    m_params.yesStep = 0u;
    m_params.noStep = 0u;
}
}
