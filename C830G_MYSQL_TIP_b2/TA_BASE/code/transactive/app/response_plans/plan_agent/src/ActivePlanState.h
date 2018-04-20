#ifndef ACTIVEPLANSTATE_H_INCLUDED
#define ACTIVEPLANSTATE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanState.h $
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
	class ActivePlanState
	{
	// Public methods
	public:
		ActivePlanState(){}
		virtual ~ActivePlanState(){}

		virtual ActivePlan::ENextStepExecuteMethod executeNextStep( ActivePlan& activePlan ) const = 0;

        virtual void stop         ( ActivePlan& activePlan, std::string& reason ) const = 0;
        virtual void complete     ( ActivePlan& activePlan ) const = 0;
		virtual void pause        ( ActivePlan& activePlan ) const = 0;
		virtual void pauseForOperator ( ActivePlan& activePlan ) const = 0;
		virtual void resume       ( ActivePlan& activePlan ) const = 0;
		virtual void reassign     ( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId ) const = 0;
		virtual void unassign     ( ActivePlan& activePlan ) const = 0;
		virtual void customiseStep( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const = 0;

	protected:
	
		// Note that there are no default implentations for stop, pause and resume
		// because these are state changes only. Unlike the methods below that require
		// some behaviour (other than changing state) of the ActivePlan object.
		ActivePlan::ENextStepExecuteMethod defaultExecuteNextStep( ActivePlan& activePlan ) const;
		void defaultReassign       ( ActivePlan& activePlan, const std::string& sessionId, const std::string& pmId) const;
		void defaultUnassign       ( ActivePlan& activePlan ) const;
		void defaultCustomiseStep  ( ActivePlan& activePlan, const TA_Base_Core::StepCustomisableDetail& newDetail ) const;

		/**
		  * changeState
		  *
		  * Only ActivePlanState is allowed to access the changeState
		  * method of ActivePlan, and it provides that access to it's
		  * children through protected inheritance.
		  *
		  */
        void changeState( ActivePlan& activePlan, ActivePlanState* state ) const;

	private:
		ActivePlanState& operator=( const ActivePlanState& );
		ActivePlanState( const ActivePlanState& );

	}; // ActivePlanState
}; // TA_Base_App

#endif // ACTIVEPLANSTATE_H_INCLUDED
