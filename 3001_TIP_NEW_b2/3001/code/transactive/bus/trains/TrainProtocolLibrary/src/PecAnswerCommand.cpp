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
										ProtocolPecTypes::EPecNumber pecNumber, PecTypes::EPecTrainSource pecTrainSource )
            : AbstractPecCommand( trainId, MESSAGE_TYPE, origin, carNumber, pecNumber, true, pecTrainSource )
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
		LOGBEFORECOTS("boost::dynamic_pointer_cast");
        PecCommandReceivedEventPtr pecCommandReceivedEvent = boost::dynamic_pointer_cast< PecCommandReceivedEvent > ( response );
		LOGAFTERCOTS("boost::dynamic_pointer_cast");
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
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
            pecCommandReceivedEvent = boost::dynamic_pointer_cast< PecCommandReceivedEvent >( getResponse() );
			LOGAFTERCOTS("boost::dynamic_pointer_cast");
        }

        FUNCTION_EXIT;
        return pecCommandReceivedEvent;
    }

}
