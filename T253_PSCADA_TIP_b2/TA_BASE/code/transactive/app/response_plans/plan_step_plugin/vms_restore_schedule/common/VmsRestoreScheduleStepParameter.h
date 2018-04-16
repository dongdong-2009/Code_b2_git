#ifndef VMS_RESTORE_SCHEDULE_STEP_PARAMETER_INCLUDE_H
#define VMS_RESTORE_SCHEDULE_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_App
{
class VmsRestoreScheduleStepParameter: public BaseStepParameter
{
public:
    VmsRestoreScheduleStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail)
    {
    }

    VmsRestoreScheduleStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType)
    {
    }

    ~VmsRestoreScheduleStepParameter() {}

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization) {}
private:
    //VmsRestoreScheduleStepParameter(const VmsRestoreScheduleStepParameter&);
    VmsRestoreScheduleStepParameter& operator=(const VmsRestoreScheduleStepParameter&);
};
}

#endif //LOG_STEP_PARAMETER_INCLUDE_H