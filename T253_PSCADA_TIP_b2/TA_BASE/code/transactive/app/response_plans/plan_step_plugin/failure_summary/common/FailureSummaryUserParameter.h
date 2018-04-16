#ifndef FAILURESUMMARYPARAMETER_INCLUDE_H
#define FAILURESUMMARYPARAMETER_INCLUDE_H

#include <string>
#include <vector>
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

namespace TA_Base_App
{
	struct FailureStepDetail
	{
		unsigned long pos;
		std::string name;
		std::string error;
		FailureStepDetail() : pos(0)
		{
		}
	};

class FailureSummaryUserParameter : public SerializableParameter
{
public:
    FailureSummaryUserParameter()
    {
    }

    FailureSummaryUserParameter(const std::vector<FailureStepDetail>& failedSteps)
    {
        this->failedSteps = failedSteps;
    }

    FailureSummaryUserParameter(const std::vector<std::string>& vecParameters)
    {
        deserialize(vecParameters);
    }

    ~FailureSummaryUserParameter() {}

    std::vector<FailureStepDetail> failedSteps;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(failedSteps, "failedSteps");
    }
};

}

namespace TA_Base_Bus
{
	TA_SERIALIZATION(TA_Base_App::FailureStepDetail, (pos)(name)(error))
}

#endif //LOGUSERPARAMETER_INCLUDE_H