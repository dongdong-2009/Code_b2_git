
#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"

namespace TA_Base_App
{

PlanStep::PlanStep(IPlanNode* parent, BaseStepParameter& refParameter, bool isNewStep) : 
m_bParametersInited(false),
m_parent(parent),
m_refParameter(refParameter),
m_newStep(isNewStep)
{
}

PlanStep::~PlanStep()
{
}

CPlanStepEditParameterPage* PlanStep::getParameterEditPage()
{
	return NULL;
}

IPlanNode* PlanStep::getParentPlan() const
{ 
	return m_parent;
}

void PlanStep::populateParameter( const std::vector<std::string>& vecParams )
{
    if ( !isParametersValid( vecParams ))
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Invalid parameters size %u retrieved from database", vecParams.size() );
    }

    m_refParameter.deserialize( vecParams );
}

void PlanStep::copyParameter( const std::vector<std::string>& vecParams )
{
    m_refParameter.deserialize( vecParams );

    clearFlowParameters();
}

std::vector<std::string> PlanStep::compactParameter()
{
    return m_refParameter.serialize();
}

bool PlanStep::editable()
{
	return true;
}

unsigned long PlanStep::getStepPkey() const
{
    FUNCTION_ENTRY( "getStepPkey" );
    FUNCTION_EXIT;
    return m_refParameter.basic.pkey;
}

unsigned long PlanStep::getStepId() const
{
	FUNCTION_ENTRY( "getStepId" );
	FUNCTION_EXIT;
	return m_refParameter.basic.id;
}


unsigned long PlanStep::getStepPosition() const
{
	FUNCTION_ENTRY( "getStepPosition" );
	FUNCTION_EXIT;
	return m_refParameter.basic.position;
}


const std::string PlanStep::getStepName() const
{
	FUNCTION_ENTRY( "getStepName" );
	FUNCTION_EXIT;
	return m_refParameter.basic.name;
}


const std::string PlanStep::getStepDescription() const
{
	FUNCTION_ENTRY( "getStepDescription" );
	FUNCTION_EXIT;
	return m_refParameter.basic.description;
}

const std::string PlanStep::getStepGraph() const
{
	FUNCTION_ENTRY( "getStepGraph" );
	FUNCTION_EXIT;
	return m_refParameter.basic.graph;
}

int PlanStep::getStepType() const
{
	FUNCTION_ENTRY( "getStepType" );
	FUNCTION_EXIT;
	return m_refParameter.basic.type;
}


bool PlanStep::isStepSkipped() const
{
	FUNCTION_ENTRY( "isStepSkipped" );
	FUNCTION_EXIT;
	return m_refParameter.basic.skip;
}


bool PlanStep::isStepSkippable() const
{
	FUNCTION_ENTRY( "isStepSkippable" );
	FUNCTION_EXIT;
	return m_refParameter.basic.skippable;
}

bool PlanStep::ignoreFailure() const
{
	FUNCTION_ENTRY( "ignoreFailure" );
	FUNCTION_EXIT;
	return m_refParameter.basic.ignoreFailure;
}


time_t PlanStep::getStepDelay() const
{
	FUNCTION_ENTRY( "getStepDelay" );
	FUNCTION_EXIT;
	return m_refParameter.basic.delay;
}

void PlanStep::setStepPosition(const unsigned long stepPosition)
{
	m_refParameter.basic.position = stepPosition;
}

void PlanStep::setStepPkey( const unsigned long pkey )
{
    m_refParameter.basic.pkey = pkey;
}

void PlanStep::setStepId( const unsigned long stepId )
{
    m_refParameter.basic.id = stepId;
}

void PlanStep::setStepName(const std::string& name)
{
	m_refParameter.basic.name = name.c_str();
}

void PlanStep::setStepDescription( const std::string& description )
{
	m_refParameter.basic.description = description.c_str();
}

void PlanStep::setStepGraph( const std::string& graph )
{
	m_refParameter.basic.graph = graph.c_str( );
}

void PlanStep::setStepSkippable(bool skip)
{
	m_refParameter.basic.skippable = skip;
}

void PlanStep::setStepSkipped(bool skip)
{
	m_refParameter.basic.skip = skip;
}

void PlanStep::setIgnoreFailure(bool ignoreFailure)
{
	m_refParameter.basic.ignoreFailure = ignoreFailure;
}

void PlanStep::setStepDelay( const unsigned long delay )
{
	m_refParameter.basic.delay = delay;
}

bool PlanStep::hasIncoming()
{
    return true;
}

bool PlanStep::hasOutgoing()
{
    return true;
}

bool PlanStep::allowMultipleIncoming()
{
    return false;
}

bool PlanStep::allowMultipleOutgoing()
{
    return false;
}

void PlanStep::reloadParameters()
{
    // Ensure step parameters are loaded.
    if ( !m_bParametersInited )
    {
        ACE_Write_Guard<ACE_RW_Mutex> paramGuard( m_mtxStepParameters );

        if ( !m_bParametersInited )
        {
            if ( !m_newStep )
            {
                std::vector<std::string> vecParams = PlanAgentAccessFactory::instance().getStepParameters( m_refParameter.basic.pkey );
                populateParameter( vecParams );
            }

            // Overridden in derived classes to provide list of steps
            createPlanStepParameters();

            m_bParametersInited = true;
        }
    }
}

bool PlanStep::retrieveIncomings( std::vector<unsigned int>& vecIncomings )
{
    // Return false as default no custom incoming step, overwrite this function in sub classes if step has incoming steps
    return false;
}

bool PlanStep::retrieveOutgoings( std::vector<unsigned int>& vecOutgoings )
{
    // Return false as default no custom outgoing step, overwrite this function in sub classes if step has outgoing steps
    return false;
}

bool PlanStep::canParallelHandle()
{
	// Return false as default no parallel handling, overwrite this function in sub classes if step can handle parallely
	return false;
}

void PlanStep::outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing )
{
    // Do nothing, overwrite this function in sub classes if step has outgoing steps
}

void PlanStep::incomingStepChanged( const unsigned int unPrevIncoming, const unsigned int unNewIncoming )
{
    // Do nothing, overwrite this function in sub classes if step has incoming steps
}

void PlanStep::linkOutgoingStep( const unsigned int unPrevOutgoing, const unsigned int unOutgoing )
{
    if ( !hasCustomOutgoingStep() )
    {
        return;
    }
    
    reloadParameters();

    outgoingStepChanged( unPrevOutgoing, unOutgoing );
}

void PlanStep::linkIncomingStep( const unsigned int unPrevIncoming, const unsigned int unIncoming )
{
    if ( !hasCustomIncomingStep() )
    {
        return;
    }

    reloadParameters();

    incomingStepChanged( unPrevIncoming, unIncoming );
}

bool PlanStep::hasCustomIncomingStep() const
{
    return false;
}

bool PlanStep::updated()
{
    return m_refParameter.updated;
}

void PlanStep::updateFlowParameters()
{
}

void PlanStep::resetFlowParameters()
{
}

void PlanStep::clearFlowParameters()
{
}

}