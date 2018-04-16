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
#if !defined(ETtisOriginalCommand_H)
#define ETtisOriginalCommand_H

namespace TA_IRS_Bus
{

    /**
     * Original command number for TTIS
     * @author Adam Radics
     * @version 1.0
     * @created 05-Feb-2008 3:20:08 PM
     */
    enum ETtisOriginalCommand
    {
        TtisFreeTextMessage = 0,
        TtisPreStoredMessage = 1,
        TtisLibraryDownload = 2,
        TtisResetMessage = 4,
        TtisScheduleDownload = 5,
        TtisScheduleUpgrade = 6,
        TtisLibraryUpgrade = 7
    };

}

#endif // !defined(ETtisOriginalCommand_H)
