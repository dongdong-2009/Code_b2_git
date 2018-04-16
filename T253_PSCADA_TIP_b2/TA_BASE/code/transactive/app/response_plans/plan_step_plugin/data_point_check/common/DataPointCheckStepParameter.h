#ifndef DATA_POINT_CHECK_STEP_PARAMETER_INCLUDE_H
#define DATA_POINT_CHECK_STEP_PARAMETER_INCLUDE_H

#include <string>

#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "app/response_plans/plan_step_plugin/common/src/DataPointEnum.h"
#include "bus/response_plans/PlanDataAccess/src/Serialization.h"

namespace TA_Base_App
{
const char* const ParamDataPoint = "Data Point Name";
const char* const ParamValueType = "Value Type";
const char* const ParamValue = "Value";
const char* const ParamTestType = "Test Type";
const char* const ParamSuccessStep = "Success Step";
const char* const ParamFailureStep = "Failure Step";

class DataPointCheckStepParameter: public BaseStepParameter
{
public:
    DataPointCheckStepParameter( const BasicParameter& detail ) : BaseStepParameter(detail),
    valueType(UNDEFINED_DATA_POINT),
    testType(UNDEFINED_TEST),
    successStep(0),
    failureStep(0),
    dataPoint(""),
    value("")
    {
    }

	DataPointCheckStepParameter( const unsigned long stepId, const int stepType ) : BaseStepParameter(stepId, stepType),
	valueType(UNDEFINED_DATA_POINT),
	testType(UNDEFINED_TEST),
	successStep(0),
    failureStep(0),
    dataPoint(""),
    value("")
	{
	}

    virtual ~DataPointCheckStepParameter()
    {
    }

    int valueType;
    int testType;
    unsigned int successStep;
    unsigned int failureStep;
    std::string dataPoint;
    std::string value;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        ioSerialization.serialize(dataPoint, ParamDataPoint);
        ioSerialization.serialize(valueType, ParamValueType);
        ioSerialization.serialize(value, ParamValue);
        ioSerialization.serialize(testType, ParamTestType);
        ioSerialization.serialize(successStep, ParamSuccessStep);
        ioSerialization.serialize(failureStep, ParamFailureStep);
    }

private:
    //DataPointCheckStepParameter(const DataPointCheckStepParameter&);
    DataPointCheckStepParameter& operator=(const DataPointCheckStepParameter&);
};
}

#endif //DATA_POINT_CHECK_STEP_PARAMETER_INCLUDE_H