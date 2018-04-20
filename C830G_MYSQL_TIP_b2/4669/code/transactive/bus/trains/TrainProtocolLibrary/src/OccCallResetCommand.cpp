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
#include "bus/trains/TrainProtocolLibrary/src/OccCallResetCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType OccCallResetCommand::MESSAGE_TYPE = 69;
    const unsigned short OccCallResetCommand::MESSAGE_LENGTH = 7;


    OccCallResetCommand::OccCallResetCommand( CommonTypes::TrainIdType trainId, unsigned char origin )
            : TimsCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin, false )
    {
        FUNCTION_ENTRY( "OccCallResetCommand" );
        FUNCTION_EXIT;
    }


    OccCallResetCommand::~OccCallResetCommand()
    {
        FUNCTION_ENTRY( "~OccCallResetCommand" );
        FUNCTION_EXIT;
    }


    TrainCommand::TrainResponseResult OccCallResetCommand::setResponse( TrainEventPtr response )
    {
        FUNCTION_ENTRY( "setResponse" );
        FUNCTION_EXIT;
        return TrainResponseResult( false, false );
    }

}
