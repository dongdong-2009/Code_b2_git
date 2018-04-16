#ifndef PROMPTSTEPRESPONSEPARAMETER_INCLUDE_H
#define PROMPTSTEPRESPONSEPARAMETER_INCLUDE_H

#include <string>
#include <vector>
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"

namespace TA_Base_App
{
class PromptStepResponseParameter : public SerializableParameter
{
public:
    PromptStepResponseParameter(const unsigned int inResponse) : response(inResponse)
    {
    }
    PromptStepResponseParameter(const std::vector<std::string>& vecParameters)
    {
        deserialize(vecParameters);
    }
    ~PromptStepResponseParameter() {}

    int response;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(response, "response");
    }

private:
    PromptStepResponseParameter(const PromptStepResponseParameter&);
    PromptStepResponseParameter& operator=(const PromptStepResponseParameter&);
};
}

#endif //LOGUSERPARAMETER_INCLUDE_H