/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanExecutingState.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2011/03/29 13:47:47 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"


namespace TA_Base_App
{
	ActivePlan::ENextStepExecuteMethod ActivePlanExecutingStateImpl::executeNextStep( ActivePlan& activePlan ) const
	{
		return( defaultExecuteNextStep( activePlan ) );
	}

	void ActivePlanExecutingStateImpl::stop( ActivePlan& activePlan, std::string& reason ) const
	{
		activePlan.setIsActive( false );

		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
														 TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE, reason );
	}


	void ActivePlanExecutingStateImpl::complete( ActivePlan& activePlan ) const
	{
		activePlan.setIsActive( false );

		std::string remark = "Plan completed successfully";

		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
														 TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE, remark );		
	}

    void ActivePlanExecutingStateImpl::pause( ActivePlan& activePlan ) const
	{
        if ( activePlan.isPausePending() )
        {
            return;
        }

        activePlan.schedulePause();

        changeState( activePlan, ActivePlanPausedState::instance() );

        activePlan.publishNextStepDetail();

//		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(), activePlan.getStepNumber(), TA_Base_Core::PAUSED_ACTIVE_STEP_STATE, "Plan paused by operator" );

        return;

		changeState( activePlan, ActivePlanPausedState::instance() );
		// the plan manager would like to see which step will be executed next
		// rather than which step is currently loaded so we need a special
		// update message to give then that information.
		activePlan.publishNextStepDetail();

		std::string remark = activePlan.getRemark();
		
		if ( remark.empty() )
			remark = "Plan paused by operator";

		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(), activePlan.getStepNumber(), TA_Base_Core::PAUSED_ACTIVE_STEP_STATE, remark );

//		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
//														 TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE , remark);
	}

	void ActivePlanExecutingStateImpl::pauseForOperator( ActivePlan& activePlan ) const
	{
		changeState( activePlan, ActivePlanOperatorPausedState::instance() );
		// the plan manager would like to see which step will be executed next
		// rather than which step is currently loaded so we need a special
		// update message to give then that information.
		activePlan.publishNextStepDetail();
        activePlan.setOperatorResponseInterlock(true);

        // Update to Operator Pause state for Monitor Agent update
        std::string remark = "Waiting for Operator's response";
        ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(), TA_Base_Core::OPERATOR_PAUSED_ACTIVE_PLAN_STATE, remark );

		ActivePlanBulletinBoard::instance()->sendOperatorPausedStateUpdate( activePlan.getPlan() );
	}

	void ActivePlanExecutingStateImpl::resume( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

	void ActivePlanExecutingStateImpl::reassign( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId ) const
	{
		activePlan.scheduleTakeControl( sessionId, pmId );
	}

	void ActivePlanExecutingStateImpl::unassign( ActivePlan& activePlan ) const
	{
		defaultUnassign( activePlan );
	}

    void ActivePlanExecutingStateImpl::customiseStep( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const
	{
		// Do nothing.
	}

} // TA_Base_App
