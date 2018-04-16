#ifndef ACTIVE_PROMPT_STEP_INCLUDED_H
#define ACTIVE_PROMPT_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/prompt/common/PromptStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActivePromptStep: public ActiveStep
    {
    public:
        ActivePromptStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActivePromptStep();

    protected:
		virtual void initImpl();
        virtual bool executeImpl();
        virtual int waitImpl();

    private:
        ActivePromptStep(const ActivePromptStep&);
        ActivePromptStep& operator=(const ActivePromptStep&);
    private:
        TA_Base_App::PromptStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;

    };

}


#endif //ACTIVE_PROMPT_STEP_INCLUDED_H
