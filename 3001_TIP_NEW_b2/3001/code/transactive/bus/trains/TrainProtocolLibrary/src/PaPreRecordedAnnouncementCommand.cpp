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
#include "bus/trains/TrainProtocolLibrary/src/PaPreRecordedAnnouncementCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PaCommandReceivedEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PaPreRecordedAnnouncementCommand::MESSAGE_TYPE = 12;
    //const unsigned short PaPreRecordedAnnouncementCommand::MESSAGE_LENGTH = 13;
    const unsigned short PaPreRecordedAnnouncementCommand::MESSAGE_LENGTH = 16;
    //const unsigned short PaPreRecordedAnnouncementCommand::REQUEST_MODE_POSITION = 5;
    const unsigned short PaPreRecordedAnnouncementCommand::IDENTIFIER_TABLE_TYPE_POSITION = 6;
    const unsigned short PaPreRecordedAnnouncementCommand::IDENTIFIER_TABLE_TYPE_VALUE = 0;
    const unsigned short PaPreRecordedAnnouncementCommand::IDENTIFIER_TABLE_OFFSET_POSITION = 7;
    //const unsigned short PaPreRecordedAnnouncementCommand::LIBRARY_VERSION_POSITION = 8;
    const unsigned short PaPreRecordedAnnouncementCommand::LIBRARY_VERSION_POSITION = 9;
    //const unsigned short PaPreRecordedAnnouncementCommand::ANNOUNCEMENT_ID_POSITION = 10;
    const unsigned short PaPreRecordedAnnouncementCommand::ANNOUNCEMENT_ID_POSITION = 13;
	const unsigned short PaPreRecordedAnnouncementCommand::MESSAGE_PRIORITY_POSITION = 5;


    PaPreRecordedAnnouncementCommand::PaPreRecordedAnnouncementCommand( CommonTypes::TrainIdType trainId,
                                                                        unsigned char origin,
                                                                        ProtocolPaTypes::EPaRequestMode requestMode,
                                                                        //unsigned char messageId,
                                                                        unsigned short messageId,
                                                                        //unsigned short libraryVersion,
                                                                        unsigned long libraryVersion,
                                                                        unsigned char announcementId,
																		TA_Base_Bus::ITrainPaCorba::EDvaMsgPriority messagePriority)
        : TimsCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, true ),
          m_requestMode( requestMode ),
          m_messageId( messageId ),
          m_libraryVersion( libraryVersion ),
          m_announcementId( announcementId ),
		  m_messagePriority ( messagePriority )
    {
        FUNCTION_ENTRY( "PaPreRecordedAnnouncementCommand" );

        // Sets fields
        //setByte( REQUEST_MODE_POSITION, static_cast<unsigned char>( m_requestMode ) );
        setByte( MESSAGE_PRIORITY_POSITION, static_cast<unsigned char>(m_messagePriority) );
        setByte( IDENTIFIER_TABLE_TYPE_POSITION, IDENTIFIER_TABLE_TYPE_VALUE );
        //setByte( IDENTIFIER_TABLE_OFFSET_POSITION, m_messageId );
        setWord( IDENTIFIER_TABLE_OFFSET_POSITION, m_messageId );
        //setWord( LIBRARY_VERSION_POSITION, m_libraryVersion );
        setLong( LIBRARY_VERSION_POSITION, m_libraryVersion );
        setByte( ANNOUNCEMENT_ID_POSITION, m_announcementId );

        // Adds a PaCommandReceivedEvent as a response
        addResponseType( PaCommandReceivedEvent::getStaticType() );

        FUNCTION_EXIT;
    }


    PaPreRecordedAnnouncementCommand::~PaPreRecordedAnnouncementCommand()
    {
        FUNCTION_ENTRY( "~PaPreRecordedAnnouncementCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult PaPreRecordedAnnouncementCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );
		LOGBEFORECOTS("boost::dynamic_pointer_cast");
        PaCommandReceivedEventPtr paCommandReceivedEvent = boost::dynamic_pointer_cast< PaCommandReceivedEvent > ( response );
		LOGAFTERCOTS("boost::dynamic_pointer_cast");
        // if the response is a PaCommandReceivedEvent
        if ( paCommandReceivedEvent )
        {
            // do further validation to ensure the original command and announcement ID match
            // if they do not then the response is not for this command
            if ( ( PaCommandReceivedEvent::PaPreRecordedAnnouncement == paCommandReceivedEvent->getOriginalCommand() ) &&
                 ( paCommandReceivedEvent->getAnnouncementId() == m_announcementId ) )
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


    PaCommandReceivedEventPtr PaPreRecordedAnnouncementCommand::getCommandReceivedResponse()
    {
        FUNCTION_ENTRY( "getCommandReceivedResponse" );

        PaCommandReceivedEventPtr paCommandReceivedEvent;

        if ( getResponse() )
        {
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
            paCommandReceivedEvent = boost::dynamic_pointer_cast< PaCommandReceivedEvent > ( getResponse() );
			LOGAFTERCOTS("boost::dynamic_pointer_cast");
        }

        FUNCTION_EXIT;
        return paCommandReceivedEvent;
    }


    std::string PaPreRecordedAnnouncementCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        // append the following:
        // "[RequestMode=X][MessageId=X][LibraryVersion=X][AnnouncementId=X]"
        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[RequestMode=" << static_cast<int>( m_requestMode ) << "]"
        << "[Messageid=" << static_cast<unsigned int>( m_messageId ) << "]"
        << "LibraryVersion=" << m_libraryVersion << "]"
        << "AnnouncementId=" << static_cast<unsigned int>( m_announcementId ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
