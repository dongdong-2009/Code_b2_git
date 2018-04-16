#include "stdafx.h"
#include "MftFailureSummaryStepComponent.h"
#include "FailureSummaryStep.h"
#include "ExecutableFailureSummaryStep.h"
#include "TaskFailureSummary.h"

namespace TA_Base_App
{


	PlanStep* MftFailureSummaryStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
	{
        return new FailureSummaryStep(parent, stepDetail);
	}

	PlanStep* MftFailureSummaryStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
	{
        return new FailureSummaryStep(parent, stepId, stepType);
	}

	InteractiveStep* MftFailureSummaryStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
	{
		return new ExecutableFailureSummaryStep(unId,eType,ulLocation,ulPlanId,ulInstanceNum,ulStepNum,vecParameters);
	}

    MftFailureSummaryStepComponentImpl::MftFailureSummaryStepComponentImpl()
    {
    }

    MftFailureSummaryStepComponentImpl::~MftFailureSummaryStepComponentImpl()
    {
    }

    IActivityComponent& MftFailureSummaryStepComponentImpl::getActivityComponent()
    {
        return *this;
    }

    int MftFailureSummaryStepComponentImpl::getStepType()
    {
        return m_eType;
    }

    int MftFailureSummaryStepComponentImpl::getAccessType()
    {
        return m_eAccess;
    }

    CreateActivitycPtr MftFailureSummaryStepComponentImpl::getActivityCreationFuncPtr()
    {
        return createWorkflowActivity;
    }

    std::string MftFailureSummaryStepComponentImpl::getName()
    {
		return TaskFailureSummary::Name;
    }

    std::string MftFailureSummaryStepComponentImpl::getResource()
    {
		return TaskFailureSummary::Resource;
    }

    std::string MftFailureSummaryStepComponentImpl::getCatagroyName()
    {
		return "Event";
    }

	WorkflowActivity* MftFailureSummaryStepComponentImpl::createWorkflowActivity()
	{
		return new TaskFailureSummary();
	}

	std::vector<std::pair<std::string, std::vector<int>>> MftFailureSummaryStepComponentImpl::getShortcuts()
	{
		return m_vecShortcuts;
	}

	bool MftFailureSummaryStepComponentImpl::readonly()
	{
		return false;
	}

}