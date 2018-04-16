/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision: #3 $
  *
  * Last modification: $Date: 2012/06/19 $
  * Last modified by:  $Author: raghu.babu $
  *
  */

#include "app/trains/train_agent/src/TrainActivityMonitor.h"
#include "app/trains/train_agent/src/IActivityTimerHandler.h"

//#include "bus/managed_agent/src/AgentManager.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    TrainActivityMonitor::TrainActivityMonitor( unsigned long oa1Timeout,
                                                unsigned long atcTimeout,
                                                IActivityTimerHandler& callback )
            : m_oa1Timeout( oa1Timeout ),
              //m_atcTimeout( atcTimeout ),
              m_timerUtil( TA_Base_Core::SingletonTimerUtil::getInstance() ),
              m_callback( callback )
    {
        FUNCTION_ENTRY( "TrainActivityMonitor" );
        FUNCTION_EXIT;
    }


    TrainActivityMonitor::~TrainActivityMonitor()
    {
        FUNCTION_ENTRY( "~TrainActivityMonitor" );

        // Cancels all timers, and removes the timer utility reference
        cancelAllTimers();
        m_timerUtil.removeInstance();

        FUNCTION_EXIT;
    }


    void TrainActivityMonitor::addTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "addTrain" );

        // schedules both timers for the train
        scheduleTimer( trainId, TimerOa1, m_oa1Timers, m_oa1TimerLock, m_oa1Timeout, "OA1" );
        //scheduleTimer( trainId, TimerAtc, m_atcTimers, m_atcTimerLock, m_atcTimeout, "ATC" );

        FUNCTION_EXIT;
    }


    void TrainActivityMonitor::oa1Received( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "oa1Received" );

        scheduleTimer( trainId, TimerOa1, m_oa1Timers, m_oa1TimerLock, m_oa1Timeout, "OA1" );

        FUNCTION_EXIT;
    }


    void TrainActivityMonitor::atcReceived( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "atcReceived" );

        //scheduleTimer( trainId, TimerAtc, m_atcTimers, m_atcTimerLock, m_atcTimeout, "ATC" );

        FUNCTION_EXIT;
    }


    void TrainActivityMonitor::cancelAllTimers()
    {
        FUNCTION_ENTRY( "cancelAllTimers" );

        // acquires both locks
        TA_Base_Core::ThreadGuard hbGuard( m_oa1TimerLock );
        //TA_Base_Core::ThreadGuard tsGuard( m_atcTimerLock );

        // stop all timers
        m_timerUtil.stopPeriodicTimeOutClock( this );

        // delete and clears everything from the timer maps
        ActivityTimerMap::iterator iter;

        for ( iter = m_oa1Timers.begin();
              m_oa1Timers.end() != iter; ++iter )
        {
            delete iter->second;
			iter->second = NULL;
        }

        m_oa1Timers.clear();

       /* for ( iter = m_atcTimers.begin();
              m_atcTimers.end() != iter ; ++iter )
        {
            delete iter->second;
        }

        m_atcTimers.clear();*/

        FUNCTION_EXIT;
    }


    void TrainActivityMonitor::threadedTimerExpired( long timerId, void* userData )
    {
        FUNCTION_ENTRY( "threadedTimerExpired" );

        // cast user data into an ActivityTimer*, get the train id and timer type that expired
        ActivityTimer* activityTimer = reinterpret_cast< ActivityTimer* >( userData );

        // Checks the type of timer that expired

        switch ( activityTimer->type )
        {

            case TimerOa1:
            {
                bool expired = false;
                {
                    TA_Base_Core::ThreadGuard guard( m_oa1TimerLock );

                    // only reset in the case the id matches, it could have been rescheduled just before the lock
                    // also, if it was just rescheduled, it means theres no need to consider it as timed out

                    if ( activityTimer->timerId == timerId )
                    {
                        // timer expired, set to 0 for next timer schedule
                        activityTimer->timerId = 0;
                        expired = true;
                    }
                }

                // only notify the observer if the last scheduled timer expired
                // in case something failed to cancel, we could get old timers expiring

                if ( true == expired )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                 "OA1 Timer %d of Train %d expired",
                                 timerId, activityTimer->trainId );

                    // notify of expiration
                    m_callback.processOa1Timeout( activityTimer->trainId );
                }
            }

            break;

            /*case TimerAtc:
            {
                bool expired = false;
                {
                    TA_Base_Core::ThreadGuard guard( m_atcTimerLock );

                    // only reset in the case the id matches, it could have been rescheduled just before the lock
                    // also, if it was just rescheduled, it means theres no need to consider it as timed out

                    if ( activityTimer->timerId == timerId )
                    {
                        // timer expired, set to 0 for next timer schedule
                        activityTimer->timerId = 0;
                        expired = true;
                    }
                }

                // only notify the observer if the last scheduled timer expired
                // in case something failed to cancel, we could get old timers expiring

                if ( true == expired )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                 "ATC Timer %d of Train %d expired",
                                 timerId, activityTimer->trainId );

                    // notify of expiration
                    m_callback.processAtcTimeout( activityTimer->trainId );
                }
            }*/

            break;

            default:
                TA_ASSERT( false, "Invalid timer type expired" );
				// fall through
        }

        FUNCTION_EXIT;
    }


    void TrainActivityMonitor::scheduleTimer( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              ETimerType timerType,
                                              ActivityTimerMap& timerMap,
                                              TA_Base_Core::ReEntrantThreadLockable& mapLock,
                                              unsigned long timeout,
                                              const std::string& timerString )
    {
        FUNCTION_ENTRY( "scheduleTimer" );

        // lock the map
        TA_Base_Core::ThreadGuard guard( mapLock );

        // find an existing timer
        ActivityTimerMap::iterator findIter = timerMap.find( trainId );

        // pointer to the timer data
        ActivityTimer* timerItem = NULL;

        // if an existing timer was found

        if ( findIter != timerMap.end() )
        {
            // store a pointer to it
            timerItem = findIter->second;

            // check if there is a timer running

            if ( timerItem->timerId != 0 )
            {
                // if so, cancel the running timer
                m_timerUtil.stopPeriodicTimeOutClock( this, timerItem->timerId );
            }
        }
        else
        {
            // nothing was found, so make a new timer item
            timerItem = new ActivityTimer();
            timerItem->trainId = trainId;
            timerItem->type = timerType;
            timerMap.insert( ActivityTimerMap::value_type( trainId, timerItem ) );
        }

        // schedule the actual timer
        timerItem->timerId = m_timerUtil.startPeriodicTimeOutClock ( this, timeout, true, reinterpret_cast<void*>( timerItem ) );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "%s Timer %d of Train %d scheduled",
                     timerString.c_str(), timerItem->timerId, trainId );

        FUNCTION_EXIT;
    }

} // TA_IRS_App
