/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointSetStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#include "app/response_plans/plan_step_plugin/data_point_set/editor/src/StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_step_plugin/data_point_set/editor/src/DataPointSetStep.h"
#include "app/response_plans/plan_step_plugin/data_point_set/editor/src/PlanStepEditDataPointSetPage.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

DataPointSetStep::DataPointSetStep(IPlanNode* parent,const BasicParameter &stepDetail) : PlanStep(parent,m_params,false),
m_params(stepDetail),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "DataPointSetStep" );
    FUNCTION_EXIT;
}


DataPointSetStep::DataPointSetStep(IPlanNode* parent,const unsigned long stepId, const int stepType) : PlanStep(parent,m_params,true),
m_params(stepId, stepType),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "DataPointSetStep" );
    FUNCTION_EXIT;
}


DataPointSetStep::DataPointSetStep(const DataPointSetStep& planStep) : PlanStep(planStep.m_parent, m_params, true),
m_params(planStep.m_params),
m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "DataPointSetStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


DataPointSetStep::~DataPointSetStep()
{
    FUNCTION_ENTRY( "~DataPointSetStep" );
    FUNCTION_EXIT;
}


PlanStep* DataPointSetStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new DataPointSetStep(*this);
}


void DataPointSetStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* DataPointSetStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditDataPointSetPage(this);
}

bool DataPointSetStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t DataPointSetParameterSize = 3u;

	bool bValid = ( DataPointSetParameterSize == parameters.size() );
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for data point set step [%u].", parameters.size(), DataPointSetParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string DataPointSetStep::getStepTypeName() const
{
	return "Data point set";
}

IActivityParameter& TA_Base_App::DataPointSetStep::getActivityParameter()
{
    reloadParameters();

	return m_parActivityParameter;
}

}
