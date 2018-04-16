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
#include "bus/trains/TrainProtocolLibrary/src/PaCyclicPreRecordedAnnouncementCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PaCommandReceivedEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PaCyclicPreRecordedAnnouncementCommand::MESSAGE_TYPE = 25;
    const unsigned short PaCyclicPreRecordedAnnouncementCommand::MESSAGE_LENGTH = 17;
    const unsigned short PaCyclicPreRecordedAnnouncementCommand::REQUEST_MODE_POSITION = 5;
    const unsigned short PaCyclicPreRecordedAnnouncementCommand::IDENTIFIER_TABLE_TYPE_POSITION = 6;
    const unsigned short PaCyclicPreRecordedAnnouncementCommand::IDENTIFIER_TABLE_TYPE_VALUE = 0;
    const unsigned short PaCyclicPreRecordedAnnouncementCommand::IDENTIFIER_TABLE_OFFSET_POSITION = 7;
    const unsigned short PaCyclicPreRecordedAnnouncementCommand::LIBRARY_VERSION_POSITION = 8;
    const unsigned short PaCyclicPreRecordedAnnouncementCommand::T1_DURATION_POSITION = 10;
    const unsigned short PaCyclicPreRecordedAnnouncementCommand::T2_PERIOD_POSITION = 12;
    const unsigned short PaCyclicPreRecordedAnnouncementCommand::ANNOUNCEMENT_ID_POSITION = 14;


    PaCyclicPreRecordedAnnouncementCommand::PaCyclicPreRecordedAnnouncementCommand( CommonTypes::TrainIdType trainId,
                                                                                    unsigned char origin,
                                                                                    ProtocolPaTypes::EPaRequestMode requestMode,
                                                                                    unsigned char messageId,
                                                                                    unsigned short libraryVersion,
                                                                                    unsigned short period,
                                                                                    unsigned short duration,
                                                                                    unsigned char announcementId )
        : TimsCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, true ),
          m_requestMode( requestMode ),
          m_messageId( messageId ),
          m_libraryVersion( libraryVersion ),
          m_t1Duration( duration ),
          m_t2Period( period ),
          m_announcementId( announcementId )
    {
        FUNCTION_ENTRY( "PaCyclicPreRecordedAnnouncementCommand" );

        setByte( REQUEST_MODE_POSITION, static_cast<unsigned char>( m_requestMode ) );
        setByte( IDENTIFIER_TABLE_OFFSET_POSITION, m_messageId );
        setWord( LIBRARY_VERSION_POSITION, m_libraryVersion );
        setByte( ANNOUNCEMENT_ID_POSITION, m_announcementId );
        setWord( T1_DURATION_POSITION, m_t1Duration );
        setWord( T2_PERIOD_POSITION, m_t2Period );

        addResponseType( PaCommandReceivedEvent::getStaticType() );

        FUNCTION_EXIT;
    }


    PaCyclicPreRecordedAnnouncementCommand::~PaCyclicPreRecordedAnnouncementCommand()
    {
        FUNCTION_ENTRY( "~PaCyclicPreRecordedAnnouncementCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult PaCyclicPreRecordedAnnouncementCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );

        PaCommandReceivedEventPtr paCommandReceiveevent = boost::dynamic_pointer_cast< PaCommandReceivedEvent >( response );

        if ( paCommandReceiveevent )
        {
            if ( PaCommandReceivedEvent::PaCyclicAnnouncement == paCommandReceiveevent->getOriginalCommand() &&
                 m_announcementId == paCommandReceiveevent->getAnnouncementId() )
            {
                FUNCTION_EXIT;
                return TimsCommand::setResponse( response );
            }
        }
        else
        {
            FUNCTION_EXIT;
            return TimsCommand::setResponse( response );
        }

        FUNCTION_EXIT;
        return TrainCommand::TrainResponseResult( false, false );
    }


    PaCommandReceivedEventPtr PaCyclicPreRecordedAnnouncementCommand::getCommandReceivedResponse()
    {
        FUNCTION_ENTRY( "getCommandReceivedResponse" );

        PaCommandReceivedEventPtr paCommandReceivedEvent;

        if ( getResponse() )
        {
            paCommandReceivedEvent = boost::dynamic_pointer_cast< PaCommandReceivedEvent >( getResponse() );
        }

        FUNCTION_EXIT;
        return paCommandReceivedEvent;
    }


    std::string PaCyclicPreRecordedAnnouncementCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        //append the following:
        //"[RequestMode=X][MessageId][LibraryVersion=X][Period=X][Duration=X][AnnouncementId=X]"
        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[RequestMode=" << static_cast<int>( m_requestMode ) << "]"
                  << "[MessageId=" << static_cast<unsigned int>( m_messageId ) << "]"
                  << "[LibraryVesion=" << static_cast<unsigned int>( m_libraryVersion ) << "]"
                  << "[Period=" << static_cast<unsigned int>( m_t2Period ) << "]"
                  << "[Duration=" << static_cast<unsigned int>( m_t1Duration ) << "]"
                  << "[AnnouncementId=" << static_cast< unsigned int>( m_announcementId ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
