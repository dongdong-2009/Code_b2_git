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

#if !defined(ProtocolTtisTypes_H)
#define ProtocolTtisTypes_H

#include <string>

namespace TA_IRS_Bus
{

    namespace ProtocolTtisTypes
    {

        /**
         * Represents TTIS PID selection
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:45 PM
         */
        enum ETtisPid
        {
            TtisPidAllTrain = 0x0,
            TtisPidCar1 = 0x10,
            TtisPidCar2 = 0x20,
            TtisPidCar3 = 0x30,
            TtisPidCar1Pid1 = 0x11,
            TtisPidCar1Pid2 = 0x12,
            TtisPidCar1Pid3 = 0x13,
            TtisPidCar1Pid4 = 0x14,
            TtisPidCar2Pid1 = 0x21,
            TtisPidCar2Pid2 = 0x22,
            TtisPidCar2Pid3 = 0x23,
            TtisPidCar2Pid4 = 0x24,
            TtisPidCar3Pid1 = 0x31,
            TtisPidCar3Pid2 = 0x32,
            TtisPidCar3Pid3 = 0x33,
            TtisPidCar3Pid4 = 0x34
        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:45 PM
         */
        enum ETtisFontSize
        {
            TtisFontSmall = 0,
            TtisFontLarge = 2
        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:45 PM
         */
        enum ETtisJustification
        {
            TtisJustificationLeft = 0,
            TtisJustificationCentre = 1,
            TtisJustificationRight = 2
        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:45 PM
         */
        enum ETtisIntensity
        {
            TtisIntensityNormal = 0,
            TtisIntensityHigh = 1
        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:45 PM
         */
        enum ETtisDisplayMode
        {
            TtisDisplayModeScrollLeft = 0,
            TtisDisplayModeScrollRight = 1,
            TtisDisplayModeScrollUp = 2,
            TtisDisplayModeScrollDown = 3,
            TtisDisplayModeInstantOn = 4,
            TtisDisplayModeBlinking = 5,
            TtisDisplayModeWiping = 6,
            TtisDisplayModeLikeSnow = 7
        };


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:45 PM
         */
        enum ETtisPriority
        {
            ETtisPriorityOne = 1,
            ETtisPriorityTwo = 2,
            ETtisPriorityThree = 3,
            ETtisPriorityFour = 4,
            ETtisPriorityFive = 5,
            ETtisPrioritySix = 6,
            ETtisPrioritySeven = 7,
            ETtisPriorityEight = 8
        };


        /**
         * The type of message to reset
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:45 PM
         */
        enum ETtisResetMessageType
        {
            TtisResetAll = 0,
            TtisResetEmergency = 1,
            TtisResetNormal = 2,
            TtisResetPriority6 = 4
        };


        /**
         * Result of a TTIS command
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:45 PM
         */
        enum ETtisResult
        {
            TtisProceeding = 1,
            TtisInUse = 2,
            TtisLibraryUpgraded = 3,
            TtisReset = 4,
            TtisFailed = 5,
            TtisDifferentVersion = 10,
			TtisDutyHandover = 20
        };


        /**
         * Represents an hour/minute time for start/stop times in TTIS messages.
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:45 PM
         */

        struct TtisTime
        {

            /**
             * hour, 0-23
             */
            unsigned char hour;

            /**
             * minute, 0-59
             */
            unsigned char minute;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * Utility functions to print simple types to string
         */
        std::string ttisPidToString( ETtisPid ttisPid );
        std::string ttisFontSizeToString( ETtisFontSize ttisFontSize );
        std::string ttisJustificationToString( ETtisJustification ttisJustification );
        std::string ttisIntensityToString( ETtisIntensity ttisIntensity );
        std::string ttisDisplayModeToString( ETtisDisplayMode ttisDisplayMode );
        std::string ttisPriorityToString( ETtisPriority ttisPriority );
        std::string ttisResetMessageTypeToString( ETtisResetMessageType ttisResetMessageType );
        std::string ttisResultToString( ETtisResult ttisResult );

    }

}

#endif // !defined(ProtocolTtisTypes_H)
