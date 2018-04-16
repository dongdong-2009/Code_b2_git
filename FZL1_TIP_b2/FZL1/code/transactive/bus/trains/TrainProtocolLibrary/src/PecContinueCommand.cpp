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
#include "bus/trains/TrainProtocolLibrary/src/PecContinueCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType PecContinueCommand::MESSAGE_TYPE = 58;


    PecContinueCommand::PecContinueCommand( CommonTypes::TrainIdType trainId,
                                            unsigned char origin,
                                            ProtocolPecTypes::EPecCarNumber carNumber,
                                            ProtocolPecTypes::EPecNumber pecNumber )
            : AbstractPecCommand( trainId, MESSAGE_TYPE, origin, carNumber, pecNumber, false )
    {
        FUNCTION_ENTRY( "PecContinueCommand" );
        FUNCTION_EXIT;
    }


    PecContinueCommand::~PecContinueCommand()
    {
        FUNCTION_ENTRY( "~PecContinueCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult PecContinueCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );
        FUNCTION_EXIT;
        return TrainResponseResult( false, false );
    }

}
