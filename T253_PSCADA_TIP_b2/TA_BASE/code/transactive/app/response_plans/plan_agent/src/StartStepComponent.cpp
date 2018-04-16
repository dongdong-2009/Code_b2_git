#include "app/response_plans/plan_agent/src/StartStepComponent.h"
#include "app/response_plans/plan_agent/src/ActiveStartStep.h"

namespace TA_Base_App
{
    ActiveStep* StartStepComponentImpl::createActiveStep( const BasicParameter& detail, IActivePlan& plan )
    {
        return new ActiveStartStep( detail, plan );
    }

    void StartStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    StartStepComponentImpl::StartStepComponentImpl()
    {

    }

    StartStepComponentImpl::~StartStepComponentImpl()
    {

    }

}