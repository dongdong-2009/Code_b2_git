#ifndef TRAIN_PA_STEP_PARAMETER_INCLUDE_H
#define TRAIN_PA_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "app/response_plans/plan_step_plugin/pa/common/PaCommonDef.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"


namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamLibVersion = "Library Version";
const char* const ParamLibSection = "Library Section";
const char* const ParamMessageId = "Message Id";

const char* const ParamTrainId = "Train Id";
const char* const ParamTrainGroup = "Train Group";
const char* const ParamMessage = "Message";
const char* const ParamCyclic = "Cyclic";
const char* const ParamDuration = "Duration";

class TrainPaStepParameter: public BaseStepParameter
{
public:
    TrainPaStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
          trainId(0),
          trainGroup(0),
          cyclic(false),
          duration(60)
    {
    }

    TrainPaStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
          trainId(0),
          trainGroup(0),
          cyclic(false),
          duration(60)
    {
    }

    virtual ~TrainPaStepParameter()
    {
    }

    unsigned short trainId;
    int trainGroup;
    PAMessage message;
    bool cyclic;
    unsigned long duration;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(trainId, ParamTrainId);
        ioSerialization.serialize(trainGroup, ParamTrainGroup);
        ioSerialization.structSerialize(message, ParamMessage);
        ioSerialization.serialize(cyclic, ParamCyclic);
        ioSerialization.serialize(duration, ParamDuration);
    }

private:
    //TrainPaStepParameter(const TrainPaStepParameter&);
    TrainPaStepParameter& operator=(const TrainPaStepParameter&);
};
}

namespace TA_Base_Bus
{
	TA_SERIALIZATION(TA_Base_App::PAMessage, (libVersion)(libSection)(messageId))
}

#endif //TRAIN_PA_STEP_PARAMETER_INCLUDE_H