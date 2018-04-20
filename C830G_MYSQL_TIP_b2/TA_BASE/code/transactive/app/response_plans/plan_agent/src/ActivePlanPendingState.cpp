/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPendingState.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

namespace TA_Base_App
{
	ActivePlan::ENextStepExecuteMethod ActivePlanPendingStateImpl::executeNextStep( ActivePlan& activePlan ) const
	{
		// Do nothing.
		ACE_OS::sleep( ACE_Time_Value( 0, 10000 ) );
		return( ActivePlan::NORMAL_NSEM );
	}

	void ActivePlanPendingStateImpl::stop( ActivePlan& activePlan, std::string& reason ) const
	{
		activePlan.setIsActive( false );


		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
														 TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE, reason );
	}

	void ActivePlanPendingStateImpl::complete( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

	void ActivePlanPendingStateImpl::pause( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

	void ActivePlanPendingStateImpl::pauseForOperator( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

	void ActivePlanPendingStateImpl::resume( ActivePlan& activePlan ) const
	{
		changeState( activePlan, ActivePlanExecutingState::instance() );

		std::string remark = "Plan resumed";

		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
														 TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE, remark );
	}

	void ActivePlanPendingStateImpl::reassign( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId) const
	{
        defaultReassign( activePlan, sessionId, pmId );

    }

	void ActivePlanPendingStateImpl::unassign( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

		void ActivePlanPendingStateImpl::customiseStep( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const
	{
		defaultCustomiseStep( activePlan, newDetail );
	}

} // TA_Base_App
