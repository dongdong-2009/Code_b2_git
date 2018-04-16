/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchSchematicStep.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#include "StdAfx.h"
#include "resource.h"

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "LaunchSchematicStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TextualStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/NumericStepParameter.h"
#include "PlanStepEditLaunchSchematicPage.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

LaunchSchematicStep::LaunchSchematicStep(IPlanNode* parent, const BasicParameter &stepDetail) :
    PlanStep(parent, m_params, false),
    m_schematicNameParameter(NULL),
    m_commandLineParameter(NULL),
    m_screenNumberParameter(NULL),
	m_params(stepDetail),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "LaunchSchematicStep" );
    FUNCTION_EXIT;
}


LaunchSchematicStep::LaunchSchematicStep(IPlanNode* parent, const unsigned long stepId, const int stepType) :
    PlanStep(parent,m_params,true),
    m_schematicNameParameter(NULL),
    m_commandLineParameter(NULL),
    m_screenNumberParameter(NULL),
    m_params(stepId, stepType),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "LaunchSchematicStep" );
    FUNCTION_EXIT;
}


LaunchSchematicStep::LaunchSchematicStep(const LaunchSchematicStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
    m_schematicNameParameter(NULL),
    m_commandLineParameter(NULL),
    m_screenNumberParameter(NULL),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "LaunchSchematicStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


LaunchSchematicStep::~LaunchSchematicStep()
{
    FUNCTION_ENTRY( "~LaunchSchematicStep" );

	delete m_schematicNameParameter;
	delete m_commandLineParameter;
	delete m_screenNumberParameter;

    FUNCTION_EXIT;
}


PlanStep* LaunchSchematicStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new LaunchSchematicStep(*this);
}


void LaunchSchematicStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_schematicNameParameter;
    delete m_commandLineParameter;
    delete m_screenNumberParameter;

	m_schematicNameParameter = new TextualStepParameter(*this, ParamSchematic, m_params.schematic);
	m_commandLineParameter = new TextualStepParameter(*this, ParamCommands, m_params.commands);
	m_screenNumberParameter = new NumericStepParameter(*this, ParamScreen, m_params.screen);

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* LaunchSchematicStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditLaunchSchematicParameterPage(this);
}

bool LaunchSchematicStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t LaunchSchematicParameterSize = 3u;

	bool bValid = ( LaunchSchematicParameterSize == parameters.size() );
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for launch schematic step [%u].", parameters.size(), LaunchSchematicParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

std::string LaunchSchematicStep::getStepTypeName() const
{
	return "Launch schematic";
}

IActivityParameter& LaunchSchematicStep::getActivityParameter()
{
    reloadParameters();

	return m_parActivityParameter;
}

}
