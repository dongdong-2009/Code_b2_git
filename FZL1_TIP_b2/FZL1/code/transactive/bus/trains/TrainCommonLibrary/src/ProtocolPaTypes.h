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
  * These are types defined in the interface document.
  * The values assigned to enumerations are defined by this document
  * and cannot be changed (unless the document is changed)
  */

#if !defined(ProtocolPaTypes_H)
#define ProtocolPaTypes_H

#include <string>

namespace TA_IRS_Bus
{

    namespace ProtocolPaTypes
    {

        /**
         * Whether a normal request or override.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:42 PM
         */
        enum EPaRequestMode
        {
            PaRequest = 0,
            PaOverride = 3
        };


        /**
         * The return status of a PA command
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:42 PM
         */
        enum EPaCommandResult
        {
            PaProceeding = 1,
            PaInUse = 2,
            PaInUseOptionToOverride = 3,
            PaInUseFirstCycleCancelled = 4,
            PaReset = 5,
            PaFailed = 6,
            PaDifferentOrigin = 8,
            PaDifferentId = 9,
            PaDifferentVersion = 10
        };


        /**
         * This is the status of a cyclic or prerecorded message completion
         * @author adamr
         * @version 1.0
         * @created 18-Mar-2008 3:18:59 PM
         */
        enum EMessageCompletionStatus
        {
            Successful = 1,
            InterruptedByOcc = 2,
            InterruptedByPsc = 3,
            InterruptedByDriver = 4
        };


        /**
         * Utility functions to print simple types to string
         */
        std::string paRequestModeToString( EPaRequestMode paRequestMode );
        std::string paCommandResultToString( EPaCommandResult paCommandResult );
        std::string messageCompletionStatusToString( EMessageCompletionStatus messageCompletionStatus );

    }
}

#endif // !defined(ProtocolPaTypes_H)
