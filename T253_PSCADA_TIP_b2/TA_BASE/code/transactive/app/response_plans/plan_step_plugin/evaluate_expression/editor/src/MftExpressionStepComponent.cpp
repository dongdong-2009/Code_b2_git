#include "stdafx.h"
#include "MftExpressionStepComponent.h"
#include "ExpressionStep.h"
#include "TaskEvaluateExpression.h"

namespace TA_Base_App
{


PlanStep* MftExpressionStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new ExpressionStep(parent, stepDetail);
}

PlanStep* MftExpressionStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new ExpressionStep(parent, stepId, stepType);
}

InteractiveStep* MftExpressionStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
    return NULL;
}

MftExpressionStepComponentImpl::MftExpressionStepComponentImpl()
{
}

MftExpressionStepComponentImpl::~MftExpressionStepComponentImpl()
{
}

IActivityComponent& MftExpressionStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftExpressionStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftExpressionStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftExpressionStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftExpressionStepComponentImpl::getName()
{
    return TaskEvaluateExpression::Name;
}

std::string MftExpressionStepComponentImpl::getResource()
{
    return TaskEvaluateExpression::Resource;
}

std::string MftExpressionStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftExpressionStepComponentImpl::createWorkflowActivity()
{
    return new TaskEvaluateExpression();
}

std::vector<std::pair<std::string, std::vector<int>>> MftExpressionStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftExpressionStepComponentImpl::readonly()
{
    return false;
}

}