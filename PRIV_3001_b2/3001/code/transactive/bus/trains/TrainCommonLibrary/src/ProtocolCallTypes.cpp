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

#include "bus/trains/TrainCommonLibrary/src/ProtocolCallTypes.h"

namespace TA_IRS_Bus
{

    namespace ProtocolCallTypes
    {

        /**
         * Utility functions to print simple types to string
         */
        std::string callTrainCommandStatusToString( ECallTrainCommandStatus callTrainCommandStatus )
        {
            switch ( callTrainCommandStatus )
            {

                case CallTrainProceeding:
                    return "Proceeding";
                    //break;		// cannot be passes

                case CallTrainAudioRouteInUse:
                    return "Audio Route In Use";
                    //break;		// cannot be passes

                case CallTrainAudioRouteInUseOptionToOverride:
                    return "Audio Route In Use - Option To Override";
                    //break;

                case CallTrainReset:
                    return "Reset";
                    //break;		// cannot be passes

                default:
                    break;
            }

            return "Unknown ECallTrainCommandStatus";
        }

    }

}
