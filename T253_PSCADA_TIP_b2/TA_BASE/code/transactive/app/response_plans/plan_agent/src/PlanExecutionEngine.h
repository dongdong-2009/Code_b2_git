#ifndef PLANEXECUTIONENGINE_H_INCLUDED
#define PLANEXECUTIONENGINE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanExecutionEngine.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2014/11/04 13:46:22 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Provides an interface to control the execution of 
  *  plans.
  */

#include <string>
#include <map>

#include "ace/Task.h"
#include "ace/Future.h"
#include "ace/Mutex.h"
#include "ace/RW_Mutex.h"
#include "ace/Activation_Queue.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
    struct ActivePlanId;
}

namespace TA_Base_App
{
    class ActivePlanMessageQueue;
    class SortablePlanId;
    class ActivePlan;

	//Class declaration
	class PlanExecutionEngine : public ACE_Task<ACE_MT_SYNCH>
	{
	// Public methods
	public:
		PlanExecutionEngine();
		virtual ~PlanExecutionEngine();

		virtual int open( void* );
		virtual int close( u_long flags = 0 );

		/**
		  * svc
		  *
		  * This is the entry point for the callback from ACE_Task.
		  *
		  */
		virtual int svc( void );

		/**
		  * run
		  *
		  * Asynchronous interface for running plans.
		  *
		  * @return ACE_Future<int> can be used to block or ignored.
		  *
		  */
        void run( const std::string& session, TA_Base_Core::ActivePlanId& plan, const unsigned long entity,
            const bool bPending, const bool manExecution, const bool bStartFromMonitorMode );

		/**
		  * spawn
		  *
		  * Asynchronous interface for running plans from other plans.
		  *
		  */
		void spawn( const std::string& session, const TA_Base_Core::ActivePlanId& parentPlan, 
            TA_Base_Core::ActivePlanId& childPlan, const bool bPending, const bool bStartFromMonitorMode );
	
		/**
		  * runImpl
		  *
		  * Implementation of the async run method. This
		  * should only be called by a "Method Object".
		  *
		  */
		void runImpl( std::auto_ptr<ActivePlan> activePlan );

		/**
		  * put
		  *
		  * Should be used by all callers for putting messages on
		  * the tasks queue. This method will multiplex messages
		  * out to the appropriate plans.
		  *
		  */
		virtual int put(ACE_Message_Block *mb, ACE_Time_Value *tv = 0);

        void signalAgentModeChanged( const bool bControl );
        void signalHangUpEvent();

        void terminateAndWait();

        void publishPlanStart( const std::string& session, const TA_Base_Core::ActivePlanId& plan, const bool bPending, const bool manExecution );

	private:

		PlanExecutionEngine& operator=( const PlanExecutionEngine& );
		PlanExecutionEngine( const PlanExecutionEngine& );

		void processMessages( ActivePlan& activePlan, boost::shared_ptr<ActivePlanMessageQueue> mq );

        boost::shared_ptr<ActivePlanMessageQueue> getMq( const TA_Base_Core::ActivePlanId& plan ) const;
        void makeMq( const TA_Base_Core::ActivePlanId& plan );
        void dropMq( const TA_Base_Core::ActivePlanId& plan );

		ACE_Activation_Queue m_activationQueue;

		typedef std::map< SortablePlanId, boost::shared_ptr<ActivePlanMessageQueue> > PlanMqMap;
		typedef PlanMqMap::iterator PlanMqMapIterator;
		typedef PlanMqMap::const_iterator ConstPlanMqMapIterator;

		PlanMqMap m_mqMap;
		mutable ACE_RW_Mutex m_mqMapLock;
		mutable ACE_Mutex    m_consolidateMqLock;

	// Protected attributes (deprecated)
	}; // PlanAgentImpl
}; // TA_Base_App

#endif // PLANEXECUTIONENGINE_H_INCLUDED
