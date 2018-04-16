#ifndef ACTIVE_NULL_STEP_INCLUDED_H
#define ACTIVE_NULL_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/common/src/EmptyStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveNullStep: public ActiveStep
    {
    public:
        ActiveNullStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveNullStep();

    protected:
        virtual bool executeImpl();
        virtual int waitImpl();
    private:
        ActiveNullStep(const ActiveNullStep&);
        ActiveNullStep& operator=(const ActiveNullStep&);
    private:
        TA_Base_App::EmptyStepParameter m_params;
    };
}


#endif //ACTIVE_LOG_STEP_INCLUDED_H
