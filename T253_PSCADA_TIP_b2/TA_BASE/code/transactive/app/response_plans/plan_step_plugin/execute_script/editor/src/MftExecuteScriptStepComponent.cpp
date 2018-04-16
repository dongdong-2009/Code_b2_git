#include "stdafx.h"
#include "MftExecuteScriptStepComponent.h"
#include "ExecuteScriptStep.h"
#include "TaskExecuteScript.h"

namespace TA_Base_App
{


PlanStep* MftExecuteScriptStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new ExecuteScriptStep(parent, stepDetail);
}

PlanStep* MftExecuteScriptStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new ExecuteScriptStep(parent, stepId, stepType);
}

InteractiveStep* MftExecuteScriptStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
    return NULL;
}

MftExecuteScriptStepComponentImpl::MftExecuteScriptStepComponentImpl()
{
}

MftExecuteScriptStepComponentImpl::~MftExecuteScriptStepComponentImpl()
{
}

IActivityComponent& MftExecuteScriptStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftExecuteScriptStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftExecuteScriptStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftExecuteScriptStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftExecuteScriptStepComponentImpl::getName()
{
    return TaskExecuteScript::Name;
}

std::string MftExecuteScriptStepComponentImpl::getResource()
{
    return TaskExecuteScript::Resource;
}

std::string MftExecuteScriptStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftExecuteScriptStepComponentImpl::createWorkflowActivity()
{
    return new TaskExecuteScript();
}

std::vector<std::pair<std::string, std::vector<int>>> MftExecuteScriptStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftExecuteScriptStepComponentImpl::readonly()
{
    return false;
}

}