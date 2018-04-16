/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanRunPlanStepMessage.cpp $
  *  @author:  Bart Golab
  *  @version: $Revision: #8 $
  *
  *  Last modification: $DateTime: 2015/01/08 17:33:12 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Generated when the result of a Run Plan interactive step execution request
  *  is received back from the Plan Manager.
  */

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanRunPlanStepMessage.h"
#include "app/response_plans/common/src/StringConstants.h"

namespace TA_Base_App
{
    ActivePlanRunPlanStepMessage::ActivePlanRunPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& parameters ) :
    m_ulStepNum( ulStepNum ),
	m_responseParam(parameters)
    {
    }

    ActivePlanRunPlanStepMessage::~ActivePlanRunPlanStepMessage()
    {
    }

    void ActivePlanRunPlanStepMessage::execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const
    {
        const int nStatus = m_responseParam.response;

        int nWaitStatus = (TA_Base_Core::PSCS_SUCCESS == nStatus) ? ActiveStep::eWaitSucceed : ActiveStep::eWaitFailed;

        activePlan.stepResponsed( m_ulStepNum, nWaitStatus );
    }
}