#ifndef ACTIVE_JUMP_STEP_INCLUDED_H
#define ACTIVE_JUMP_STEP_INCLUDED_H

#include "app/response_plans/plan_step_plugin/jump/common/JumpStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveJumpStep: public ActiveStep
    {
    public:
        ActiveJumpStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveJumpStep();

		virtual std::list<ActiveStep*> getNextStep();

    protected:
        virtual bool executeImpl();

    private:
        ActiveJumpStep(const ActiveJumpStep&);
        ActiveJumpStep& operator=(const ActiveJumpStep&);

    private:
        JumpStepParameter m_params;
    };

}


#endif //ACTIVE_JUMP_STEP_INCLUDED_H
