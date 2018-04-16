/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision: #1 $
  *
  * Last modification: $Date: 2018/03/14 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(TrainActivityMonitor_H)
#define TrainActivityMonitor_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/timers/src/AbstractThreadedTimeoutUser.h"
#include "core/timers/src/SingletonTimerUtil.h"

// forward declarations
namespace TA_IRS_App
{
    class IActivityTimerHandler;
}

namespace TA_IRS_App
{

    /**
     * This manages OA1 and ATC timers for a train. It notifies the observer when one of these expires.
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:16 PM
     */
    class TrainActivityMonitor : public TA_Base_Core::AbstractThreadedTimeoutUser
    {

    public:

        /**
         * Constructor
         *
         * @param oa1Timeout    The timeout before a train's OA1 transmission is considered missing
         * @param atcTimeout    The timeout before a train's ATC transmission is considered missing
         * @param callback    The callback to notify of timer expiry
         */
        TrainActivityMonitor( unsigned long oa1Timeout,
                              unsigned long atcTimeout,
                              IActivityTimerHandler& callback );


        /**
         * Destructor
         */
        virtual ~TrainActivityMonitor();


        /**
         * A train has become valid within the system, its timers must be scheduled.
         *
         * @param trainId
         */
        void addTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * An OA1 table has been received from the train, reschedule the timer.
         *
         * @param trainId
         */
        void oa1Received( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * An ATC table has been received from the train, reschedule the timer.
         *
         * @param trainId
         */
        void atcReceived( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Cancels all scheduled timers for all trains
         */
        void cancelAllTimers();


        /**
         * Called when one of the timers expires
         *
         * @param timerId
         * @param userData
         */
        virtual void threadedTimerExpired( long timerId, void* userData );


    private:

        /**
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:48:16 PM
         */
        enum ETimerType
        {
            TimerOa1 = 0,
            TimerAtc = 1
        };

        /**
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:48:16 PM
         */

        struct ActivityTimer
        {

            /**
             * The train id
             */
            TA_IRS_Bus::CommonTypes::TrainIdType trainId;


            /**
             * OA1 or ATC Timer
             */
            ETimerType type;


            /**
             * The ID of the timer
             */
            long timerId;
        };

        /**
         * A map of train IDs to scheduled timers
         * @author adamr
         * @version 1.0
         * @created 01-Apr-2008 2:48:17 PM
         */
        typedef std::map< TA_IRS_Bus::CommonTypes::TrainIdType, ActivityTimer* > ActivityTimerMap;


        /**
         * Private default constructor
         */
        TrainActivityMonitor();


        /**
         * Schedules a timer, handles the generic internal logic
         *
         * @param trainId   the train the timer is for
         * @param timerType the type of the timer
         * @param timerMap  the map to insert the details into
         * @param mapLock   the lock to guard the map
         * @param timeout   the time to schedule
         * @param timerString   a string description of the timer type for logging
         */
        void scheduleTimer( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                            ETimerType timerType,
                            ActivityTimerMap& timerMap,
                            TA_Base_Core::ReEntrantThreadLockable& mapLock,
                            unsigned long timeout,
                            const std::string& timerString );


        /**
         * Timeout for OA1 messages before the train OA1 is considered timed out
         */
        unsigned long m_oa1Timeout;


        /**
         * Timeout for ATC messages before the train ATC is considered timed out
         */
        unsigned long m_atcTimeout;


        /**
         * used to cancel schedule timers
         */
        TA_Base_Core::SingletonTimerUtil& m_timerUtil;


        /**
         * This is the callback to notify when a timer expires
         */
        IActivityTimerHandler& m_callback;


        /**
         * A map of train IDs to scheduled OA1 timers.
         */
        ActivityTimerMap m_oa1Timers;


        /**
         * A map of train IDs to scheduled ATC timers.
         */
        ActivityTimerMap m_atcTimers;


        /**
         * locks for the timer maps
         */
        TA_Base_Core::ReEntrantThreadLockable m_oa1TimerLock;
        TA_Base_Core::ReEntrantThreadLockable m_atcTimerLock;

    };

} // TA_IRS_App

#endif // !defined(TrainActivityMonitor_H)
