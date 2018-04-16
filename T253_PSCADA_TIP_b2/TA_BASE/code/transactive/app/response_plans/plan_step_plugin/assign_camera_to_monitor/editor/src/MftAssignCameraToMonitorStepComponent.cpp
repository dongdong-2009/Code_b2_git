#include "stdafx.h"
#include "MftAssignCameraToMonitorStepComponent.h"
#include "AssignCameraToMonitorStep.h"
#include "TaskAssignCameraToMonitor.h"

namespace TA_Base_App
{


PlanStep* MftAssignCameraToMonitorStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new AssignCameraToMonitorStep(parent, stepDetail);
}

PlanStep* MftAssignCameraToMonitorStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new AssignCameraToMonitorStep(parent, stepId, stepType);
}

InteractiveStep* MftAssignCameraToMonitorStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters)
{
    return NULL;
}

MftAssignCameraToMonitorStepComponentImpl::MftAssignCameraToMonitorStepComponentImpl()
{

}

MftAssignCameraToMonitorStepComponentImpl::~MftAssignCameraToMonitorStepComponentImpl()
{

}

IActivityComponent& MftAssignCameraToMonitorStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftAssignCameraToMonitorStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftAssignCameraToMonitorStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftAssignCameraToMonitorStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftAssignCameraToMonitorStepComponentImpl::getName()
{
    return TaskAssignCameraToMonitor::Name;
}

std::string MftAssignCameraToMonitorStepComponentImpl::getResource()
{
    return TaskAssignCameraToMonitor::Resource;
}

std::string MftAssignCameraToMonitorStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftAssignCameraToMonitorStepComponentImpl::createWorkflowActivity()
{
    return new TaskAssignCameraToMonitor();
}

std::vector<std::pair<std::string, std::vector<int>>> MftAssignCameraToMonitorStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftAssignCameraToMonitorStepComponentImpl::readonly()
{
    return false;
}

}