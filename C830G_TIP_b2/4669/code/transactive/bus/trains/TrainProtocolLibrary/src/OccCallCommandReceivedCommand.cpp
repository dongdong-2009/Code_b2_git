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

#include "bus/trains/TrainProtocolLibrary/src/OccCallCommandReceivedCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType OccCallCommandReceivedCommand::MESSAGE_TYPE = 68;
    const unsigned short OccCallCommandReceivedCommand::MESSAGE_LENGTH = 7;


    OccCallCommandReceivedCommand::OccCallCommandReceivedCommand( CommonTypes::TrainIdType trainId, unsigned char origin )
            : TimsCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, false )
    {
        FUNCTION_ENTRY( "OccCallCommandReceivedCommand" );
        FUNCTION_EXIT;
    }


    OccCallCommandReceivedCommand::~OccCallCommandReceivedCommand()
    {
        FUNCTION_ENTRY( "~OccCallCommandReceivedCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult OccCallCommandReceivedCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );
        FUNCTION_EXIT;
        return TrainResponseResult( false, false );
    }

}
