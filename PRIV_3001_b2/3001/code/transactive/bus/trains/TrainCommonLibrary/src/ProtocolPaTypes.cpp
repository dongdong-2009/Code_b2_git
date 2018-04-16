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

#include "bus/trains/TrainCommonLibrary/src/ProtocolPaTypes.h"

namespace TA_IRS_Bus
{

    namespace ProtocolPaTypes
    {

        std::string paRequestModeToString( EPaRequestMode paRequestMode )
        {
            switch ( paRequestMode )
            {

                case PaRequest:
                    return "Request";
                    break;

                case PaOverride:
                    return "Override";
                    break;

                default:
                    break;
            }

            return "Unknown EPaRequestMode";
        }


        std::string paCommandResultToString( EPaCommandResult paCommandResult )
        {
            switch ( paCommandResult )
            {

                case PaProceeding:
                    return "Proceeding";
                    break;

                case PaInUse:
                    return "In Use";
                    break;

                case PaInUseOptionToOverride:
                    return "In Use - Option To Override";
                    break;

                case PaInUseFirstCycleCancelled:
                    return "In Use - First Cycle Cancelled";
                    break;

                case PaReset:
                    return "Reset";
                    break;

                case PaFailed:
                    return "Failed";
                    break;

                case PaDifferentOrigin:
                    return "Different Origin";
                    break;

                case PaDifferentId:
                    return "Different Id";
                    break;

                case PaDifferentVersion:
                    return "Different Version";
                    break;

                default:
                    break;
            }

            return "Unknown EPaCommandResult";
        }


        std::string messageCompletionStatusToString( EMessageCompletionStatus messageCompletionStatus )
        {
            switch ( messageCompletionStatus )
            {

                case Successful:
                    return "Successful";
                    break;

                case InterruptedByOcc:
                    return "Interrupted By Occ";
                    break;

                case InterruptedByPsc:
                    return "Interrupted By Psc";
                    break;

                case InterruptedByDriver:
                    return "Interrupted By Driver";
                    break;

                default:
                    break;
            }

            return "Unknown EMessageCompletionStatus";
        }

    }
}

