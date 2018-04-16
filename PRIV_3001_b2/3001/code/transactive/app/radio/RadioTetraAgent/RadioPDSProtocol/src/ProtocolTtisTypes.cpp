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

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/ProtocolTtisTypes.h"

#include <sstream>
#include <iomanip>

namespace TA_IRS_App
{

    namespace ProtocolTtisTypes
    {

        std::string ttisPidToString( ETtisPid ttisPid )
        {
			std::string result;
            switch ( ttisPid )
            {

                case TtisPidAllTrain:
                    result = "AllTrain";
                    break;

                case TtisPidCar1:
                    result = "Car 1";
                    break;

                case TtisPidCar2:
                    result = "Car 2";
                    break;

                case TtisPidCar3:
                    result = "Car 3";
                    break;

                case TtisPidCar1Pid1:
                    result = "Car 1 Pid 1";
                    break;

                case TtisPidCar1Pid2:
                    result = "Car 1 Pid 2";
                    break;

                case TtisPidCar1Pid3:
                    result = "Car 1 Pid 3";
                    break;

                case TtisPidCar1Pid4:
                    result = "Car 1 Pid 4";
                    break;

                case TtisPidCar2Pid1:
                    result = "Car 2 Pid 1";
                    break;

                case TtisPidCar2Pid2:
                    result = "Car 2 Pid 2";
                    break;

                case TtisPidCar2Pid3:
                    result = "Car 2 Pid 3";
                    break;

                case TtisPidCar2Pid4:
                    result = "Car 2 Pid 4";
                    break;

                case TtisPidCar3Pid1:
                    result = "Car 3 Pid 1";
                    break;

                case TtisPidCar3Pid2:
                    result = "Car 3 Pid 2";
                    break;

                case TtisPidCar3Pid3:
                    result = "Car 3 Pid 3";
                    break;

                case TtisPidCar3Pid4:
                    result = "Car 3 Pid 4";
                    break;

                default:
					result = "Unknown ETtisPid";
                    break;
            }

            return result;
        }


        std::string ttisFontSizeToString( ETtisFontSize ttisFontSize )
        {
			std::string result;
            switch ( ttisFontSize )
            {

                case TtisFontSmall:
                    result = "Small";
                    break;

                case TtisFontLarge:
                    result = "Large";
                    break;

                default:
					result = "Unknown ETtisFontSize";
                    break;
            }

            return result;
        }


        std::string ttisJustificationToString( ETtisJustification ttisJustification )
        {
			std::string result;
            switch ( ttisJustification )
            {

                case TtisJustificationLeft:
                    result = "Left";
                    break;

                case TtisJustificationCentre:
                    result = "Centre";
                    break;

                case TtisJustificationRight:
                    result = "Right";
                    break;

                default:
					result = "Unknown ETtisJustification";
                    break;
            }

            return result;
        }


        std::string ttisIntensityToString( ETtisIntensity ttisIntensity )
        {
			std::string result;
            switch ( ttisIntensity )
            {

                case TtisIntensityNormal:
                    result = "Normal";
                    break;

                case TtisIntensityHigh:
                    result = "High";
                    break;

                default:
					result = "Unknown ETtisIntensity";
                    break;
            }

            return result;
        }


        std::string ttisDisplayModeToString( ETtisDisplayMode ttisDisplayMode )
        {
			std::string result;
            switch ( ttisDisplayMode )
            {

                case TtisDisplayModeScrollLeft:
                    result = "Scroll Left";
                    break;

                case TtisDisplayModeScrollRight:
                    result = "Scroll Right";
                    break;

                case TtisDisplayModeScrollUp:
                    result = "Scroll Up";
                    break;

                case TtisDisplayModeScrollDown:
                    result = "Scroll Down";
                    break;

                case TtisDisplayModeInstantOn:
                    result = "Instant On";
                    break;

                case TtisDisplayModeBlinking:
                    result = "Blinking";
                    break;

                case TtisDisplayModeWiping:
                    result = "Wiping";
                    break;

                case TtisDisplayModeLikeSnow:
                    result = "Like Snow";
                    break;

                default:
					result = "Unknown ETtisDisplayMode";
                    break;
            }

            return result;
        }


        std::string ttisPriorityToString( ETtisPriority ttisPriority )
        {
			std::string result;
            switch ( ttisPriority )
            {

                case ETtisPriorityOne:
                    result = "1";
                    break;

                case ETtisPriorityTwo:
                    result = "2";
                    break;

                case ETtisPriorityThree:
                    result = "3";
                    break;

                case ETtisPriorityFour:
                    result = "4";
                    break;

                case ETtisPriorityFive:
                    result = "5";
                    break;

                case ETtisPrioritySix:
                    result = "6";
                    break;

                case ETtisPrioritySeven:
                    result = "7";
                    break;

                case ETtisPriorityEight:
                    result = "8";
                    break;

                default:
					result = "Unknown ETtisPriority";
                    break;
            }

            return result;
        }


        std::string ttisResetMessageTypeToString( ETtisResetMessageType ttisResetMessageType )
        {
			std::string result;
            switch ( ttisResetMessageType )
            {

                case TtisResetAll:
                    result = "All";
                    break;

                case TtisResetEmergency:
                    result = "Emergency";
                    break;

                case TtisResetNormal:
                    result = "Normal";
                    break;

                case TtisResetPriority6:
                    result = "Priority 6";
                    break;

                default:
					result = "Unknown ETtisResetMessageType";
                    break;
            }

            return result;
        }


        std::string ttisResultToString( ETtisResult ttisResult )
        {
			std::string result;
            switch ( ttisResult )
            {

                case TtisProceeding:
                    result = "Proceeding";
                    break;

                case TtisInUse:
                    result = "In Use";
                    break;

                case TtisLibraryUpgraded:
                    result = "Library Upgraded";
                    break;

                case TtisReset:
                    result = "Reset";
                    break;

                case TtisFailed:
                    result = "Failed";
                    break;

                case TtisDifferentVersion:
                    result = "Different Version";
                    break;

                default:
					result = "Unknown ETtisResult";
                    break;
            }

            return result;
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

