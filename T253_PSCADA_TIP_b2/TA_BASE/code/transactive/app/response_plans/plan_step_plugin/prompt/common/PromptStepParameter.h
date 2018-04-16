#ifndef PROMPT_STEP_PARAMETER_INCLUDE_H
#define PROMPT_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamMessage = "Prompt Message";
const char* const ParamDismissTimeout = "Dismiss Timeout";

class PromptStepParameter: public BaseStepParameter
{
public:
    PromptStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
		  message(""),
          dismissTimeout( 60u )
    {
    }

    PromptStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
		  message(""),
          dismissTimeout( 60u )
    {
    }
    virtual ~PromptStepParameter() {}

    std::string message;
    unsigned long dismissTimeout;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(message, ParamMessage);
        ioSerialization.serialize(dismissTimeout, ParamDismissTimeout);
    }

private:
    //PromptStepParameter(const PromptStepParameter&);
    PromptStepParameter& operator=(const PromptStepParameter&);
};
}

#endif //PROMPT_STEP_PARAMETER_INCLUDE_H