/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveTerminatePlanStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #8 $
  *
  *  Last modification: $DateTime: 2015/01/08 17:33:12 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/PlanExecutor.h"
#include "app/response_plans/plan_agent/src/ActiveTerminatePlanStep.h"

// namespace alias for Corba structures/types
 
namespace TA_Base_App
{

ActiveTerminatePlanStep::ActiveTerminatePlanStep( const BasicParameter& detail, IActivePlan& plan ) : ActiveStep( m_params, plan ),
m_params( detail )
{
}

bool ActiveTerminatePlanStep::executeImpl()
{
	FUNCTION_ENTRY( "ActiveTerminatePlanStep::executeImpl()" );

	bool instanceFound = false;

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "%s: terminating plan id %lu", dumpContext().c_str(), m_params.plan );

    std::list<TA_Base_Core::ActivePlanId> instanceList;
	ActivePlanBulletinBoard::instance()->getActiveInstances( m_params.plan, instanceList );

    std::ostringstream reasonStrm;
    reasonStrm << "Terminated by instance " << m_iPlan.id().instance << " of plan '" << m_iPlan.name() << "'";

    std::string reason = reasonStrm.str();

	for( std::list<TA_Base_Core::ActivePlanId>::iterator it = instanceList.begin(); it != instanceList.end(); ++it )
	{
		// make sure we don't tell ourself to terminate...
		if( m_iPlan.id().plan != (*it).plan )
		{
			PlanExecutor::instance()->stop( *it, reason );
			instanceFound = true;
		}
		else
		{
			if( m_iPlan.id().instance != (*it).instance )
			{
				PlanExecutor::instance()->stop( *it, reason );
				instanceFound = true;
			}
		}
	}

	FUNCTION_EXIT;
	return instanceFound;
}

int ActiveTerminatePlanStep::waitImpl()
{
    int nWaitRes = ActivePlanBulletinBoard::instance()->isActive( m_params.plan, m_iPlan.id() ) ? eAsyncWaitContinue : eWaitSucceed;

    static const ACE_Time_Value IdleTime( 0, 500000 ); // 500 millisecond
    ACE_OS::sleep( IdleTime );

    return nWaitRes;
}

} // TA_Base_App
