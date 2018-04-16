#ifndef FAILURESUMMARYRESPONSEPARAMETER_INCLUDE_H
#define FAILURESUMMARYRESPONSEPARAMETER_INCLUDE_H

#include <string>
#include <vector>
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"

namespace TA_Base_App
{

class FailureSummaryResponseParameter : public SerializableParameter
{
public:
    FailureSummaryResponseParameter(const unsigned int inResponse) : response(inResponse)
    {
    }
    FailureSummaryResponseParameter(const std::vector<std::string>& vecParameters)
    {
        deserialize(vecParameters);
    }
    ~FailureSummaryResponseParameter() {}

    int response;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(response, "response");
    }
};

}

#endif //LOGUSERPARAMETER_INCLUDE_H