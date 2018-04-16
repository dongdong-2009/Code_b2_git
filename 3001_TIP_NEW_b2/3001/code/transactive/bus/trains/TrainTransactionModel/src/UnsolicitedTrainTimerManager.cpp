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


#include "bus/trains/TrainTransactionModel/src/UnsolicitedTrainTimerManager.h"
#include "bus/trains/TrainTransactionModel/src/IUnsolicitedTrainTimerCallback.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    /**
     * Storage for associating a timer with a train
     * @author adamr
     * @version 1.0
     * @created 30-Jan-2008 2:54:06 PM
     */
    struct TimerItem
    {
        /**
         * Train the timer is for
         */
        CommonTypes::TrainIdType trainId;


        /**
         * Scheduled timer ID
         */
        long timerId;


        /**
         * Arbitrary description of this timer
         */
        std::string description;


        /**
         * flag to indicate if this timer should be logged
         */
        bool logTimerOn;
    };



    UnsolicitedTrainTimerManager::UnsolicitedTrainTimerManager( IUnsolicitedTrainTimerCallback& callback )
        : m_timerUtil( TA_Base_Core::SingletonTimerUtil::getInstance() ),
          m_callback( callback )
    {
        FUNCTION_ENTRY( "UnsolicitedTrainTimerManager" );
        FUNCTION_EXIT;
    }


    UnsolicitedTrainTimerManager::~UnsolicitedTrainTimerManager()
    {
        FUNCTION_ENTRY( "~UnsolicitedTrainTimerManager" );
                
        // acquire the map lock
        TA_THREADGUARD( m_mapLock );

        // clear the timer map
        m_timerIdentifierMap.clear();

        // Stop all timers this object has started
        std::vector<void*> userDataItems = m_timerUtil.stopPeriodicTimeOutClock( this );

        // delete all userdata items returned
        for ( std::vector<void*>::iterator iter = userDataItems.begin();
              iter != userDataItems.end(); ++iter )
        {
            TimerItem* item = reinterpret_cast< TimerItem* > ( *iter );
            delete item;
			item = NULL;
        }

        // remove the instance of m_timerUtil
        TA_Base_Core::SingletonTimerUtil::removeInstance();

        FUNCTION_EXIT;
    }


    void UnsolicitedTrainTimerManager::startTimer( CommonTypes::TrainIdType trainId,
                                                   unsigned long timeoutMillis,
                                                   bool logTimerOn,
                                                   std::string description )
    {
        // acquire the map lock
        TA_THREADGUARD( m_mapLock );

        TimerItem* userDefinedData = NULL;
        bool rescheduled = false;
        bool logDetails = logTimerOn;

        // find the existing timer if any
        TrainTimerMap::iterator iter = m_timerIdentifierMap.find( trainId );

        // existing timer found, it must be cancelled
        if ( iter != m_timerIdentifierMap.end() )
        {
            // cancel the timer and store the user defined data
            void* arg = m_timerUtil.stopPeriodicTimeOutClock( this, iter->second );

            // cast to the user defined data
            userDefinedData = reinterpret_cast<TimerItem*>( arg );

            // check whether the cancelling of this timer should be logged
            logDetails |= userDefinedData->logTimerOn;

            rescheduled = true;
        }
        else
        {
            // no existing timer found, create a new timer item
            userDefinedData = new TimerItem();
        }

        TA_ASSERT( userDefinedData != NULL, "User defined data must exist at this point" );

        userDefinedData->trainId = trainId;
        userDefinedData->description = description;
        userDefinedData->logTimerOn = logTimerOn;


        long scheduleId =  // <- This is the ID you use to cancel the timer
            m_timerUtil.startPeriodicTimeOutClock( this, timeoutMillis, true,
                                                   reinterpret_cast<void*>( userDefinedData ) );

        // store the timer ID in the user defined data
        userDefinedData->timerId = scheduleId;

        // add it to the timer map as well
        m_timerIdentifierMap[ trainId ] = scheduleId;

        // Log the timer if required
        if ( true == logDetails )
        {
            if ( true == rescheduled )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                             "UnsolicitedTrainTimer %s rescheduled for train %d.",
                             userDefinedData->description.c_str(), userDefinedData->trainId );
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                             "UnsolicitedTrainTimer %s started for train %d.",
                             userDefinedData->description.c_str(), userDefinedData->trainId );
            }
        }
    }


    void UnsolicitedTrainTimerManager::endTimer( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {

        TA_THREADGUARD( m_mapLock );

        // find the existing timer if any
        TrainTimerMap::iterator iter = m_timerIdentifierMap.find( trainId );

        // existing timer found, it must be cancelled
        if ( iter != m_timerIdentifierMap.end() )
        {
            long timerId = iter->second;
            m_timerIdentifierMap.erase( iter );

            // cancel the timer and store the user defined data
            void* arg = m_timerUtil.stopPeriodicTimeOutClock( this, timerId );

            // cast to the user defined data
            TimerItem* userDefinedData = reinterpret_cast<TimerItem*>( arg );

            if ( NULL != userDefinedData )
            {
                // log the timer if required
                if ( true == userDefinedData->logTimerOn )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                 "UnsolicitedTrainTimer %s ended for train %d.",
                                 userDefinedData->description.c_str(), userDefinedData->trainId );
                }

                // delete the user defined data
                delete userDefinedData;
                userDefinedData = NULL;
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                             "UnsolicitedTrainTimer ended for train %d. Timer item is NULL",
                             trainId );
            }
        }
    }


    void UnsolicitedTrainTimerManager::threadedTimerExpired( long timerId, void* userData )
    {
        if ( NULL == userData )
        {
            return;
        }

        CommonTypes::TrainIdType trainId = 0;

        {
            TA_THREADGUARD( m_mapLock );

            // cast to the user defined data
            TimerItem* userDefinedData = reinterpret_cast<TimerItem*>( userData );

            if ( timerId != userDefinedData->timerId )
            {
                // an old timer wasnt cancelled in ACE properly
                return;
            }

            trainId = userDefinedData->trainId;

            // find the existing timer if any
            TrainTimerMap::iterator iter = m_timerIdentifierMap.find( trainId );

            // matching existing timer found?

            if ( ( iter != m_timerIdentifierMap.end() ) &&
                 ( iter->second == timerId )  )
            {
                m_timerIdentifierMap.erase( iter );

                // log the timer if required
                if ( true == userDefinedData->logTimerOn )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                 "UnsolicitedTrainTimer %s expired for train %d.",
                                 userDefinedData->description.c_str(), trainId );
                }

                // delete the user defined data
                delete userDefinedData;
                userDefinedData = NULL;
            }
            else
            {
                // an old timer wasnt cancelled in ACE properly
                return;
            }
        }

        m_callback.handleTrainTimeout( trainId );
    }

}