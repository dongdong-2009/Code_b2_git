#include "StartStep.h"
#include "EventStart.h"
#include "MftStartStepComponent.h"

namespace TA_Base_App
{

PlanStep* MftStartStepComponentImpl::createStep(IPlanNode* parent, BasicParameter& stepDetail)
{
    return new StartStep(parent, stepDetail);
}

PlanStep* MftStartStepComponentImpl::createStep(IPlanNode* parent, const unsigned long stepId, const int stepType)
{
    return new StartStep(parent, stepId, stepType);
}

InteractiveStep* MftStartStepComponentImpl::createExecutableStep(const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId,
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters)
{
    return NULL;
}

MftStartStepComponentImpl::MftStartStepComponentImpl()
{
}

MftStartStepComponentImpl::~MftStartStepComponentImpl()
{
}

IActivityComponent& MftStartStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftStartStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftStartStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftStartStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftStartStepComponentImpl::getName()
{
    return EventStart::Name;
}

std::string MftStartStepComponentImpl::getResource()
{
    return EventStart::Resource;
}

std::string MftStartStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftStartStepComponentImpl::createWorkflowActivity()
{
    return new EventStart();
}

std::vector<std::pair<std::string, std::vector<int>>> MftStartStepComponentImpl::getShortcuts()
{
	return m_vecShortcuts;
}

bool MftStartStepComponentImpl::readonly()
{
    return true;
}

}
