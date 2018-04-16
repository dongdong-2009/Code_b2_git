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
#include "bus/trains/TrainProtocolLibrary/src/PecResetCommandReceivedEvent.h"

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
									  PecTypes::EPecTrainSource pecTrainSource,
                                      bool responseExpected )
            : AbstractPecCommand( trainId, MESSAGE_TYPE, origin, carNumber, pecNumber, responseExpected, pecTrainSource )
    {
        FUNCTION_ENTRY( "PecResetCommand" );

        //setByte( PEC_RQSORACK_POSITION, rqsOrAck ); //Bug : It will override the bit position of PEC Source

        if ( true == responseExpected )
        {
            addResponseType( PecResetCommandReceivedEvent::getStaticType() );
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
		LOGBEFORECOTS("boost::dynamic_pointer_cast");
        PecResetCommandReceivedEventPtr pecResetCommandReceivedEvent = boost::dynamic_pointer_cast< PecResetCommandReceivedEvent > ( response );
		LOGAFTERCOTS("boost::dynamic_pointer_cast");
        if ( pecResetCommandReceivedEvent )
        {
            if ( ( pecResetCommandReceivedEvent->getCar() == m_carNumber ) &&
                 ( pecResetCommandReceivedEvent->getPec() == m_pecNumber ) )
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


    const PecResetCommandReceivedEventPtr PecResetCommand::getPecResetResponse()
    {
        FUNCTION_ENTRY( "getPecResetResponse" );

        PecResetCommandReceivedEventPtr pecResetEvent;

        if ( getResponse() )
        {
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
            pecResetEvent = boost::dynamic_pointer_cast< PecResetCommandReceivedEvent > ( getResponse() );
			LOGAFTERCOTS("boost::dynamic_pointer_cast");
        }

        FUNCTION_EXIT;

        return pecResetEvent;
    }

}
