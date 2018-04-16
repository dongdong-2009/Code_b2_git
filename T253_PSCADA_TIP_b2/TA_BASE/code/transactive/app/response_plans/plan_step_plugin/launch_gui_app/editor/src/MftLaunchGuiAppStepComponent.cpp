#include "stdafx.h"
#include "MftLaunchGuiAppStepComponent.h"
#include "LaunchGuiAppStep.h"
#include "ExecutableLaunchGuiAppStep.h"
#include "TaskLaunchGuiApp.h"

namespace TA_Base_App
{


	PlanStep* MftLaunchGuiAppStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
	{
        return new LaunchGuiAppStep(parent, stepDetail);
	}

	PlanStep* MftLaunchGuiAppStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
	{
        return new LaunchGuiAppStep(parent, stepId, stepType);
	}

	InteractiveStep* MftLaunchGuiAppStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
	{
		return new ExecutableLaunchGuiAppStep(unId,eType,ulLocation,ulPlanId,ulInstanceNum,ulStepNum,vecParameters);
	}

    MftLaunchGuiAppStepComponentImpl::MftLaunchGuiAppStepComponentImpl()
    {
    }

    MftLaunchGuiAppStepComponentImpl::~MftLaunchGuiAppStepComponentImpl()
    {
    }

    IActivityComponent& MftLaunchGuiAppStepComponentImpl::getActivityComponent()
    {
        return *this;
    }

    int MftLaunchGuiAppStepComponentImpl::getStepType()
    {
        return m_eType;
    }

    int MftLaunchGuiAppStepComponentImpl::getAccessType()
    {
        return m_eAccess;
    }

    CreateActivitycPtr MftLaunchGuiAppStepComponentImpl::getActivityCreationFuncPtr()
    {
        return createWorkflowActivity;
    }

    std::string MftLaunchGuiAppStepComponentImpl::getName()
    {
        return TaskLaunchGuiApp::Name;
    }

    std::string MftLaunchGuiAppStepComponentImpl::getResource()
    {
        return TaskLaunchGuiApp::Resource;
    }

    std::string MftLaunchGuiAppStepComponentImpl::getCatagroyName()
    {
        return TaskLaunchGuiApp::CategroyName;
    }

	WorkflowActivity* MftLaunchGuiAppStepComponentImpl::createWorkflowActivity()
	{
		return new TaskLaunchGuiApp;
	}

	std::vector<std::pair<std::string, std::vector<int>>> MftLaunchGuiAppStepComponentImpl::getShortcuts()
	{
		return m_vecShortcuts;
	}

	bool MftLaunchGuiAppStepComponentImpl::readonly()
	{
		return false;
	}

}