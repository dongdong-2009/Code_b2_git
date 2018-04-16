#include "stdafx.h"
#include "MftTrainPaStepComponent.h"
#include "TrainPAStep.h"
#include "TaskTrainPa.h"

namespace TA_Base_App
{


PlanStep* MftTrainPaStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new TrainPAStep(parent, stepDetail);
}

PlanStep* MftTrainPaStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new TrainPAStep(parent, stepId, stepType);
}

InteractiveStep* MftTrainPaStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
    return NULL;
}

MftTrainPaStepComponentImpl::MftTrainPaStepComponentImpl()
{
}

MftTrainPaStepComponentImpl::~MftTrainPaStepComponentImpl()
{
}

IActivityComponent& MftTrainPaStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftTrainPaStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftTrainPaStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftTrainPaStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftTrainPaStepComponentImpl::getName()
{
    return TaskTrainPa::Name;
}

std::string MftTrainPaStepComponentImpl::getResource()
{
    return TaskTrainPa::Resource;
}

std::string MftTrainPaStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftTrainPaStepComponentImpl::createWorkflowActivity()
{
    return new TaskTrainPa();
}

std::vector<std::pair<std::string, std::vector<int>>> MftTrainPaStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftTrainPaStepComponentImpl::readonly()
{
    return false;
}

}