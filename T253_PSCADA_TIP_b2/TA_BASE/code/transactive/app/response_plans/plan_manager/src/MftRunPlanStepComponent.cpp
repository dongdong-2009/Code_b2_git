#include "RunPlanStep.h"
#include "TaskRunPlan.h"
#include "MftRunPlanStepComponent.h"

namespace TA_Base_App
{


PlanStep* MftRunPlanStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new RunPlanStep(parent, stepDetail);
}

PlanStep* MftRunPlanStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new RunPlanStep(parent, stepId, stepType);
}

InteractiveStep* MftRunPlanStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
	const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
	return NULL;
}

MftRunPlanStepComponentImpl::MftRunPlanStepComponentImpl()
{
}

MftRunPlanStepComponentImpl::~MftRunPlanStepComponentImpl()
{
}

IActivityComponent& MftRunPlanStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftRunPlanStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftRunPlanStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftRunPlanStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftRunPlanStepComponentImpl::getName()
{
    return TaskRunPlan::Name;
}

std::string MftRunPlanStepComponentImpl::getResource()
{
    return TaskRunPlan::Resource;
}

std::string MftRunPlanStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftRunPlanStepComponentImpl::createWorkflowActivity()
{
    return new TaskRunPlan();
}

std::vector<std::pair<std::string, std::vector<int>>> MftRunPlanStepComponentImpl::getShortcuts()
{
	return m_vecShortcuts;
}

bool MftRunPlanStepComponentImpl::readonly()
{
    return false;
}

}