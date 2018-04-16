#include "stdafx.h"
#include "MftStationTisStepComponent.h"
#include "StationTisStep.h"
#include "TaskStationTis.h"

namespace TA_Base_App
{


PlanStep* MftStationTisStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new StationTisStep(parent, stepDetail);
}

PlanStep* MftStationTisStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new StationTisStep(parent, stepId, stepType);
}

InteractiveStep* MftStationTisStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
    return NULL;
}

MftStationTisStepComponentImpl::MftStationTisStepComponentImpl()
{
}

MftStationTisStepComponentImpl::~MftStationTisStepComponentImpl()
{
}

IActivityComponent& MftStationTisStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftStationTisStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftStationTisStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftStationTisStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftStationTisStepComponentImpl::getName()
{
    return TaskStationTis::Name;
}

std::string MftStationTisStepComponentImpl::getResource()
{
    return TaskStationTis::Resource;
}

std::string MftStationTisStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftStationTisStepComponentImpl::createWorkflowActivity()
{
    return new TaskStationTis();
}

std::vector<std::pair<std::string, std::vector<int>>> MftStationTisStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftStationTisStepComponentImpl::readonly()
{
    return false;
}

}