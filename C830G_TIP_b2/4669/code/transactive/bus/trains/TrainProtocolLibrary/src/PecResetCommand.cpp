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
#include "bus/trains/TrainProtocolLibrary/src/PecResetCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PecResetEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PecResetCommand::MESSAGE_TYPE = 52;
    const unsigned short PecResetCommand::PEC_RQSORACK_POSITION = 6;
    
    PecResetCommand::PecResetCommand( CommonTypes::TrainIdType trainId,
                                      unsigned char origin,
                                      ProtocolPecTypes::EPecCarNumber carNumber,
                                      ProtocolPecTypes::EPecNumber pecNumber,
									  ProtocolPecTypes::EPECFlagNumber rqsOrAck,
                                      bool responseExpected )
            : AbstractPecCommand( trainId, MESSAGE_TYPE, origin, carNumber, pecNumber, responseExpected )
    {
        FUNCTION_ENTRY( "PecResetCommand" );

        setByte( PEC_RQSORACK_POSITION, rqsOrAck );

        if ( true == responseExpected )
        {
            addResponseType( PecResetEvent::getStaticType() );
        }

        FUNCTION_EXIT;
    }


    PecResetCommand::~PecResetCommand()
    {
        FUNCTION_ENTRY( "~PecResetCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult PecResetCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );

        PecResetEventPtr pecResetEvent = boost::dynamic_pointer_cast< PecResetEvent > ( response );

        if ( pecResetEvent )
        {
            if ( ( pecResetEvent->getCar() == m_carNumber ) &&
                 ( pecResetEvent->getPec() == m_pecNumber ) )
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


    const PecResetEventPtr PecResetCommand::getPecResetResponse()
    {
        FUNCTION_ENTRY( "getPecResetResponse" );

        PecResetEventPtr pecResetEvent;

        if ( getResponse() )
        {
            pecResetEvent = boost::dynamic_pointer_cast< PecResetEvent > ( getResponse() );
        }

        FUNCTION_EXIT;

        return pecResetEvent;
    }

}
