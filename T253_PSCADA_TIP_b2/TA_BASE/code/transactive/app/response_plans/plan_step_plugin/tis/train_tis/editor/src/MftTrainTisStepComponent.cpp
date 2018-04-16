#include "MftTrainTisStepComponent.h"
#include "TrainTisStep.h"
#include "TaskTrainTis.h"

namespace TA_Base_App
{


PlanStep* MftTrainTisStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new TrainTisStep(parent, stepDetail);
}

PlanStep* MftTrainTisStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new TrainTisStep(parent, stepId, stepType);
}

InteractiveStep* MftTrainTisStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters)
{
    return NULL;
}

MftTrainTisStepComponentImpl::MftTrainTisStepComponentImpl()
{
}

MftTrainTisStepComponentImpl::~MftTrainTisStepComponentImpl()
{
}

IActivityComponent& MftTrainTisStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftTrainTisStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftTrainTisStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftTrainTisStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftTrainTisStepComponentImpl::getName()
{
    return TaskTrainTis::Name;
}

std::string MftTrainTisStepComponentImpl::getResource()
{
    return TaskTrainTis::Resource;
}

std::string MftTrainTisStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftTrainTisStepComponentImpl::createWorkflowActivity()
{
    return new TaskTrainTis();
}

std::vector<std::pair<std::string, std::vector<int>>> MftTrainTisStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftTrainTisStepComponentImpl::readonly()
{
    return false;
}

}