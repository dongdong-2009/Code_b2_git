
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanLaunchSchematicStepMessage.cpp $
  *  @author:  Bart Golab
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/07/10 16:23:57 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Generated when the result of a Launch Schematic interactive step execution request
  *  is received back from the Plan Manager.
  */
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "ActivePlanLaunchSchematicStepMessage.h"
#include "app/response_plans/common/src/StringConstants.h"
/*
#include "app/response_plans/plan_agent/src/ActivePlan.h"
#include "app/response_plans/plan_agent/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActivePlanLaunchSchematicStepMessage.h"
*/


namespace TA_Base_App
{
    ActivePlanLaunchSchematicStepMessage::ActivePlanLaunchSchematicStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
		: m_ulStepNum(ulStepNum),
		  m_responseParam(vecData)
    {
    }


    ActivePlanLaunchSchematicStepMessage::~ActivePlanLaunchSchematicStepMessage()
    {
    }


    void ActivePlanLaunchSchematicStepMessage::execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const
    {
        const int nWaitStatus = ( TA_Base_Core::PSCS_SUCCESS == m_responseParam.response ) ? ActiveStep::eWaitSucceed : ActiveStep::eWaitFailed;

        activePlan.stepResponsed( m_ulStepNum, nWaitStatus );
    }
}