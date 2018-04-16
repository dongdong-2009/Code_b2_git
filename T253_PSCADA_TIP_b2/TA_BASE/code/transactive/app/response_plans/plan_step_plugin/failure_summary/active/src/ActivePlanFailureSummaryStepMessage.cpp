/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanFailureSummaryStepMessage.cpp $
  *  @author:  Lizette Lingo
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/07/10 16:23:57 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Generated when the result of a Failure summary dialog display
  *  is received back from the Plan Manager.
  */
#include "app/response_plans/common/src/StringConstants.h"
#include "ActivePlanFailureSummaryStepMessage.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
/*
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanFailureSummaryStepMessage.h"
*/

namespace TA_Base_App
{
    ActivePlanFailureSummaryStepMessage::ActivePlanFailureSummaryStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
		: m_ulStepNum(ulStepNum),
		  m_responseParam(vecData)
    {
    }


    ActivePlanFailureSummaryStepMessage::~ActivePlanFailureSummaryStepMessage()
    {
    }


    void ActivePlanFailureSummaryStepMessage::execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const
    {
        const int nStatus = m_responseParam.response;

        int nWaitStatus = ActiveStep::eWaitFailed;

        if ( TA_Base_Core::PSCS_ACKNOWLEDGEMENT == nStatus )
        {
            nWaitStatus = ActiveStep::eWaitSucceed;
        }

        activePlan.stepResponsed( m_ulStepNum, nWaitStatus );
    }
}