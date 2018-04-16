#ifndef ACTIVE_PLAN_DECISION_STEP_MESSAGE_INCLUDED_H
#define ACTIVE_PLAN_DECISION_STEP_MESSAGE_INCLUDED_H

#include <string>
#include <vector>
#include "app/response_plans/plan_step_plugin/common_active/src/IActiveStepMessage.h"

namespace TA_Base_App
{
    class ActivePlanDecisionStepMessage: public IActiveStepMessage
    {
    public:
        //ActivePlanDecisionStepMessage(const PlanMessageExecutionResultMap& results);
        ActivePlanDecisionStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData );
        virtual ~ActivePlanDecisionStepMessage();
        virtual void execute( IActivePlan& activePlan, IMessageSender& refMsgSender ) const; // TODO:To be discarded
    private:
        ActivePlanDecisionStepMessage(const ActivePlanDecisionStepMessage&);
        ActivePlanDecisionStepMessage& operator=(const ActivePlanDecisionStepMessage&);

        unsigned long m_ulStepNum;
        std::vector<std::string> m_vecData;
    };

}

#endif //ACTIVE_PLAN_DECISION_STEP_MESSAGE_INCLUDED_H
