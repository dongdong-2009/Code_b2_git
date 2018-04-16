#ifndef ACTIVEPLANEXECUTINGSTATE_H_INCLUDED
#define ACTIVEPLANEXECUTINGSTATE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanExecutingState.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

namespace TA_Base_App
{
	//Class declaration
	class ActivePlanExecutingStateImpl : public ActivePlanState
	{
	// Public methods
	public:

		friend class ACE_Singleton<ActivePlanExecutingStateImpl, ACE_Recursive_Thread_Mutex>;

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
		ActivePlanExecutingStateImpl(){}
		~ActivePlanExecutingStateImpl(){}
		ActivePlanExecutingStateImpl( const ActivePlanExecutingStateImpl& );
		ActivePlanExecutingStateImpl& operator=( const ActivePlanExecutingStateImpl& );
	
	}; // ActivePlanExecutingState
	typedef ACE_Singleton<ActivePlanExecutingStateImpl, ACE_Recursive_Thread_Mutex> ActivePlanExecutingState;
}; // TA_Base_App

#endif // ACTIVEPLANEXECUTINGSTATE_H_INCLUDED
