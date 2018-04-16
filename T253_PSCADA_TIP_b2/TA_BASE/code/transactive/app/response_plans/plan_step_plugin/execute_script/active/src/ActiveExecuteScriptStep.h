#ifndef ACTIVE_EXECUTE_SCRIPT_STEP_INCLUDED_H
#define ACTIVE_EXECUTE_SCRIPT_STEP_INCLUDED_H

#include "app/response_plans/plan_step_plugin/execute_script/common/ExecuteScriptStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"

namespace TA_Base_Core
{
    struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveExecuteScriptStep: public ActiveStep
    {
    public:
        ActiveExecuteScriptStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveExecuteScriptStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveExecuteScriptStep(const ActiveExecuteScriptStep&);
        ActiveExecuteScriptStep& operator=(const ActiveExecuteScriptStep&);
    private:
        TA_Base_App::ExecuteScriptStepParameter m_params;

        //TA_Base_Core::CommsMessageSender* m_commsSender;

    };

}


#endif //ACTIVE_EXECUTE_SCRIPT_STEP_INCLUDED_H
