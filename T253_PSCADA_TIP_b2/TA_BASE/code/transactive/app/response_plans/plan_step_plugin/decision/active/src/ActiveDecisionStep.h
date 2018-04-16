#ifndef ACTIVE_DECISION_STEP_INCLUDED_H
#define ACTIVE_DECISION_STEP_INCLUDED_H

#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_step_plugin/decision/common/DecisionStepParameter.h"

namespace TA_Base_App
{
    class ActiveDecisionStep: public ActiveStep
    {
    public:
        ActiveDecisionStep(const BasicParameter& detail, IActivePlan& plan);
        virtual ~ActiveDecisionStep();

		virtual std::list<ActiveStep*> getNextStep();
        unsigned long getNextDecisionStep(const int status);

    protected:
		virtual void initImpl();
        virtual bool executeImpl();
		virtual int waitImpl();
		
    private:
        ActiveDecisionStep(const ActiveDecisionStep&);
        ActiveDecisionStep& operator=(const ActiveDecisionStep&);

    private:
        DecisionStepParameter m_params;
		unsigned long m_nextStepId;

        //TA_Base_Core::CommsMessageSender* m_commsSender;
    };
}


#endif //ACTIVE_DECISION_STEP_INCLUDED_H
