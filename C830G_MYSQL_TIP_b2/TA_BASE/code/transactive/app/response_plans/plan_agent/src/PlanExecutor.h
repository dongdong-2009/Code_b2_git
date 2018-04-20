#ifndef PLANEXECUTOR_H_INCLUDED
#define PLANEXECUTOR_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanExecutor.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

namespace TA_Base_App
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "PlanExecutor" and that "PlanExecutorImpl" can
	// not be used.
	class PlanExecutorImpl
	{
	// Public methods
	public:

		friend class ACE_Singleton<PlanExecutorImpl, ACE_Recursive_Thread_Mutex>;

        // Add a parameter to indicate Plan is started in Monitor mode
		void run( const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, const bool manExecution = false, const bool bStartFromMonitorMode = false );
		void customise( const std::string& session, const std::string& pmId, TA_Base_Core::ActivePlanId& plan, const bool bStartFromMonitorMode = false );
        void spawnPending(const std::string& session, const std::string& pmId, const TA_Base_Core::ActivePlanId& parentPlan, TA_Base_Core::ActivePlanId& childPlan, const bool bStartFromMonitorMode = false );
        void stop( const TA_Base_Core::ActivePlanId& plan, std::string& reason );
		void complete( const TA_Base_Core::ActivePlanId& plan );
		void pause( const TA_Base_Core::ActivePlanId& plan );
		void pauseForOperator( const TA_Base_Core::ActivePlanId& plan );
		void resume( const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepNumber resumeStepPosition = -1 );
        void reassign( const std::string& session, const std::string& planManagerId, const TA_Base_Core::ActivePlanId& plan );
		void unassign( const TA_Base_Core::ActivePlanId& plan );
		void customiseStep( const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail );
        void processDecisionStepResponse(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status);
        void processPromptStepResponse(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status);
        void processLogStepResponse(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& logMessage);
        void processRunPlanStepResult(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error);
        void processLaunchGuiAppStepResult(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error);
        void processLaunchSchematicStepResult(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error);
		//lizettejl++ (DP-Changes)
		void processFailureSummaryStepResult(const TA_Base_Core::ActivePlanId& plan, TA_Base_Core::EPlanStepCompletionStatus status, const std::string& error);
		//++lizettejl
		void updateActivePlanDetail( TA_Base_Core::ActivePlanDetail& detail );
        void processCustomisationHistoryUpdate( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate );

		/**
		  * waitForRunningPlans
		  *
		  * Allows the Plan Agent to end plans before all the underlying
		  * services are shut down by the Generic Agent.
		  */
		void waitForRunningPlans( void );

        void clearAllPlans();
        void signalAllPlansIntoControl();

	private:
		PlanExecutorImpl();
		~PlanExecutorImpl();
		PlanExecutorImpl( const PlanExecutorImpl& otherPlanExecutor );
		PlanExecutorImpl& operator=( const PlanExecutorImpl& otherPlanExecutor );

        void sendMessage( const TA_Base_Core::ActivePlanId& plan, IActivePlanMessage* controlMessage );

		std::auto_ptr<PlanExecutionEngine> m_executionEngine;

	}; // PlanExecutorImpl
	typedef ACE_Singleton<PlanExecutorImpl, ACE_Recursive_Thread_Mutex> PlanExecutor;
} // TA_Base_App

#endif //PLANEXECUTOR_H_INCLUDED
