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
#include "bus/trains/TrainProtocolLibrary/src/PecAnswerCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PecCommandReceivedEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PecAnswerCommand::MESSAGE_TYPE = 51;


    PecAnswerCommand::PecAnswerCommand( CommonTypes::TrainIdType trainId,
                                        unsigned char origin,
                                        ProtocolPecTypes::EPecCarNumber carNumber,
                                        ProtocolPecTypes::EPecNumber pecNumber )
            : AbstractPecCommand( trainId, MESSAGE_TYPE, origin, carNumber, pecNumber, true )
    {
        FUNCTION_ENTRY( "PecAnswerCommand" );

        addResponseType( PecCommandReceivedEvent::getStaticType() );

        FUNCTION_EXIT;
    }


    PecAnswerCommand::~PecAnswerCommand()
    {
        FUNCTION_ENTRY( "~PecAnswerCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult PecAnswerCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );

        PecCommandReceivedEventPtr pecCommandReceivedEvent = boost::dynamic_pointer_cast< PecCommandReceivedEvent > ( response );

        if ( pecCommandReceivedEvent )
        {
            if ( ( pecCommandReceivedEvent->getCar() == m_carNumber ) &&
                 ( pecCommandReceivedEvent->getPec() == m_pecNumber )   )
            {
                FUNCTION_EXIT;
                return AbstractPecCommand::setResponse( response );
            }
        }
        else
        {
            FUNCTION_EXIT;
            return AbstractPecCommand::setResponse( response );
        }

        FUNCTION_EXIT;
        return TrainCommand::TrainResponseResult( false, false );
    }


    PecCommandReceivedEventPtr PecAnswerCommand::getCommandReceivedResponse()
    {
        FUNCTION_ENTRY( "getCommandReceivedResponse" );
        PecCommandReceivedEventPtr pecCommandReceivedEvent;

        if ( getResponse() )
        {
            pecCommandReceivedEvent = boost::dynamic_pointer_cast< PecCommandReceivedEvent >( getResponse() );
        }

        FUNCTION_EXIT;
        return pecCommandReceivedEvent;
    }

}
