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
#include "PlanAgentAccess.h"

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

TA_Base_Core::ActivePlanDetails *PlanAgentAccess::getActivePlanDetails(TA_Base_Core::NodeId planNodeId, unsigned long location)
{
	FUNCTION_ENTRY( "getActivePlanDetails" );

	// Guard the method to ensure access by client code is synchronous
	TA_Base_Core::ThreadGuard guard(m_singletonLock);

	PAA_BEGIN_TRY
	{
		TA_Base_Core::ActivePlanDetails_var activePlanDetailsVar;

		m_planAgentMap.getPlanAgent(location).getActivePlanDetail(planNodeId, activePlanDetailsVar.out());

		FUNCTION_EXIT;
		return activePlanDetailsVar._retn();
	}
	PAA_END_TRY(getActivePlanDetail);

	FUNCTION_EXIT;
	return NULL;
}

}