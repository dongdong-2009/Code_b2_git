#ifndef DATA_POINT_SET_STEP_PARAMETER_INCLUDE_H
#define DATA_POINT_SET_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"
#include "app/response_plans/plan_step_plugin/common/src/DataPointEnum.h"

namespace TA_Base_App
{
const char* const ParamDataPoint = "Data Point Name";
const char* const ParamNewValueType = "New Value Type";
const char* const ParamNewValue = "New Value";

class DataPointSetStepParameter: public BaseStepParameter
{
public:
    DataPointSetStepParameter(const BasicParameter& detail) : BaseStepParameter(detail),
    newValueType(UNDEFINED_DATA_POINT),
	dataPoint(""),
	newValue("")
    {
    }

    DataPointSetStepParameter(const unsigned long stepId, const int stepType) : BaseStepParameter(stepId, stepType),
    newValueType(UNDEFINED_DATA_POINT),
    dataPoint(""),
    newValue("")
    {
    }

    virtual ~DataPointSetStepParameter()
    {
    }

    int newValueType;
    std::string dataPoint;
    std::string newValue;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(dataPoint, ParamDataPoint);
        ioSerialization.serialize(newValueType, ParamNewValueType);
        ioSerialization.serialize(newValue, ParamNewValue);
    }

private:
    //DataPointSetStepParameter(const DataPointSetStepParameter&);
    DataPointSetStepParameter& operator=(const DataPointSetStepParameter&);
};
}

#endif //DATA_POINT_SET_STEP_PARAMETER_INCLUDE_H