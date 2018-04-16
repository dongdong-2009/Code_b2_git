#include "stdafx.h"
#include "MftPromptStepComponent.h"
#include "PromptStep.h"
#include "ExecutablePromptStep.h"
#include "TaskPrompt.h"

namespace TA_Base_App
{


	PlanStep* MftPromptStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
	{
        return new PromptStep(parent, stepDetail);
	}

	PlanStep* MftPromptStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
	{
        return new PromptStep(parent, stepId, stepType);
	}

	InteractiveStep* MftPromptStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
	{
		return new ExecutablePromptStep(unId,eType,ulLocation,ulPlanId,ulInstanceNum,ulStepNum,vecParameters);
	}

    MftPromptStepComponentImpl::MftPromptStepComponentImpl()
    {
    }

    MftPromptStepComponentImpl::~MftPromptStepComponentImpl()
    {
    }

    IActivityComponent& MftPromptStepComponentImpl::getActivityComponent()
    {
        return *this;
    }

    int MftPromptStepComponentImpl::getStepType()
    {
		return m_eType;
    }

    int MftPromptStepComponentImpl::getAccessType()
    {
        return m_eAccess;
    }

    CreateActivitycPtr MftPromptStepComponentImpl::getActivityCreationFuncPtr()
    {
        return createWorkflowActivity;
    }

    std::string MftPromptStepComponentImpl::getName()
    {
		return TaskPrompt::Name;
    }

    std::string MftPromptStepComponentImpl::getResource()
    {
		return TaskPrompt::Resource;
    }

    std::string MftPromptStepComponentImpl::getCatagroyName()
    {
		return "Event";
    }

	WorkflowActivity* MftPromptStepComponentImpl::createWorkflowActivity()
	{
		return new TaskPrompt();
	}

	std::vector<std::pair<std::string, std::vector<int>>> MftPromptStepComponentImpl::getShortcuts()
	{
		return m_vecShortcuts;
	}

    bool MftPromptStepComponentImpl::readonly()
    {
        return false;
    }

}