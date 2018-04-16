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

#include "bus/trains/TrainProtocolLibrary/src/TtisScheduleUpgradeCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType TtisScheduleUpgradeCommand::MESSAGE_TYPE = 45;
    const unsigned short TtisScheduleUpgradeCommand::MESSAGE_LENGTH = 7;

    
    TtisScheduleUpgradeCommand::TtisScheduleUpgradeCommand( CommonTypes::TrainIdType trainId, unsigned char origin )
            : AbstractTtisCommand( trainId,
                                   MESSAGE_LENGTH,
                                   MESSAGE_TYPE,
                                   origin,
                                   TtisScheduleUpgrade )
    {
        FUNCTION_ENTRY( "TtisScheduleUpgradeCommand" );
        FUNCTION_EXIT;
    }


    TtisScheduleUpgradeCommand::~TtisScheduleUpgradeCommand()
    {
        FUNCTION_ENTRY( "~TtisScheduleUpgradeCommand" );
        FUNCTION_EXIT;
    }
}
