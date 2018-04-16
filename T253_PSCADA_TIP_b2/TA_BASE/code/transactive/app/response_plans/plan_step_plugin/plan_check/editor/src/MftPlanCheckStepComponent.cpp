#include "stdafx.h"
#include "MftPlanCheckStepComponent.h"
#include "PlanCheckStep.h"
#include "TaskPlanCheck.h"

namespace TA_Base_App
{


PlanStep* MftPlanCheckStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new PlanCheckStep(parent, stepDetail);
}

PlanStep* MftPlanCheckStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new PlanCheckStep(parent, stepId, stepType);
}

InteractiveStep* MftPlanCheckStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
    return NULL;
}

MftPlanCheckStepComponentImpl::MftPlanCheckStepComponentImpl()
{
}

MftPlanCheckStepComponentImpl::~MftPlanCheckStepComponentImpl()
{
}

IActivityComponent& MftPlanCheckStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftPlanCheckStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftPlanCheckStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftPlanCheckStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftPlanCheckStepComponentImpl::getName()
{
    return TaskPlanCheck::Name;
}

std::string MftPlanCheckStepComponentImpl::getResource()
{
    return TaskPlanCheck::Resource;
}

std::string MftPlanCheckStepComponentImpl::getCatagroyName()
{
    return TaskPlanCheck::CategroyName;
}

WorkflowActivity* MftPlanCheckStepComponentImpl::createWorkflowActivity()
{
    return new TaskPlanCheck();
}

std::vector<std::pair<std::string, std::vector<int>>> MftPlanCheckStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftPlanCheckStepComponentImpl::readonly()
{
    return false;
}

}