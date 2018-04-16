//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanCheckTimerStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/02/05 23:52:53 $
// Last modified by:  $Author: qi.huang $
//
// <description>
#include "StdAfx.h"

#if defined (STEPTYPE_PLAN_CHECK_TIMER)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "resource.h"
#include "TimeSpanStepParameter.h"
#include "StepNumberStepParameter.h"
#include "PlanStepPlanCheckTimerPage.h"
#include "PlanCheckTimerStep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{


    PlanCheckTimerStep::PlanCheckTimerStep( PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail ):
        PlanStep(parent, stepDetail),
        m_timeSpanParameter(NULL),
        m_successParameter(NULL),
        m_failureParameter(NULL)
    {
        FUNCTION_ENTRY( "PlanCheckTimerStep" );
        FUNCTION_EXIT;
    }

    PlanCheckTimerStep::PlanCheckTimerStep( PlanNode* parent,PlanStepId stepId ) :
        PlanStep(parent, stepId, TA_Base_Core::PLAN_CHECK_TIMER_STEP),
        m_timeSpanParameter(NULL),
        m_successParameter(NULL),
        m_failureParameter(NULL)
    {
        FUNCTION_ENTRY( "PlanCheckTimerStep" );
        FUNCTION_EXIT;
    }

    PlanCheckTimerStep::PlanCheckTimerStep( const PlanCheckTimerStep& planStep):
        PlanStep(planStep),
        m_timeSpanParameter(NULL),
        m_successParameter(NULL),
        m_failureParameter(NULL)
    {
        FUNCTION_ENTRY( "PlanCheckTimerStep" );
        m_timeSpanParameter = new TimeSpanStepParameter(*this, *planStep.m_timeSpanParameter);
        m_successParameter = new StepNumberStepParameter(*this, *planStep.m_successParameter);
        m_failureParameter = new StepNumberStepParameter(*this, *planStep.m_failureParameter);

        addPlanStepParameter(*m_timeSpanParameter);
        addPlanStepParameter(*m_successParameter);
        addPlanStepParameter(*m_failureParameter);
        FUNCTION_EXIT;
    }

    PlanCheckTimerStep::~PlanCheckTimerStep()
    {

    }

    PlanStep* PlanCheckTimerStep::clone()
    {
        FUNCTION_ENTRY( "clone" );

        // Ensure parameters are loaded
        PlanStepParameterList stepParameters;
        getPlanStepParameters(stepParameters);

        FUNCTION_EXIT;
        return new PlanCheckTimerStep(*this);
    }

    void PlanCheckTimerStep::createPlanStepParameters( PlanStepParameterList &stepParameters )
    {

        FUNCTION_ENTRY( "createPlanStepParameters" );

        delete m_timeSpanParameter;
        delete m_successParameter;
        delete m_failureParameter;

        m_timeSpanParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIME_SPAN), 0, TimeSpanStepParameter::TR_SECONDS);
        m_successParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_SUCCESS_STEP), 0);
        m_failureParameter = new StepNumberStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_FAILURE_STEP), 0);

        stepParameters.clear();
        stepParameters.push_back(m_timeSpanParameter);
        stepParameters.push_back(m_successParameter);
        stepParameters.push_back(m_failureParameter);

        FUNCTION_EXIT;
    }

    void PlanCheckTimerStep::createPlanStepParameters( const TA_Base_Core::StepParameters &parameterSet, PlanStepParameterList &stepParameters )
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        if (parameterSet._d() != TA_Base_Core::PLAN_CHECK_TIMER_STEP)
        {
            std::ostringstream errorMessage;
            errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for plan start time check step.";

            TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::PLAN_CHECK_TIMER_STEP));
        }
        delete m_timeSpanParameter;
        delete m_successParameter;
        delete m_failureParameter;

        m_timeSpanParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIME_SPAN), parameterSet.planCheckTimer().timeSpan, TimeSpanStepParameter::TR_SECONDS);
        m_successParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_SUCCESS_STEP), parameterSet.planCheckTimer().successStep);
        m_failureParameter = new StepNumberStepParameter(*this,getParameterName(IDS_STEP_PARAMETER_FAILURE_STEP), parameterSet.planCheckTimer().failureStep);

        stepParameters.clear();
        stepParameters.push_back(m_timeSpanParameter);
        stepParameters.push_back(m_successParameter);
        stepParameters.push_back(m_failureParameter);
    }

    void PlanCheckTimerStep::getUpdatedParameters( TA_Base_Core::Step& step )
    {
        FUNCTION_ENTRY( "getUpdatedParameters" );

        TA_Base_Core::PlanCheckTimerParameters  parameter;

        parameter.timeSpan = m_timeSpanParameter->getNumericValue();
        parameter.successStep = m_successParameter->getStepId();
        parameter.failureStep = m_failureParameter->getStepId();

        step.parameters.planCheckTimer(parameter);

        FUNCTION_EXIT;
    }

    CPlanStepEditParameterPage* PlanCheckTimerStep::getParameterEditPage()
    {
        FUNCTION_ENTRY( "getParameterEditPage" );
        FUNCTION_EXIT;
        return new CPlanStepPlanCheckTimerPage(this);
    }



}

#endif //  STEPTYPE_PLAN_CHECK_TIMER