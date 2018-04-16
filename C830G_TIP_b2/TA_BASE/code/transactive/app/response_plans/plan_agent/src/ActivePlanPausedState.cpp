/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanPausedState.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

namespace TA_Base_App
{
	ActivePlan::ENextStepExecuteMethod ActivePlanPausedStateImpl::executeNextStep( ActivePlan& activePlan ) const
	{
		// Do nothing.
		ACE_OS::sleep( ACE_Time_Value( 0, PlanAgentUser::getInstance()->getPausedPlanSleepTime ()) );
		return( ActivePlan::INTERLOCKED_NSEM );
	}

	void ActivePlanPausedStateImpl::stop( ActivePlan& activePlan, std::string& reason ) const
	{
		activePlan.setIsActive( false );

		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
														 TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE, reason );
	}

	void ActivePlanPausedStateImpl::complete( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

    void ActivePlanPausedStateImpl::pause( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

	void ActivePlanPausedStateImpl::pauseForOperator( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

	void ActivePlanPausedStateImpl::resume( ActivePlan& activePlan ) const
	{
		changeState( activePlan, ActivePlanExecutingState::instance() );

        // If interactive step timeout and paused, the lock should release when operator resume plan
        activePlan.setOperatorResponseInterlock(false);

		std::string remark = "Plan resumed";

        ActivePlanBulletinBoard::instance()->updateStepStartTime( activePlan.getPlan(), ACE_OS::gettimeofday() );
		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(), TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE, remark );
	}

	void ActivePlanPausedStateImpl::reassign( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId ) const
	{
		defaultReassign( activePlan, sessionId, pmId );
	}

	void ActivePlanPausedStateImpl::unassign( ActivePlan& activePlan ) const
	{
		defaultUnassign( activePlan );
	}

		void ActivePlanPausedStateImpl::customiseStep( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const
	{
		defaultCustomiseStep( activePlan, newDetail );
	}
} // TA_Base_App
