/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PopupMFTMessageStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/exceptions/src/PlanStepException.h"
#include "app/response_plans/plan_step_plugin/popup_mft_message/editor/src/PopupMFTMessageStep.h"
#include "app/response_plans/plan_step_plugin/popup_mft_message/editor/src/PlanStepEditPopupMFTMessagePage.h"


namespace TA_Base_App
{

static const std::string MESSAGESTYLE_DESCRIPTION[6] =
{
    "OK", //MB_OK 0x00000000L
    "OK and Cancel",//MB_OKCANCEL 0x00000001L
    "Abort, Retry, and Ignore",//MB_ABORTRETRYIGNORE 0x00000002L
    "Yes, No, and Cancel",//MB_YESNOCANCEL 0x00000003L
    "Yes and No",//MB_YESNO 0x00000004L
    "Retry and Cancel"//MB_RETRYCANCEL 0x00000005L
};
/////////////////////////////////////////////////////////////////////////////
// IPlanStep

PopupMFTMessageStep::PopupMFTMessageStep( IPlanNode* parent, const BasicParameter& stepDetail ) :
    PlanStep( parent, m_params, false ),
    m_params( stepDetail ),
    m_parActivityParameter( m_params, *parent )
{
    FUNCTION_ENTRY( "PopupMFTMessageStep" );
    FUNCTION_EXIT;
}


PopupMFTMessageStep::PopupMFTMessageStep( IPlanNode* parent, const unsigned long stepId, const int stepType ) :
    PlanStep( parent, m_params, true ),
    m_params( stepId, stepType ),
    m_parActivityParameter( m_params, *parent )
{
    FUNCTION_ENTRY( "PopupMFTMessageStep" );
    FUNCTION_EXIT;
}


PopupMFTMessageStep::PopupMFTMessageStep( const PopupMFTMessageStep& planStep ) :
    PlanStep( planStep.m_parent, m_params, true ),
    m_params( planStep.m_params ),
    m_parActivityParameter( m_params, *( planStep.m_parent ) )
{
    FUNCTION_ENTRY( "PopupMFTMessageStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


PopupMFTMessageStep::~PopupMFTMessageStep()
{
    FUNCTION_ENTRY( "~PopupMFTMessageStep" );
    FUNCTION_EXIT;
}


PlanStep* PopupMFTMessageStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new PopupMFTMessageStep( *this );
}


void PopupMFTMessageStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* PopupMFTMessageStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditPopupMFTMessagePage( this );
}

IActivityParameter& PopupMFTMessageStep::getActivityParameter()
{
    reloadParameters();
    return m_parActivityParameter;
}

bool PopupMFTMessageStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t PopupMFTMessageParameterSize = 12u;

    bool bValid = ( PopupMFTMessageParameterSize == parameters.size( ) );
    if( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for popup mft message step [%u].", parameters.size(), PopupMFTMessageParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

std::string PopupMFTMessageStep::getStepTypeName() const
{
    return "Popup mft message";
}

void PopupMFTMessageStep::outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing )
{
    if( unPrevOutgoing == m_params.abortStep )
    {
        m_params.abortStep = unNewOutgoing;
    }
    else if( unPrevOutgoing == m_params.retryStep )
    {
        m_params.retryStep = unNewOutgoing;
    }
    else if( unPrevOutgoing == m_params.ignoreStep )
    {
        m_params.ignoreStep = unNewOutgoing;
    }
    else if( unPrevOutgoing == m_params.okStep )
    {
        m_params.okStep = unNewOutgoing;
    }
    else if( unPrevOutgoing == m_params.cancelStep )
    {
        m_params.cancelStep = unNewOutgoing;
    }
    else if( unPrevOutgoing == m_params.yesStep )
    {
        m_params.yesStep = unNewOutgoing;
    }
    else if( unPrevOutgoing == m_params.noStep )
    {
        m_params.noStep = unNewOutgoing;
    }
    else if( unPrevOutgoing == m_params.pendingStep )
    {
        m_params.pendingStep = unNewOutgoing;
    }
}

bool PopupMFTMessageStep::allowMultipleIncoming()
{
	return false;
}

bool PopupMFTMessageStep::allowMultipleOutgoing()
{
	return true;
}

bool PopupMFTMessageStep::retrieveOutgoings( std::vector<unsigned int>& vecOutgoings )
{
	if ( 0u != m_params.abortStep )
	{
		vecOutgoings.push_back( m_params.abortStep );
	}

	if ( 0u != m_params.retryStep )
	{
		vecOutgoings.push_back( m_params.retryStep );
	}

	if ( 0u != m_params.ignoreStep )
	{
		vecOutgoings.push_back( m_params.ignoreStep );
	}

	if ( 0u != m_params.okStep )
	{
		vecOutgoings.push_back( m_params.okStep );
	}

	if ( 0u != m_params.cancelStep )
	{
		vecOutgoings.push_back( m_params.cancelStep );
	}

	if ( 0u != m_params.yesStep )
	{
		vecOutgoings.push_back( m_params.yesStep );
	}

	if ( 0u != m_params.noStep )
	{
		vecOutgoings.push_back( m_params.noStep );
	}

	if ( 0u != m_params.pendingStep )
	{
		vecOutgoings.push_back( m_params.pendingStep );
	}

	return true;
}

bool PopupMFTMessageStep::hasCustomOutgoingStep() const
{
	return true;
}

bool PopupMFTMessageStep::canIgnoreFailure() const
{
	return true;
}

void PopupMFTMessageStep::clearFlowParameters()
{
    m_params.abortStep = 0u;
    m_params.retryStep = 0u;
    m_params.ignoreStep = 0u;
    m_params.okStep = 0u;
    m_params.cancelStep = 0u;
    m_params.yesStep = 0u;
    m_params.noStep = 0u;
    m_params.pendingStep = 0u;
}
}
