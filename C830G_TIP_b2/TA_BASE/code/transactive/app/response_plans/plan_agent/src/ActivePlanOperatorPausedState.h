#ifndef ACTIVE_PLAN_OPERATOR_PAUSED_STATE_HEADER_INCLUDED
#define ACTIVE_PLAN_OPERATOR_PAUSED_STATE_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanOperatorPausedState.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

namespace TA_Base_App
{
	//Class declaration
	class ActivePlanOperatorPausedStateImpl : public ActivePlanState
	{
	// Public methods
	public:

		friend class ACE_Singleton<ActivePlanOperatorPausedStateImpl, ACE_Recursive_Thread_Mutex>;

		virtual ActivePlan::ENextStepExecuteMethod executeNextStep( ActivePlan& activePlan ) const;

		virtual void stop         ( ActivePlan& activePlan, std::string& reason ) const;
		virtual void complete     ( ActivePlan& activePlan ) const;
		virtual void pause        ( ActivePlan& activePlan ) const;
		virtual void pauseForOperator ( ActivePlan& activePlan ) const;
		virtual void resume       ( ActivePlan& activePlan ) const;
		virtual void reassign     ( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId ) const;
		virtual void unassign     ( ActivePlan& activePlan ) const;
			virtual void customiseStep( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const;

	private:
		ActivePlanOperatorPausedStateImpl(){}
		~ActivePlanOperatorPausedStateImpl(){}
		ActivePlanOperatorPausedStateImpl( const ActivePlanOperatorPausedStateImpl& );
		ActivePlanOperatorPausedStateImpl& operator=( const ActivePlanOperatorPausedStateImpl& );

	}; // ActivePlanOperatorPausedStateImpl
	typedef ACE_Singleton<ActivePlanOperatorPausedStateImpl, ACE_Recursive_Thread_Mutex> ActivePlanOperatorPausedState;
} // TA_Base_App

#endif //ACTIVE_PLAN_OPERATOR_PAUSED_STATE_HEADER_INCLUDED
