#ifndef ACTIVE_PLAN_CHECK_STEP_INCLUDED_H
#define ACTIVE_PLAN_CHECK_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/plan_check_timer/common/PlanCheckTimerStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActivePlanCheckTimerStep: public ActiveStep
    {
    public:
        ActivePlanCheckTimerStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActivePlanCheckTimerStep();

		virtual std::list<ActiveStep*> getNextStep();

    protected:
        virtual bool executeImpl();
    private:
        ActivePlanCheckTimerStep(const ActivePlanCheckTimerStep&);
        ActivePlanCheckTimerStep& operator=(const ActivePlanCheckTimerStep&);
		unsigned long getTime();
    private:
        TA_Base_App::PlanCheckTimerStepParameter m_params;

		unsigned long m_nextStepId;
    };

}


#endif //ACTIVE_LOG_STEP_INCLUDED_H
