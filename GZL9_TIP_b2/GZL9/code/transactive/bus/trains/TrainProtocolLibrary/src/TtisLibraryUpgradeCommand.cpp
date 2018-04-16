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

#include "bus/trains/TrainProtocolLibrary/src/TtisLibraryUpgradeCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType  TtisLibraryUpgradeCommand::MESSAGE_TYPE = 43;
    const unsigned short TtisLibraryUpgradeCommand::MESSAGE_LENGTH = 7;


    TtisLibraryUpgradeCommand::TtisLibraryUpgradeCommand( CommonTypes::TrainIdType trainId, unsigned char origin )
            : AbstractTtisCommand( trainId,
                                   MESSAGE_LENGTH,
                                   MESSAGE_TYPE, origin,
                                   TtisLibraryUpgrade )
    {
        FUNCTION_ENTRY( "TtisLibraryUpgradeCommand" );
        FUNCTION_EXIT;
    }


    TtisLibraryUpgradeCommand::~TtisLibraryUpgradeCommand()
    {
        FUNCTION_ENTRY( "~TtisLibraryUpgradeCommand" );
        FUNCTION_EXIT;
    }

}
