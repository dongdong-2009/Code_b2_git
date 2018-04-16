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
#include "bus/trains/TrainProtocolLibrary/src/ChangeModeCommand.h"

#include "bus/trains/TrainProtocolLibrary/src/AtcEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/AudioCabToCabEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/BadCommandEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/CallTrainCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/CctvCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeAreaCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/ChangeoverStatusEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/EndOfOccCallEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/EndOfTtisDownloadEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/Oa1Event.h"
#include "bus/trains/TrainProtocolLibrary/src/PaAudioSwitchEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaReadyForLiveAnnouncementEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaRequestForResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PaTrainMessageCompletedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecActivatedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecCommandReceivedEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecReadyForConversationEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecRequestForResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecResetEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/PecSelectedByDriverEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/RequestForOccCallEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/TestCallEvent.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisCommandReceivedEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType ChangeModeCommand::MESSAGE_TYPE = 73;
    const unsigned short ChangeModeCommand::MESSAGE_LENGTH = 8;
    const unsigned short ChangeModeCommand::MODE_POSITION = 5;


    ChangeModeCommand::ChangeModeCommand( CommonTypes::TrainIdType trainId,
                                          unsigned char origin,
                                          ETrainMode mode )
            : TimsCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, true )
    {
        FUNCTION_ENTRY( "ChangeModeCommand" );

        m_mode = mode;
        setByte( MODE_POSITION, static_cast<unsigned char>( m_mode ) );

        //add ALL responses as valid responses. Any incoming message in the protocol library is a valid response for this command.
        addResponseType( AtcEvent::getStaticType() );
        addResponseType( AudioCabToCabEvent::getStaticType() );
        addResponseType( BadCommandEvent::getStaticType() );
        addResponseType( CallTrainCommandReceivedEvent::getStaticType() );
        addResponseType( CctvCommandReceivedEvent::getStaticType() );
        addResponseType( ChangeAreaCommandReceivedEvent::getStaticType() );
        addResponseType( ChangeoverStatusEvent::getStaticType() );
        addResponseType( EndOfOccCallEvent::getStaticType() );
        addResponseType( EndOfTtisDownloadEvent::getStaticType() );
        addResponseType( Oa1Event::getStaticType() );
        addResponseType( PaAudioSwitchEvent::getStaticType() );
        addResponseType( PaCommandReceivedEvent::getStaticType() );
        addResponseType( PaReadyForLiveAnnouncementEvent::getStaticType() );
        addResponseType( PaRequestForResetEvent::getStaticType() );
        addResponseType( PaResetEvent::getStaticType() );
        addResponseType( PaTrainMessageCompletedEvent::getStaticType() );
        addResponseType( PecActivatedEvent::getStaticType() );
        addResponseType( PecCommandReceivedEvent::getStaticType() );
        addResponseType( PecReadyForConversationEvent::getStaticType() );
        addResponseType( PecRequestForResetEvent::getStaticType() );
        addResponseType( PecResetEvent::getStaticType() );
        addResponseType( PecSelectedByDriverEvent::getStaticType() );
        addResponseType( RequestForOccCallEvent::getStaticType() );
        addResponseType( TestCallEvent::getStaticType() );
        addResponseType( TtisCommandReceivedEvent::getStaticType() );

        FUNCTION_EXIT;
    }


    ChangeModeCommand::~ChangeModeCommand()
    {
        FUNCTION_ENTRY( "~ChangeModeCommand" );
        FUNCTION_EXIT;
    }


    TrainMessageType ChangeModeCommand::getStaticMessageType()
    {
        FUNCTION_ENTRY( "getStaticMessageType" );
        FUNCTION_EXIT;
        return MESSAGE_TYPE;
    }


    std::string ChangeModeCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        //append the following:"[Mode=X]"
        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[Mode=" << static_cast<unsigned int>( m_mode ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    TrainCommand::TrainResponseResult ChangeModeCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );

        TrainCommand::TrainResponseResult result = TimsCommand::setResponse( response );

        if ( true == result.transactionComplete )
        {
            // this command doesnt consume events
            result.eventConsumed = false;
        }

        FUNCTION_EXIT;
        return result;
    }

}
