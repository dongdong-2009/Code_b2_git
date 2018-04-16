#include "EndStep.h"
#include "EventEnd.h"
#include "MftEndStepComponent.h"

namespace TA_Base_App
{


PlanStep* MftEndStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new EndStep(parent, stepDetail);
}

PlanStep* MftEndStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new EndStep(parent, stepId, stepType);
}

InteractiveStep* MftEndStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
	const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
	return NULL;
}

MftEndStepComponentImpl::MftEndStepComponentImpl()
{
}

MftEndStepComponentImpl::~MftEndStepComponentImpl()
{
}

IActivityComponent& MftEndStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftEndStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftEndStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftEndStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftEndStepComponentImpl::getName()
{
    return EventEnd::Name;
}

std::string MftEndStepComponentImpl::getResource()
{
    return EventEnd::Resource;
}

std::string MftEndStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftEndStepComponentImpl::createWorkflowActivity()
{
    return new EventEnd();
}

std::vector<std::pair<std::string, std::vector<int>>> MftEndStepComponentImpl::getShortcuts()
{
	return m_vecShortcuts;
}

bool MftEndStepComponentImpl::readonly()
{
    return true;
}

}