//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableRunPlanStep.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"
#include "resource.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/common/src/RpcAgency.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/RightsManager.h"
#include "ExecutableRunPlanStep.h"
#include "RunPlanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// ExecutableRunPlanStep

ExecutableRunPlanStep::ExecutableRunPlanStep(const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
											 const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters) :
InteractiveStep(unId, eType, ulLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters),
m_runPlanDlg( new CRunPlanDlg(eType, ulLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters) )
{
    FUNCTION_ENTRY( "ExecutableRunPlanStep" );

    //TA_ASSERT(eType == TA_Base_Core::RUN_PLAN_STEP, "Invalid step parameter type");

    FUNCTION_EXIT;
}


ExecutableRunPlanStep::~ExecutableRunPlanStep()
{
    FUNCTION_ENTRY( "~ExecutableRunPlanStep" );
    FUNCTION_EXIT;
}


void ExecutableRunPlanStep::launch()
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
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Execute RunPlanStep fail");
	}

	if ( !isRunning() )
	{
		FUNCTION_EXIT;
		return;
	}

	m_runPlanDlg->DoModal();

    FUNCTION_EXIT;
}

void ExecutableRunPlanStep::close()
{
	if ( NULL != m_runPlanDlg )
	{
		m_runPlanDlg->EndDialog( IDABORT );
	}
}
