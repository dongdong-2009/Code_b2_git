/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointTimeCheckStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PlanStepException.h"
#include "app/response_plans/plan_step_plugin/data_point_time_check/editor/src/DataPointTimeCheckStep.h"
#include "app/response_plans/plan_step_plugin/data_point_time_check/editor/src/PlanStepEditDPTimeCheckPage.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

DataPointTimeCheckStep::DataPointTimeCheckStep(IPlanNode* parent,const BasicParameter &stepDetail) :
    PlanStep(parent,m_params,false),
	m_params(stepDetail),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "DataPointTimeCheckStep" );
    FUNCTION_EXIT;
}


DataPointTimeCheckStep::DataPointTimeCheckStep(IPlanNode* parent,const unsigned long stepId, const int stepType) :
    PlanStep(parent,m_params,true),
    m_params(stepId, stepType),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "DataPointTimeCheckStep" );
    FUNCTION_EXIT;
}


DataPointTimeCheckStep::DataPointTimeCheckStep(const DataPointTimeCheckStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "DataPointTimeCheckStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


DataPointTimeCheckStep::~DataPointTimeCheckStep()
{
    FUNCTION_ENTRY( "~DataPointTimeCheckStep" );
    FUNCTION_EXIT;
}


PlanStep* DataPointTimeCheckStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new DataPointTimeCheckStep(*this);
}


void DataPointTimeCheckStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* DataPointTimeCheckStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditDataPointTimeCheckPage(this);
}

bool DataPointTimeCheckStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t DataPointTimeCheckParameterSize = 4u;

	bool bValid = ( DataPointTimeCheckParameterSize == parameters.size() );
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for data point time check step [%u].", parameters.size(), DataPointTimeCheckParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string DataPointTimeCheckStep::getStepTypeName() const
{
	return "Data point time check";
}

IActivityParameter& DataPointTimeCheckStep::getActivityParameter()
{
    reloadParameters();
	return m_parActivityParameter;
}

void DataPointTimeCheckStep::outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing )
{
    if ( unPrevOutgoing == m_params.successStep )
    {
        m_params.successStep = unNewOutgoing;
    }
    else if ( unPrevOutgoing == m_params.failureStep )
    {
        m_params.failureStep = unNewOutgoing;
    }
}

bool DataPointTimeCheckStep::allowMultipleIncoming()
{
	return false;
}

bool DataPointTimeCheckStep::allowMultipleOutgoing()
{
	return true;
}

bool DataPointTimeCheckStep::retrieveOutgoings( std::vector<unsigned int>& vecOutgoings )
{
	if ( 0u != m_params.successStep )
	{
		vecOutgoings.push_back( m_params.successStep );
	}

	if ( 0u != m_params.failureStep )
	{
		vecOutgoings.push_back( m_params.failureStep );
	}

	return true;
}

bool DataPointTimeCheckStep::hasCustomOutgoingStep() const
{
	return true;
}

bool DataPointTimeCheckStep::canIgnoreFailure() const
{
	return true;
}

void DataPointTimeCheckStep::clearFlowParameters()
{
    m_params.successStep = 0u;
    m_params.failureStep = 0u;
}
}
