#include "MftParallelGatewayStepComponent.h"
#include "ParallelGatewayStep.h"
#include "GatewayParallel.h"

namespace TA_Base_App
{


PlanStep* MftParallelGatewayStepComponentImpl::createStep( IPlanNode* parent, BasicParameter& stepDetail )
{
    return new ParallelGatewayStep(parent, stepDetail);
}

PlanStep* MftParallelGatewayStepComponentImpl::createStep( IPlanNode* parent, const unsigned long stepId, const int stepType )
{
    return new ParallelGatewayStep(parent, stepId, stepType);
}

InteractiveStep* MftParallelGatewayStepComponentImpl::createExecutableStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
	const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters )
{
	return NULL;
}

MftParallelGatewayStepComponentImpl::MftParallelGatewayStepComponentImpl()
{
}

MftParallelGatewayStepComponentImpl::~MftParallelGatewayStepComponentImpl()
{
}

IActivityComponent& MftParallelGatewayStepComponentImpl::getActivityComponent()
{
    return *this;
}

int MftParallelGatewayStepComponentImpl::getStepType()
{
    return m_eType;
}

int MftParallelGatewayStepComponentImpl::getAccessType()
{
    return m_eAccess;
}

CreateActivitycPtr MftParallelGatewayStepComponentImpl::getActivityCreationFuncPtr()
{
    return createWorkflowActivity;
}

std::string MftParallelGatewayStepComponentImpl::getName()
{
	return GatewayParallel::Name;
}

std::string MftParallelGatewayStepComponentImpl::getResource()
{
	return GatewayParallel::Resource;
}

std::string MftParallelGatewayStepComponentImpl::getCatagroyName()
{
	return "Event";
}

WorkflowActivity* MftParallelGatewayStepComponentImpl::createWorkflowActivity()
{
	return new GatewayParallel();
}

std::vector<std::pair<std::string, std::vector<int>>> MftParallelGatewayStepComponentImpl::getShortcuts()
{
	return m_vecShortcuts;
}

bool MftParallelGatewayStepComponentImpl::readonly()
{
    return false;
}

}