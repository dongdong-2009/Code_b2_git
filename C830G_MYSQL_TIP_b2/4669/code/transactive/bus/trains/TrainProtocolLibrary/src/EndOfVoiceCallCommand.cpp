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
#include "bus/trains/TrainProtocolLibrary/src/EndOfVoiceCallCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/CallTrainCommandReceivedEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType EndOfVoiceCallCommand::MESSAGE_TYPE = 79;
    const unsigned short EndOfVoiceCallCommand::MESSAGE_LENGTH = 7;


    EndOfVoiceCallCommand::EndOfVoiceCallCommand( CommonTypes::TrainIdType trainId, unsigned char origin )
            : TimsCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, true )
    {
        FUNCTION_ENTRY( "EndOfVoiceCallCommand" );

        // Adds a CallTrainCommandReceivedEventReader as a response
        addResponseType( CallTrainCommandReceivedEvent::getStaticType() );

        FUNCTION_EXIT;
    }


    EndOfVoiceCallCommand::~EndOfVoiceCallCommand()
    {
        FUNCTION_ENTRY( "~EndOfVoiceCallCommand" );
        FUNCTION_EXIT;
    }


    CallTrainCommandReceivedEventPtr EndOfVoiceCallCommand::getCommandReceivedResponse()
    {
        FUNCTION_ENTRY( "getCommandReceivedResponse" );

        CallTrainCommandReceivedEventPtr callTrainCommandReceivedEvent;

        if ( getResponse() )
        {
            callTrainCommandReceivedEvent = boost::dynamic_pointer_cast< CallTrainCommandReceivedEvent > ( getResponse() );
        }

        FUNCTION_EXIT;
        return callTrainCommandReceivedEvent;
    }

}
