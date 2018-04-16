
#include "TerminatePlanStep.h"
#include "TaskTerminatePlan.h"
#include "MftTerminatePlanStepComponent.h"

namespace TA_Base_App
{


PlanStep* MftTerminatePlanStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new TerminatePlanStep(parent, stepDetail);
}

PlanStep* MftTerminatePlanStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new TerminatePlanStep(parent, stepId, stepType);
}

InteractiveStep* MftTerminatePlanStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
	const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
	return NULL;
}

MftTerminatePlanStepComponentImpl::MftTerminatePlanStepComponentImpl()
{
}

MftTerminatePlanStepComponentImpl::~MftTerminatePlanStepComponentImpl()
{
}

IActivityComponent& MftTerminatePlanStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftTerminatePlanStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftTerminatePlanStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftTerminatePlanStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftTerminatePlanStepComponentImpl::getName()
{
    return TaskTerminatePlan::Name;
}

std::string MftTerminatePlanStepComponentImpl::getResource()
{
    return TaskTerminatePlan::Resource;
}

std::string MftTerminatePlanStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftTerminatePlanStepComponentImpl::createWorkflowActivity()
{
    return new TaskTerminatePlan();
}

std::vector<std::pair<std::string, std::vector<int>>> MftTerminatePlanStepComponentImpl::getShortcuts()
{
	return m_vecShortcuts;
}

bool MftTerminatePlanStepComponentImpl::readonly()
{
    return false;
}

}