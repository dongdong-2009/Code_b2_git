#ifndef LAUNCH_GUI_STEP_PARAMETER_INCLUDE_H
#define LAUNCH_GUI_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamApp = "Application";
const char* const ParamArgs = "Command Line";
const char* const ParamPosition = "Position";
const char* const ParamPositionTop = "Y Coordinate";
const char* const ParamPositionLeft = "X Coordinate";
const char* const ParamPositionWidth = "Width";
const char* const ParamPositionHeight = "Height";

class LaunchGuiAppStepParameter: public BaseStepParameter
{
public:
    LaunchGuiAppStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          app(0),
		  args("")
    {

    }

    LaunchGuiAppStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          app(0),
		  args("")
    {

    }
    virtual ~LaunchGuiAppStepParameter() {}

    unsigned long app;
    std::string args;
    struct WindowPosition
    {
        unsigned long top;
        unsigned long left;
        unsigned long width;
        unsigned long height;
        WindowPosition() : top(155), left(0), width(0), height(0)
        {
        }
    };
    WindowPosition pos;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(app, ParamApp);
        ioSerialization.serialize(args, ParamArgs);
        ioSerialization.structSerialize(pos, ParamPosition);
    }

private:
    //LaunchGuiAppStepParameter(const LaunchGuiAppStepParameter&);
    LaunchGuiAppStepParameter& operator=(const LaunchGuiAppStepParameter&);
};
}

namespace TA_Base_Bus
{
	TA_SERIALIZATION(TA_Base_App::LaunchGuiAppStepParameter::WindowPosition, (top)(left)(width)(height))
}

#endif //LAUNCH_GUI_STEP_PARAMETER_INCLUDE_H