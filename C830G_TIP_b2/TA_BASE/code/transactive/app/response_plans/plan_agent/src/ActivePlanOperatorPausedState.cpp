/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanOperatorPausedState.cpp $
  *  @author:  Mahaveer Pareek
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
	ActivePlan::ENextStepExecuteMethod ActivePlanOperatorPausedStateImpl::executeNextStep( ActivePlan& activePlan ) const
	{
		// Do nothing.
		ACE_OS::sleep( ACE_Time_Value( 0, PlanAgentUser::getInstance()->getPausedPlanSleepTime ()) );
		return( ActivePlan::INTERLOCKED_NSEM );
	}

	void ActivePlanOperatorPausedStateImpl::stop( ActivePlan& activePlan, std::string& reason ) const
	{
		activePlan.setIsActive( false );

		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
														 TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE, reason );
	}

	void ActivePlanOperatorPausedStateImpl::complete( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}

	void ActivePlanOperatorPausedStateImpl::pause( ActivePlan& activePlan ) const
	{
//		changeState( activePlan, ActivePlanPausedState::instance() );

        if ( activePlan.isPausePending() )
        {
            return;
        }
        
        activePlan.schedulePause();
        changeState( activePlan, ActivePlanPausedState::instance() );
       
        activePlan.publishNextStepDetail();

//         std::string remark = "Plan paused";

//        activePlan.setOperatorResponseInterlock(false);
//		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
//														 TA_Base_Core::PAUSED_ACTIVE_PLAN_STATE, remark );
	}

	void ActivePlanOperatorPausedStateImpl::pauseForOperator( ActivePlan& activePlan ) const
	{
		// Do nothing.
	}


	void ActivePlanOperatorPausedStateImpl::resume( ActivePlan& activePlan ) const
	{
		changeState( activePlan, ActivePlanExecutingState::instance() );

        activePlan.setOperatorResponseInterlock(false);

		std::string remark = "Plan resumed";

		ActivePlanBulletinBoard::instance()->update( activePlan.getPlan(),
														 TA_Base_Core::EXECUTING_ACTIVE_PLAN_STATE, remark );
	}

	void ActivePlanOperatorPausedStateImpl::reassign( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId ) const
	{
		activePlan.scheduleTakeControl( sessionId, pmId );
	}

	void ActivePlanOperatorPausedStateImpl::unassign( ActivePlan& activePlan ) const
	{
		defaultUnassign( activePlan );
	}

	void ActivePlanOperatorPausedStateImpl::customiseStep( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const
	{
		defaultCustomiseStep( activePlan, newDetail );
	}
} // TA_Base_App
