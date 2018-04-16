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
#include "bus/trains/TrainProtocolLibrary/src/PecHoldCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PecActivatedEvent.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PecHoldCommand::MESSAGE_TYPE = 60;


    PecHoldCommand::PecHoldCommand( CommonTypes::TrainIdType trainId,
                                        unsigned char origin,
                                        ProtocolPecTypes::EPecCarNumber carNumber,
										ProtocolPecTypes::EPecNumber pecNumber, PecTypes::EPecTrainSource pecTrainSource )
            : AbstractPecCommand( trainId, MESSAGE_TYPE, origin, carNumber, pecNumber, true, pecTrainSource )
    {
        FUNCTION_ENTRY( "PecHoldCommand" );

        addResponseType( PecActivatedEvent::getStaticType() );

        FUNCTION_EXIT;
    }


    PecHoldCommand::~PecHoldCommand()
    {
        FUNCTION_ENTRY( "~PecHoldCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult PecHoldCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );
		LOGBEFORECOTS("boost::dynamic_pointer_cast");
        PecActivatedEventPtr pecActivatedEvent = boost::dynamic_pointer_cast< PecActivatedEvent > ( response );
		LOGAFTERCOTS("boost::dynamic_pointer_cast");
        if ( pecActivatedEvent )
        {
            if ( ( pecActivatedEvent->getCar() == m_carNumber ) &&
                 ( pecActivatedEvent->getPec() == m_pecNumber )   )
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


    PecActivatedEventPtr PecHoldCommand::getPecActivatedResponse()
    {
        FUNCTION_ENTRY( "getPecActivatedResponse" );
        PecActivatedEventPtr pecActivatedEvent;

        if ( getResponse() )
        {
			LOGBEFORECOTS("boost::dynamic_pointer_cast");
            pecActivatedEvent = boost::dynamic_pointer_cast< PecActivatedEvent >( getResponse() );
			LOGAFTERCOTS("boost::dynamic_pointer_cast");
        }

        FUNCTION_EXIT;
        return pecActivatedEvent;
    }

}
