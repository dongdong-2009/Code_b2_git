#include "app/response_plans/plan_agent/src/RunPlanStepComponent.h"
#include "app/response_plans/plan_agent/src/ActiveRunPlanStep.h"
#include "app/response_plans/plan_agent/src/ActivePlanRunPlanStepMessage.h"

namespace TA_Base_App
{
    ActiveStep* RunPlanStepComponentImpl::createActiveStep( const BasicParameter& detail, IActivePlan& plan )
    {
        return new ActiveRunPlanStep( detail, plan );
    }

    IActiveStepMessage* RunPlanStepComponentImpl::createPlanStepMessage( const unsigned long ulStepNum, const std::vector<std::string>& parameters )
	{
        return new ActivePlanRunPlanStepMessage( ulStepNum, parameters );
	}

    void RunPlanStepComponentImpl::validateStepParameter(const std::vector<std::string>& parameters)
    {

    }

    RunPlanStepComponentImpl::RunPlanStepComponentImpl()
    {

    }

    RunPlanStepComponentImpl::~RunPlanStepComponentImpl()
    {

    }

}