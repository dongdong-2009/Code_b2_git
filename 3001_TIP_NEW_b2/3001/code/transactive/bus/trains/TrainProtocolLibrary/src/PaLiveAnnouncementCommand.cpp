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
#include "bus/trains/TrainProtocolLibrary/src/PaLiveAnnouncementCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PaCommandReceivedEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PaLiveAnnouncementCommand::MESSAGE_TYPE = 11;
    const unsigned short PaLiveAnnouncementCommand::MESSAGE_LENGTH = 9;
    const unsigned short PaLiveAnnouncementCommand::REQUEST_MODE_POSITION = 5;
    const unsigned short PaLiveAnnouncementCommand::ANNOUNCEMENT_ID_POSITION = 6;


    PaLiveAnnouncementCommand::PaLiveAnnouncementCommand( CommonTypes::TrainIdType trainId,
                                                          unsigned char origin,
                                                          ProtocolPaTypes::EPaRequestMode requestMode,
                                                          unsigned char announcementId )
        : TimsCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, true ),
          m_requestMode( requestMode ),
          m_announcementId( announcementId )
    {
        FUNCTION_ENTRY( "PaLiveAnnouncementCommand" );

        setByte( REQUEST_MODE_POSITION, static_cast<unsigned char>( ProtocolPaTypes::PaRequest ) );
        setByte( ANNOUNCEMENT_ID_POSITION, m_announcementId );

        addResponseType( PaCommandReceivedEvent::getStaticType() );
    }


    PaLiveAnnouncementCommand::~PaLiveAnnouncementCommand()
    {
        FUNCTION_ENTRY( "~PaLiveAnnouncementCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult PaLiveAnnouncementCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );
		LOGBEFORECOTS("boost::dynamic_pointer_cast");
        PaCommandReceivedEventPtr paCommandReceivedEvent = boost::dynamic_pointer_cast< PaCommandReceivedEvent > ( response );
		LOGAFTERCOTS("boost::dynamic_pointer_cast");
        if ( paCommandReceivedEvent )
        {
            if ( ( PaCommandReceivedEvent::PaLiveAnnouncement == paCommandReceivedEvent->getOriginalCommand() ) &&
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


    PaCommandReceivedEventPtr PaLiveAnnouncementCommand::getCommandReceivedResponse()
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


    std::string PaLiveAnnouncementCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        // append the following:
        // "[RequestMode=X][AnnouncementId=X]"
        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[RequestMode=" << static_cast<int>( m_requestMode ) << "]"
                  << "AnnouncementId=" << static_cast<unsigned int>( m_announcementId ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
