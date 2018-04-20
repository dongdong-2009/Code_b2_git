/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/alarm/alarm_list_control/src/Utilities.h $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains some utility methods for the Alarm List Control.
  */

#ifndef UTILITIES_19_05_2005_H
#define UTILITIES_19_05_2005_H

#include <string>

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	class Utilities 
	{
		public:

            /**
		      * getInstance
		      * 
		      * Returns a single static instance of Utilities
		      *
		      * @return The one and only Utilities instance
		      */
		    static Utilities& getInstance();

            /**
		      * ~Utilities
		      * 
		      * Destructor
		      */
		    virtual ~Utilities() {}

            /**
             * getDSSDisplayString
             *
             * Retrieve the DSS type as a string that is displayed in the Alarm List Control.
             *
             * @param alarm - The alarm to retrieve information for
             *
             * @return The text to display in the column for the specified alarm.
             */
            std::string getDSSDisplayString(const TA_Base_Core::AlarmDetailCorbaDef* alarm);

            /**
             * getMMSDisplayString
             *
             * Retrieve the MMS type as a string that is displayed in the Alarm List Control.
             *
             * @param alarm - The alarm to retrieve information for
             *
             * @return The text to display in the column for the specified alarm.
             */
            std::string getMMSDisplayString(const TA_Base_Core::AlarmDetailCorbaDef* alarm);

            /**
             * getAvalancheDisplayString
             *
             * Retrieve the Avalanche as a string that is displayed in the Alarm List Control.
             *
             * @param alarm - The alarm to retrieve information for
             *
             * @return The text to display in the column for the specified alarm.
             */
            std::string getAvalancheDisplayString(const TA_Base_Core::AlarmDetailCorbaDef* alarm);

        private:

            Utilities() {}
            Utilities& operator=(const Utilities& rhs);
            Utilities(const Utilities&);

            // the one and only class instance
            static Utilities* s_utilities;

            // protect singleton creation
            static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;
	};
};
#endif // !defined(UTILITIES_19_05_2005_H)
