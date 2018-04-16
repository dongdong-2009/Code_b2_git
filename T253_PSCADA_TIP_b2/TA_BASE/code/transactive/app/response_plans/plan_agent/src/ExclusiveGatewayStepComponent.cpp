#include "app/response_plans/plan_agent/src/ExclusiveGatewayStepComponent.h"
#include "app/response_plans/plan_agent/src/ActiveExclusiveGateway.h"

namespace TA_Base_App
{
    ActiveStep* ExclusiveGatewayStepComponentImpl::createActiveStep( const BasicParameter& detail, IActivePlan& plan )
    {
        return new ActiveExclusiveGateway( detail, plan );
    }

    void ExclusiveGatewayStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    ExclusiveGatewayStepComponentImpl::ExclusiveGatewayStepComponentImpl()
    {

    }

    ExclusiveGatewayStepComponentImpl::~ExclusiveGatewayStepComponentImpl()
    {

    }

}