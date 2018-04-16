#ifndef ACTIVE_PLAN_SET_TIMER_STEP_INCLUDED_H
#define ACTIVE_PLAN_SET_TIMER_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/plan_set_timer/common/PlanSetTimerStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActivePlanSetTimerStep: public ActiveStep
    {
    public:
        ActivePlanSetTimerStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActivePlanSetTimerStep();

    protected:
        virtual bool executeImpl();
    private:
        ActivePlanSetTimerStep(const ActivePlanSetTimerStep&);
        ActivePlanSetTimerStep& operator=(const ActivePlanSetTimerStep&);
		void setTime(unsigned long newTimer);
    private:
        TA_Base_App::PlanSetTimerStepParameter m_params;
    };

}


#endif //ACTIVE_LOG_STEP_INCLUDED_H
