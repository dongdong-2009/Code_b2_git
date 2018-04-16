#include "StationTisStepComponent.h"
#include "ActiveStationTisStep.h"

namespace TA_Base_App
{


    ActiveStep* StationTisStepComponentImpl::createActiveStep(const BasicParameter& refDetail, TA_Base_App::IActivePlan& iPlan)
    {
        return new ActiveStationTisStep(refDetail, iPlan);
    }

    void StationTisStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {
    }

    /*IActiveStepMessage* StationTisStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    StationTisStepComponentImpl::StationTisStepComponentImpl()
    {

    }

    StationTisStepComponentImpl::~StationTisStepComponentImpl()
    {

    }

}