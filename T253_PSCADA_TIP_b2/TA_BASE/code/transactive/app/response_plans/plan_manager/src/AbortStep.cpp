//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/AbortStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #12 $
//
// Last modification: $DateTime: 2015/05/11 18:31:24 $
// Last modified by:  $Author: huang.wenbo $
//
// <description>


#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/AbortStep.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

AbortStep::AbortStep( IPlanNode* parent, const BasicParameter &stepDetail ) : 
PlanStep(parent,m_params,false),
m_params(stepDetail),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "AbortStep" );
    FUNCTION_EXIT;
}


AbortStep::AbortStep( IPlanNode* parent, const unsigned long stepId, const int stepType ) :
PlanStep(parent,m_params,true),
m_params(stepId, stepType),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "AbortStep" );
    FUNCTION_EXIT;
}


AbortStep::AbortStep(const AbortStep &other) :
PlanStep(other.m_parent,m_params,true),
m_params(other.m_params),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "AbortStep" );
    FUNCTION_EXIT;
}


AbortStep::~AbortStep()
{
    FUNCTION_ENTRY( "~AbortStep" );
    FUNCTION_EXIT;
}


PlanStep* AbortStep::clone()
{
    FUNCTION_ENTRY( "clone" );
    FUNCTION_EXIT;
    return new AbortStep(*this);
}


void AbortStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

bool AbortStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t ParameterSize = 0u;

    bool bValid = (ParameterSize == parameters.size());
    if ( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for Abort step [%u].", parameters.size(), ParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

std::string AbortStep::getStepTypeName() const
{
	return "Abort";
}

IActivityParameter& TA_Base_App::AbortStep::getActivityParameter()
{
    reloadParameters();
	return m_parActivityParameter;
}

}
