/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TrainTransactionManager.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class manages a list of clients that are awaiting a response from a train.
  * This manager will ensure that only one message is sent to the train at a time
  * and will wait for a reply. 
  *
  */

///////////////////////////////////////////////////////////
//  TrainTransactionManager.h
//  Implementation of the Class TrainTransactionManager
//  Created on:      28-Nov-2003 02:27:56 PM
///////////////////////////////////////////////////////////

#if !defined(TrainTransactionManager_D0CA87D8_E1AB_48b9_97DB_755D2B238DDA__INCLUDED_)
#define TrainTransactionManager_D0CA87D8_E1AB_48b9_97DB_755D2B238DDA__INCLUDED_

#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "app/trains/train_agent/src/TrainTypes.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"

/**
 * This class manages a list of clients that are awaiting a response from a train.
 * This manager will ensure that only one message is sent to the train at a time
 * and will wait for a reply. 
 */
namespace TA_Base_Core
{
	class Semaphore;
}

namespace TA_IRS_App
{
	typedef std::auto_ptr<TrainEvent> TrainEventPtr;

	class TrainTransactionManager : public TA_Base_Core::Thread 
	{
		public:
			struct TrainWaitEntry
			{
				unsigned int        trainID;
				TA_Base_Core::Semaphore* waitSemaphore;
				TrainEventPtr       trainEvent;
				std::vector<int>	replyEvent; /// from TrainProtocol.h
				unsigned int        timeTick;  /// Counter used by timeout mechanism: set to number of ticks, on zero timer expires
			};

		public:

			TrainTransactionManager();
			virtual ~TrainTransactionManager();

			void setWaitTimeout(unsigned int timeMillisecs);

			bool onTrainEvent(TrainEvent& trainEvent);
			void newTransaction(unsigned int trainID, const std::vector<int> & replyEvent, unsigned int millis = 0);
			TrainEventPtr waitForTransaction(unsigned int trainID);
			void removeTransaction(unsigned int trainID);
			bool abortTransaction(TrainID trainID); /// To abort an in-progress transaction via another thread
			bool hasTrainsactions(TrainID trainID);

		private:

			TrainTransactionManager(const TrainTransactionManager& theTrainTransactionManager);

			// TIP5-PROTO - to enable pending transaction to be aborted without passing a valid event. 
			bool signalTransaction(unsigned int trainID, TrainEvent& event);
			void run();
			void terminate();
            
            /** helper method to calculate ticks from requested number of milliseconds 
            * @param millisec - time in milliseconds 
            * @returns ticks based on internal timer resolution
            */
            unsigned int calcTicks(unsigned int millisec) const;

			TA_Base_Core::NonReEntrantThreadLockable	       m_waitListLock;
			std::vector<TrainWaitEntry*>    m_waitList;
			bool                           m_doTerminate;
			unsigned int                   m_waitTimeout;

	};
};
#endif // !defined(TrainTransactionManager_D0CA87D8_E1AB_48b9_97DB_755D2B238DDA__INCLUDED_)
