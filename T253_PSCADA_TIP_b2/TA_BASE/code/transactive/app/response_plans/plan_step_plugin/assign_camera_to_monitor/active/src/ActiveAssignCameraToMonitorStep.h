#ifndef ACTIVE_ASSIGN_CAMERA_STEP_INCLUDED_H
#define ACTIVE_ASSIGN_CAMERA_STEP_INCLUDED_H

#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/common/AssignCameraToMonitorStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"

namespace TA_Base_Core
{
	struct StepDetail;
}

namespace TA_Base_App
{
    class ActiveAssignCameraToMonitorStep: public ActiveStep
    {
    public:
        ActiveAssignCameraToMonitorStep(const BasicParameter& detail, IActivePlan& flowControl);
        virtual ~ActiveAssignCameraToMonitorStep();

    protected:
        virtual bool executeImpl();
    private:
        ActiveAssignCameraToMonitorStep(const ActiveAssignCameraToMonitorStep&);
        ActiveAssignCameraToMonitorStep& operator=(const ActiveAssignCameraToMonitorStep&);
    private:
        TA_Base_App::AssignCameraToMonitorStepParameter m_params;
    };

}

#endif //ACTIVE_ASSIGN_CAMERA_STEP_INCLUDED_H
