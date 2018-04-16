#include "app/response_plans/plan_step_plugin/data_point_check/active/src/DataPointCheckStepComponent.h"
#include "app/response_plans/plan_step_plugin/data_point_check/active/src/ActiveDataPointCheckStep.h"

namespace TA_Base_App
{


    ActiveStep* DataPointCheckStepComponentImpl::createActiveStep(const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveDataPointCheckStep(detail, flowControl);
    }

    void DataPointCheckStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    /*IActiveStepMessage* DataPointCheckStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    DataPointCheckStepComponentImpl::DataPointCheckStepComponentImpl()
    {

    }

    DataPointCheckStepComponentImpl::~DataPointCheckStepComponentImpl()
    {

    }

}