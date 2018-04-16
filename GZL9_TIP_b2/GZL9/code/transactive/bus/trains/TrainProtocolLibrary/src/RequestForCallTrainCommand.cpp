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
#include "bus/trains/TrainProtocolLibrary/src/RequestForCallTrainCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType RequestForCallTrainCommand::MESSAGE_TYPE = 74;
    const unsigned short RequestForCallTrainCommand::MESSAGE_LENGTH = 8;
    const unsigned short RequestForCallTrainCommand::REQUEST_MODE_POSITION = 5;


    RequestForCallTrainCommand::RequestForCallTrainCommand( CommonTypes::TrainIdType trainId,
                                                            unsigned char origin,
                                                            ECallTrainRequestMode requestMode )
        : TimsCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, true ),
          m_requestMode( requestMode )

    {
        FUNCTION_ENTRY( "RequestForCallTrainCommand" );

        setByte( REQUEST_MODE_POSITION, static_cast<unsigned char>( m_requestMode ) );

        // Adds a CallTrainCommandReceivedEventReader as a response
        addResponseType( CallTrainCommandReceivedEvent::getStaticType() );

        FUNCTION_EXIT;
    }


    RequestForCallTrainCommand::~RequestForCallTrainCommand()
    {
        FUNCTION_ENTRY( "~RequestForCallTrainCommand" );
        FUNCTION_EXIT;
    }


    std::string RequestForCallTrainCommand::getLogString()
    {
        FUNCTION_ENTRY( "getLogString" );

        // appends more information in the format:
        // "[Request=X]"
        std::ostringstream logStream;
        logStream << TimsCommand::getLogString();
        logStream << "[Request=" << static_cast<int>( m_requestMode ) << "]";

        FUNCTION_EXIT;
        return logStream.str();
    }


    CallTrainCommandReceivedEventPtr RequestForCallTrainCommand::getCommandReceivedResponse()
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
