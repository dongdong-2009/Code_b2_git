#ifndef LAUNCH_SCHEMATIC_STEP_PARAMETER_INCLUDE_H
#define LAUNCH_SCHEMATIC_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamSchematic = "Schematic";
const char* const ParamScreen = "Screen";
const char* const ParamCommands = "Command Line";

class LaunchSchematicStepParameter: public BaseStepParameter
{
public:
    LaunchSchematicStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
		  schematic(""),
          screen(1u),
		  commands("")
    {
    }

    LaunchSchematicStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
		  schematic(""),
		  screen(1u),
		  commands("")
    {

    }
	virtual ~LaunchSchematicStepParameter() {}

    std::string schematic;
    unsigned long screen;
    std::string commands;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(schematic, ParamSchematic);
        ioSerialization.serialize(screen, ParamScreen);
        ioSerialization.serialize(commands, ParamCommands);
    }

private:
    //LaunchSchematicStepParameter(const LaunchSchematicStepParameter&);
    LaunchSchematicStepParameter& operator=(const LaunchSchematicStepParameter&);
};
}

#endif //LAUNCH_SCHEMATIC_STEP_PARAMETER_INCLUDE_H