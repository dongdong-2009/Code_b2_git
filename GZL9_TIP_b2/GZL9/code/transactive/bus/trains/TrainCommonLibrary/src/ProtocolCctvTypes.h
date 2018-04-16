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

#if !defined(ProtocolCctvTypes_H)
#define ProtocolCctvTypes_H

#include <string>

namespace TA_IRS_Bus
{

    namespace ProtocolCctvTypes
    {

        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:42 PM
         */
        enum ECctvCamera
        {
            CameraNone = 0,
            CameraOne = 1,
            CameraTwo = 2,
            CameraThree = 3,
            CameraFour = 4,
            CameraFive = 5,
            CameraSix = 6,
            CameraSeven = 7,
            CameraEight = 8
        };


        /**
         * There are a fixed number of sequences, this reduces the choices to just those.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:42 PM
         */
        enum ECctvSequenceNumber
        {
            SequenceOne = 1,
            SequenceTwo = 2,
            SequenceThree = 3,
            SequenceFour = 4,
            SequenceFive = 5,
            SequenceSix = 6
        };


        /**
         * Represents the sequence mode
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:42 PM
         */
        enum ECctvSequenceMode
        {
            SequenceHold = 0,
            SequenceForward = 1,
            SequenceReverse = 2,
            SequenceSkip = 3
        };


        /**
         * Result of a CCTV command
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:42 PM
         */
        enum ECctvCommandStatus
        {
            CctvProceeding = 1,
            CctvInUse = 2,
            CctvFailed = 3
        };


        /**
         * Utility functions to print simple types to string
         */
        std::string cctvCameraToString( ECctvCamera cctvCamera );
        std::string cctvSequenceNumberToString( ECctvSequenceNumber cctvSequenceNumber );
        std::string cctvSequenceModeToString( ECctvSequenceMode cctvSequenceMode );
        std::string cctvCommandStatusToString( ECctvCommandStatus cctvCommandStatus );

    }
}

#endif // !defined(ProtocolCctvTypes_H)
