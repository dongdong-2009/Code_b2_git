/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/trains/train_agent/src/TrainTimer.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

///////////////////////////////////////////////////////////
//  TrainTimer.h
//  Implementation of the Class TrainTimer
//  Created on:      28-Nov-2003 02:27:56 PM
///////////////////////////////////////////////////////////

#if !defined(TrainTimer_D0CA87D8_E1AB_48b9_97DB_755D2B238DDA__INCLUDED_)
#define TrainTimer_D0CA87D8_E1AB_48b9_97DB_755D2B238DDA__INCLUDED_

#include <string>

#include "app/trains/train_agent/TrainProtocolLibrary/src/TrainEvent.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"

namespace TA_Base_Core
{
	class Semaphore;
}

namespace TA_IRS_App
{
    class TrainTransactionManager;

    /**
    * This class manages an overall timer on a sequence of events/messages between a train
    * and TrainAgent. There can be only one timer active per train at a time. The class
    * also manages any TrainTransactionManager activites; terminating if the overall
    * timer expires and Transactions are still in progress. 
    *
    * @note This code is based heavily on TrainTransactionManager, but without the 
    *       TrainEvent specifics.
    */
    class TrainTimer : public TA_Base_Core::Thread 
	{

    public:
			struct TrainTimerEntry
			{
				unsigned int        trainID;
				TA_Base_Core::Semaphore* waitSemaphore;
				unsigned int        timeTick;  // Counter used by timeout mechanism: set to number of ticks, on zero timer expires
				std::string description; // Arbitrary description of this timer
                ~TrainTimerEntry();
            };

		public:

			TrainTimer(TrainTransactionManager & trainTransactionManager);
			virtual ~TrainTimer();

            void startTimer(unsigned int trainID, unsigned int millis, std::string description = "");

            /**
            * Will block caller, waiting for the timer to expire, or be interupted.
            * @exception TrainTimerTimeoutException if timer expires.
            * @return    true (always) to indicate timer was interupted prematurely
            * @return    false (error: cannot occur) - indicates train missing from list
            */
            bool waitTimer(unsigned int trainID); // waitForTransaction()

            /** 
            * Release the timer for this train
            * @todo Auto-clean during waitTimer() as its no longer of any use?
            */
            void endTimer(unsigned int trainID);

            /**
            * Interupt the timer
            */
            bool signalTimer(unsigned int trainID);

        private:

			TrainTimer(const TrainTimer& theTrainTimer);

            // TA_Base_Core::Thread implementations
            void run();
			void terminate();
            
            /** helper method to calculate ticks from requested number of milliseconds 
            * @param millisec - time in milliseconds 
            * @returns ticks based on internal timer resolution
            */
            unsigned int calcTicks(unsigned int millisec) const;

            const TrainTransactionManager & m_trainTransactionManager;
			TA_Base_Core::NonReEntrantThreadLockable	       m_waitListLock;
			std::vector<TrainTimerEntry>   m_waitList;

            // this list contains a list of train Ids that have been signalled before a timer was started
            std::vector<unsigned int>      m_signalledList;
			
            bool                           m_doTerminate;

            static const unsigned int TIMEOUT_POLL_PERIOD; // milliseconds
            static const unsigned int NEW_TRANSACTION_RETRY_PERIOD; // milliseconds
	};
};
#endif // !defined(TrainTimer_D0CA87D8_E1AB_48b9_97DB_755D2B238DDA__INCLUDED_)
