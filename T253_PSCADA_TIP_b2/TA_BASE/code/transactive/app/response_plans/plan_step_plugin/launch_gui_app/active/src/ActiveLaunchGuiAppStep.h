#ifndef ACTIVE_LAUNCH_GUI_STEP_INCLUDED_H
#define ACTIVE_LAUNCH_GUI_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/launch_gui_app/common/LaunchGuiAppStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveLaunchGuiAppStep: public ActiveStep
    {
    public:
        ActiveLaunchGuiAppStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveLaunchGuiAppStep();

    protected:
		virtual void initImpl();
        virtual bool executeImpl();
        virtual int waitImpl();

    private:
        ActiveLaunchGuiAppStep(const ActiveLaunchGuiAppStep&);
        ActiveLaunchGuiAppStep& operator=(const ActiveLaunchGuiAppStep&);
    private:
        TA_Base_App::LaunchGuiAppStepParameter m_params;
    };

}


#endif //ACTIVE_LAUNCH_GUI_STEP_INCLUDED_H
