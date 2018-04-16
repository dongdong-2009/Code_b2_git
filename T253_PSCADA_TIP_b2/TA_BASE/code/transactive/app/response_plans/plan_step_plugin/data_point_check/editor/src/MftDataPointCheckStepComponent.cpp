#include "stdafx.h"
#include "MftDataPointCheckStepComponent.h"
#include "DataPointCheckStep.h"
#include "TaskDataPointCheck.h"

namespace TA_Base_App
{


	PlanStep* MftDataPointCheckStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
	{
        return new DataPointCheckStep(parent, stepDetail);
	}

	PlanStep* MftDataPointCheckStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
	{
        return new DataPointCheckStep(parent, stepId, stepType);
	}

	InteractiveStep* MftDataPointCheckStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
	{
		return NULL;
	}

    MftDataPointCheckStepComponentImpl::MftDataPointCheckStepComponentImpl()
    {

    }

    MftDataPointCheckStepComponentImpl::~MftDataPointCheckStepComponentImpl()
    {

    }

    IActivityComponent& MftDataPointCheckStepComponentImpl::getActivityComponent()
    {
        return *this;
    }

    int MftDataPointCheckStepComponentImpl::getStepType()
    {
        return m_eType;
    }

    int MftDataPointCheckStepComponentImpl::getAccessType()
    {
        return m_eAccess;
    }

    CreateActivitycPtr MftDataPointCheckStepComponentImpl::getActivityCreationFuncPtr()
    {
        return createWorkflowActivity;
    }

    std::string MftDataPointCheckStepComponentImpl::getName()
    {
		return TaskDataPointCheck::Name;
    }

    std::string MftDataPointCheckStepComponentImpl::getResource()
    {
		return TaskDataPointCheck::Resource;
    }

    std::string MftDataPointCheckStepComponentImpl::getCatagroyName()
    {
		return "Event";
    }

	WorkflowActivity* MftDataPointCheckStepComponentImpl::createWorkflowActivity()
	{
		return new TaskDataPointCheck();
	}

	std::vector<std::pair<std::string, std::vector<int>>> MftDataPointCheckStepComponentImpl::getShortcuts()
	{
		return m_vecShortcuts;
	}

    bool MftDataPointCheckStepComponentImpl::readonly()
    {
        return false;
    }

}