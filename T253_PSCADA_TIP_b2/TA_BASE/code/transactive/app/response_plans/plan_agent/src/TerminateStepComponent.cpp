#include "app/response_plans/plan_agent/src/TerminateStepComponent.h"
#include "app/response_plans/plan_agent/src/ActiveTerminatePlanStep.h"

namespace TA_Base_App
{
    ActiveStep* TerminateStepComponentImpl::createActiveStep(const BasicParameter& detail, IActivePlan& plan)
    {
        return new ActiveTerminatePlanStep( detail, plan );
    }

    void TerminateStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    TerminateStepComponentImpl::TerminateStepComponentImpl()
    {

    }

    TerminateStepComponentImpl::~TerminateStepComponentImpl()
    {

    }

}