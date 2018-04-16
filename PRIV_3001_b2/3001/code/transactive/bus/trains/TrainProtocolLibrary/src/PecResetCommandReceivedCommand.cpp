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
#include "bus/trains/TrainProtocolLibrary/src/PecResetCommandReceivedCommand.h"
#include "bus/trains/TrainProtocolLibrary/src/PecResetEvent.h"

#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PecResetCommandReceivedCommand::MESSAGE_TYPE = 59;
	//const unsigned short MESSAGE_LENGTH = 10;
	const unsigned short PECRESET_STATUS = 7 ;

    PecResetCommandReceivedCommand::PecResetCommandReceivedCommand( CommonTypes::TrainIdType trainId,
                                        unsigned char origin,
                                        ProtocolPecTypes::EPecCarNumber carNumber,
										ProtocolPecTypes::EPecNumber pecNumber, TA_IRS_Bus::ProtocolPecTypes::EPecCommandResult status,
										PecTypes::EPecTrainSource pecTrainSource )
            : AbstractPecCommand( trainId, MESSAGE_TYPE, origin, carNumber, pecNumber, false, pecTrainSource ,10 )
    {
        FUNCTION_ENTRY( "PecResetCommandReceivedCommand" );

       // addResponseType( PecResetEvent::getStaticType() );
		setByte ( PECRESET_STATUS , status ) ;

        FUNCTION_EXIT;
    }


    PecResetCommandReceivedCommand::~PecResetCommandReceivedCommand()
    {
        FUNCTION_ENTRY( "~PecResetCommandReceivedCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult PecResetCommandReceivedCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );
		
        FUNCTION_EXIT;
        return TrainCommand::TrainResponseResult( false, false );
    }


}
