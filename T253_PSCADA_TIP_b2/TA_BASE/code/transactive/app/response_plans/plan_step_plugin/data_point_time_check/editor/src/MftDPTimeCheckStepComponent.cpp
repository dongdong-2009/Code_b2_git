#include "stdafx.h"
#include "MftDPTimeCheckStepComponent.h"
#include "DataPointTimeCheckStep.h"
#include "TaskDataPointTimeCheck.h"

namespace TA_Base_App
{


PlanStep* MftDataPointTimeCheckStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new DataPointTimeCheckStep(parent, stepDetail);
}

PlanStep* MftDataPointTimeCheckStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new DataPointTimeCheckStep(parent, stepId, stepType);
}

InteractiveStep* MftDataPointTimeCheckStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
    return NULL;
}

MftDataPointTimeCheckStepComponentImpl::MftDataPointTimeCheckStepComponentImpl()
{

}

MftDataPointTimeCheckStepComponentImpl::~MftDataPointTimeCheckStepComponentImpl()
{

}
IActivityComponent& MftDataPointTimeCheckStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftDataPointTimeCheckStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftDataPointTimeCheckStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftDataPointTimeCheckStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftDataPointTimeCheckStepComponentImpl::getName()
{
    return TaskDataPointTimeCheck::Name;
}

std::string MftDataPointTimeCheckStepComponentImpl::getResource()
{
    return TaskDataPointTimeCheck::Resource;
}

std::string MftDataPointTimeCheckStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftDataPointTimeCheckStepComponentImpl::createWorkflowActivity()
{
    return new TaskDataPointTimeCheck();
}

std::vector<std::pair<std::string, std::vector<int>>> MftDataPointTimeCheckStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftDataPointTimeCheckStepComponentImpl::readonly()
{
    return false;
}

}