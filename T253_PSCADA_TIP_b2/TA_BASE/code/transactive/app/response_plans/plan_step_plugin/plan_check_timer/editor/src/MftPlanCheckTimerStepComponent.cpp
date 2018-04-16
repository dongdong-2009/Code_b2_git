#include "stdafx.h"
#include "MftPlanCheckTimerStepComponent.h"
#include "PlanCheckTimerStep.h"
#include "TaskPlanCheckTimer.h"

namespace TA_Base_App
{


PlanStep* MftPlanCheckTimerStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new PlanCheckTimerStep(parent, stepDetail);
}

PlanStep* MftPlanCheckTimerStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new PlanCheckTimerStep(parent, stepId, stepType);
}

InteractiveStep* MftPlanCheckTimerStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
    return NULL;
}

MftPlanCheckTimerStepComponentImpl::MftPlanCheckTimerStepComponentImpl()
{
}

MftPlanCheckTimerStepComponentImpl::~MftPlanCheckTimerStepComponentImpl()
{
}

IActivityComponent& MftPlanCheckTimerStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftPlanCheckTimerStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftPlanCheckTimerStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftPlanCheckTimerStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftPlanCheckTimerStepComponentImpl::getName()
{
    return TaskPlanCheckTimer::Name;
}

std::string MftPlanCheckTimerStepComponentImpl::getResource()
{
    return TaskPlanCheckTimer::Resource;
}

std::string MftPlanCheckTimerStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftPlanCheckTimerStepComponentImpl::createWorkflowActivity()
{
    return new TaskPlanCheckTimer();
}

std::vector<std::pair<std::string, std::vector<int>>> MftPlanCheckTimerStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftPlanCheckTimerStepComponentImpl::readonly()
{
    return false;
}

}