#ifndef TRAIN_TIS_STEP_PARAMETER_INCLUDE_H
#define TRAIN_TIS_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "app/response_plans/plan_step_plugin/tis/common/TisCommonDef.h"


namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamLibVersion = "Library Version";
const char* const ParamLibSection = "Library Section";
const char* const ParamMessageId = "MessageId";

const char* const ParamTrainId = "TrainId";
const char* const ParamTrainGroup = "TrainGroup";
const char* const ParamMessage = "Message";
const char* const ParamPriority = "Priority";
const char* const ParamDuration = "Duration";

class TrainTisStepParameter: public BaseStepParameter
{
public:
    TrainTisStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          trainId(0),
          trainGroup(0),
          priority(0),
          duration(60)
    {

    }

    TrainTisStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          trainId(0),
          trainGroup(0),
          priority(0),
          duration(60)
    {
    }

    virtual ~TrainTisStepParameter()
    {
    }

    unsigned short trainId;
    int trainGroup;

    TISMessage message;
    unsigned short priority;
    unsigned long duration;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(trainId, ParamTrainId);
        ioSerialization.serialize(trainGroup, ParamTrainGroup);
        ioSerialization.structSerialize(message, ParamMessage);
        ioSerialization.serialize(priority, ParamPriority);
        ioSerialization.serialize(duration, ParamDuration);
    }

private:
    //TrainTisStepParameter(const TrainTisStepParameter&);
    TrainTisStepParameter& operator=(const TrainTisStepParameter&);
};
}

namespace TA_Base_Bus
{
	TA_SERIALIZATION(TA_Base_App::TISMessage, (libraryVersion)(librarySection)(messageId))
}

#endif //TRAIN_TIS_STEP_PARAMETER_INCLUDE_H