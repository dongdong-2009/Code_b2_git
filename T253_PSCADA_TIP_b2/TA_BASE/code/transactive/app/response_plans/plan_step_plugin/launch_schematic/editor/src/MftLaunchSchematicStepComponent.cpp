#include "stdafx.h"
#include "MftLaunchSchematicStepComponent.h"
#include "LaunchSchematicStep.h"
#include "ExecutableLaunchSchematicStep.h"
#include "TaskLaunchSchematic.h"

namespace TA_Base_App
{


	PlanStep* MftLaunchSchematicStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
	{
        return new LaunchSchematicStep(parent, stepDetail);
	}

	PlanStep* MftLaunchSchematicStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
	{
        return new LaunchSchematicStep(parent, stepId, stepType);
	}

	InteractiveStep* MftLaunchSchematicStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
	{
		return new ExecutableLaunchSchematicStep(unId,eType,ulLocation,ulPlanId,ulInstanceNum,ulStepNum,vecParameters);
	}

    MftLaunchSchematicStepComponentImpl::MftLaunchSchematicStepComponentImpl()
    {
    }

    MftLaunchSchematicStepComponentImpl::~MftLaunchSchematicStepComponentImpl()
    {
    }

    IActivityComponent& MftLaunchSchematicStepComponentImpl::getActivityComponent()
    {
        return *this;
    }

    int MftLaunchSchematicStepComponentImpl::getStepType()
    {
        return m_eType;
    }

    int MftLaunchSchematicStepComponentImpl::getAccessType()
    {
        return m_eAccess;
    }

    CreateActivitycPtr MftLaunchSchematicStepComponentImpl::getActivityCreationFuncPtr()
    {
        return createWorkflowActivity;
    }

    std::string MftLaunchSchematicStepComponentImpl::getName()
    {
		return TaskLaunchSchematic::Name;
    }

    std::string MftLaunchSchematicStepComponentImpl::getResource()
    {
		return TaskLaunchSchematic::Resource;
    }

    std::string MftLaunchSchematicStepComponentImpl::getCatagroyName()
    {
		return "Event";
    }

	WorkflowActivity* MftLaunchSchematicStepComponentImpl::createWorkflowActivity()
	{
		return new TaskLaunchSchematic();
	}

	std::vector<std::pair<std::string, std::vector<int>>> MftLaunchSchematicStepComponentImpl::getShortcuts()
	{
		return m_vecShortcuts;
	}

	bool MftLaunchSchematicStepComponentImpl::readonly()
	{
		return false;
	}

}