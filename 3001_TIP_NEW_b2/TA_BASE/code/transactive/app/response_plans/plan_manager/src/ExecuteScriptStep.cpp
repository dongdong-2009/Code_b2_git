//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/LogStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2009/12/29 14:17:48 $
// Last modified by:  $Author: grace.koh $
//
// <description>
#include "StdAfx.h"

#if defined (STEPTYPE_EXECUTE_SCRIPT)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "resource.h"
#include "TextualStepParameter.h"
#include "StepNumberStepParameter.h"
#include "PlanStepEditExecuteScriptPage.h"
#include "ExecuteScriptStep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{


    ExecuteScriptStep::ExecuteScriptStep( PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail ):
PlanStep(parent, stepDetail),
m_commandLineParameter(NULL)
{
    FUNCTION_ENTRY( "ExecuteScriptStep" );
    FUNCTION_EXIT;
}

ExecuteScriptStep::ExecuteScriptStep( PlanNode* parent,PlanStepId stepId ) :
PlanStep(parent, stepId, TA_Base_Core::EXECUTE_SCRIPT_STEP),
m_commandLineParameter(NULL)
{
    FUNCTION_ENTRY( "ExecuteScriptStep" );
    FUNCTION_EXIT;
}

ExecuteScriptStep::ExecuteScriptStep( const ExecuteScriptStep& planStep):
PlanStep(planStep),
m_commandLineParameter(NULL)
{
    FUNCTION_ENTRY( "ExecuteScriptStep" );
    m_commandLineParameter = new TextualStepParameter(*this, *planStep.m_commandLineParameter);

    addPlanStepParameter(*m_commandLineParameter);

    FUNCTION_EXIT;
}

ExecuteScriptStep::~ExecuteScriptStep()
{

}

PlanStep* ExecuteScriptStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new ExecuteScriptStep(*this);
}

void ExecuteScriptStep::createPlanStepParameters( PlanStepParameterList &stepParameters )
{

    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_commandLineParameter;


    m_commandLineParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_COMMAND_LINE), "");

    stepParameters.clear();
    stepParameters.push_back(m_commandLineParameter);

    FUNCTION_EXIT;
}

void ExecuteScriptStep::createPlanStepParameters( const TA_Base_Core::StepParameters &parameterSet, PlanStepParameterList &stepParameters )
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::EXECUTE_SCRIPT_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for execute scripte step";

        TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::EXECUTE_SCRIPT_STEP));
    }
    delete m_commandLineParameter;

    m_commandLineParameter = new TextualStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_COMMAND_LINE), parameterSet.executeScript().commands.in());

    stepParameters.clear();
    stepParameters.push_back(m_commandLineParameter);
}

void ExecuteScriptStep::getUpdatedParameters( TA_Base_Core::Step& step )
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::ExecuteScriptParameters  parameter;

    parameter.commands = CORBA::string_dup(m_commandLineParameter->getMessageText().c_str());

    step.parameters.executeScript(parameter);

    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* ExecuteScriptStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepExecuteScriptPage(this);
}



}

#endif //  STEPTYPE_PLAN_START_TIME_CHECK