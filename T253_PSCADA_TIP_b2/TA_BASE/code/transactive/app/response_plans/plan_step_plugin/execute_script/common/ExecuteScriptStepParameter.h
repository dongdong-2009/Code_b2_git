#ifndef EXECUTE_SCRIPT_STEP_PARAMETER_INCLUDE_H
#define EXECUTE_SCRIPT_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamCommands = "Commands";

struct ExecuteScriptStepParameter: public BaseStepParameter
{
public:
    ExecuteScriptStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
		  commands("")
    {

    }

    ExecuteScriptStepParameter(const unsigned long stepId, const int stepType)
		: BaseStepParameter(stepId, stepType),
		  commands("")
    {

    }
    virtual ~ExecuteScriptStepParameter() {}

    std::string commands;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(commands, ParamCommands);
    }

private:
    //ExecuteScriptStepParameter(const ExecuteScriptStepParameter&);
    ExecuteScriptStepParameter& operator=(const ExecuteScriptStepParameter&);
};
}

#endif //EXECUTE_SCRIPT_STEP_PARAMETER_INCLUDE_H