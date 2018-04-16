/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(UnsolicitedTrainTimerManager_H)
#define UnsolicitedTrainTimerManager_H



#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/timers/src/AbstractThreadedTimeoutUser.h"

#include <string>
#include <map>

// forward declarations
namespace TA_Base_Core
{
    class SingletonTimerUtil;
}


namespace TA_IRS_Bus
{
    class IUnsolicitedTrainTimerCallback;
}


namespace TA_IRS_Bus
{


    /**
     * @author adamr
     * @version 1.0
     * @created 30-Jan-2008 2:54:06 PM
     */

    class UnsolicitedTrainTimerManager : public TA_Base_Core::AbstractThreadedTimeoutUser
    {

    public:

        /**
         * Constructor
         *
         * @param callback the callback to notify of expiry
         */
        UnsolicitedTrainTimerManager( IUnsolicitedTrainTimerCallback& callback );


        /** 
         * Destructor
         */
        virtual ~UnsolicitedTrainTimerManager();


        /**
         * Starts (or resets) the timer for a train and provide an optional description for the timer
         *
         * @param trainId    The train the timer is for
         * @param timeoutMillis    How many milliseconds until the timer expires
         * @param logTimerOn    Whether to log timer schedule, cancel, and expiration
         * @param description    An arbitrary string that describes this timer
         */
        void startTimer( CommonTypes::TrainIdType trainId,
                         unsigned long timeoutMillis,
                         bool logTimerOn = false,
                         std::string description = "" );


        /**
         * Ends a timer for a train
         *
         * @param trainId    the train to end the timer for
         */
        void endTimer( CommonTypes::TrainIdType trainId );


        /**
         * Handle the timer expiration and notify the callback if it is a current timer
         *
         * @param timerId the timer that expired
         * @param userData the data scheduled with the timer
         */
        virtual void threadedTimerExpired( long timerId, void* userData );


    private:

        UnsolicitedTrainTimerManager();

        /**
         * @author adamr
         * @version 1.0
         * @created 30-Jan-2008 2:54:06 PM
         */
        typedef std::map<CommonTypes::TrainIdType, long> TrainTimerMap;


        /**
         * Used to schedule and cancel timers
         */
        TA_Base_Core::SingletonTimerUtil& m_timerUtil;


        /**
         * Callback for timeout events
         */
        IUnsolicitedTrainTimerCallback& m_callback;


        /**
         * Used to lock the timer map
         */
        TA_Base_Core::ReEntrantThreadLockable m_mapLock;


        /**
         * Used to associate train IDs with timer IDs
         */
        TrainTimerMap m_timerIdentifierMap;

    };
}

#endif // !defined(UnsolicitedTrainTimerManager_H)
