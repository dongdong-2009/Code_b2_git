//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutablePromptStep.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/RightsManager.h"
#include "ExecutablePromptStep.h"
#include "PromptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// ExecutablePromptStep

ExecutablePromptStep::ExecutablePromptStep(const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
										   const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters) :
InteractiveStep(unId, eType, ulLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters),
m_promptDlg( new CPromptDlg(eType, ulLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters) )
{
    FUNCTION_ENTRY( "ExecutablePromptStep" );

    //TA_ASSERT(eType == TA_Base_Core::PROMPT_STEP, "Invalid step parameter type");

    FUNCTION_EXIT;
}


ExecutablePromptStep::~ExecutablePromptStep()
{
    FUNCTION_ENTRY( "~ExecutablePromptStep" );
    FUNCTION_EXIT;
}


void ExecutablePromptStep::launch()
{
    FUNCTION_ENTRY( "launch" );

	if ( !isRunning() )
	{
		FUNCTION_EXIT;
		return;
	}

	try
	{
		TA_Base_Bus::RpcAgency::getInstance().asyncSendAck( RightsManager::getInstance().locationId(), m_unId );
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Execute PromptStep fail");
	}

	if ( !isRunning() )
	{
		FUNCTION_EXIT;
		return;
	}

	m_promptDlg->DoModal();

    FUNCTION_EXIT;
}

void ExecutablePromptStep::close()
{
	if ( NULL != m_promptDlg )
	{
		m_promptDlg->EndDialog( IDABORT );
	}
}

