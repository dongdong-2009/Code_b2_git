#ifndef DATA_POINT_TIME_CHECK_STEP_PARAMETER_INCLUDE_H
#define DATA_POINT_TIME_CHECK_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"
//#include "bus/scada/common_library/IDL/src/IDataPointAccessCorbaDef.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{
const char* const ParamDataPoint = "Data Point Name";
const char* const ParamTimeSpan = "Timespan";
const char* const ParamSuccessStep = "Success Step";
const char* const ParamFailureStep = "Failure Step";

class DataPointTimeCheckStepParameter: public BaseStepParameter
{
public:
    DataPointTimeCheckStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
		  dataPoint(""),
          timeSpan(1),
          successStep(0),
          failureStep(0)
    {
    }

    DataPointTimeCheckStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
		  dataPoint(""),
          timeSpan(1),
          successStep(0),
          failureStep(0)
    {

    }
    virtual ~DataPointTimeCheckStepParameter() {}

    std::string dataPoint;
    unsigned long timeSpan;
    unsigned int successStep;
    unsigned int failureStep;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(dataPoint, ParamDataPoint);
        ioSerialization.serialize(timeSpan, ParamTimeSpan);
        ioSerialization.serialize(successStep, ParamSuccessStep);
        ioSerialization.serialize(failureStep, ParamFailureStep);
    }

private:
    //DataPointTimeCheckParameter(const DataPointTimeCheckParameter&);
    DataPointTimeCheckStepParameter& operator=(const DataPointTimeCheckStepParameter&);
};
}

#endif //DATA_POINT_TIME_CHECK_STEP_PARAMETER_INCLUDE_H