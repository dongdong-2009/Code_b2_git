#ifndef ACTIVEPLAN_H_INCLUDED
#define ACTIVEPLAN_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlan.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

namespace TA_Base_App
{
	//Class declaration
	class ActivePlan : public IActivePlanFlow
	{

	// Public methods
	public:

		enum ENextStepExecuteMethod{ NORMAL_NSEM, INTERLOCKED_NSEM, UNDEFINED_NSEM };
		friend class ActivePlanState;

        // Add a parameter to indicate the plan is start in Monitor mode
		ActivePlan( const TA_Base_Core::ActivePlanId& plan, 
            TA_Base_Core::EActivePlanState initialState, const bool bStateSyncDone );
		virtual ~ActivePlan();
		bool getIsActive( void ){ return( m_isActive ); }

		virtual void setIsActive( bool isActive )
        { 
            m_isActive = isActive; 
        }

		ENextStepExecuteMethod executeNextStep( void );

        void stop         ( std::string& reason );
		void pause        ( void );
        void complete     ( void );
		void pauseForOperator ( void );
		//lizettejl++ - Resume Paused Plan (DP-changes)
		// void resume       ( void );
		void resume       ( TA_Base_Core::StepNumber resumeStepPosition = -1);
		//++lizettejl
		void reassign     (const std::string& sessionId, const std::string& pmId);
		void unassign     ( void );
        void customiseStep( const TA_Base_Core::StepCustomisableDetail& newDetail );
		void customiseSteps( const TA_Base_Core::StepCustomisableDetails& newDetails );

		virtual void pausePlan ( const std::string& remark, const std::string& failedStepAlarmId = "" );
		virtual std::string getRemark( void );

        //Every step must call only once either of the following setNextStep method.
        // setNextStep(void) should only be used for default next step, whereas
        // setNextStep(step) should be used to set any step in the plan
		virtual void setNextStep( void );
        virtual void setCurrentStepPosition( TA_Base_Core::StepNumber stepNumber);
		virtual void setNextStep( TA_Base_Core::StepId step );                    
        void publishNextStepDetail( void ) const;

        virtual void setPlanStepFailed() {m_stepFailed =  true;}

        virtual bool anyPlanStepFailed() { return m_stepFailed; }

		virtual const TA_Base_Core::ActivePlanId& getPlan( void ) const{ return( m_plan ); }

		/**
		  * setInactiveInterlock
		  *
		  * Sets a plan that the next step must wait for
		  * before executing. No instance is provided for the
		  * plan as _all_ instances must be in an inactive
		  * state before the step will execute.
		  *
		  */
		virtual void setTerminatedInterlock( TA_Base_Core::NodeId interlock );

		/**
		  * setCompletedInterlock
		  *
		  * Sets a plan (and instance) that the next step
		  * must wait for before executing. The given plan
		  * must be in the completed state before the step
		  * executes.
		  *
		  */
		virtual void setCompletedInterlock( const TA_Base_Core::ActivePlanId& interlock );

        /**
          * setOperatorResponseInterlock
          *
          * Sets a plan that the next step must wait for
          * before executing. The given plan must be in the
          * completed state before the step executes.
          */
        virtual void setOperatorResponseInterlock(bool interlock );

		virtual TA_Base_Core::StepNumber getStepNumber();
        virtual ActiveStep* getCurrentStep();
        void scheduleTakeControl( const std::string session, const std::string& pmId );
        void schedulePause();
        bool isPausePending() { return m_isPausePending; }
        void setInteractiveStep(bool isInteractiveStep){m_isCurrentInteractiveStep = isInteractiveStep;}
        void setInteractiveStepFailed() { m_hasInteractiveStepFailed = true;}
        void changeState(ActivePlanState* state);

        void setStateSyncDone( bool bSyncDone = true );

	private:
		ActivePlan& operator=( const ActivePlan& );
		ActivePlan( const ActivePlan& );
		bool interlockOk( void );
		ActivePlan::ENextStepExecuteMethod executeNextStepImpl( void );
		void customiseStepImpl( const TA_Base_Core::StepCustomisableDetail& newDetail );
		void reassignImpl( const std::string& sessionId, const std::string& pmId );
        void transferControl(void);
		std::string dumpContext( void );
		TA_Base_Core::ActivePlanId m_plan;
		ActivePlanState* m_state;
		bool m_isActive;
		std::string m_remark;
		std::auto_ptr<ActiveStepList> m_steps;
		ActiveStepListIterator m_stepIterator;
		bool m_terminatedInterlockIsSet;
		bool m_completedInterlockIsSet;
		bool m_operatorResponseInterlockIsSet;
		bool m_hasPlanCompleted;

        bool m_stepFailed;
        std::string m_failedStepAlarmId;

		TA_Base_Core::NodeId m_terminatedInterlock;
		TA_Base_Core::ActivePlanId m_completedInterlock;

		//TA_Base_Core::ActivePlanId m_operatorResponseInterlock;
		const ACE_Time_Value m_resultTimeout;

		// TODO: This will be replaced by identical funcitonality
		//       in the ActivePlanBulletinBoard.
		//no need for a whole list as we only need the last step executed
		TA_Base_Core::StepNumber m_lastExecutedStep;

        //Member variables for taking control
        //In execution mode, Take control can only be effective when current step is complete or 
        // failed
        std::string m_newSessionId;
        std::string m_newPmId;
        bool m_isTakeControlPending;
        bool m_isPausePending;
        bool m_isCurrentInteractiveStep;
        bool m_hasInteractiveStepFailed;

        // Boolen member to indicate whether processing of state synchronization message done
        bool m_bStateSyncDone;

	}; // ActivePlan
}; // TA_Base_App



#endif // ACTIVEPLAN_H_INCLUDED

