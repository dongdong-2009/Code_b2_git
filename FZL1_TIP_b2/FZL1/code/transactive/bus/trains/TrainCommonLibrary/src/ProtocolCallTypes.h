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

#if !defined(ProtocolCallTypes_H)
#define ProtocolCallTypes_H

#include <string>

namespace TA_IRS_Bus
{

    namespace ProtocolCallTypes
    {

        /**
         * Represents the result of a call train command
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:41 PM
         */
        enum ECallTrainCommandStatus
        {
            CallTrainProceeding = 1,
            CallTrainAudioRouteInUse = 2,
            CallTrainAudioRouteInUseOptionToOverride = 3,
            CallTrainReset = 5
        };


        /**
         * Utility functions to print simple types to string
         */
        std::string callTrainCommandStatusToString( ECallTrainCommandStatus callTrainCommandStatus );

    }

}

#endif // !defined(ProtocolCallTypes_H)
