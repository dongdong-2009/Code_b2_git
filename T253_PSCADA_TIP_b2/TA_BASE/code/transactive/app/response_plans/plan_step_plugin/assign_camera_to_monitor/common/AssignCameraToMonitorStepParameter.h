#ifndef ASSIGN_CAMERA_TO_MONITOR_STEP_PARAMETER_INCLUDE_H
#define ASSIGN_CAMERA_TO_MONITOR_STEP_PARAMETER_INCLUDE_H

#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamMonitor = "Monitor";
const char* const ParamCamera = "Camera";

class AssignCameraToMonitorStepParameter: public BaseStepParameter
{
public:
    AssignCameraToMonitorStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          camera(0),
          monitor(0)
    {
    }

    AssignCameraToMonitorStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          camera(0),
          monitor(0)
    {

    }
    virtual ~AssignCameraToMonitorStepParameter() {}

    unsigned long camera;
    unsigned long monitor;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(camera, ParamCamera);
        ioSerialization.serialize(monitor, ParamMonitor);
    }

private:
    //AssignCameraToMonitorStepParameter(const AssignCameraToMonitorStepParameter&);
    AssignCameraToMonitorStepParameter& operator=(const AssignCameraToMonitorStepParameter&);
};
}

#endif //ASSIGN_CAMERA_TO_MONITOR_STEP_PARAMETER_INCLUDE_H
