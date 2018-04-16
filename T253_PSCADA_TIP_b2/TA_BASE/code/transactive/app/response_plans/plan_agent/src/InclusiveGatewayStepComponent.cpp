#include "app/response_plans/plan_agent/src/InclusiveGatewayStepComponent.h"
#include "app/response_plans/plan_agent/src/ActiveInclusiveGateway.h"

namespace TA_Base_App
{
    ActiveStep* InclusiveGatewayStepComponentImpl::createActiveStep( const BasicParameter& detail, IActivePlan& plan )
    {
        return new ActiveInclusiveGateway( detail, plan );
    }

    void InclusiveGatewayStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    InclusiveGatewayStepComponentImpl::InclusiveGatewayStepComponentImpl()
    {

    }

    InclusiveGatewayStepComponentImpl::~InclusiveGatewayStepComponentImpl()
    {

    }

}