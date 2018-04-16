/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/FutureManager.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef FUTURE_MANAGER_H_INCLUDED
#define FUTURE_MANAGER_H_INCLUDED


#include <string>
#include <vector>
#include "ace/Task.h"
#include "ace/Future.h"
#include "ace/Time_Value.h"

#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchFunction.h"
#include "app/radio/radio_mft_agent_icdv4/src/FutureResult.h"
#include "app/radio/radio_mft_agent_icdv4/src/TimeoutCallback.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"



class RadioFuture
{
	public:

		ACE_Future<FutureResultT> m_future;
};


class FutureManager   : public virtual ACE_Task_Base
{
	public:

		FutureManager();

		virtual ~FutureManager();

		virtual int  svc (void);

		/*
		 * Method used to dispatch thread in scheduling timers 
		 * for handling timeouts(refer to Timeout Callback) for this CDI Command
		 */
		void attachFuture(ACE_Future<FutureResultT>& aceFuture);

		/*
		 * Method used to cancel the TimeoutCallback
		 * and set the Future's TransactionState as aborted
		 */
		void detachFuture(ACE_Future<FutureResultT>& future);

		/*
		 * Method used to cancel the TimeoutCallback
		 * and set the Future's TransactionState as aborted
		 */
		void updateFuture(ACE_Future<FutureResultT>& future);

		/*
		 * Method used to remove the ACE_Future when transaction is
		 * completed or aborted
		 */		
		void removeFuture(ACE_Future<FutureResultT>& future, bool bTimeout);

		/*
		 * Method used to retrieve the ACE_Future 
		 * given the commandID
		 */		
		ACE_Future<FutureResultT> getFutureResult(CommandID id);
        ACE_Future<FutureResultT> getFutureResultForResource(CommandID id, unsigned long resourceId);
        ACE_Future<FutureResultT> getFutureResultForChannel(CommandID id, unsigned long channel);

		/*
		 * Used for debugging content of future list
		 */		
        std::string getFutureList();

	private:

		/*
		 * Method used to get timeout value from configuration params
		 */	
		void getTimeoutValue();

		/*
		 * Method to remove the m_FutureResults & stop m_timeoutCallbacks
		 */	
		void cleanup();


	private:

		/*
		 * Collection of ACE_Future<FutureResult_T> contained in
		 * RadioFuture class
		 */		
		std::vector< RadioFuture > m_FutureResults;

		/*
		 * Collection of TimeoutCallbacks for memory management
		 */	
		std::vector< TimeoutCallback* > m_timeoutCallbacks;

		//provides a thread lock
		TA_Base_Core::ReEntrantThreadLockable m_lock;
		
		int m_timeout;
        bool m_deactivate;

};

#endif 
