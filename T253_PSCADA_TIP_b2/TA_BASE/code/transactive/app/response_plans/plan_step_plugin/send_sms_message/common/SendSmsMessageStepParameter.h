#ifndef SEND_SMS_MESSAGE_STEP_PARAMETER_INCLUDE_H
#define SEND_SMS_MESSAGE_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_App
{
class SendSmsMessageStepParameter: public BaseStepParameter
{
public:
    SendSmsMessageStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail)
    {
    }

    SendSmsMessageStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType)
    {
    }
	virtual ~SendSmsMessageStepParameter() {}
protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization) {}
private:
    //SendSmsMessageStepParameter(const SendSmsMessageStepParameter&);
    SendSmsMessageStepParameter& operator=(const SendSmsMessageStepParameter&);
};
}

#endif //LOG_STEP_PARAMETER_INCLUDE_H