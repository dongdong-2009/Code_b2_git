#ifndef EVALUATE_EXPRESSION_STEP_PARAMETER_INCLUDE_H
#define EVALUATE_EXPRESSION_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

namespace TA_Base_App
{
const char* const ParamExpression = "Expression";
const char* const ParamSuccessStep = "SuccessStep";
const char* const ParamFailureStep = "FailureStep";

class EvaluateExpressionStepParameter: public BaseStepParameter
{
public:
    EvaluateExpressionStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
		  expression(""),
          successStep(0),
          failureStep(0)
    {

    }

    EvaluateExpressionStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
		  expression(""),
          successStep(0),
          failureStep(0)
    {

    }
    virtual ~EvaluateExpressionStepParameter() {}

    std::string expression;
    unsigned int successStep;
    unsigned int failureStep;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(expression, ParamExpression);
        ioSerialization.serialize(successStep, ParamSuccessStep);
        ioSerialization.serialize(failureStep, ParamFailureStep);
    }

private:
    //EvaluateExpressionStepParameter(const EvaluateExpressionStepParameter&);
    EvaluateExpressionStepParameter& operator=(const EvaluateExpressionStepParameter&);
};
}

#endif //EVALUATE_EXPRESSION_STEP_PARAMETER_INCLUDE_H