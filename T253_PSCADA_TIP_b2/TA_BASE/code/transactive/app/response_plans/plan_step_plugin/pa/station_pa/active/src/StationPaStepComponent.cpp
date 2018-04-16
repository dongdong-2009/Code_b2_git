#include "app/response_plans/plan_step_plugin/pa/station_pa/active/src/StationPaStepComponent.h"
#include "app/response_plans/plan_step_plugin/pa/station_pa/active/src/ActiveStationPaStep.h"

namespace TA_Base_App
{


    ActiveStep* StationPaStepComponentImpl::createActiveStep( const BasicParameter& refDetail, TA_Base_App::IActivePlan& iPlan)
    {
        return new TA_Base_App::ActiveStationPaStep(refDetail, iPlan);
    }

    void StationPaStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    /*IActiveStepMessage* StationPaStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    StationPaStepComponentImpl::StationPaStepComponentImpl()
    {

    }

    StationPaStepComponentImpl::~StationPaStepComponentImpl()
    {

    }

}