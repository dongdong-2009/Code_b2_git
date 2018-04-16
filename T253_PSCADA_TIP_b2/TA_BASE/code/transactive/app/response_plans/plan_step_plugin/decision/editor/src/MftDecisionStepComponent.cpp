#include "stdafx.h"
#include "MftDecisionStepComponent.h"
#include "DecisionStep.h"
#include "ExecutableDecisionStep.h"
#include "TaskDecision.h"

namespace TA_Base_App
{


	PlanStep* MftDecisionStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
	{
        return new DecisionStep(parent, stepDetail);
	}

	PlanStep* MftDecisionStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
	{
        return new DecisionStep(parent, stepId, stepType);
	}

	InteractiveStep* MftDecisionStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
	{
		return new ExecutableDecisionStep(unId, eType, ulLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters);
	}

    MftDecisionStepComponentImpl::MftDecisionStepComponentImpl()
    {
    }

    MftDecisionStepComponentImpl::~MftDecisionStepComponentImpl()
    {
    }

    IActivityComponent& MftDecisionStepComponentImpl::getActivityComponent()
    {
        return *this;
    }

    int MftDecisionStepComponentImpl::getStepType()
    {
        return m_eType;
    }

    int MftDecisionStepComponentImpl::getAccessType()
    {
        return m_eAccess;
    }

    CreateActivitycPtr MftDecisionStepComponentImpl::getActivityCreationFuncPtr()
    {
        return createWorkflowActivity;
    }

    std::string MftDecisionStepComponentImpl::getName()
    {
		return TaskDecision::Name;
    }

    std::string MftDecisionStepComponentImpl::getResource()
    {
		return TaskDecision::Resource;
    }

    std::string MftDecisionStepComponentImpl::getCatagroyName()
    {
		return "Event";
    }

	WorkflowActivity* MftDecisionStepComponentImpl::createWorkflowActivity()
	{
		return new TaskDecision();
	}

	std::vector<std::pair<std::string, std::vector<int>>> MftDecisionStepComponentImpl::getShortcuts()
	{
		return m_vecShortcuts;
	}

    bool MftDecisionStepComponentImpl::readonly()
    {
        return false;
    }

}