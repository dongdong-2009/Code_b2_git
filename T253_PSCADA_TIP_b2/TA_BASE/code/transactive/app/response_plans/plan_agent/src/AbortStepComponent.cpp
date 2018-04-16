#include "app/response_plans/plan_agent/src/AbortStepComponent.h"
#include "app/response_plans/plan_agent/src/ActiveAbortStep.h"

namespace TA_Base_App
{
    TA_Base_App::ActiveStep* AbortStepComponentImpl::createActiveStep( const BasicParameter& detail, IActivePlan& plan )
    {
        return new ActiveAbortStep( detail, plan );
    }

    void AbortStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    AbortStepComponentImpl::AbortStepComponentImpl()
    {

    }

    AbortStepComponentImpl::~AbortStepComponentImpl()
    {

    }

}