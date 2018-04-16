//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanSetTimerStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>
#include "StdAfx.h"

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "resource.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeSpanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/StepNumberStepParameter.h"
#include "PlanStepPlanSetTimerPage.h"
#include "PlanSetTimerStep.h"

namespace TA_Base_App
{


PlanSetTimerStep::PlanSetTimerStep( IPlanNode* parent, const BasicParameter& stepDetail ):
    PlanStep( parent, m_params, false ),
    m_timeSpanParameter( NULL ),
    m_params( stepDetail ),
    m_parActivityParameter( m_params )
{
    FUNCTION_ENTRY( "PlanSetTimerStep" );
    FUNCTION_EXIT;
}

PlanSetTimerStep::PlanSetTimerStep( IPlanNode* parent, const unsigned long stepId , const int stepType ) :
    PlanStep( parent, m_params, true ),
    m_timeSpanParameter( NULL ),
    m_params( stepId, stepType ),
    m_parActivityParameter( m_params )
{
    FUNCTION_ENTRY( "PlanSetTimerStep" );
    FUNCTION_EXIT;
}

PlanSetTimerStep::PlanSetTimerStep( const PlanSetTimerStep& planStep ):
    PlanStep( planStep.m_parent, m_params, true ),
    m_timeSpanParameter( NULL ),
    m_params( planStep.m_params ),
    m_parActivityParameter( m_params )
{
    FUNCTION_ENTRY( "PlanSetTimerStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}

PlanSetTimerStep::~PlanSetTimerStep()
{
    delete m_timeSpanParameter;
}

PlanStep* PlanSetTimerStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new PlanSetTimerStep( *this );
}

void PlanSetTimerStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_timeSpanParameter;

    m_timeSpanParameter = new TimeSpanStepParameter( *this, ParamTime, m_params.time, TimeSpanStepParameter::TR_SECONDS );

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* PlanSetTimerStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepPlanSetTimerPage( this );
}

bool PlanSetTimerStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t PlanSetTimerParameterSize = 1u;

    bool bValid = ( PlanSetTimerParameterSize == parameters.size( ) );
    if( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for plan set timer step [%u].", parameters.size(), PlanSetTimerParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

std::string PlanSetTimerStep::getStepTypeName() const
{
    return "Plan set timer";
}

IActivityParameter& PlanSetTimerStep::getActivityParameter()
{
    reloadParameters();
    return m_parActivityParameter;
}

}