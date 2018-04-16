#include "app/response_plans/plan_step_plugin/data_point_set/active/src/DatapointSetStepComponent.h"
#include "app/response_plans/plan_step_plugin/data_point_set/active/src/ActiveDatapointSetStep.h"

namespace TA_Base_App
{


    ActiveStep* DataPointSetStepComponentImpl::createActiveStep(const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActiveDataPointSetStep(detail, flowControl);
    }

    void DataPointSetStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    /*IActiveStepMessage* DataPointSetStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActiveDataPointSetStep(ulStepNum, vecData);
    }*/

    DataPointSetStepComponentImpl::DataPointSetStepComponentImpl()
    {

    }

    DataPointSetStepComponentImpl::~DataPointSetStepComponentImpl()
    {

    }

}