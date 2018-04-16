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
  *
  */

#include "bus/trains/TrainCommonLibrary/src/ProtocolCommonTypes.h"

namespace TA_IRS_Bus
{

    namespace ProtocolCommonTypes
    {

        std::string badCommandErrorToString( EBadCommandError badCommandError )
        {
            switch ( badCommandError )
            {

                case BadChecksum:
                    return "Bad Checksum";
                    break;

                case UnknownCommand:
                    return "Unknown Command";
                    break;

                case BadParameter:
                    return "Bad Parameter";
                    break;

                case UnexpectedCommand:
                    return "Unexpected Command";
                    break;

                default:
                    break;
            }

            return "Unknown EBadCommandError";
        }


        std::string timsOriginToString( ETimsOrigin timsOrigin )
        {
            switch ( timsOrigin )
            {

                case MpuMain:
                    return "Mpu Main";
                    break;

                case MpuBackup:
                    return "Mpu Backup";
                    break;

                default:
                    break;
            }

            return "Unknown ETimsOrigin";
        }

    }

}

