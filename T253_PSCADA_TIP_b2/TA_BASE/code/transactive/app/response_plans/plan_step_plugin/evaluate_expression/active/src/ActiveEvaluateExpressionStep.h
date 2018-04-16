#ifndef ACTIVE_EVALUATE_EXPRESSION_STEP_INCLUDED_H
#define ACTIVE_EVALUATE_EXPRESSION_STEP_INCLUDED_H

#include "app/response_plans/plan_step_plugin/evaluate_expression/common/EvaluateExpressionStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
    struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveEvaluateExpressionStep: public ActiveStep
    {
    public:
        ActiveEvaluateExpressionStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveEvaluateExpressionStep();
        
		virtual std::list<ActiveStep*> getNextStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveEvaluateExpressionStep(const ActiveEvaluateExpressionStep&);
        ActiveEvaluateExpressionStep& operator=(const ActiveEvaluateExpressionStep&);
    private:
        TA_Base_App::EvaluateExpressionStepParameter m_params;

		unsigned long m_nextStepId;
    };

}


#endif //ACTIVE_EVALUATE_EXPRESSION_STEP_INCLUDED_H
