//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/StartStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #10 $
//
// Last modification: $DateTime: 2014/12/30 18:50:08 $
// Last modified by:  $Author: huang.wenbo $
//
// <description>

#include "core/utilities/src/DebugUtil.h"
#include "StartStep.h"

namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// PlanStep

StartStep::StartStep( IPlanNode* parent, const BasicParameter& stepDetail ) : PlanStep( parent, m_params, false ),
m_params( stepDetail ),
m_parActivityParameter( m_params )
{
    FUNCTION_ENTRY("StartStep");

    m_parActivityParameter.responseRequired( false );
    m_parActivityParameter.failureIgnorable( false );
    m_parActivityParameter.skippable( false );
    m_parActivityParameter.delayable( false );

    FUNCTION_EXIT;
}

StartStep::StartStep( IPlanNode* parent, const unsigned long stepId, const int stepType ) : PlanStep( parent, m_params, true ),
m_params( stepId, stepType ),
m_parActivityParameter( m_params )
{
    FUNCTION_ENTRY("StartStep");

    m_parActivityParameter.responseRequired( false );
    m_parActivityParameter.failureIgnorable( false );
    m_parActivityParameter.skippable( false );
    m_parActivityParameter.delayable( false );
    FUNCTION_EXIT;
}

StartStep::StartStep(const StartStep& planStep) : PlanStep(planStep.m_parent, m_params, true),
m_params(planStep.m_params),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY("StartStep");

    m_parActivityParameter.responseRequired( false );
    m_parActivityParameter.failureIgnorable( false );
    m_parActivityParameter.skippable( false );
    m_parActivityParameter.delayable( false );

    FUNCTION_EXIT;
}

StartStep::~StartStep()
{
    FUNCTION_ENTRY("~StartStep");
    FUNCTION_EXIT;
}

PlanStep* StartStep::clone()
{
    FUNCTION_ENTRY("clone");
    FUNCTION_EXIT;
    return new StartStep(*this);
}

void StartStep::createPlanStepParameters()
{
    FUNCTION_ENTRY("createPlanStepParameters");
    FUNCTION_EXIT;
}

bool StartStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t ParameterSize = 0u;

    bool bValid = (ParameterSize == parameters.size());
    if ( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for Start step [%u].", parameters.size(), ParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

std::string StartStep::getStepTypeName() const
{
    return "Start";
}

bool StartStep::editable()
{
    FUNCTION_ENTRY("editable");
    FUNCTION_EXIT;
    return false;
}

bool StartStep::hasIncoming()
{
    FUNCTION_ENTRY( "hasIncoming" );
    FUNCTION_EXIT;
    return false;
}

bool StartStep::hasOutgoing()
{
    FUNCTION_ENTRY( "hasOutgoing" );
    FUNCTION_EXIT;
    return true;
}

IActivityParameter& StartStep::getActivityParameter()
{
    reloadParameters();
    return m_parActivityParameter;
}

bool StartStep::hasCustomOutgoingStep() const
{
    return false;
}

bool StartStep::canIgnoreFailure() const
{
    return true;
}

}
