#include "app/response_plans/plan_agent/src/EndStepComponent.h"
#include "app/response_plans/plan_agent/src/ActiveEndStep.h"

namespace TA_Base_App
{
    ActiveStep* EndStepComponentImpl::createActiveStep( const BasicParameter& detail, IActivePlan& plan )
    {
        return new ActiveEndStep( detail, plan );
    }

    void EndStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    EndStepComponentImpl::EndStepComponentImpl()
    {

    }

    EndStepComponentImpl::~EndStepComponentImpl()
    {

    }

}