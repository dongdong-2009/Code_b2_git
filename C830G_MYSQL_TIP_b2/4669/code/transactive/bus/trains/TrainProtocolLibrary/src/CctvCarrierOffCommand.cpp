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

#include "bus/trains/TrainProtocolLibrary/src/CctvCarrierOffCommand.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType CctvCarrierOffCommand::MESSAGE_TYPE = 2;
    const unsigned short CctvCarrierOffCommand::MESSAGE_LENGTH = 7;


    CctvCarrierOffCommand::CctvCarrierOffCommand( CommonTypes::TrainIdType trainId, unsigned char origin )
            : AbstractCctvCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin )
    {
        FUNCTION_ENTRY( "CctvCarrierOffCommand" );
        FUNCTION_EXIT;
    }

    
    CctvCarrierOffCommand::~CctvCarrierOffCommand()
    {
        FUNCTION_ENTRY( "~CctvCarrierOffCommand" );
        FUNCTION_EXIT;
    }

}
