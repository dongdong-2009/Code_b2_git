/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/ShutdownSystemStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#if defined (STEPTYPE_SHUTDOWN_SYSTEM)

#include "ShutdownSystemStep.h"
#include "core/utilities/src/TAAssert.h"
#include "YesNoStepParameter.h"
#include "resource.h"
#include "PlanStepEditShutdownSystemPage.h"
#include "core/exceptions/src/PlanStepException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanStep

ShutdownSystemStep::ShutdownSystemStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail) :
    PlanStep(parent,stepDetail),
    m_notifyPeerParameter(NULL)
{
    FUNCTION_ENTRY( "ShutdownSystemStep" );
    FUNCTION_EXIT;
}


ShutdownSystemStep::ShutdownSystemStep(PlanNode* parent,PlanStepId stepId) :
    PlanStep(parent,stepId,TA_Base_Core::SHUTDOWN_SYSTEM_STEP),
    m_notifyPeerParameter(NULL)
{
    FUNCTION_ENTRY( "ShutdownSystemStep" );
    FUNCTION_EXIT;
}


ShutdownSystemStep::ShutdownSystemStep(const ShutdownSystemStep& planStep) :
    PlanStep(planStep),
    m_notifyPeerParameter(NULL)
{
    FUNCTION_ENTRY( "ShutdownSystemStep" );

    m_notifyPeerParameter = new YesNoStepParameter(*this, *planStep.m_notifyPeerParameter);

    addPlanStepParameter(*m_notifyPeerParameter);

    FUNCTION_EXIT;
}


ShutdownSystemStep::~ShutdownSystemStep()
{
    FUNCTION_ENTRY( "~ShutdownSystemStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}


PlanStep* ShutdownSystemStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    PlanStepParameterList stepParameters;
    getPlanStepParameters(stepParameters);

    FUNCTION_EXIT;
    return new ShutdownSystemStep(*this);
}


void ShutdownSystemStep::createPlanStepParameters(PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    delete m_notifyPeerParameter;

    m_notifyPeerParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NOTIFY_PEER), true);

    stepParameters.clear();
    stepParameters.push_back(m_notifyPeerParameter);


    FUNCTION_EXIT;
}


void ShutdownSystemStep::createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                         PlanStepParameterList &stepParameters)
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    if (parameterSet._d() != TA_Base_Core::SHUTDOWN_SYSTEM_STEP)
    {
        std::ostringstream errorMessage;
        errorMessage << "Invalid parameter set type (" << parameterSet._d() << ") for shutdown system step.";

        TA_THROW( TA_Base_Core::PlanStepException(errorMessage.str(), TA_Base_Core::SHUTDOWN_SYSTEM_STEP));
    }

    delete m_notifyPeerParameter;

    m_notifyPeerParameter = new YesNoStepParameter(*this, getParameterName(IDS_STEP_PARAMETER_NOTIFY_PEER), parameterSet.shutdownSystem().notifyPeer);

    stepParameters.clear();
    stepParameters.push_back(m_notifyPeerParameter);


    FUNCTION_EXIT;
}


void ShutdownSystemStep::getUpdatedParameters(TA_Base_Core::Step& step)
{
    FUNCTION_ENTRY( "getUpdatedParameters" );

    TA_Base_Core::ShutdownSystemParameters  parameter;

    parameter.notifyPeer = m_notifyPeerParameter->getAffirmative();

    step.parameters.shutdownSystem(parameter);

    FUNCTION_EXIT;
}


CPlanStepEditParameterPage* ShutdownSystemStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditShutdownSystemPage(this);
}


#endif // defined (STEPTYPE_SHUTDOWN_SYSTEM)
