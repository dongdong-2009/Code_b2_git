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
#include "bus/trains/TrainProtocolLibrary/src/CctvCarrierOnCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType CctvCarrierOnCommand::MESSAGE_TYPE = 1;
    const unsigned short CctvCarrierOnCommand::MESSAGE_LENGTH = 7;

    
    CctvCarrierOnCommand::CctvCarrierOnCommand( CommonTypes::TrainIdType trainId, unsigned char origin )
            : AbstractCctvCommand( trainId, MESSAGE_LENGTH, MESSAGE_TYPE, origin )
    {
        FUNCTION_ENTRY( "CctvCarrierOnCommand" );
        FUNCTION_EXIT;

    }


    CctvCarrierOnCommand::~CctvCarrierOnCommand()
    {
        FUNCTION_ENTRY( "~CctvCarrierOnCommand" );
        FUNCTION_EXIT;
    }

}
