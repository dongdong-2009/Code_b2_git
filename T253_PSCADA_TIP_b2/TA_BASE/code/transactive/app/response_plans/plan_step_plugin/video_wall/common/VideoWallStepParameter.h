#ifndef VIDEO_WALL_STEP_PARAMETER_INCLUDE_H
#define VIDEO_WALL_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
class VideoWallStepParameter: public BaseStepParameter
{
public:
    VideoWallStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail)
    {
    }

    VideoWallStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType)
    {
    }

    ~VideoWallStepParameter() {}

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization) {}

private:
    //VideoWallStepParameter(const VideoWallStepParameter&);
    VideoWallStepParameter& operator=(const VideoWallStepParameter&);
};
}

#endif //LOG_STEP_PARAMETER_INCLUDE_H