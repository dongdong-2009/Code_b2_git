
#include "app/response_plans/plan_step_plugin/decision/active/src/ActivePlanDecisionStepMessage.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IMessageSender.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"


namespace TA_Base_App
{

    ActivePlanDecisionStepMessage::ActivePlanDecisionStepMessage(const unsigned long ulStepNum, const std::vector<std::string>& vecData) : 
    m_ulStepNum(ulStepNum),
    m_vecData( vecData )
    {
    }

    ActivePlanDecisionStepMessage::~ActivePlanDecisionStepMessage()
    {
    }

    void ActivePlanDecisionStepMessage::execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const
    {
        activePlan.stepData( m_ulStepNum, m_vecData );
    }

}