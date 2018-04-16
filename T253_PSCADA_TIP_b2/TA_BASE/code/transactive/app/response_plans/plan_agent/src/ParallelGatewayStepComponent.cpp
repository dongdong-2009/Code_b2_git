#include "app/response_plans/plan_agent/src/ParallelGatewayStepComponent.h"
#include "app/response_plans/plan_agent/src/ActiveParallelGateway.h"

namespace TA_Base_App
{
    ActiveStep* ParallelGatewayStepComponentImpl::createActiveStep( const BasicParameter& detail, IActivePlan& plan )
    {
        return new ActiveParallelGateway( detail, plan );
    }

    void ParallelGatewayStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    ParallelGatewayStepComponentImpl::ParallelGatewayStepComponentImpl()
    {

    }

    ParallelGatewayStepComponentImpl::~ParallelGatewayStepComponentImpl()
    {

    }

}