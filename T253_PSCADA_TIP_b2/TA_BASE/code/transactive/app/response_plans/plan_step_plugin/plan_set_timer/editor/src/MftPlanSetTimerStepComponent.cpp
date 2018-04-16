#include "stdafx.h"
#include "MftPlanSetTimerStepComponent.h"
#include "PlanSetTimerStep.h"
#include "TaskPlanSetTimer.h"

namespace TA_Base_App
{


PlanStep* MftPlanSetTimerStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new PlanSetTimerStep(parent, stepDetail);
}

PlanStep* MftPlanSetTimerStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new PlanSetTimerStep(parent, stepId, stepType);
}

InteractiveStep* MftPlanSetTimerStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
    return NULL;
}

MftPlanSetTimerStepComponentImpl::MftPlanSetTimerStepComponentImpl()
{

}

MftPlanSetTimerStepComponentImpl::~MftPlanSetTimerStepComponentImpl()
{

}
IActivityComponent& MftPlanSetTimerStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftPlanSetTimerStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftPlanSetTimerStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftPlanSetTimerStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftPlanSetTimerStepComponentImpl::getName()
{
    return TaskPlanSetTimer::Name;
}

std::string MftPlanSetTimerStepComponentImpl::getResource()
{
    return TaskPlanSetTimer::Resource;
}

std::string MftPlanSetTimerStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftPlanSetTimerStepComponentImpl::createWorkflowActivity()
{
    return new TaskPlanSetTimer();
}

std::vector<std::pair<std::string, std::vector<int>>> MftPlanSetTimerStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftPlanSetTimerStepComponentImpl::readonly()
{
    return false;
}

}