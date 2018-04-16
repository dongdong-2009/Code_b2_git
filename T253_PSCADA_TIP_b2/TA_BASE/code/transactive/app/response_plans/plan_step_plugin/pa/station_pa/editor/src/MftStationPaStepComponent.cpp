#include "stdafx.h"
#include "MftStationPaStepComponent.h"
#include "StationPAStep.h"
#include "TaskStationPa.h"

namespace TA_Base_App
{


PlanStep* MftStationPaStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new StationPAStep(parent, stepDetail);
}

PlanStep* MftStationPaStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new StationPAStep(parent, stepId, stepType);
}

InteractiveStep* MftStationPaStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
    return NULL;
}

MftStationPaStepComponentImpl::MftStationPaStepComponentImpl()
{

}

MftStationPaStepComponentImpl::~MftStationPaStepComponentImpl()
{
}

IActivityComponent& MftStationPaStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftStationPaStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftStationPaStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftStationPaStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftStationPaStepComponentImpl::getName()
{
    return TaskStationPa::Name;
}

std::string MftStationPaStepComponentImpl::getResource()
{
    return TaskStationPa::Resource;
}

std::string MftStationPaStepComponentImpl::getCatagroyName()
{
    return TaskStationPa::CategroyName;
}

WorkflowActivity* MftStationPaStepComponentImpl::createWorkflowActivity()
{
    return new TaskStationPa();
}

std::vector<std::pair<std::string, std::vector<int>>> MftStationPaStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftStationPaStepComponentImpl::readonly()
{
    return false;
}

}