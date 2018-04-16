#ifndef VMS_SET_SCENE_STEP_PARAMETER_INCLUDE_H
#define VMS_SET_SCENE_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_App
{
class VmsSetSceneStepParameter: public BaseStepParameter
{
public:
    VmsSetSceneStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail)
    {
    }

    VmsSetSceneStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType)
    {
    }

    ~VmsSetSceneStepParameter() {}

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization) {}
private:
    //VmsSetSceneStepParameter(const VmsSetSceneStepParameter&);
    VmsSetSceneStepParameter& operator=(const VmsSetSceneStepParameter&);
};
}

#endif //LOG_STEP_PARAMETER_INCLUDE_H