/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/RunPlanStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #18 $
  *
  * Last modification: $DateTime: 2015/05/11 18:10:24 $
  * Last modified by:  $Author: qi.huang $
  *
  * <description>
  *
  **/

#include "StdAfx.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PlanStepException.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/YesNoStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanPathStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "PlanStepEditRunPlanPage.h"
#include "RunPlanStep.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

RunPlanStep::RunPlanStep( IPlanNode* parent, const BasicParameter& stepDetail ) :
    PlanStep(parent,m_params,false),
    m_planParameter(NULL),
    m_launchParameter(NULL),
    m_waitParameter(NULL),
	m_params(stepDetail),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "RunPlanStep" );
    FUNCTION_EXIT;
}


RunPlanStep::RunPlanStep( IPlanNode* parent, const unsigned long stepId, const int stepType ) :
    PlanStep(parent,m_params,true),
    m_planParameter(NULL),
    m_launchParameter(NULL),
    m_waitParameter(NULL),
	m_params(stepId,stepType),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "RunPlanStep" );
    FUNCTION_EXIT;
}


RunPlanStep::RunPlanStep(const RunPlanStep& planStep) :
    PlanStep(planStep.m_parent, m_params, true),
    m_planParameter(NULL),
    m_launchParameter(NULL),
    m_waitParameter(NULL),
	m_params(planStep.m_params),
	m_parActivityParameter(m_params)
{
    FUNCTION_ENTRY( "RunPlanStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


RunPlanStep::~RunPlanStep()
{
    FUNCTION_ENTRY( "~RunPlanStep" );

    delete m_planParameter;
    delete m_launchParameter;
    delete m_waitParameter;

    FUNCTION_EXIT;
}


PlanStep* RunPlanStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new RunPlanStep(*this);
}


void RunPlanStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_planParameter;
    delete m_launchParameter;
    delete m_waitParameter;

    m_planParameter = new PlanPathStepParameter( *this, ParamPlan, m_params.plan );
    m_launchParameter = new YesNoStepParameter( *this, ParamManualLaunch, m_params.manualLaunch );
    m_waitParameter = new YesNoStepParameter( *this, ParamWaitForCompletion, m_params.waitForCompletion );

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* RunPlanStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditRunPlanPage(this);
}

bool RunPlanStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t ParameterSize = 3u;

    bool bValid = (ParameterSize == parameters.size());
    if ( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for RunPlan step [%u].", parameters.size(), ParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

std::string RunPlanStep::getStepTypeName() const
{
	return "Run plan";
}


IActivityParameter& RunPlanStep::getActivityParameter()
{
    reloadParameters();

	return m_parActivityParameter;
}

}