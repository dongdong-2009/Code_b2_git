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

#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"

namespace TA_IRS_Bus
{

    namespace ProtocolPecTypes
    {

        std::string pecCarNumberToString( EPecCarNumber pecCarNumber )
        {
            switch ( pecCarNumber )
            {

                case AllCars:
                    return "All Cars";
                    break;

                case CarOne:
                    return "Car 1";
                    break;

                case CarTwo:
                    return "Car 2";
                    break;

                case CarThree:
                    return "Car 3";
                    break;

                default:
                    break;
            }

            return "Unknown EPecCarNumber";
        }


        std::string pecNumberToString( EPecNumber pecNumber )
        {
            switch ( pecNumber )
            {

                case AllPec:
                    return "All Pec";
                    break;

                case PecOne:
                    return "Pec 1";
                    break;

                case PecTwo:
                    return "Pec 2";
                    break;

                case PecThree:
                    return "Pec 3";
                    break;

                case PecFour:
                    return "Pec 4";
                    break;

                default:
                    break;
            }

            return "Unknown EPecNumber";
        }

		std::string pecFlagToString( EPECFlagNumber flagNumber )
        {
            switch ( flagNumber )
            {
				
			case Request:
				return "Request";
				break;
				
			case Acknowledge:
				return "Acknowledge";
				break;
				
			case Error:
				return "Error";
				break;
				
			default:
				break;
            }
			
            return "Unknown EFlagNumber";
        }


        std::string pecCommandResultToString( EPecCommandResult pecCommandResult )
        {
            switch ( pecCommandResult )
            {

                case PecProceeding:
                    return "Proceeding";
                    break;

                case PecInUse:
                    return "In Use";
                    break;

                case PecFailed:
                    return "Failed";
                    break;

                default:
                    break;
            }

            return "Unknown EPecCommandResult";
        }
    }

}

