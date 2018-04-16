#include "InclusiveGatewayStep.h"
#include "GatewayInclusive.h"
#include "MftInclusiveGatewayStepComponent.h"

namespace TA_Base_App
{


PlanStep* MftInclusiveGatewayStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new InclusiveGatewayStep(parent, stepDetail);
}

PlanStep* MftInclusiveGatewayStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new InclusiveGatewayStep(parent, stepId, stepType);
}

InteractiveStep* MftInclusiveGatewayStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
	const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
	return NULL;
}

MftInclusiveGatewayStepComponentImpl::MftInclusiveGatewayStepComponentImpl()
{
}

MftInclusiveGatewayStepComponentImpl::~MftInclusiveGatewayStepComponentImpl()
{
}

IActivityComponent& MftInclusiveGatewayStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftInclusiveGatewayStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftInclusiveGatewayStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftInclusiveGatewayStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftInclusiveGatewayStepComponentImpl::getName()
{
    return GatewayInclusive::Name;
}

std::string MftInclusiveGatewayStepComponentImpl::getResource()
{
    return GatewayInclusive::Resource;
}

std::string MftInclusiveGatewayStepComponentImpl::getCatagroyName()
{
    return "Gateway";
}

WorkflowActivity* MftInclusiveGatewayStepComponentImpl::createWorkflowActivity()
{
    return new GatewayInclusive();
}

std::vector<std::pair<std::string, std::vector<int>>> MftInclusiveGatewayStepComponentImpl::getShortcuts()
{
	return m_vecShortcuts;
}

bool MftInclusiveGatewayStepComponentImpl::readonly()
{
    return false;
}

}