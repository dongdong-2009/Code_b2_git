#ifndef PLANEXECUTOR_H_INCLUDED
#define PLANEXECUTOR_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanExecutor.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #11 $
  *
  *  Last modification: $DateTime: 2014/11/04 13:46:22 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

#include <string>

#include "ace/Singleton.h"

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_Core
{
    struct ActivePlanId;
    struct StepCustomisableDetail;
    struct CustomisationHistoryUpdate;
    struct ExecutionHistoryUpdate;
    struct CurrentPlanStepDetail;
    struct ActivePlanDetail;
    struct ActivePlanSharedData;
}

namespace TA_Base_App
{
	// Class declaration
	// Note that a typedef following the class declaration declares the
	// type "PlanExecutor" and that "PlanExecutorImpl" can
	// not be used.
    class IActivePlanMessage;
    class PlanExecutionEngine;

	class PlanExecutorImpl
	{
	// Public methods
	public:

		friend class ACE_Singleton<PlanExecutorImpl, ACE_Recursive_Thread_Mutex>;

		void run( const std::string& session, TA_Base_Core::ActivePlanId& plan, const unsigned long entity, const bool bPending = false, const bool manExecution = false, const bool bStartFromMonitorMode = false );
        void spawnPending(const std::string& session, const TA_Base_Core::ActivePlanId& parentPlan, TA_Base_Core::ActivePlanId& childPlan, const bool bStartFromMonitorMode = false );
        void stop( const TA_Base_Core::ActivePlanId& plan, const std::string& reason );
		void complete( const TA_Base_Core::ActivePlanId& plan );
		void pause( const TA_Base_Core::ActivePlanId& plan );
		void resume( const TA_Base_Core::ActivePlanId& plan );
        void reassign( const std::string& session, const TA_Base_Core::ActivePlanId& plan );
		void unassign( const TA_Base_Core::ActivePlanId& plan );
        void customiseStep( const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::StepCustomisableDetail& detail );
        void executeStep( const TA_Base_Core::ActivePlanId& plan, const TA_Base_Core::ActiveStepDetail& detail );
        void updateActivePlanDetail( TA_Base_Core::ActivePlanDetail& detail );
        void processPlanSharedDataUpdate( TA_Base_Core::ActivePlanSharedData& detail );
        void processCustomisationHistoryUpdate( const TA_Base_Core::CustomisationHistoryUpdate& historyUpdate );
        void processExecutionHistoryUpdate( const TA_Base_Core::ExecutionHistoryUpdate& historyUpdate );
        void processStepResponse(const int eType, const TA_Base_Core::ActivePlanId& plan, const unsigned int unStepId, const std::vector<std::string>& parameters);
		/**
		  * waitForRunningPlans
		  *
		  * Allows the Plan Agent to end plans before all the underlying
		  * services are shut down by the Generic Agent.
		  */
		void waitForRunningPlans( void );

        void changedToMonitor();
        void changedToControl();

        void sendMessage( const TA_Base_Core::ActivePlanId& plan, IActivePlanMessage* controlMessage );

	private:
		PlanExecutorImpl();
		~PlanExecutorImpl();
		PlanExecutorImpl( const PlanExecutorImpl& otherPlanExecutor );
		PlanExecutorImpl& operator=( const PlanExecutorImpl& otherPlanExecutor );

		PlanExecutionEngine* m_executionEngine;

	}; // PlanExecutorImpl
	typedef ACE_Singleton<PlanExecutorImpl, ACE_Recursive_Thread_Mutex> PlanExecutor;
} // TA_Base_App

#endif //PLANEXECUTOR_H_INCLUDED
