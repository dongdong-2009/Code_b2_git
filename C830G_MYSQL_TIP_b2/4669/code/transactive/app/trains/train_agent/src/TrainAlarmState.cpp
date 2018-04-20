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

#include "app/trains/train_agent/src/TrainAlarmState.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    TrainAlarmState::TrainAlarmState( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
            : TA_IRS_Bus::TrainState( trainId ),
              m_oa1AlarmId( "" ),
              m_atcAlarmId( "" ),
              m_radioModeAlarmId( "" ),
              m_cctvPreConflictAlarmId( "" )
    {
        FUNCTION_ENTRY( "TrainAlarmState" );
        FUNCTION_EXIT;
    }


    TrainAlarmState::~TrainAlarmState()
    {
        FUNCTION_ENTRY( "~TrainAlarmState" );
        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CommonTypes::ETrainResource TrainAlarmState::getResourceCategory() const
    {
        FUNCTION_ENTRY( "getResourceCategory" );
        FUNCTION_EXIT;
        return TA_IRS_Bus::CommonTypes::TrainAlarmResource;
    }


    const std::string& TrainAlarmState::getOa1AlarmId() const
    {
        FUNCTION_ENTRY( "getOa1AlarmId" );
        FUNCTION_EXIT;
        return m_oa1AlarmId;
    }


    const std::string& TrainAlarmState::getAtcAlarmId() const
    {
        FUNCTION_ENTRY( "getAtcAlarmId" );
        FUNCTION_EXIT;
        return m_atcAlarmId;
    }


    const std::string& TrainAlarmState::getCctvPreConflictAlarmId() const
    {
        FUNCTION_ENTRY( "getCctvPreConflictAlarmId" );
        FUNCTION_EXIT;
        return m_cctvPreConflictAlarmId;
    }


    const std::string& TrainAlarmState::getRadioModeAlarmId() const
    {
        FUNCTION_ENTRY( "getRadioModeAlarmId" );
        FUNCTION_EXIT;
        return m_radioModeAlarmId;
    }


    void TrainAlarmState::setOa1AlarmId( const std::string& alarmId )
    {
        FUNCTION_ENTRY( "setOa1AlarmId" );

        if ( alarmId != m_oa1AlarmId )
        {
            m_oa1AlarmId = alarmId;
            setDataChangedWithTimestamp();
        }

        FUNCTION_EXIT;
    }


    void TrainAlarmState::setAtcAlarmId( const std::string& alarmId )
    {
        FUNCTION_ENTRY( "setAtcAlarmId" );

        if ( alarmId != m_atcAlarmId )
        {
            m_atcAlarmId = alarmId;
            setDataChangedWithTimestamp();
        }

        FUNCTION_EXIT;
    }


    void TrainAlarmState::setCctvPreConflictAlarmId( const std::string& alarmId )
    {
        FUNCTION_ENTRY( "setCctvPreConflictAlarmId" );

        if ( alarmId != m_cctvPreConflictAlarmId )
        {
            m_cctvPreConflictAlarmId = alarmId;
            setDataChangedWithTimestamp();
        }

        FUNCTION_EXIT;
    }


    void TrainAlarmState::setRadioModeAlarmId( const std::string& alarmId )
    {
        FUNCTION_ENTRY( "setRadioModeAlarmId" );

        if ( alarmId != m_radioModeAlarmId )
        {
            m_radioModeAlarmId = alarmId;
            setDataChangedWithTimestamp();
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::StateUpdateTypes::TrainAlarmSyncState TrainAlarmState::getSyncState() const
    {
        FUNCTION_ENTRY( "getSyncState" );

        TA_IRS_Bus::StateUpdateTypes::TrainAlarmSyncState state;

        state.trainId = getTrainId();
        state.atcAlarmId = m_atcAlarmId;
        state.oa1AlarmId = m_oa1AlarmId;
        state.radioModeAlarmId = m_radioModeAlarmId;
        state.cctvPreConflictAlarmId = m_cctvPreConflictAlarmId;
        state.timestamp = getTimestamp();

        FUNCTION_EXIT;
        return state;
    }


    void TrainAlarmState::setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainAlarmSyncState& state )
    {
        FUNCTION_ENTRY( "setSyncState" );

        // If the timestamp in the given state structure is less than the getTimestamp() of this state,
        // discard the sync state structure - log an error and return.

        if ( state.timestamp < getTimestamp() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "Received old sync state for train %d [sync time: %d][state time: %d]",
                         getTrainId(), state.timestamp, getTimestamp() );

            FUNCTION_EXIT;
            return;
        }

        m_atcAlarmId = state.atcAlarmId;
        m_oa1AlarmId = state.oa1AlarmId;
        m_radioModeAlarmId = state.radioModeAlarmId;
        m_cctvPreConflictAlarmId = state.cctvPreConflictAlarmId;

        // the data has changed, but a sync is not required
        setDataChangedWithTimestamp( state.timestamp, false );

        FUNCTION_EXIT;
    }

} // TA_IRS_App
