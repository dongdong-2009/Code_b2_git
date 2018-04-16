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
#include "bus/trains/TrainProtocolLibrary/src/AbstractTtisCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/TtisCommandReceivedEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{

    AbstractTtisCommand::AbstractTtisCommand( CommonTypes::TrainIdType trainId,
                                              unsigned int length,
                                              TrainMessageType commandType,
                                              unsigned char origin,
                                              ETtisOriginalCommand originalCommand )
        : TimsCommand( trainId, length, commandType, origin ),
          m_originalCommand( originalCommand )
    {
        FUNCTION_ENTRY( "AbstractTtisCommand" );

        // Adds a TtisCommandReceivedEventReader::getStaticType() as a response
        addResponseType( TtisCommandReceivedEvent::getStaticType() );

        FUNCTION_EXIT;
    }


    AbstractTtisCommand::~AbstractTtisCommand()
    {
        FUNCTION_ENTRY( "~AbstractTtisCommand" );
        FUNCTION_EXIT;
    }


    TtisCommandReceivedEventPtr AbstractTtisCommand::getCommandReceivedResponse()
    {
        FUNCTION_ENTRY( "getCommandReceivedResponse" );

        TtisCommandReceivedEventPtr ttisCommandReceivedEventPtr;

        if ( getResponse() )
        {
            ttisCommandReceivedEventPtr = boost::dynamic_pointer_cast<TtisCommandReceivedEvent>( getResponse() );
        }

        return  ttisCommandReceivedEventPtr;
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult AbstractTtisCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );

        TtisCommandReceivedEventPtr ttisCommandReceivedEvet = boost::dynamic_pointer_cast<TtisCommandReceivedEvent>( response );

        if ( ttisCommandReceivedEvet )
        {
            if ( ttisCommandReceivedEvet->getOriginalCommand() == m_originalCommand )
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
        return TrainResponseResult( false, false );
    }
}
