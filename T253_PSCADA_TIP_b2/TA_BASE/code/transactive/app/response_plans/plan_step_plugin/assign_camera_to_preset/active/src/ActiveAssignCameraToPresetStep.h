#ifndef ACTIVE_ASSIGN_CAMERA_TO_PRESET_STEP_INCLUDED_H
#define ACTIVE_ASSIGN_CAMERA_TO_PRESET_STEP_INCLUDED_H

//#include "app/response_plans/plan_agent/src/IActiveStepImplementation.h"

#include "app/response_plans/plan_step_plugin/assign_camera_to_preset/common/AssignCameraToPresetStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveAssignCameraToPresetStep: public ActiveStep
    {
    public:
        ActiveAssignCameraToPresetStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveAssignCameraToPresetStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveAssignCameraToPresetStep(const ActiveAssignCameraToPresetStep&);
        ActiveAssignCameraToPresetStep& operator=(const ActiveAssignCameraToPresetStep&);
    private:
        TA_Base_App::AssignCameraToPresetStepParameter m_params;
    };
}


#endif //ACTIVE_ASSIGN_CAMERA_TO_PRESET_STEP_INCLUDED_H
