#ifndef ACTIVE_PLAN_CHECK_STEP_INCLUDED_H
#define ACTIVE_PLAN_CHECK_STEP_INCLUDED_H

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/plan_check/common/PlanCheckStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActivePlanCheckStep: public ActiveStep
    {
    public:
        ActivePlanCheckStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActivePlanCheckStep();

		virtual std::list<ActiveStep*> getNextStep();

    protected:
        virtual bool executeImpl();
    private:
        ActivePlanCheckStep(const ActivePlanCheckStep&);
        ActivePlanCheckStep& operator=(const ActivePlanCheckStep&);

		bool isActive( const TA_Base_Core::ActivePlanDetails& activePlanDetails, const TA_Base_Core::ActivePlanId& ignoreInstance ) const;
		bool isActive( const TA_Base_Core::EActivePlanState activePlanState) const;
    private:
        TA_Base_App::PlanCheckStepParameter m_params;
		unsigned long m_nextStepId;
    };
}


#endif //ACTIVE_LOG_STEP_INCLUDED_H
