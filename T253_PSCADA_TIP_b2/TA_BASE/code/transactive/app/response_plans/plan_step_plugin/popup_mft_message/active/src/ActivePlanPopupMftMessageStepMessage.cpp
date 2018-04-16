/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPopupMftMessageStepMessage.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/07/10 16:23:57 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "ActivePlanPopupMftMessageStepMessage.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
/*
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePopupMFTMessageStep.h"
*/

namespace TA_Base_App
{

    ActivePlanPopupMftMessageStepMessage::ActivePlanPopupMftMessageStepMessage(const unsigned long ulStepNum, const std::vector<std::string>& vecData)
		: m_ulStepNum(ulStepNum),
		m_vecData( vecData )
    {
    }

    void ActivePlanPopupMftMessageStepMessage::execute(IActivePlan& activePlan, IMessageSender& refMsgSender) const
    {
		activePlan.stepData( m_ulStepNum, m_vecData );
    }

}