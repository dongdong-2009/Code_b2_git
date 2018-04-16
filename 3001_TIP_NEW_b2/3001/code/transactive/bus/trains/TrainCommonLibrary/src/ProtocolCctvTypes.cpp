/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * These are types defined in the interface document.
  * The values assigned to enumerations are defined by this document
  * and cannot be changed (unless the document is changed)
  */

#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"

namespace TA_IRS_Bus
{

    namespace ProtocolCctvTypes
    {

        std::string cctvCameraToString( ECctvCamera cctvCamera )
        {
            switch ( cctvCamera )
            {

                case CameraNone:
                    return "No Camera";
                    //break;

                case CameraOne:
                    return "Camera 1";
                    //break;

                case CameraTwo:
                    return "Camera 2";
                    //break;

                case CameraThree:
                    return "Camera 3";
                    //break;

                case CameraFour:
                    return "Camera 4";
                    //break;

                case CameraFive:
                    return "Camera 5";
                    //break;

                case CameraSix:
                    return "Camera 6";
                    //break;

                case CameraSeven:
                    return "Camera 7";
                    //break;

                case CameraEight:
                    return "Camera 8";
                   // break;

                default:
                    break;
            }

            return "Unknown ECctvCamera";
        }


        std::string cctvSequenceNumberToString( ECctvSequenceNumber cctvSequenceNumber )
        {
            switch ( cctvSequenceNumber )
            {

                case SequenceOne:
                    return "Sequence 1";
                    //break;

                case SequenceTwo:
                    return "Sequence 2";
                    //break;

                case SequenceThree:
                    return "Sequence 3";
                    //break;

                case SequenceFour:
                    return "Sequence 4";
                    //break;

                case SequenceFive:
                    return "Sequence 5";
                    //break;

                case SequenceSix:
                    return "Sequence 6";
                    //break;

                default:
                    break;
            }

            return "Unknown ECctvSequenceNumber";
        }


        std::string cctvSequenceModeToString( ECctvSequenceMode cctvSequenceMode )
        {
            switch ( cctvSequenceMode )
            {

                case SequenceHold:
                    return "Hold";
                    //break;

                case SequenceForward:
                    return "Forward";
                    //break;

                case SequenceReverse:
                    return "Reverse";
                    //break;

                case SequenceSkip:
                    return "Skip";
                    //break;

                default:
                    break;
            }

            return "Unknown ECctvSequenceMode";
        }


        std::string cctvCommandStatusToString( ECctvCommandStatus cctvCommandStatus )
        {
            switch ( cctvCommandStatus )
            {

                case CctvProceeding:
                    return "Proceeding";
                    //break;

                case CctvInUse:
                    return "In Use";
                    //break;

                case CctvFailed:
                    return "Failed";
                    //break;

                default:
                    break;
            }

            return "Unknown ECctvCommandStatus";
        }

    }
}

