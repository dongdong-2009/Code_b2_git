#ifndef RUNPLANUSERPARAMETER_INCLUDE_H
#define RUNPLANUSERPARAMETER_INCLUDE_H

#include <string>
#include <vector>
#include "app/response_plans/plan_step_plugin/common/src/SerializableParameter.h"

namespace TA_Base_App
{

const char* const ParamChildPlanId = "Child Plan Id";
const char* const ParamChildPlanInstance = "Child Plan Instance";
const char* const ParamChildPlanLocation = "Child Plan Location";

class RunPlanUserParameter : public SerializableParameter
{
public:
    RunPlanUserParameter(const unsigned int inChildPlanId, const unsigned int inChildPlanInstance, const unsigned int inChildPlanLocation) :
    childPlanId(inChildPlanId),
    childPlanInstance(inChildPlanInstance),
    childPlanLocation(inChildPlanLocation)
    {
    }
    
    RunPlanUserParameter(const std::vector<std::string>& vecParameters) : 
    childPlanId(0u),
    childPlanInstance(0u),
    childPlanLocation(0u)
    {
        deserialize(vecParameters);
    }

    ~RunPlanUserParameter() {}

public:
    unsigned int childPlanId;
    unsigned int childPlanInstance;
    unsigned int childPlanLocation;

protected:
    virtual void serializeImp( TA_Base_Bus::Serialization& ioSerialization )
    {
        ioSerialization.serialize( childPlanId, ParamChildPlanId );
        ioSerialization.serialize( childPlanInstance, ParamChildPlanInstance );
        ioSerialization.serialize( childPlanLocation, ParamChildPlanLocation );
    }
};

}

#endif //LOGUSERPARAMETER_INCLUDE_H