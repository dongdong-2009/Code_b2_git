#ifndef PLAN_CHECK_STEP_PARAMETER_INCLUDE_H
#define PLAN_CHECK_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamPlan = "Plan";
const char* const ParamYesStep = "Yes Step";
const char* const ParamNoStep = "No Step";

class PlanCheckStepParameter : public BaseStepParameter
{
public:
    PlanCheckStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          plan(0),
          yesStep(0),
          noStep(0)
    {

    }

    PlanCheckStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          plan(0),
          yesStep(0),
          noStep(0)
    {
    }
    virtual ~PlanCheckStepParameter() {}

    unsigned long plan;
    unsigned int yesStep;
    unsigned int noStep;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(plan, ParamPlan);
        ioSerialization.serialize(yesStep, ParamYesStep);
        ioSerialization.serialize(noStep, ParamNoStep);
    }

private:
    //PlanCheckStepParameter(const PlanCheckStepParameter&);
    PlanCheckStepParameter& operator=(const PlanCheckStepParameter&);
};
}

#endif //PLAN_CHECK_STEP_PARAMETER_INCLUDE_H