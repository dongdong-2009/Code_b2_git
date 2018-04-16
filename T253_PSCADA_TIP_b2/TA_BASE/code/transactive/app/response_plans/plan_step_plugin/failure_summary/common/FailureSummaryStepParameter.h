#ifndef FAILURE_SUMMARY_STEP_PARAMETER_INCLUDE_H
#define FAILURE_SUMMARY_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamFailureCategory = "Failure Category";

class FailureSummaryStepParameter: public BaseStepParameter
{
public:
    FailureSummaryStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          failureCategory(SFC_SET_DATA_POINT_STEPS)
    {

    }

    FailureSummaryStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          failureCategory(SFC_SET_DATA_POINT_STEPS)
    {

    }
    virtual ~FailureSummaryStepParameter() {}

    enum EStepFailureCategory
    {
        SFC_ALL_STEPS,
        SFC_SET_DATA_POINT_STEPS,
        SFC_UNDEFINED
    };

    EStepFailureCategory failureCategory;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        int tmp = static_cast<int>(failureCategory);
        ioSerialization.serialize(tmp, ParamFailureCategory);
        failureCategory = static_cast<EStepFailureCategory>(tmp);
    }

private:
    //FailureSummaryStepParameter(const FailureSummaryStepParameter&);
    FailureSummaryStepParameter& operator=(const FailureSummaryStepParameter&);
};
}

#endif //FAILURE_SUMMARY_STEP_PARAMETER_INCLUDE_H