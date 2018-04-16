#include "ExclusiveGatewayStep.h"
#include "GatewayExclusive.h"
#include "MftExclusiveGatewayStepComponent.h"

namespace TA_Base_App
{


PlanStep* MftExclusiveGatewayStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new ExclusiveGatewayStep(parent, stepDetail);
}

PlanStep* MftExclusiveGatewayStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new ExclusiveGatewayStep(parent, stepId, stepType);
}

InteractiveStep* MftExclusiveGatewayStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
	const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters)
{
	return NULL;
}

MftExclusiveGatewayStepComponentImpl::MftExclusiveGatewayStepComponentImpl()
{
}

MftExclusiveGatewayStepComponentImpl::~MftExclusiveGatewayStepComponentImpl()
{
}

IActivityComponent& MftExclusiveGatewayStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftExclusiveGatewayStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftExclusiveGatewayStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftExclusiveGatewayStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftExclusiveGatewayStepComponentImpl::getName()
{
    return GatewayExclusive::Name;
}

std::string MftExclusiveGatewayStepComponentImpl::getResource()
{
    return GatewayExclusive::Resource;
}

std::string MftExclusiveGatewayStepComponentImpl::getCatagroyName()
{
    return "Gateway";
}

WorkflowActivity* MftExclusiveGatewayStepComponentImpl::createWorkflowActivity()
{
    return new GatewayExclusive();
}

std::vector<std::pair<std::string, std::vector<int>>> MftExclusiveGatewayStepComponentImpl::getShortcuts()
{
	return m_vecShortcuts;
}

bool MftExclusiveGatewayStepComponentImpl::readonly()
{
    return false;
}

}