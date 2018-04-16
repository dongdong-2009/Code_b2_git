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


#include "bus/trains/TrainTransactionModel/src/TrainTimer.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimerException.h"

#include "core/timers/src/SingletonTimerUtil.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    TrainTimer::TrainTimer( unsigned long waitTime,
                            TA_Base_Core::SingletonTimerUtil& timerUtil )
          : m_timer( 0 ),
            m_semaphore( 0 ),
            m_expired( false ),
            m_timerUtil( timerUtil )
    {
        FUNCTION_ENTRY( "TrainTimer" );

		m_timerUtil.startPeriodicTimeOutClock( this, waitTime, true );
        
        FUNCTION_EXIT;
    }


    TrainTimer::~TrainTimer()
    {
        FUNCTION_ENTRY( "~TrainTimer" );

        // cancels the timer
        m_timerUtil.stopPeriodicTimeOutClock( this );

        // Also signals the semaphore before exiting just
        // in case something is blocked on it.
        m_semaphore.post();

        FUNCTION_EXIT;
    }


    void TrainTimer::timerExpired( long timerId, void* userData )
    {
        FUNCTION_ENTRY( "timerExpired" );

        // sets m_expired to true
        m_expired = true;

        // signals the semaphore
        m_semaphore.post();

        // sets the timer ID to 0
        m_timer = 0;

        FUNCTION_EXIT;
    }


    void TrainTimer::wait()
    {
        FUNCTION_ENTRY( "wait" );

        // waits on the semaphore
        m_semaphore.wait();

        // When signaled (or if the semaphore was signaled previously)
        // check m_expired - if it is true, throw an exception

        if ( true == m_expired )
        {
            TA_THROW( TrainTimerException( "TrainTimer has expired" ) );
        }

        FUNCTION_EXIT;
    }


    void TrainTimer::signal()
    {
        FUNCTION_ENTRY( "signal" );

        // make sure the timer doesnt expire
        m_timerUtil.stopPeriodicTimeOutClock( this );

        // signals the semaphore
        m_semaphore.post();

        // sets the timer ID to 0
        m_timer = 0;

        FUNCTION_EXIT;
    }
}
