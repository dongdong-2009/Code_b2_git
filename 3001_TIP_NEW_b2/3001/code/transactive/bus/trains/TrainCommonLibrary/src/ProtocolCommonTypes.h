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

#if !defined(ProtocolCommonTypes_H)
#define ProtocolCommonTypes_H

#include <string>

namespace TA_IRS_Bus
{

    namespace ProtocolCommonTypes
    {

        /**
         * Represents an error from the train
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:41 PM
         */
        enum EBadCommandError
        {
            BadChecksum = 1,
            UnknownCommand = 2,
            BadParameter = 3,
            UnexpectedCommand = 4
        };


        /**
         * Represents MPU Main or MPU Backup
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:41 PM
         */
        enum ETimsOrigin
        {
            MpuMain = 0,
            MpuBackup = 1,
			DriverCallAcsu = 9 //Lucky 4/13/2011
        };


        /**
         * Utility functions to print simple types to string
         */
        std::string badCommandErrorToString( EBadCommandError badCommandError );
        std::string timsOriginToString( ETimsOrigin timsOrigin );

    }

}

#endif // !defined(ProtocolCommonTypes_H)
