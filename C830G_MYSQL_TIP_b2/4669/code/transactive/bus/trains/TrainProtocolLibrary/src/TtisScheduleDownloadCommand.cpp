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

#include "bus/trains/TrainProtocolLibrary/src/TtisScheduleDownloadCommand.h"

#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_Bus
{
    const TrainMessageType TtisScheduleDownloadCommand::MESSAGE_TYPE = 44;
    const unsigned short TtisScheduleDownloadCommand::MESSAGE_LENGTH = 7;


    TtisScheduleDownloadCommand::TtisScheduleDownloadCommand( CommonTypes::TrainIdType trainId, unsigned char origin )
            : AbstractTtisCommand( trainId,
                                   MESSAGE_LENGTH,
                                   MESSAGE_TYPE,
                                   origin,
                                   TtisScheduleDownload )
    {
        FUNCTION_ENTRY( "TtisScheduleDownloadCommand" );
        FUNCTION_EXIT;
    }


    TtisScheduleDownloadCommand::~TtisScheduleDownloadCommand()
    {
        FUNCTION_ENTRY( "~TtisScheduleDownloadCommand" );
        FUNCTION_EXIT;
    }

}
