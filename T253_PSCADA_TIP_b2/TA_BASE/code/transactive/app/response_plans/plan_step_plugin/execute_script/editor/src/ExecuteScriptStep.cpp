//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/LogStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2009/12/29 14:17:48 $
// Last modified by:  $Author: grace.koh $
//
// <description>
#include "StdAfx.h"

#include "core/utilities/src/TAAssert.h"
#include "resource.h"
#include "PlanStepEditExecuteScriptPage.h"
#include "ExecuteScriptStep.h"

namespace TA_Base_App
{
ExecuteScriptStep::ExecuteScriptStep( IPlanNode* parent, const BasicParameter& stepDetail ):
    PlanStep( parent, m_params, false ),
    m_params( stepDetail ),
    m_parActivityParameter( m_params )
{
    FUNCTION_ENTRY( "ExecuteScriptStep" );
    FUNCTION_EXIT;
}

ExecuteScriptStep::ExecuteScriptStep( IPlanNode* parent, const unsigned long stepId , const int stepType ) :
    PlanStep( parent, m_params, true ),
    m_params( stepId, stepType ),
    m_parActivityParameter( m_params )
{
    FUNCTION_ENTRY( "ExecuteScriptStep" );
    FUNCTION_EXIT;
}

ExecuteScriptStep::ExecuteScriptStep( const ExecuteScriptStep& planStep ):
    PlanStep( planStep.m_parent, m_params, true ),
    m_params( planStep.m_params ),
    m_parActivityParameter( m_params )
{
    FUNCTION_ENTRY( "ExecuteScriptStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}

ExecuteScriptStep::~ExecuteScriptStep()
{
}

PlanStep* ExecuteScriptStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new ExecuteScriptStep( *this );
}

void ExecuteScriptStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* ExecuteScriptStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepExecuteScriptPage( this );
}

bool ExecuteScriptStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t ExecuteScriptParameterSize = 1u;

    bool bValid = ( ExecuteScriptParameterSize == parameters.size() );
    if( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for execute script step [%u].", parameters.size(), ExecuteScriptParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

IActivityParameter& ExecuteScriptStep::getActivityParameter()
{
    reloadParameters();
    return m_parActivityParameter;
}

std::string TA_Base_App::ExecuteScriptStep::getStepTypeName() const
{
    return "Execute script";
}
}