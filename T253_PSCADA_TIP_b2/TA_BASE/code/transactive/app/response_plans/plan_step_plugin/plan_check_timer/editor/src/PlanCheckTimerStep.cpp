//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanCheckTimerStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>
#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_step_plugin/plan_check_timer/editor/src/PlanStepPlanCheckTimerPage.h"
#include "app/response_plans/plan_step_plugin/plan_check_timer/editor/src/PlanCheckTimerStep.h"

namespace TA_Base_App
{


PlanCheckTimerStep::PlanCheckTimerStep( IPlanNode* parent, const BasicParameter& stepDetail ):
    PlanStep( parent, m_params, false ),
    m_params( stepDetail ),
    m_parActivityParameter( m_params, *m_parent )
{
    FUNCTION_ENTRY( "PlanCheckTimerStep" );
    FUNCTION_EXIT;
}

PlanCheckTimerStep::PlanCheckTimerStep( IPlanNode* parent, const unsigned long stepId , const int stepType ) :
    PlanStep( parent, m_params, true ),
    m_params( stepId, stepType ),
    m_parActivityParameter( m_params, *m_parent )
{
    FUNCTION_ENTRY( "PlanCheckTimerStep" );
    FUNCTION_EXIT;
}

PlanCheckTimerStep::PlanCheckTimerStep( const PlanCheckTimerStep& planStep ):
    PlanStep( planStep.m_parent, m_params, true ),
    m_params( planStep.m_params ),
    m_parActivityParameter( m_params, *m_parent )
{
    FUNCTION_ENTRY( "PlanCheckTimerStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}

PlanCheckTimerStep::~PlanCheckTimerStep()
{
}

PlanStep* PlanCheckTimerStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new PlanCheckTimerStep( *this );
}

void PlanCheckTimerStep::createPlanStepParameters()
{

    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* PlanCheckTimerStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepPlanCheckTimerPage( this );
}

bool PlanCheckTimerStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t PlanCheckTimerParameterSize = 3u;

    bool bValid = ( PlanCheckTimerParameterSize == parameters.size( ) );
    if( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for plan check timer step [%u].", parameters.size(), PlanCheckTimerParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

std::string PlanCheckTimerStep::getStepTypeName() const
{
    return "Plan check timer";
}

IActivityParameter& PlanCheckTimerStep::getActivityParameter()
{
    reloadParameters();
    return m_parActivityParameter;
}

void PlanCheckTimerStep::outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing )
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

bool PlanCheckTimerStep::allowMultipleIncoming()
{
	return false;
}

bool PlanCheckTimerStep::allowMultipleOutgoing()
{
	return true;
}

bool PlanCheckTimerStep::retrieveOutgoings( std::vector<unsigned int>& vecOutgoings )
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

bool PlanCheckTimerStep::hasCustomOutgoingStep() const
{
	return true;
}

bool PlanCheckTimerStep::canIgnoreFailure() const
{
	return true;
}

void PlanCheckTimerStep::clearFlowParameters()
{
    m_params.successStep = 0u;
    m_params.failureStep = 0u;
}
}
