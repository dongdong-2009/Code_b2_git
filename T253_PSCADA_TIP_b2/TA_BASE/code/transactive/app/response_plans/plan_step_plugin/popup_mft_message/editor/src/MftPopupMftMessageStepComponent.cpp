#include "stdafx.h"
#include "MftPopupMftMessageStepComponent.h"
#include "PopupMFTMessageStep.h"
#include "MftMessageActiveStep.h"
#include "TaskPopupMftMessage.h"

namespace TA_Base_App
{


PlanStep* MftPopupMftMessageStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new PopupMFTMessageStep(parent, stepDetail);
}

PlanStep* MftPopupMftMessageStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new PopupMFTMessageStep(parent, stepId, stepType);
}

InteractiveStep* MftPopupMftMessageStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
    return new MftMessageActiveStep(unId,eType,ulLocation,ulPlanId,ulInstanceNum,ulStepNum,vecParameters);
}

MftPopupMftMessageStepComponentImpl::MftPopupMftMessageStepComponentImpl()
{

}

MftPopupMftMessageStepComponentImpl::~MftPopupMftMessageStepComponentImpl()
{

}
IActivityComponent& MftPopupMftMessageStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftPopupMftMessageStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftPopupMftMessageStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftPopupMftMessageStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftPopupMftMessageStepComponentImpl::getName()
{
    return TaskPopupMftMessage::Name;
}

std::string MftPopupMftMessageStepComponentImpl::getResource()
{
    return TaskPopupMftMessage::Resource;
}

std::string MftPopupMftMessageStepComponentImpl::getCatagroyName()
{
    return "Event";
}

WorkflowActivity* MftPopupMftMessageStepComponentImpl::createWorkflowActivity()
{
    return new TaskPopupMftMessage();
}

std::vector<std::pair<std::string, std::vector<int>>> MftPopupMftMessageStepComponentImpl::getShortcuts()
{
    return m_vecShortcuts;
}

bool MftPopupMftMessageStepComponentImpl::readonly()
{
    return false;
}

}