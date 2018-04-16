#ifndef JUMP_STEP_PARAMETER_INCLUDE_H
#define JUMP_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamStepId = "StepId";

class JumpStepParameter: public BaseStepParameter
{
public:
    JumpStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          stepId(0)
    {

    }

    JumpStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          stepId(0)
    {

    }
    virtual ~JumpStepParameter() {}

    unsigned int stepId;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(stepId, ParamStepId);
    }

private:
    //JumpStepParameter(const JumpStepParameter&);
    JumpStepParameter& operator=(const JumpStepParameter&);
};
}

#endif //JUMP_STEP_PARAMETER_INCLUDE_H