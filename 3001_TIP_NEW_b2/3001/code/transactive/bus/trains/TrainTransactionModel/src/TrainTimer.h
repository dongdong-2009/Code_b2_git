/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(TrainTimer_H)
#define TrainTimer_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/synchronisation/src/Semaphore.h"


// forward declarations
namespace TA_Base_Core
{
    class SingletonTimerUtil;
}


namespace TA_IRS_Bus
{
    /**
     * This is a timer object used for train operations.
     *
     * It can be started, waited on, signalled and stopped.
     * @author Adam Radics
     * @version 1.0
     * @created 22-Oct-2007 12:19:55 PM
     */

    class TrainTimer: public TA_Base_Core::ITimeoutCallback
    {

    public:


        /**
         * Creates and starts the timer
         *
         * @param waitTime    The timer value in milliseconds
         * @param timerUtil   Used to schedule the timer
         */
        TrainTimer( unsigned long waitTime, TA_Base_Core::SingletonTimerUtil& timerUtil );


        /**
         * Cleans up the timer
         */
        ~TrainTimer();


        /**
         * Handles the timer expiry
         *
         * @param timerId   The timer that expired
         * @param userData  The data scheduled with the timer
         */
        virtual void timerExpired( long timerId, void* userData );


        /**
         * Waits until the timer is signalled
         *
         * @exception TrainTimerException if the timer expires while waiting
         *                                (or in case it has expired before waiting)
         */
        void wait();


        /**
         * Signals the timer
         */
        void signal();


    private:

        TrainTimer();
        TrainTimer(const TrainTimer&);
        TrainTimer& operator=(const TrainTimer&);


        /**
         * The ID of the timer
         */
        long m_timer;


        /**
         * The semaphore to wait on
         */
        TA_Base_Core::Semaphore m_semaphore;


        /**
         * Whether the timer expired
         */
        bool m_expired;


        /**
         * Used to schedule the timer
         */
        TA_Base_Core::SingletonTimerUtil& m_timerUtil;

    };

}

#endif // !defined(TrainTimer_H)
