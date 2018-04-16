/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchGuiAppStep.cpp $
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

#include "core/utilities/src/TAAssert.h"
#include "LaunchGuiAppStep.h"
#include "resource.h"
#include "GuiAppTypeStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TextualStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/NumericStepParameter.h"
#include "PlanStepEditLaunchGuiAppParameterPage.h"
#include "core/exceptions/src/PlanStepException.h"

const unsigned long TA_Base_App::LaunchGuiAppStep::DEFAULT_X_POSITION = 0;
const unsigned long TA_Base_App::LaunchGuiAppStep::DEFAULT_Y_POSITION = 155;
const unsigned long TA_Base_App::LaunchGuiAppStep::DEFAULT_WIDTH      = -1;
const unsigned long TA_Base_App::LaunchGuiAppStep::DEFAULT_HEIGHT     = -1;

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

LaunchGuiAppStep::LaunchGuiAppStep(IPlanNode* parent, const BasicParameter &stepDetail) :
    PlanStep(parent, m_params, false),
    m_appTypeParameter(NULL),
    m_commandLineParameter(NULL),
    m_xPositionParameter(NULL),
    m_yPositionParameter(NULL),
    m_widthParameter(NULL),
    m_heightParameter(NULL),
	m_params(stepDetail),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "LaunchGuiAppStep" );
    FUNCTION_EXIT;
}


LaunchGuiAppStep::LaunchGuiAppStep(IPlanNode* parent, const unsigned long stepId, const int stepType) :
    PlanStep(parent, m_params, true),
    m_appTypeParameter(NULL),
    m_commandLineParameter(NULL),
    m_xPositionParameter(NULL),
    m_yPositionParameter(NULL),
    m_widthParameter(NULL),
    m_heightParameter(NULL),
    m_params(stepId, stepType),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "LaunchGuiAppStep" );
    FUNCTION_EXIT;
}


LaunchGuiAppStep::LaunchGuiAppStep(const LaunchGuiAppStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
    m_appTypeParameter(NULL),
    m_commandLineParameter(NULL),
    m_xPositionParameter(NULL),
    m_yPositionParameter(NULL),
    m_widthParameter(NULL),
    m_heightParameter(NULL),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "LaunchGuiAppStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


LaunchGuiAppStep::~LaunchGuiAppStep()
{
    FUNCTION_ENTRY( "~LaunchGuiAppStep" );

	delete m_appTypeParameter;
	delete m_commandLineParameter;
	delete m_xPositionParameter;
	delete m_yPositionParameter;
	delete m_widthParameter;
	delete m_heightParameter;

    FUNCTION_EXIT;
}


PlanStep* LaunchGuiAppStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new LaunchGuiAppStep(*this);
}


void LaunchGuiAppStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_appTypeParameter;
    delete m_commandLineParameter;
    delete m_xPositionParameter;
    delete m_yPositionParameter;
    delete m_widthParameter;
    delete m_heightParameter;

    m_appTypeParameter = new GuiAppTypeStepParameter(*this, ParamApp, m_params.app, true);
    m_commandLineParameter = new TextualStepParameter(*this, ParamArgs, m_params.args);
	m_xPositionParameter = new NumericStepParameter(*this, ParamPositionLeft, m_params.pos.left);
	m_yPositionParameter = new NumericStepParameter(*this, ParamPositionTop, m_params.pos.top);
    m_widthParameter = new NumericStepParameter(*this, ParamPositionWidth, m_params.pos.width, true);
    m_heightParameter = new NumericStepParameter(*this, ParamPositionHeight, m_params.pos.height, true);

    FUNCTION_EXIT;
}

bool LaunchGuiAppStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t LaunchGuiAppParameterSize = 3u;

	bool bValid = ( LaunchGuiAppParameterSize == parameters.size() );
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for launch GUI application step [%u].", parameters.size(), LaunchGuiAppParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

CPlanStepEditParameterPage* LaunchGuiAppStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditLaunchGuiAppParameterPage(this);
}

std::string LaunchGuiAppStep::getStepTypeName() const
{
	return "Launch GUI Application";
}

IActivityParameter& LaunchGuiAppStep::getActivityParameter()
{
    reloadParameters();
	return m_parActivityParameter;
}

}
