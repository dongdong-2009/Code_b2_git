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

#include "app/trains/train_agent/src/TrainCallState.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    TrainCallState::TrainCallState( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
            : TA_IRS_Bus::TrainState( trainId ),
              m_incomingCall( false ),
              m_incomingCallTime( 0 ),
              m_sessionId( "" ),
              m_consoleId( 0 ),
              m_callId( 0 ),
              m_callType( TA_IRS_Bus::CallTypes::NoCall )
    {
        FUNCTION_ENTRY( "TrainCallState" );
        FUNCTION_EXIT;
    }


    TrainCallState::~TrainCallState()
    {
        FUNCTION_ENTRY( "~TrainCallState" );
        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CommonTypes::ETrainResource TrainCallState::getResourceCategory() const
    {
        FUNCTION_ENTRY( "getResourceCategory" );
        FUNCTION_EXIT;
        return TA_IRS_Bus::CommonTypes::TrainCallResource;
    }


    TA_Base_Core::IThreadLockable& TrainCallState::getCommandLock()
    {
        FUNCTION_ENTRY( "getCommandLock" );
        FUNCTION_EXIT;
        return m_commandLock;
    }


    bool TrainCallState::hasIncomingCall() const
    {
        FUNCTION_ENTRY( "hasIncomingCall" );
        FUNCTION_EXIT;
        return m_incomingCall;
    }


    time_t TrainCallState::getIncomingCallTime() const
    {
        FUNCTION_ENTRY( "getIncomingCallTime" );
        FUNCTION_EXIT;
        return m_incomingCallTime;
    }


    TA_IRS_Bus::CallTypes::ETrainCallType TrainCallState::getCallType() const
    {
        FUNCTION_ENTRY( "getCallType" );
        FUNCTION_EXIT;
        return m_callType;
    }


    const std::string& TrainCallState::getSessionId() const
    {
        FUNCTION_ENTRY( "getSessionId" );
        FUNCTION_EXIT;
        return m_sessionId;
    }


    unsigned long TrainCallState::getCallId() const
    {
        FUNCTION_ENTRY( "getCallId" );
        FUNCTION_EXIT;
        return m_callId;
    }


    unsigned long TrainCallState::getConsoleId() const
    {
        FUNCTION_ENTRY( "getConsoleId" );
        FUNCTION_EXIT;
        return m_consoleId;
    }


    void TrainCallState::setIncomingCall( time_t timestamp )
    {
        FUNCTION_ENTRY( "setIncomingCall" );

        // if there is not already an incoming call
        // set this new incoming call

        if ( false == m_incomingCall )
        {
            m_incomingCall = true;
            m_incomingCallTime = timestamp;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainCallState::clearIncomingCall( time_t timestamp )
    {
        FUNCTION_ENTRY( "clearIncomingCall" );

        // if there is an incoming call
        // clear the incoming call

        if ( true == m_incomingCall )
        {
            m_incomingCall = false;
            m_incomingCallTime = 0;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainCallState::setCallDetails( const std::string& sessionId,
                                         TA_IRS_Bus::CallTypes::ETrainCallType callType,
                                         time_t timestamp,
                                         unsigned long consoleId,
                                         unsigned long callId )
    {
        FUNCTION_ENTRY( "setCallDetails" );

        // if the given data differs, set it
        // and flag the state change

        if ( ( sessionId != m_sessionId ) ||
             ( callType != m_callType ) ||
             ( consoleId != m_consoleId ) ||
             ( callId != m_callId ) )
        {
            m_sessionId = sessionId;
            m_callType = callType;
            m_consoleId = consoleId;
            m_callId = callId;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainCallState::clearCall( time_t timestamp )
    {
        FUNCTION_ENTRY( "clearCall" );

        // if the given data differs, set it
        // and flag the state change

        if ( ( "" != m_sessionId ) ||
             ( TA_IRS_Bus::CallTypes::NoCall != m_callType ) ||
             ( 0 != m_consoleId ) ||
             ( 0 != m_callId ) )
        {
            m_sessionId = "";
            m_callType = TA_IRS_Bus::CallTypes::NoCall;
            m_consoleId = 0;
            m_callId = 0;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::StateUpdateTypes::TrainCallSyncState TrainCallState::getSyncState() const
    {
        FUNCTION_ENTRY( "getSyncState" );

        TA_IRS_Bus::StateUpdateTypes::TrainCallSyncState state;

        state.trainId = getTrainId();
        state.incomingCall = m_incomingCall;
        state.incomingCallTime = m_incomingCallTime;
        state.sessionId = m_sessionId;
        state.callType = m_callType;
        state.consoleId = m_consoleId;
        state.callId = m_callId;
        state.timestamp = getTimestamp();

        FUNCTION_EXIT;
        return state;
    }


    void TrainCallState::setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCallSyncState& state )
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

        m_incomingCall = state.incomingCall;
        m_incomingCallTime = state.incomingCallTime;
        m_sessionId = state.sessionId;
        m_callType = state.callType;
        m_consoleId = state.consoleId;
        m_callId = state.callId;

        // the data has changed, but a sync is not required
        setDataChangedWithTimestamp( state.timestamp, false );

        FUNCTION_EXIT;
    }

} // TA_IRS_App
