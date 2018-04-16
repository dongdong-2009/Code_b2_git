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

#include <time.h>
#include "bus/trains/TrainTransactionModel/src/TrainState.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{



    TrainState::TrainState( CommonTypes::TrainIdType trainId )
            : m_trainId( trainId ),
              m_trainStatusManager( NULL ),
              m_dataChanged( false ),
              m_synchronisationRequired( false ),
              m_timeStamp( 0 )
    {
        FUNCTION_ENTRY( "TrainState" );
        FUNCTION_EXIT;
    }


    TrainState::~TrainState()
    {
        FUNCTION_ENTRY( "~TrainState" );
        FUNCTION_EXIT;
    }


    CommonTypes::TrainIdType TrainState::getTrainId() const
    {
        FUNCTION_ENTRY( "getTrainId" );
        FUNCTION_EXIT;
        return m_trainId;
    }


    time_t TrainState::getTimestamp() const
    {
        FUNCTION_ENTRY( "getTimestamp" );
        FUNCTION_EXIT;
        return m_timeStamp;
    }


    TA_Base_Core::IThreadLockable& TrainState::getLock() const
    {
        FUNCTION_ENTRY( "getLock" );
        FUNCTION_EXIT;
        return m_lock;
    }


    bool TrainState::hasDataChanged() const
    {
        FUNCTION_ENTRY( "hasDataChanged" );
        FUNCTION_EXIT;
        return m_dataChanged;
    }


    bool TrainState::isSynchronisationRequired() const
    {
        FUNCTION_ENTRY( "isSynchronisationRequired" );
        FUNCTION_EXIT;
        return m_synchronisationRequired;
    }


    TrainStatusManager& TrainState::getTrainStatusManager() const
    {
        FUNCTION_ENTRY( "getTrainStatusManager" );

        FUNCTION_EXIT;
        return *m_trainStatusManager;
    }


    void TrainState::setTimestamp( time_t timestamp )
    {
        FUNCTION_ENTRY( "setTimestamp" );

        m_timeStamp = timestamp;

        FUNCTION_EXIT;
    }


    void TrainState::resetDataChanged()
    {
        FUNCTION_ENTRY( "resetDataChanged" );

        // sets m_dataChanged to false
        m_dataChanged = false;

        FUNCTION_EXIT;
    }


    void TrainState::setSynchronisationComplete()
    {
        FUNCTION_ENTRY( "setSynchronisationComplete" );

        m_synchronisationRequired = false;

        FUNCTION_EXIT;
    }


    void TrainState::setTrainStatusManager( TrainStatusManager& owner )
    {
        FUNCTION_ENTRY( "setTrainStatusManager" );

        m_trainStatusManager = &owner;

        FUNCTION_EXIT;
    }


    void TrainState::setDataChanged( bool synchronisationRequired )
    {
        FUNCTION_ENTRY( "setDataChanged" );

        m_dataChanged = true;

        // can only set the flag, not clear it
        if ( true == synchronisationRequired )
        {
            m_synchronisationRequired = true;
        }

        FUNCTION_EXIT;
    }


    void TrainState::setDataChangedWithTimestamp( time_t timestamp, bool synchronisationRequired )
    {
        FUNCTION_ENTRY( "setDataChangedWithTimestamp" );

        m_dataChanged = true;

        if ( 0 == timestamp )
        {
            time( &m_timeStamp );
        }
        else
        {
            m_timeStamp = timestamp;
        }

        if ( true == synchronisationRequired )
        {
            m_synchronisationRequired = true;
        }

        FUNCTION_EXIT;
    }

}
