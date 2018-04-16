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

#include "bus/trains/TrainProtocolLibrary/src/PaLiveResetCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PaLiveResetCommand::MESSAGE_TYPE = 14;
    const unsigned short PaLiveResetCommand::MESSAGE_LENGTH = 9;
    const unsigned short PaLiveResetCommand::ANNOUNCEMENT_ID_POSITION = 6;


    PaLiveResetCommand::PaLiveResetCommand( CommonTypes::TrainIdType trainId,
                                    unsigned char origin,
                                    unsigned char announcementId )
            : TimsCommand( trainId,
                           MESSAGE_LENGTH,
                           MESSAGE_TYPE,
                           origin,
                           true ),
              m_announcementId( announcementId )
    {

        FUNCTION_ENTRY( "PaLiveResetCommand" );

        // Sets the fields
        setByte( ANNOUNCEMENT_ID_POSITION, m_announcementId );

        // Adds a PaCommandReceivedEventReader as a response
        addResponseType( PaCommandReceivedEvent::getStaticType() );

        // Adds a PaTrainMessageCompletedEventReader as a response
        addResponseType( PaTrainMessageCompletedEvent::getStaticType() );

        FUNCTION_EXIT;
    }


    PaLiveResetCommand::~PaLiveResetCommand()
    {
        FUNCTION_ENTRY( "~PaLiveResetCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult PaLiveResetCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );
		LOGBEFORECOTS("boost::dynamic_pointer_cast");
        PaCommandReceivedEventPtr paCommandReceivedEvent = boost::dynamic_pointer_cast<PaCommandReceivedEvent>( response );
		LOGAFTERCOTS("boost::dynamic_pointer_cast");
		LOGBEFORECOTS("boost::dynamic_pointer_cast");
        PaTrainMessageCompletedEventPtr paTrainMessageCompleteEvent = boost::dynamic_pointer_cast< PaTrainMessageCompletedEvent > ( response );
		LOGAFTERCOTS("boost::dynamic_pointer_cast");

        // check if the response is a PaCommandReceivedEvent
        if ( paCommandReceivedEvent )
        {
            // check the event is for this command type with this announcement ID
            if ( ( paCommandReceivedEvent->getOriginalCommand() == PaCommandReceivedEvent::PaLiveReset ) &&
                 ( paCommandReceivedEvent->getAnnouncementId() == m_announcementId ) )

            {
                FUNCTION_EXIT;
                return TimsCommand::setResponse( response );
            }
        }
        // check if the response is a PaTrainMessageCompletedEvent
        else if ( paTrainMessageCompleteEvent )
        {
            if ( paTrainMessageCompleteEvent->getAnnouncementId() == m_announcementId )
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


    PaCommandReceivedEventPtr PaLiveResetCommand::getCommandReceivedResponse()
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


    PaTrainMessageCompletedEventPtr PaLiveResetCommand::getTrainMessageCompletedResponse()
    {
        FUNCTION_ENTRY( "getTrainMessageCompletedResponse" );

        PaTrainMessageCompletedEventPtr paTrainMessageCompleteEvent;

        if ( getResponse() )
        {
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
            paTrainMessageCompleteEvent = boost::dynamic_pointer_cast<PaTrainMessageCompletedEvent> ( getResponse() );
			LOGAFTERCOTS("boost::dynamic_pointer_cast");
        }

        FUNCTION_EXIT;
        return paTrainMessageCompleteEvent;
    }


    std::string PaLiveResetCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[AnnouncementId=" << static_cast<unsigned int>( m_announcementId ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }

}
