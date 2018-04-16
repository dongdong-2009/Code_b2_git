/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TerminatePlanStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #18 $
  *
  * Last modification: $DateTime: 2015/05/11 18:10:24 $
  * Last modified by:  $Author: qi.huang $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PlanStepException.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanPathStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "TerminatePlanStep.h"
#include "PlanStepEditTerminatePlanPage.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

TerminatePlanStep::TerminatePlanStep( IPlanNode* parent, const BasicParameter& stepDetail ) :
    PlanStep(parent,m_params,false),
	m_planParameter(NULL),
	m_params(stepDetail),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "TerminatePlanStep" );
    FUNCTION_EXIT;
}


TerminatePlanStep::TerminatePlanStep( IPlanNode* parent, const unsigned long stepId, const int stepType ) :
    PlanStep(parent,m_params,true),
	m_planParameter(NULL),
    m_params(stepId, stepType),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "TerminatePlanStep" );
    FUNCTION_EXIT;
}


TerminatePlanStep::TerminatePlanStep(const TerminatePlanStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
	m_planParameter(NULL),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "TerminatePlanStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


TerminatePlanStep::~TerminatePlanStep()
{
    FUNCTION_ENTRY( "~TerminatePlanStep" );

    delete m_planParameter;

    FUNCTION_EXIT;
}


PlanStep* TerminatePlanStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new TerminatePlanStep(*this);
}


void TerminatePlanStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_planParameter;
    m_planParameter = new PlanPathStepParameter( *this, ParamPlan, m_params.plan );

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* TerminatePlanStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditTerminatePlanPage(this);
}

bool TerminatePlanStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t ParameterSize = 1u;

    bool bValid = (ParameterSize == parameters.size());
    if ( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for TerminatePlan step [%u].", parameters.size(), ParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

std::string TerminatePlanStep::getStepTypeName() const
{
	return "Terminate plan";
}

IActivityParameter& TA_Base_App::TerminatePlanStep::getActivityParameter()
{
    reloadParameters();

	return m_parActivityParameter;
}

}