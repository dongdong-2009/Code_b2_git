#include "DataPointTimeCheckStepComponent.h"
#include "ActiveDataPointTimeCheckStep.h"
//#include "ActivePlanLogStepMessage.h"

namespace TA_Base_App
{


    ActiveStep* DataPointTimeCheckStepComponentImpl::createActiveStep(const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveDataPointTimeCheckStep(detail, flowControl);
    }

    void DataPointTimeCheckStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    /*IActiveStepMessage* DataPointTimeCheckStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    DataPointTimeCheckStepComponentImpl::DataPointTimeCheckStepComponentImpl()
    {

    }

    DataPointTimeCheckStepComponentImpl::~DataPointTimeCheckStepComponentImpl()
    {

    }

}