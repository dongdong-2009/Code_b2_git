#include "stdafx.h"
#include "MftDataPointSetStepComponent.h"
#include "DataPointSetStep.h"
#include "TaskDataPointSet.h"

namespace TA_Base_App
{


	PlanStep* MftDataPointSetStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
	{
        return new DataPointSetStep(parent, stepDetail);
	}

	PlanStep* MftDataPointSetStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
	{
        return new DataPointSetStep(parent, stepId, stepType);
	}

	InteractiveStep* MftDataPointSetStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
	{
		return NULL;
	}

    MftDataPointSetStepComponentImpl::MftDataPointSetStepComponentImpl()
    {
    }

    MftDataPointSetStepComponentImpl::~MftDataPointSetStepComponentImpl()
    {
    }

    IActivityComponent& MftDataPointSetStepComponentImpl::getActivityComponent()
    {
        return *this;
    }

    int MftDataPointSetStepComponentImpl::getStepType()
    {
        return m_eType;
    }

    int MftDataPointSetStepComponentImpl::getAccessType()
    {
        return m_eAccess;
    }

    CreateActivitycPtr MftDataPointSetStepComponentImpl::getActivityCreationFuncPtr()
    {
        return createWorkflowActivity;
    }

    std::string MftDataPointSetStepComponentImpl::getName()
    {
		return TaskDataPointSet::Name;
    }

    std::string MftDataPointSetStepComponentImpl::getResource()
    {
		return TaskDataPointSet::Resource;
    }

    std::string MftDataPointSetStepComponentImpl::getCatagroyName()
    {
		return "Event";
    }

	WorkflowActivity* MftDataPointSetStepComponentImpl::createWorkflowActivity()
	{
		return new TaskDataPointSet();
	}

	std::vector<std::pair<std::string, std::vector<int>>> MftDataPointSetStepComponentImpl::getShortcuts()
	{
		return m_vecShortcuts;
	}

    bool MftDataPointSetStepComponentImpl::readonly()
    {
        return false;
    }

}