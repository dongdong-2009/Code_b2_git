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

#include "app/trains/train_agent/src/TrainTisState.h"

#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <algorithm>

// STL Helper function forward declarations
namespace
{
    // used to check if a PecPoint is answered
    bool framesMatch( const TA_IRS_Bus::TtisTypes::TtisFrameData& lhs, 
                      const TA_IRS_Bus::TtisTypes::TtisFrameData& rhs );
}


namespace TA_IRS_App
{

    TrainTisState::TrainTisState( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
            : TA_IRS_Bus::TrainState( trainId ),
              m_downloadType( TA_IRS_Bus::TtisTypes::TtisNone ),
              m_currentDownloadVersion( 0 ),
              m_sessionId( "" ),
              m_transferTsi( "" )
    {
        FUNCTION_ENTRY( "TrainTisState" );
        FUNCTION_EXIT;
    }


    TrainTisState::~TrainTisState()
    {
        FUNCTION_ENTRY( "~TrainTisState" );
        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CommonTypes::ETrainResource TrainTisState::getResourceCategory() const
    {
        FUNCTION_ENTRY( "getResourceCategory" );
        FUNCTION_EXIT;
        return TA_IRS_Bus::CommonTypes::TrainTisResource;
    }


    TA_Base_Core::IThreadLockable& TrainTisState::getCommandLock()
    {
        FUNCTION_ENTRY( "getCommandLock" );
        FUNCTION_EXIT;
        return m_commandLock;
    }


    TA_IRS_Bus::TtisTypes::ETtisDownloadType TrainTisState::getCurrentDownloadType() const
    {
        FUNCTION_ENTRY( "getCurrentDownloadType" );
        FUNCTION_EXIT;
        return m_downloadType;
    }


    unsigned short TrainTisState::getCurrentDownloadVersion() const
    {
        FUNCTION_ENTRY( "getCurrentDownloadVersion" );
        FUNCTION_EXIT;
        return m_currentDownloadVersion;
    }


    const TA_IRS_Bus::TtisTypes::TtisFrameList& TrainTisState::getCurrentDownloadData() const
    {
        FUNCTION_ENTRY( "getCurrentDownloadData" );
        FUNCTION_EXIT;
        return m_currentDownload;
    }


    std::string TrainTisState::getTransferTsi() const
    {
        FUNCTION_ENTRY( "getTransferTsi" );
        FUNCTION_EXIT;
        return m_transferTsi;
    }


    std::string TrainTisState::getSessionId() const
    {
        FUNCTION_ENTRY( "getSessionId" );
        FUNCTION_EXIT;
        return m_sessionId;
    }


    const TA_IRS_Bus::TtisTypes::TtisFrameData& TrainTisState::getDownloadFrame( unsigned short frameNumber ) const
    {
        FUNCTION_ENTRY( "getDownloadFrame" );

        // check if the frame number is out of range
        if ( ( m_currentDownload.size() < frameNumber ) ||
             ( 0 == frameNumber ) )
        {
            std::ostringstream frame;
            frame << frameNumber;

            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Frame does not exist in the data being downloaded to the train", "Frame", frame.str() ) );
        }

        FUNCTION_EXIT;
        return m_currentDownload[frameNumber - 1];
    }


    void TrainTisState::setDownload( const std::string& sessionId,
                                     TA_IRS_Bus::TtisTypes::ETtisDownloadType downloadType,
                                     unsigned short downloadVersion,
                                     const TA_IRS_Bus::TtisTypes::TtisFrameList& data,
                                     const std::string& transferTsi,
                                     time_t timestamp )
    {
        FUNCTION_ENTRY( "setDownload" );

        // the data can not be directly compared,
        // so the comparison needs to be a bit smarter
        bool differs = ( ( sessionId != m_sessionId ) ||
                         ( downloadType != m_downloadType ) ||
                         ( downloadVersion != m_currentDownloadVersion ) ||
                         ( transferTsi != m_transferTsi ) );
        
        // now only check the data if necessary
        if ( false == differs )
        {
            // first check the number of frames
            differs = ( data.size() != m_currentDownload.size() );

            // now check the data if necessary
            if ( false == differs )
            {
                differs = !std::equal( m_currentDownload.begin(), m_currentDownload.end(), data.begin(), framesMatch );
            }
        }
        
        if ( true == differs )
        {
            m_sessionId = sessionId;
            m_downloadType = downloadType;
            m_currentDownloadVersion = downloadVersion;
            m_transferTsi = transferTsi;
            
            // set the raw data
            m_currentDownload = data;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainTisState::setTransferTsi( const std::string& transferTsi,
                                        time_t timestamp )
    {
        FUNCTION_ENTRY( "setTransferTsi" );

        if ( transferTsi != m_transferTsi )
        {
            m_transferTsi = transferTsi;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainTisState::clearDownload( time_t timestamp )
    {
        FUNCTION_ENTRY( "clearDownload" );

        if ( ( false == m_sessionId.empty() ) ||
             ( TA_IRS_Bus::TtisTypes::TtisNone != m_downloadType ) ||
             ( 0 != m_currentDownloadVersion ) ||
             ( false == m_currentDownload.empty() ) ||
             ( false == m_transferTsi.empty() ) )
        {
            m_sessionId = "";
            m_downloadType = TA_IRS_Bus::TtisTypes::TtisNone;
            m_currentDownloadVersion = 0;
            m_currentDownload.clear();
            m_transferTsi = "";

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainTisState::transferComplete( time_t timestamp )
    {
        FUNCTION_ENTRY( "transferComplete" );

        if ( false == m_transferTsi.empty() )
        {
            m_transferTsi = "";

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::StateUpdateTypes::TrainTisSyncState TrainTisState::getSyncState() const
    {
        FUNCTION_ENTRY( "getSyncState" );

        TA_IRS_Bus::StateUpdateTypes::TrainTisSyncState state;

        state.trainId = getTrainId();
        state.downloadType = m_downloadType;
        state.currentDownloadVersion = m_currentDownloadVersion;
        state.currentDownload = m_currentDownload;
        state.sessionId = m_sessionId;
        state.transferTsi = m_transferTsi;
        state.timestamp = getTimestamp();

        FUNCTION_EXIT;
        return state;
    }


    void TrainTisState::setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainTisSyncState& state )
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

        m_downloadType = state.downloadType;
        m_currentDownloadVersion = state.currentDownloadVersion;
        m_currentDownload = state.currentDownload;
        m_sessionId = state.sessionId;
        m_transferTsi = state.transferTsi;

        // the data has changed, but a sync is not required
        setDataChangedWithTimestamp( state.timestamp, false );

        FUNCTION_EXIT;
    }

} // TA_IRS_App

// STL Helper function implementation
namespace
{
    // used to check if a PecPoint is answered
    bool framesMatch( const TA_IRS_Bus::TtisTypes::TtisFrameData& lhs, 
                      const TA_IRS_Bus::TtisTypes::TtisFrameData& rhs )
    {
        return ( lhs.frameData == rhs.frameData );
    }
}
