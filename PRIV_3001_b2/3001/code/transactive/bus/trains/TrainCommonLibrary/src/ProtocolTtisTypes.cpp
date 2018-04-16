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

#include "bus/trains/TrainCommonLibrary/src/ProtocolTtisTypes.h"

#include <sstream>
#include <iomanip>

namespace TA_IRS_Bus
{

    namespace ProtocolTtisTypes
    {

        std::string ttisPidToString( ETtisPid ttisPid )
        {
            switch ( ttisPid )
            {

                case TtisPidAllTrain:
                    return "AllTrain";
                    //break;

                case TtisPidCar1:
                    return "Car 1";
                    //break;

                case TtisPidCar2:
                    return "Car 2";
                    //break;

                case TtisPidCar3:
                    return "Car 3";
                    //break;

                case TtisPidCar1Pid1:
                    return "Car 1 Pid 1";
                    //break;

                case TtisPidCar1Pid2:
                    return "Car 1 Pid 2";
                    //break;

                case TtisPidCar1Pid3:
                    return "Car 1 Pid 3";
                    //break;

                case TtisPidCar1Pid4:
                    return "Car 1 Pid 4";
                    //break;

                case TtisPidCar2Pid1:
                    return "Car 2 Pid 1";
                    //break;

                case TtisPidCar2Pid2:
                    return "Car 2 Pid 2";
                    //break;

                case TtisPidCar2Pid3:
                    return "Car 2 Pid 3";
                    //break;

                case TtisPidCar2Pid4:
                    return "Car 2 Pid 4";
                    //break;

                case TtisPidCar3Pid1:
                    return "Car 3 Pid 1";
                    //break;

                case TtisPidCar3Pid2:
                    return "Car 3 Pid 2";
                    //break;

                case TtisPidCar3Pid3:
                    return "Car 3 Pid 3";
                    //break;

                case TtisPidCar3Pid4:
                    return "Car 3 Pid 4";
                    //break;

                default:
                    break;
            }

            return "Unknown ETtisPid";
        }


        std::string ttisFontSizeToString( ETtisFontSize ttisFontSize )
        {
            switch ( ttisFontSize )
            {

                case TtisFontSmall:
                    return "Small";
                    //break;

                case TtisFontLarge:
                    return "Large";
                    //break;

                default:
                    break;
            }

            return "Unknown ETtisFontSize";
        }


        std::string ttisJustificationToString( ETtisJustification ttisJustification )
        {
            switch ( ttisJustification )
            {

                case TtisJustificationLeft:
                    return "Left";
                    //break;

                case TtisJustificationCentre:
                    return "Centre";
                    //break;

                case TtisJustificationRight:
                    return "Right";
                    //break;

                default:
                    break;
            }

            return "Unknown ETtisJustification";
        }


        std::string ttisIntensityToString( ETtisIntensity ttisIntensity )
        {
            switch ( ttisIntensity )
            {

                case TtisIntensityNormal:
                    return "Normal";
                    //break;

                case TtisIntensityHigh:
                    return "High";
                    //break;

                default:
                    break;
            }

            return "Unknown ETtisIntensity";
        }


        std::string ttisDisplayModeToString( ETtisDisplayMode ttisDisplayMode )
        {
            switch ( ttisDisplayMode )
            {

                case TtisDisplayModeScrollLeft:
                    return "Scroll Left";
                    //break;

                case TtisDisplayModeScrollRight:
                    return "Scroll Right";
                    //break;

                case TtisDisplayModeScrollUp:
                    return "Scroll Up";
                    //break;

                case TtisDisplayModeScrollDown:
                    return "Scroll Down";
                    //break;

                case TtisDisplayModeInstantOn:
                    return "Instant On";
                    //break;

                case TtisDisplayModeBlinking:
                    return "Blinking";
                    //break;

                case TtisDisplayModeWiping:
                    return "Wiping";
                    //break;

                case TtisDisplayModeLikeSnow:
                    return "Like Snow";
                    //break;

                default:
                    break;
            }

            return "Unknown ETtisDisplayMode";
        }


        std::string ttisPriorityToString( ETtisPriority ttisPriority )
        {
            switch ( ttisPriority )
            {

                case ETtisPriorityOne:
                    return "1";
                    //break;

                case ETtisPriorityTwo:
                    return "2";
                    //break;

                case ETtisPriorityThree:
                    return "3";
                    //break;

                case ETtisPriorityFour:
                    return "4";
                    //break;

                case ETtisPriorityFive:
                    return "5";
                    //break;

                case ETtisPrioritySix:
                    return "6";
                    //break;

                case ETtisPrioritySeven:
                    return "7";
                    //break;

                case ETtisPriorityEight:
                    return "8";
                    //break;

                default:
                    break;
            }

            return "Unknown ETtisPriority";
        }


        std::string ttisResetMessageTypeToString( ETtisResetMessageType ttisResetMessageType )
        {
            switch ( ttisResetMessageType )
            {

                case TtisResetAll:
                    return "All";
                    //break;

                case TtisResetEmergency:
                    return "Emergency";
                    //break;

                case TtisResetNormal:
                    return "Normal";
                    //break;

                case TtisResetPriority6:
                    return "Priority 6";
                    //break;

                default:
                    break;
            }

            return "Unknown ETtisResetMessageType";
        }


        std::string ttisResultToString( ETtisResult ttisResult )
        {
            switch ( ttisResult )
            {

                case TtisProceeding:
                    return "Proceeding";
                    //break;

                case TtisInUse:
                    return "In Use";
                    //break;

                case TtisLibraryUpgraded:
                    return "Library Upgraded";
                    //break;

                case TtisReset:
                    return "Reset";
                    //break;

                case TtisFailed:
                    return "Failed";
                    //break;

                case TtisDifferentVersion:
                    return "Different Version";
                    //break;

                default:
                    break;
            }

            return "Unknown ETtisResult";
        }


        std::string TtisTime::getLogString() const
        {
            std::ostringstream result;

            result << std::setw( 2 ) << std::setfill( '0' ) << static_cast< unsigned int>( hour )
                   << ":"
                   << std::setw( 2 ) << std::setfill( '0' ) << static_cast< unsigned int>( minute );

            return result.str();
        }

    }

}

