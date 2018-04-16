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

#include "app/trains/train_agent/src/TrainPaState.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    TrainPaState::TrainPaState( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
            : TA_IRS_Bus::TrainState( trainId ),
              m_sessionId( "" ),
              m_consoleId( 0 ),
              m_liveAnnouncementCallId( 0 ),
              m_liveAnnouncementId( 0 ),
              m_liveAnnouncementStaticGroup( "" ),
              m_autoContinueCounter( 0 ),
              m_continueRequired( false )
    {
        FUNCTION_ENTRY( "TrainPaState" );
        FUNCTION_EXIT;
    }


    TrainPaState::~TrainPaState()
    {
        FUNCTION_ENTRY( "~TrainPaState" );
        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CommonTypes::ETrainResource TrainPaState::getResourceCategory() const
    {
        FUNCTION_ENTRY( "getResourceCategory" );
        FUNCTION_EXIT;
        return TA_IRS_Bus::CommonTypes::TrainPaResource;
    }


    TA_Base_Core::IThreadLockable& TrainPaState::getCommandLock()
    {
        FUNCTION_ENTRY( "getCommandLock" );
        FUNCTION_EXIT;
        return m_commandLock;
    }


    TA_IRS_Bus::PaTypes::AnnouncementId TrainPaState::getAnnouncementId() const
    {
        FUNCTION_ENTRY( "getAnnouncementId" );
        FUNCTION_EXIT;
        return m_liveAnnouncementId;
    }


    std::string TrainPaState::getAnnouncementStaticGroup() const
    {
        FUNCTION_ENTRY( "getAnnouncementStaticGroup" );
        FUNCTION_EXIT;
        return m_liveAnnouncementStaticGroup;
    }


    const std::string& TrainPaState::getSessionId() const
    {
        FUNCTION_ENTRY( "getSessionId" );
        FUNCTION_EXIT;
        return m_sessionId;
    }


    unsigned long TrainPaState::getConsoleId() const
    {
        FUNCTION_ENTRY( "getConsoleId" );
        FUNCTION_EXIT;
        return m_consoleId;
    }


    unsigned long TrainPaState::getCallId() const
    {
        FUNCTION_ENTRY( "getCallId" );
        FUNCTION_EXIT;
        return m_liveAnnouncementCallId;
    }


    unsigned char TrainPaState::getAutoContinueCount() const
    {
        FUNCTION_ENTRY( "getAutoContinueCount" );
        FUNCTION_EXIT;
        return m_autoContinueCounter;
    }


    bool TrainPaState::isContinueRequired()
    {
        FUNCTION_ENTRY( "isContinueRequired" );
        FUNCTION_EXIT;
        return m_continueRequired;
    }


    void TrainPaState::setAnnouncementDetails( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                               const std::string& sessionId,
                                               time_t timestamp,
                                               const std::string& staticGroup )
    {
        FUNCTION_ENTRY( "setAnnouncementDetails" );

        // if any data items differ, set them

        if ( ( announcementId != m_liveAnnouncementId ) ||
             ( sessionId != m_sessionId ) ||
             ( staticGroup != m_liveAnnouncementStaticGroup ) ||
             ( 0 != m_autoContinueCounter ) )
        {
            m_liveAnnouncementId = announcementId;
            m_sessionId = sessionId;
            m_liveAnnouncementStaticGroup = staticGroup;
            m_autoContinueCounter = 0;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainPaState::resetAnnouncementDetails( time_t timestamp )
    {
        FUNCTION_ENTRY( "resetAnnouncementDetails" );

        // if any data items differ, set them

        if ( ( 0 != m_liveAnnouncementId ) ||
             ( false == m_sessionId.empty() ) ||
             ( false == m_liveAnnouncementStaticGroup.empty() ) ||
             ( 0 != m_autoContinueCounter ) ||
             ( 0 != m_consoleId ) ||
             ( 0 != m_liveAnnouncementCallId ) ||
             ( false != m_continueRequired ) )
        {
            m_liveAnnouncementId = 0;
            m_sessionId = "";
            m_liveAnnouncementStaticGroup = "";
            m_autoContinueCounter = 0;
            m_consoleId = 0;
            m_liveAnnouncementCallId = 0;
            m_continueRequired = false;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainPaState::setCallDetails( const std::string& sessionId,
                                       unsigned long consoleId,
                                       unsigned long callId,
                                       time_t timestamp )
    {
        FUNCTION_ENTRY( "setCallDetails" );

        // if any data items differ, set them

        if ( ( sessionId != m_sessionId ) ||
             ( consoleId != m_consoleId ) ||
             ( callId != m_liveAnnouncementCallId ) )
        {
            m_sessionId = sessionId;
            m_consoleId = consoleId;
            m_liveAnnouncementCallId = callId;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainPaState::clearCall()
    {
        FUNCTION_ENTRY( "clearCall" );

        if ( ( 0 != m_consoleId ) ||
             ( 0 != m_liveAnnouncementCallId ) )
        {
            m_consoleId = 0;
            m_liveAnnouncementCallId = 0;

            setDataChangedWithTimestamp();
        }

        FUNCTION_EXIT;
    }


    const void TrainPaState::incrementAutoContinueCount()
    {
        FUNCTION_ENTRY( "incrementAutoContinueCount" );

        ++m_autoContinueCounter;

        setDataChangedWithTimestamp();

        FUNCTION_EXIT;
    }


    void TrainPaState::setContinueRequired( time_t timestamp )
    {
        FUNCTION_ENTRY( "setContinueRequired" );

        if ( false == m_continueRequired )
        {
            m_continueRequired = true;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    const void TrainPaState::resetAutoContinueCount( time_t timestamp )
    {
        FUNCTION_ENTRY( "resetAutoContinueCount" );

        if ( ( true == m_continueRequired ) ||
             ( 0 != m_autoContinueCounter ) )
        {
            m_continueRequired = false;
            m_autoContinueCounter = 0;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::StateUpdateTypes::TrainPaSyncState TrainPaState::getSyncState() const
    {
        FUNCTION_ENTRY( "getSyncState" );

        TA_IRS_Bus::StateUpdateTypes::TrainPaSyncState state;
        state.trainId = getTrainId();
        state.consoleId = m_consoleId;
        state.sessionId = m_sessionId;
        state.liveAnnouncementCallId = m_liveAnnouncementCallId;
        state.liveAnnouncementId = m_liveAnnouncementId;
        state.liveAnnouncementStaticGroup = m_liveAnnouncementStaticGroup;
        state.autoContinueAckCount = m_autoContinueCounter;
        state.continueRequired = m_continueRequired;
        state.timestamp = getTimestamp();

        FUNCTION_EXIT;
        return state;
    }


    void TrainPaState::setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainPaSyncState& state )
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

        m_consoleId = state.consoleId;
        m_sessionId = state.sessionId;
        m_liveAnnouncementCallId = state.liveAnnouncementCallId;
        m_liveAnnouncementId = state.liveAnnouncementId;
        m_liveAnnouncementStaticGroup = state.liveAnnouncementStaticGroup;
        m_autoContinueCounter = state.autoContinueAckCount;
        m_continueRequired = state.continueRequired;

        // the data has changed, but a sync is not required
        setDataChangedWithTimestamp( state.timestamp, false );

        FUNCTION_EXIT;
    }

} // TA_IRS_App
