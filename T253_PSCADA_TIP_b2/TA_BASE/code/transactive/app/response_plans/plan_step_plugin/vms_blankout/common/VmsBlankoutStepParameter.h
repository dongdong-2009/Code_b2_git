#ifndef VMS_BLANK_OUT_STEP_PARAMETER_INCLUDE_H
#define VMS_BLANK_OUT_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
class VmsBlankoutStepParameter: public BaseStepParameter
{
public:
    VmsBlankoutStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail)
    {
    }

    VmsBlankoutStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType)
    {
    }

    ~VmsBlankoutStepParameter() {}

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization) {}

private:
    //VmsBlankoutStepParameter(const VmsBlankoutStepParameter&);
    VmsBlankoutStepParameter& operator=(const VmsBlankoutStepParameter&);
};
}

#endif //LOG_STEP_PARAMETER_INCLUDE_H