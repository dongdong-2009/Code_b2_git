#include "stdafx.h"
#include "MftShutdownSystemStepComponent.h"
#include "ShutdownSystemStep.h"
#include "TaskShutdownSystem.h"

namespace TA_Base_App
{


PlanStep* MftShutdownSystemStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new ShutdownSystemStep(parent, stepDetail);
}

PlanStep* MftShutdownSystemStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new ShutdownSystemStep(parent, stepId, stepType);
}

InteractiveStep* MftShutdownSystemStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
    return NULL;
}

MftShutdownSystemStepComponentImpl::MftShutdownSystemStepComponentImpl()
{
}

MftShutdownSystemStepComponentImpl::~MftShutdownSystemStepComponentImpl()
{
}

IActivityComponent& MftShutdownSystemStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftShutdownSystemStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftShutdownSystemStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftShutdownSystemStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftShutdownSystemStepComponentImpl::getName()
{
    return TaskShutdownSystem::Name;
}

std::string MftShutdownSystemStepComponentImpl::getResource()
{
    return TaskShutdownSystem::Resource;
}

std::string MftShutdownSystemStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftShutdownSystemStepComponentImpl::createWorkflowActivity()
{
    return new TaskShutdownSystem();
}

std::vector<std::pair<std::string, std::vector<int>>> MftShutdownSystemStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftShutdownSystemStepComponentImpl::readonly()
{
    return false;
}

}