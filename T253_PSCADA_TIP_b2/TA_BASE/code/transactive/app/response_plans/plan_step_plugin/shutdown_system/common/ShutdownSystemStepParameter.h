#ifndef SHUTDOWN_SYSTEM_STEP_PARAMETER_INCLUDE_H
#define SHUTDOWN_SYSTEM_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamNotifyPeer = "Notify Peer";

class ShutdownSystemStepParameter: public BaseStepParameter
{
public:
    ShutdownSystemStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          notifyPeer(true)
    {

    }

    ShutdownSystemStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          notifyPeer(true)
    {
    }

    ShutdownSystemStepParameter::~ShutdownSystemStepParameter()
    {
    }

	bool notifyPeer;


protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(notifyPeer, ParamNotifyPeer);
    }

private:
    //ShutdownSystemStepParameter(const ShutdownSystemStepParameter&);
    ShutdownSystemStepParameter& operator=(const ShutdownSystemStepParameter&);
};
}

#endif //SHUTDOWN_SYSTEM_STEP_PARAMETER_INCLUDE_H