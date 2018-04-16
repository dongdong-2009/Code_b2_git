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
#include "bus/trains/TrainProtocolLibrary/src/TtisCommandReceivedEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>


namespace TA_IRS_Bus
{
    const TrainMessageType TtisCommandReceivedEvent::MESSAGE_TYPE = 42;
    const unsigned short TtisCommandReceivedEvent::EXPECTED_MESSAGE_LENGTH = 11;
    const unsigned short TtisCommandReceivedEvent::RESULT_POSITION = 5;
    const unsigned short TtisCommandReceivedEvent::ORIGINAL_COMMAND_POSITION = 6;
    const unsigned short TtisCommandReceivedEvent::LIBRARY_VERSION_POSITION = 7;
    const unsigned short TtisCommandReceivedEvent::SCHEDULE_LIBRARY_VERSION = 8;


    TtisCommandReceivedEvent::TtisCommandReceivedEvent( const TrainMessageData& data,
                                                        const std::string& sendingTsi,
                                                        const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY ( "TtisCommandReceivedEvent" );

        m_result = static_cast<ProtocolTtisTypes::ETtisResult>( getByte( RESULT_POSITION ) );
        m_originalCommand = static_cast<ETtisOriginalCommand>( getByte( ORIGINAL_COMMAND_POSITION ) );
        m_libraryVersion = getByte( LIBRARY_VERSION_POSITION );
        m_scheduleLibraryVersion = getByte( SCHEDULE_LIBRARY_VERSION );

        switch ( m_result )
        {
            // valid values

            case ProtocolTtisTypes::TtisProceeding:
                // fall through

            case ProtocolTtisTypes::TtisInUse:
                // fall through

            case ProtocolTtisTypes::TtisLibraryUpgraded:
                // fall through

            case ProtocolTtisTypes::TtisReset:
                // fall through

            case ProtocolTtisTypes::TtisFailed:
                // fall through

            case ProtocolTtisTypes::TtisDifferentVersion:
                break;

            // invalid values
            default:
                std::ostringstream resultStr;
                resultStr << static_cast<int>( m_result );
                TA_THROW( ProtocolException( "Invalid result received", "Result (Status 5)", resultStr.str() ) );
        }

        switch ( m_originalCommand )
        {
            // valid values

            case TtisFreeTextMessage:
                // fall through

            case TtisPreStoredMessage:
                // fall through

            case TtisLibraryDownload:
                // fall through

            case TtisLibraryUpgrade:
                // fall through

            case TtisResetMessage:
                // fall through

            case TtisScheduleDownload:
                // fall through
                
            case TtisScheduleUpgrade:
                break;

            // invalid values
            default:
                std::ostringstream originalCommandStr;
                originalCommandStr << static_cast<int>( m_originalCommand );
                TA_THROW( ProtocolException( "Invalid original command received", "Original Command 2", originalCommandStr.str() ) );
        }

        FUNCTION_EXIT;
    }


    TtisCommandReceivedEvent::~TtisCommandReceivedEvent()
    {
        FUNCTION_ENTRY( "~TtisCommandReceivedEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType TtisCommandReceivedEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    ProtocolTtisTypes::ETtisResult TtisCommandReceivedEvent::getResult() const
    {
        FUNCTION_ENTRY( "getResult" );
        FUNCTION_EXIT;
        return m_result;
    }


    ETtisOriginalCommand TtisCommandReceivedEvent::getOriginalCommand() const
    {
        FUNCTION_ENTRY( "getOriginalCommand" );
        FUNCTION_EXIT;
        return m_originalCommand;
    }


    unsigned char TtisCommandReceivedEvent::getLibraryVersion() const
    {
        FUNCTION_ENTRY( "getLibraryVersion" );
        FUNCTION_EXIT;
        return m_libraryVersion;
    }


    unsigned char TtisCommandReceivedEvent::getScheduleLibraryVersion() const
    {
        FUNCTION_ENTRY( "getScheduleVersion" );
        FUNCTION_EXIT;
        return m_scheduleLibraryVersion;        
    }


    std::string TtisCommandReceivedEvent::getLogString() const
    {
        FUNCTION_ENTRY( "getLogString" );

        // appends more information in the format:
        // "[Result=X][OriginalCommand=X][LibraryVersion=X]
        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "[Result=" << static_cast<int>( m_result ) << "]"
                  << "[OriginalCommand=" << static_cast<int> ( m_originalCommand ) << "]"
                  << "[LibraryVersion=" << static_cast<unsigned short>( m_libraryVersion ) << "]"
                  << "[ScheduleVersion=" << static_cast<unsigned short>( m_scheduleLibraryVersion ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    unsigned short TtisCommandReceivedEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

}   // end of namespace
