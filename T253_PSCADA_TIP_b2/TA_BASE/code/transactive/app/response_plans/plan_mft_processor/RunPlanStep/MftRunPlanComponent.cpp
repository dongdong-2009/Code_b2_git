#include "MftRunPlanComponent.h"
#include "ExecutableRunPlanStep.h"

namespace TA_Base_App
{

	PlanStep* MftRunPlanComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
	{
        return NULL;
	}

	PlanStep* MftRunPlanComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
	{
        return NULL;
	}

	InteractiveStep* MftRunPlanComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
	{
		return new ExecutableRunPlanStep(unId,eType,ulLocation,ulPlanId,ulInstanceNum, ulStepNum, vecParameters);
	}

    MftRunPlanComponentImpl::MftRunPlanComponentImpl()
    {
    }

    MftRunPlanComponentImpl::~MftRunPlanComponentImpl()
    {
    }

    IActivityComponent& MftRunPlanComponentImpl::getActivityComponent()
    {
		// this function will cannot be used in this position.
		IActivityComponent* activityComponent = NULL;
        return *activityComponent;
    }
}