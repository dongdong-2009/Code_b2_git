#include "stdafx.h"
#include "MftJumpStepComponent.h"
#include "JumpStep.h"
#include "TaskJump.h"

namespace TA_Base_App
{


	PlanStep* MftJumpStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
	{
        return new JumpStep(parent, stepDetail);
	}

	PlanStep* MftJumpStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
	{
        return new JumpStep(parent, stepId, stepType);
	}

	InteractiveStep* MftJumpStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
	{
		return NULL;
	}

    MftJumpStepComponentImpl::MftJumpStepComponentImpl()
    {
    }

    MftJumpStepComponentImpl::~MftJumpStepComponentImpl()
    {
    }

    IActivityComponent& MftJumpStepComponentImpl::getActivityComponent()
    {
        return *this;
    }

    int MftJumpStepComponentImpl::getStepType()
    {
        return m_eType;
    }

    int MftJumpStepComponentImpl::getAccessType()
    {
        return m_eAccess;
    }

    CreateActivitycPtr MftJumpStepComponentImpl::getActivityCreationFuncPtr()
    {
        return createWorkflowActivity;
    }

    std::string MftJumpStepComponentImpl::getName()
    {
		return TaskJump::Name;
    }

    std::string MftJumpStepComponentImpl::getResource()
    {
		return TaskJump::Resource;
    }

    std::string MftJumpStepComponentImpl::getCatagroyName()
    {
		return "Event";
    }

	WorkflowActivity* MftJumpStepComponentImpl::createWorkflowActivity()
	{
		return new TaskJump();
	}

	std::vector<std::pair<std::string, std::vector<int>>> MftJumpStepComponentImpl::getShortcuts()
	{
		return m_vecShortcuts;
	}

    bool MftJumpStepComponentImpl::readonly()
    {
        return false;
    }

}