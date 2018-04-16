#ifndef PLAN_SET_TIMER_STEP_PARAMETER_INCLUDE_H
#define PLAN_SET_TIMER_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamTime = "Timespan";

class PlanSetTimerStepParameter: public BaseStepParameter
{
public:
    PlanSetTimerStepParameter::PlanSetTimerStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          time(1)
    {
    }

    PlanSetTimerStepParameter::PlanSetTimerStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          time(1)
    {
    }
    virtual ~PlanSetTimerStepParameter() {}

    unsigned long time;
protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(time, ParamTime);
    }
private:
    //PlanSetTimerStepParameter(const PlanSetTimerStepParameter&);
    PlanSetTimerStepParameter& operator=(const PlanSetTimerStepParameter&);
};
}

#endif //LOG_STEP_PARAMETER_INCLUDE_H