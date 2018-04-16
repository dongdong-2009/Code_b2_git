/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ShutdownSystemStep.cpp $
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

#include "ShutdownSystemStep.h"
#include "core/utilities/src/TAAssert.h"
#include "resource.h"
#include "PlanStepEditShutdownSystemPage.h"
#include "core/exceptions/src/PlanStepException.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

ShutdownSystemStep::ShutdownSystemStep(IPlanNode* parent,const BasicParameter &stepDetail) :
    PlanStep(parent,m_params,false),
	m_params(stepDetail),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "ShutdownSystemStep" );
    FUNCTION_EXIT;
}


ShutdownSystemStep::ShutdownSystemStep(IPlanNode* parent,const unsigned long stepId, const int stepType) :
    PlanStep(parent,m_params,true),
	m_params(stepId, stepType),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "ShutdownSystemStep" );
    FUNCTION_EXIT;
}


ShutdownSystemStep::ShutdownSystemStep(const ShutdownSystemStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "ShutdownSystemStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


ShutdownSystemStep::~ShutdownSystemStep()
{
    FUNCTION_ENTRY( "~ShutdownSystemStep" );
    FUNCTION_EXIT;
}


PlanStep* ShutdownSystemStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new ShutdownSystemStep(*this);
}


void ShutdownSystemStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* ShutdownSystemStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditShutdownSystemPage(this);
}

bool ShutdownSystemStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t ShutdownSystemParameterSize = 1u;

	bool bValid = (ShutdownSystemParameterSize == parameters.size( ));
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for shutdown system step [%u].", parameters.size(), ShutdownSystemParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string ShutdownSystemStep::getStepTypeName() const
{
	return "Shutdown system";
}

IActivityParameter& TA_Base_App::ShutdownSystemStep::getActivityParameter()
{
    reloadParameters();
	return m_parActivityParameter;
}

}
