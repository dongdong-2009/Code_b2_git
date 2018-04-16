//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/EndStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #16 $
//
// Last modification: $DateTime: 2015/05/11 18:31:24 $
// Last modified by:  $Author: huang.wenbo $
//
// <description>

#include "core/utilities/src/DebugUtil.h"
#include "EndStep.h"

namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// PlanStep

EndStep::EndStep( IPlanNode* parent, const BasicParameter &stepDetail ) : PlanStep(parent, m_params, false),
m_params(stepDetail),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "EndStep" );

    m_parActivityParameter.responseRequired( false );
    m_parActivityParameter.failureIgnorable( false );
    m_parActivityParameter.skippable( false );
    m_parActivityParameter.delayable( false );

    FUNCTION_EXIT;
}

EndStep::EndStep(IPlanNode* parent, const unsigned long stepId, const int stepType) : PlanStep(parent, m_params, true),
m_params(stepId, stepType),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "EndStep" );

    m_parActivityParameter.responseRequired( false );
    m_parActivityParameter.failureIgnorable( false );
    m_parActivityParameter.skippable( false );
    m_parActivityParameter.delayable( false );

    FUNCTION_EXIT;
}

EndStep::EndStep(const EndStep& planStep) : PlanStep(planStep.m_parent, m_params, true),
m_params(planStep.m_params),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "EndStep" );

    m_parActivityParameter.responseRequired( false );
    m_parActivityParameter.failureIgnorable( false );
    m_parActivityParameter.skippable( false );
    m_parActivityParameter.delayable( false );

    FUNCTION_EXIT;
}

EndStep::~EndStep()
{
    FUNCTION_ENTRY( "~EndStep" );
    FUNCTION_EXIT;
}

PlanStep* EndStep::clone()
{
    FUNCTION_ENTRY( "clone" );
    FUNCTION_EXIT;
    return new EndStep(*this);
}

void EndStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

bool EndStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t ParameterSize = 0u;

    bool bValid = (ParameterSize == parameters.size());
    if ( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for End step [%u].", parameters.size(), ParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

std::string EndStep::getStepTypeName() const
{
	return "End";
}

bool EndStep::editable()
{
    FUNCTION_ENTRY( "editable" );
    FUNCTION_EXIT;
    return false;
}

bool EndStep::hasIncoming()
{
    FUNCTION_ENTRY( "hasIncoming" );
    FUNCTION_EXIT;
    return true;
}

bool EndStep::hasOutgoing()
{
    FUNCTION_ENTRY( "hasOutgoing" );
    FUNCTION_EXIT;
    return false;
}

IActivityParameter& EndStep::getActivityParameter()
{
    reloadParameters();
	return m_parActivityParameter;
}

bool EndStep::hasCustomOutgoingStep() const
{
    return false;
}

bool EndStep::canIgnoreFailure() const
{
    return true;
}

}