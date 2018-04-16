/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointCheckStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_step_plugin/data_point_check/editor/src/PlanStepEditDataPointCheckPage.h"
#include "app/response_plans/plan_step_plugin/data_point_check/editor/src/DataPointTestTypeStepParameter.h"
#include "app/response_plans/plan_step_plugin/data_point_check/editor/src/DataPointCheckStep.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

DataPointCheckStep::DataPointCheckStep(IPlanNode* parent,const BasicParameter &stepDetail) :
    PlanStep(parent,m_params,false),
	m_params(stepDetail),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "DataPointCheckStep" );
    FUNCTION_EXIT;
}


DataPointCheckStep::DataPointCheckStep(IPlanNode* parent,const unsigned long stepId, const int stepType) :
    PlanStep(parent,m_params,true),
	m_params(stepId,stepType),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "DataPointCheckStep" );
    FUNCTION_EXIT;
}


DataPointCheckStep::DataPointCheckStep(const DataPointCheckStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "DataPointCheckStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


DataPointCheckStep::~DataPointCheckStep()
{
    FUNCTION_ENTRY( "~DataPointCheckStep" );
    FUNCTION_EXIT;
}


PlanStep* DataPointCheckStep::clone()
{
    FUNCTION_ENTRY( "clone" );

	// Ensure parameters are loaded
	reloadParameters();

    FUNCTION_EXIT;
    return new DataPointCheckStep(*this);
}


void DataPointCheckStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* DataPointCheckStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditDataPointCheckPage(this);
}

bool DataPointCheckStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t DataPointCheckParameterSize = 6u;

	bool bValid = ( DataPointCheckParameterSize == parameters.size() );
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for data point check step [%u].", parameters.size(), DataPointCheckParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string DataPointCheckStep::getStepTypeName() const
{
	return "Data point check";
}

IActivityParameter& DataPointCheckStep::getActivityParameter()
{
    reloadParameters();

	return m_parActivityParameter;
}

void DataPointCheckStep::outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing )
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

bool DataPointCheckStep::allowMultipleIncoming()
{
    return false;
}

bool DataPointCheckStep::allowMultipleOutgoing()
{
    return true;
}

void DataPointCheckStep::clearFlowParameters()
{
    m_params.successStep = 0u;
    m_params.failureStep = 0u;
}

bool DataPointCheckStep::retrieveOutgoings( std::vector<unsigned int>& vecOutgoings )
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

bool DataPointCheckStep::hasCustomOutgoingStep() const
{
    return true;
}

bool DataPointCheckStep::canIgnoreFailure() const
{
    return false;
}

}

