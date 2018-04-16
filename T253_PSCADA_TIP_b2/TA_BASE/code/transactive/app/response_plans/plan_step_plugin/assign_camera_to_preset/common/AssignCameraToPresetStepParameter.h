#ifndef ASSIGN_CAMERA_TO_PRESET_STEP_PARAMETER_INCLUDE_H
#define ASSIGN_CAMERA_TO_PRESET_STEP_PARAMETER_INCLUDE_H

#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{

const char* const ParamCamera = "Camera";
const char* const ParamPreset = "Preset";

class AssignCameraToPresetStepParameter : public BaseStepParameter
{
public:
    AssignCameraToPresetStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          camera(0),
          preset(0)
    {

    }

    AssignCameraToPresetStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          camera(0),
          preset(0)
    {

    }
    virtual ~AssignCameraToPresetStepParameter() {}
    unsigned long camera;
    unsigned long preset;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(camera, ParamCamera);
        ioSerialization.serialize(preset, ParamPreset);
    }

private:
    //AssignCameraToPresetStepParameter(const AssignCameraToPresetStepParameter&);
    AssignCameraToPresetStepParameter& operator=(const AssignCameraToPresetStepParameter&);
};
}

#endif //ASSIGN_CAMERA_TO_PRESET_STEP_PARAMETER_INCLUDE_H
