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
#include "bus/trains/TrainProtocolLibrary/src/PaCommandReceivedEvent.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>


namespace TA_IRS_Bus
{
    const TrainMessageType PaCommandReceivedEvent::MESSAGE_TYPE = 19;
    //const unsigned short PaCommandReceivedEvent::EXPECTED_MESSAGE_LENGTH = 14;
    const unsigned short PaCommandReceivedEvent::EXPECTED_MESSAGE_LENGTH = 17;
    const unsigned short PaCommandReceivedEvent::REQUEST_MODE_POSITION = 5;
    const unsigned short PaCommandReceivedEvent::ORIGINAL_COMMAND_POSITION = 6;
    const unsigned short PaCommandReceivedEvent::RESULT_POSITION = 7;
    const unsigned short PaCommandReceivedEvent::LIBRARY_VERSION_POSITION = 8;
    //const unsigned short PaCommandReceivedEvent::ANNOUNCEMENT_ID_POSITION = 10;
    const unsigned short PaCommandReceivedEvent::ANNOUNCEMENT_ID_POSITION = 12;


    PaCommandReceivedEvent::PaCommandReceivedEvent( const TrainMessageData& data,
                                                    const std::string& sendingTsi,
                                                    const std::string& destinationTsi )
            : TimsEvent( data, sendingTsi, destinationTsi )
    {
        FUNCTION_ENTRY( "PaCommandReceivedEvent" );

        m_requestMode = static_cast< ProtocolPaTypes::EPaRequestMode >( getByte( REQUEST_MODE_POSITION ) );
        m_originalCommand = static_cast< EPaOriginalCommand >( getByte( ORIGINAL_COMMAND_POSITION ) );
        m_commandResult = static_cast< ProtocolPaTypes::EPaCommandResult >( getByte( RESULT_POSITION ) );
        //m_libraryVersion = getWord( LIBRARY_VERSION_POSITION );
        m_libraryVersion = getLong( LIBRARY_VERSION_POSITION );
        m_announcementId = getByte( ANNOUNCEMENT_ID_POSITION );

        switch ( m_requestMode )
        {
            // valid values

            case ProtocolPaTypes::PaRequest:
                // fall through

            case ProtocolPaTypes::PaOverride:
                break;

            // invalid values
            default:
                std::ostringstream requestMode;
                requestMode << static_cast<int>( m_requestMode );
                TA_THROW( ProtocolException( "Invalid request mode received", "RequestMode (Status 2)", requestMode.str() ) );
				// fall through
        }

        switch ( m_originalCommand )
        {
            // valid values

            case PaLiveAnnouncement:
                // fall through

            case PaPreRecordedAnnouncement:
                // fall through

            //case PaCyclicAnnouncement:
            case PaDvaReset:
                // fall through

            //case PaReset:
            case PaLiveReset:
                break;

            // invalid values
            default:
                std::ostringstream originalCommand;
                originalCommand << static_cast<int>( m_originalCommand );
                TA_THROW( ProtocolException( "Invalid original command received", "OriginalCommand", originalCommand.str() ) );
				// fall through
        }


        switch ( m_commandResult )
        {
            // valid values

            case ProtocolPaTypes::PaProceeding:
                // fall through

            case ProtocolPaTypes::PaInUse:
                // fall through

            case ProtocolPaTypes::PaInUseOptionToOverride:
                // fall through

            case ProtocolPaTypes::PaInUseFirstCycleCancelled:
                // fall through

            case ProtocolPaTypes::PaReset:
                // fall through

            case ProtocolPaTypes::PaFailed:
                // fall through

            case ProtocolPaTypes::PaDifferentOrigin:
                // fall through

            case ProtocolPaTypes::PaDifferentId:
                // fall through

            case ProtocolPaTypes::PaDifferentVersion:
                break;
            
				// invalid values
			default:
                std::ostringstream commandStr;
                commandStr << static_cast<int>( m_commandResult );
                TA_THROW( ProtocolException( "Invalid command result received", "CommandResult (Status 3)", commandStr.str() ) );
				// fall through
        }

        FUNCTION_EXIT;
    }


    PaCommandReceivedEvent::~PaCommandReceivedEvent()
    {
        FUNCTION_ENTRY( "~PaCommandReceivedEvent" );
        FUNCTION_EXIT;
    }


    TrainMessageType PaCommandReceivedEvent::getStaticType()
    {
        FUNCTION_ENTRY( "getStaticType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;

    }


    ProtocolPaTypes::EPaRequestMode PaCommandReceivedEvent::getRequestMode() const
    {
        FUNCTION_ENTRY( "getRequestMode" );
        FUNCTION_EXIT;
        return m_requestMode;

    }


    PaCommandReceivedEvent::EPaOriginalCommand PaCommandReceivedEvent::getOriginalCommand() const
    {
        FUNCTION_ENTRY( "getOriginalCommand" );
        FUNCTION_EXIT;
        return m_originalCommand;

    }


    ProtocolPaTypes::EPaCommandResult PaCommandReceivedEvent::getCommandResult() const
    {
        FUNCTION_ENTRY( "getCommandResult" );
        FUNCTION_EXIT;
        return m_commandResult;

    }


    //unsigned short PaCommandReceivedEvent::getLibraryVersion() const
    unsigned long PaCommandReceivedEvent::getLibraryVersion() const
    {
        FUNCTION_ENTRY( "getLibraryVersion" );
        FUNCTION_EXIT;
        return m_libraryVersion;

    }


    unsigned short PaCommandReceivedEvent::getAnnouncementId() const
    {
        FUNCTION_ENTRY( "getAnnouncementId" );
        FUNCTION_EXIT;
        return m_announcementId;

    }


    std::string PaCommandReceivedEvent::getLogString() const
    {
        FUNCTION_ENTRY( "PaCommandReceivedEvent" );

        std::ostringstream logStream;
        logStream << TimsEvent::getLogString();
        logStream << "RequesMdde=" << static_cast<int> ( m_requestMode ) << "]"
        << "OriginalCOmmand=" << static_cast <int> ( m_originalCommand ) << "]"
        << "CommandResult=" << static_cast<int> ( m_commandResult ) << "]"
        << "LibraryVersion=" << static_cast<unsigned int> ( m_libraryVersion ) << "]"
        << "AnnouncementId=" << static_cast<unsigned int> ( m_announcementId ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    unsigned short PaCommandReceivedEvent::getExpectedMessageLength() const
    {
        FUNCTION_ENTRY( "getExpectedMessageLength" );
        FUNCTION_EXIT;
        return EXPECTED_MESSAGE_LENGTH;
    }

} //end of namespace
