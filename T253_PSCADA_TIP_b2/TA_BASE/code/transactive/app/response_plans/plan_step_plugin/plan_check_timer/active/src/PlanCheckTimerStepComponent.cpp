#include "PlanCheckTimerStepComponent.h"
#include "ActivePlanCheckTimerStep.h"

namespace TA_Base_App
{


    ActiveStep* PlanCheckTimerStepComponentImpl::createActiveStep( const BasicParameter& detail, TA_Base_App::IActivePlan& flowControl)
    {
        return new TA_Base_App::ActivePlanCheckTimerStep(detail, flowControl);
    }

    void PlanCheckTimerStepComponentImpl::validateStepParameter( const std::vector<std::string>& parameters )
    {

    }

    /*IActiveStepMessage* PlanCheckTimerStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& vecData )
    {
        return new TA_Base_App::ActivePlanLogStepMessage(ulStepNum, vecData);
    }*/

    PlanCheckTimerStepComponentImpl::PlanCheckTimerStepComponentImpl()
    {

    }

    PlanCheckTimerStepComponentImpl::~PlanCheckTimerStepComponentImpl()
    {

    }

}