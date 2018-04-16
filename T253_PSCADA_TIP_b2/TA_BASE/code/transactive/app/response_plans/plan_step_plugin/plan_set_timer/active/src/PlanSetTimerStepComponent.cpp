#include "PlanSetTimerStepComponent.h"
#include "ActivePlanSetTimerStep.h"

namespace TA_Base_App
{


    ActiveStep* PlanSetTimerStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActivePlanSetTimerStep(detail, flowControl);
    }

    void PlanSetTimerStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* PlanSetTimerStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    PlanSetTimerStepComponentImpl::PlanSetTimerStepComponentImpl()
    {

    }

    PlanSetTimerStepComponentImpl::~PlanSetTimerStepComponentImpl()
    {

    }

}