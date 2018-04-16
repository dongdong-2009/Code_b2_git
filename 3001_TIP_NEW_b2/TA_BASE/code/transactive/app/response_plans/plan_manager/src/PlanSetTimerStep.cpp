//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanSetTimerStep.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/02/05 23:52:53 $
// Last modified by:  $Author: qi.huang $
//
// <description>
#include "StdAfx.h"

#if defined (STEPTYPE_PLAN_SET_TIMER)

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/PlanStepException.h"
#include "resource.h"
#include "TimeSpanStepParameter.h"
#include "StepNumberStepParameter.h"
#include "PlanStepPlanSetTimerPage.h"
#include "PlanSetTimerStep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{


    PlanSetTimerStep::PlanSetTimerStep( PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail ):
        PlanStep(parent, stepDetail),
        m_timeSpanParameter(NULL)
    {
        FUNCTION_ENTRY( "PlanSetTimerStep" );
        FUNCTION_EXIT;
    }

    PlanSetTimerStep::PlanSetTimerStep( PlanNode* parent,PlanStepId stepId ) :
        PlanStep(parent, stepId, TA_Base_Core::PLAN_SET_TIMER_STEP),
        m_timeSpanParameter(NULL)
    {
        FUNCTION_ENTRY( "PlanSetTimerStep" );
        FUNCTION_EXIT;
    }

    PlanSetTimerStep::PlanSetTimerStep( const PlanSetTimerStep& planStep):
        PlanStep(planStep),
        m_timeSpanParameter(NULL)
    {
        FUNCTION_ENTRY( "PlanSetTimerStep" );
		
        m_timeSpanParameter = new TimeSpanStepParameter(*this, *planStep.m_timeSpanParameter);
        addPlanStepParameter(*m_timeSpanParameter);
		
        FUNCTION_EXIT;
    }

    PlanSetTimerStep::~PlanSetTimerStep()
    {

    }

    PlanStep* PlanSetTimerStep::clone()
    {
        FUNCTION_ENTRY( "clone" );

        // Ensure parameters are loaded
        PlanStepParameterList stepParameters;
        getPlanStepParameters(stepParameters);

        FUNCTION_EXIT;
        return new PlanSetTimerStep(*this);
    }

    void PlanSetTimerStep::createPlanStepParameters( PlanStepParameterList &stepParameters )
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        delete m_timeSpanParameter;

        m_timeSpanParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIME_SPAN), 0, TimeSpanStepParameter::TR_SECONDS);

        stepParameters.clear();
        stepParameters.push_back(m_timeSpanParameter);

        FUNCTION_EXIT;
    }

    void PlanSetTimerStep::createPlanStepParameters( const TA_Base_Core::StepParameters &parameterSet, PlanStepParameterList &stepParameters )
    {
        FUNCTION_ENTRY( "createPlanStepParameters" );

        if (parameterSet._d() != TA_Base_Core::PLAN_SET_TIMER_STEP)
        {
            std::ostringstream errorMessage;
            errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for plan set timer step.";

            TA_THROW(TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::PLAN_SET_TIMER_STEP));
        }
        delete m_timeSpanParameter;

        m_timeSpanParameter = new TimeSpanStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_TIME_SPAN), parameterSet.planSetTimer().time, TimeSpanStepParameter::TR_SECONDS);

        stepParameters.clear();
        stepParameters.push_back(m_timeSpanParameter);
    }

    void PlanSetTimerStep::getUpdatedParameters( TA_Base_Core::Step& step )
    {
        FUNCTION_ENTRY( "getUpdatedParameters" );

        TA_Base_Core::PlanSetTimerParameters  parameter;

        parameter.time = m_timeSpanParameter->getNumericValue();

        step.parameters.planSetTimer(parameter);

        FUNCTION_EXIT;
    }

    CPlanStepEditParameterPage* PlanSetTimerStep::getParameterEditPage()
    {
        FUNCTION_ENTRY( "getParameterEditPage" );
        FUNCTION_EXIT;
        return new CPlanStepPlanSetTimerPage(this);
    }

}

#endif //  STEPTYPE_PLAN_SET_TIMER