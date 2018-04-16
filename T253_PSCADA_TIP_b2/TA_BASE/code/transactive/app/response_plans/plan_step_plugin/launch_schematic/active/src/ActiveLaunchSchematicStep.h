#ifndef ACTIVE_LAUNCH_SCHEMATIC_STEP_INCLUDED_H
#define ACTIVE_LAUNCH_SCHEMATIC_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"
#include "app/response_plans/plan_step_plugin/launch_schematic/common/LaunchSchematicStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveLaunchSchematicStep: public ActiveStep
    {
    public:
        ActiveLaunchSchematicStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveLaunchSchematicStep();

    protected:
		virtual void initImpl();
        virtual bool executeImpl();
        virtual int waitImpl();

    private:
        ActiveLaunchSchematicStep(const ActiveLaunchSchematicStep&);
        ActiveLaunchSchematicStep& operator=(const ActiveLaunchSchematicStep&);
    private:
        TA_Base_App::LaunchSchematicStepParameter m_params;
    };

}


#endif //ACTIVE_LAUNCH_SCHEMATIC_STEP_INCLUDED_H
