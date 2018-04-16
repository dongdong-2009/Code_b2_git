#include "app/response_plans/plan_manager/src/AbortStep.h"
#include "app/response_plans/plan_manager/src/EventAbort.h"
#include "app/response_plans/plan_manager/src/MftAbortStepComponent.h"

namespace TA_Base_App
{


PlanStep* MftAbortStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new AbortStep(parent, stepDetail);
}

PlanStep* MftAbortStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new AbortStep(parent, stepId, stepType);
}

InteractiveStep* MftAbortStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
	const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
	return NULL;
}

MftAbortStepComponentImpl::MftAbortStepComponentImpl()
{
}

MftAbortStepComponentImpl::~MftAbortStepComponentImpl()
{
}

IActivityComponent& MftAbortStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftAbortStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftAbortStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftAbortStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftAbortStepComponentImpl::getName()
{
    return EventAbort::Name;
}

std::string MftAbortStepComponentImpl::getResource()
{
    return EventAbort::Resource;
}

std::string MftAbortStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftAbortStepComponentImpl::createWorkflowActivity()
{
    return new EventAbort();
}

std::vector<std::pair<std::string, std::vector<int>>> MftAbortStepComponentImpl::getShortcuts()
{
	return m_vecShortcuts;
}

bool MftAbortStepComponentImpl::readonly()
{
    return false;
}

}