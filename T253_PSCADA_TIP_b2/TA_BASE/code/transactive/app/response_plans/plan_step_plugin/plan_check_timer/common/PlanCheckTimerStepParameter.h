#ifndef PLAN_CHECK_TIMER_STEP_PARAMETER_INCLUDE_H
#define PLAN_CHECK_TIMER_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamTimeSpan = "Timespan";
const char* const ParamSuccessStep = "Success Step";
const char* const ParamFailureStep = "Failure Step";

class PlanCheckTimerStepParameter: public BaseStepParameter
{
public:
    PlanCheckTimerStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          timeSpan(1),
          successStep(0),
          failureStep(0)
    {
    }

    PlanCheckTimerStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          timeSpan(1),
          successStep(0),
          failureStep(0)
    {

    }
    virtual ~PlanCheckTimerStepParameter()
    {
    }

    unsigned long timeSpan;
    unsigned int successStep;
    unsigned int failureStep;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(timeSpan, "timeSpan");
        ioSerialization.serialize(successStep, "successStep");
        ioSerialization.serialize(failureStep, "failureStep");
    }

private:
    //PlanCheckTimerStepParameter(const PlanCheckTimerStepParameter&);
    PlanCheckTimerStepParameter& operator=(const PlanCheckTimerStepParameter&);
};
}

#endif //LOG_STEP_PARAMETER_INCLUDE_H