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

#if !defined(ProtocolPecTypes_H)
#define ProtocolPecTypes_H

#include <string>

namespace TA_IRS_Bus
{

    namespace ProtocolPecTypes
    {

        /**
         * Represents a car
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:43 PM
         */
        enum EPecCarNumber
        {
            AllCars = 0,
            CarOne = 1,
            CarTwo = 2,
            CarThree = 3
        };


        /**
         * Represents a PEC within a car
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:43 PM
         */
        enum EPecNumber
        {
            AllPec = 0,
            PecOne = 1,
            PecTwo = 2,
            PecThree = 3,
            PecFour = 4
        };
		
		/**
         * Represents a PEC Event flag
         * @author Li Chao
         * @version 1.0
         * @created 8-April-2009 3:50:43 PM
         */
        enum EPECFlagNumber
        {
            Error = 0,
            Request = 1,
            Acknowledge = 2
        };

        /**
         * The result of a PEC command
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:43 PM
         */
        enum EPecCommandResult
        {
            PecProceeding = 1,
            PecInUse = 2,
            PecFailed = 3
        };


        /**
         * Utility functions to print simple types to string
         */
        std::string pecCarNumberToString( EPecCarNumber pecCarNumber );
        std::string pecNumberToString( EPecNumber pecNumber );
        std::string pecCommandResultToString( EPecCommandResult pecCommandResult );
		std::string pecFlagToString( EPECFlagNumber flagNumber );
    }

}

#endif // !defined(ProtocolPecTypes_H)
