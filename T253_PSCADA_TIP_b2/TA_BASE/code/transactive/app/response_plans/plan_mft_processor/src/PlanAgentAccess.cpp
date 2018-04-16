/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanAgentAccess.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #7 $
  *
  * Last modification: $DateTime: 2014/11/19 17:14:45 $
  * Last modified by:  $Author: huangjian $
  *
  * Thread Safe Singleton to wrap access to the Plan Agent's CORBA Interface
  */

#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/plan_agent/src/PlanAgentLibrary.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/RightsManager.h"
#include "app/response_plans/plan_mft_processor/src/PlanAgentAccess.h"

namespace TA_Base_App
{

// Initialize Statics
TA_Base_Core::NonReEntrantThreadLockable PlanAgentAccess::m_singletonLock;
PlanAgentAccess* TA_Base_App::PlanAgentAccess::m_thePlanAgentAccess = NULL;

////////////////////////////////////////////////////////////////////////////
// PlanAgentAccess

PlanAgentAccess::PlanAgentAccess()
{
    FUNCTION_ENTRY( "PlanAgentAccess" );

    FUNCTION_EXIT;
}


PlanAgentAccess::~PlanAgentAccess()
{
    FUNCTION_ENTRY( "~PlanAgentAccess" );

    FUNCTION_EXIT;
}

PlanAgentAccess& PlanAgentAccess::getInstance()
{
    FUNCTION_ENTRY( "getInstance" );

    // The first call to this function will create the object,
    // thereafter it will return the object
    if ( m_thePlanAgentAccess == NULL )
    {
        // Guard the method to ensure access by client code is synchronous
        TA_Base_Core::ThreadGuard guard(m_singletonLock);

        if ( m_thePlanAgentAccess == NULL )
        {
            m_thePlanAgentAccess = new PlanAgentAccess();
        }
    }

    FUNCTION_EXIT;
    return *m_thePlanAgentAccess;
}


void PlanAgentAccess::destroyInstance()
{
    FUNCTION_ENTRY( "destroyInstance" );

    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    if ( NULL != m_thePlanAgentAccess )
    {
        delete m_thePlanAgentAccess;

        m_thePlanAgentAccess = NULL;
    }

    FUNCTION_EXIT;
}

std::string PlanAgentAccess::getNodePath(const unsigned long ulLocationKey, const unsigned long nodeId)
{
    FUNCTION_ENTRY( "getNodePath" );

    PAA_BEGIN_TRY
    {
        TA_Base_Core::NodePath_var nodePathVar;

        m_planAgentMap.getPlanAgent(ulLocationKey).getNodePath(nodeId, nodePathVar.out());

        FUNCTION_EXIT;
        return nodePathVar.in();
    }
    PAA_END_TRY(getNodePath);
}


unsigned long PlanAgentAccess::getStepPosition(const unsigned long ulLocationKey, const unsigned long ulPlanNodeId, const unsigned long ulStepId)
{
    FUNCTION_ENTRY( "getStepPosition" );

    PAA_BEGIN_TRY
    {
        unsigned long ulPosition = 0u;

        m_planAgentMap.getPlanAgent(ulLocationKey).getStepPosition(ulPlanNodeId, ulStepId, ulPosition);

        FUNCTION_EXIT;
        return ulPosition;
    }
    PAA_END_TRY(getNodePath);
}


void PlanAgentAccess::processStepResponse( const TA_Base_Core::ActivePlanId& planId, TA_Base_Core::InteractiveStepData& stepDetail )
{
    FUNCTION_ENTRY( "processStepResponse" );

    PAA_BEGIN_TRY
    {
        m_planAgentMap.getPlanAgent(planId.location).processStepResponse( getSession().c_str(), planId, stepDetail );
    }
    PAA_END_TRY(processStepResponse);

    FUNCTION_EXIT;
}

std::string PlanAgentAccess::getSession()
{
    if ( m_strSession.empty() )
    {
        m_strSession = RightsManager::getInstance().sessionId();
    }

    return m_strSession;
}

}