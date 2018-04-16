/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/utilities/src/Hostname.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  *
  * This file contains the definition of a utility class called Hostname.
  * The purpose of this class is to provide standard methods for determining
  * the hostname of the machine that the application is running on.
  *
  * This class came about because a number of different methods were being
  * employed to determine the host name and they weren't giving the same
  * results.  Some methods returned only the canononical name, whereas
  * others return the fully qualified name.
  *
  */

#if !defined (__Hostname_h__)
#define __Hostname_h__

#include <string>
#include <exception>
#include "DebugUtil.h"

namespace TA_Base_Core
{

    class Hostname
    {
    public:

        /*
         * Get the fully qualified hostname of the host.
         */
        static std::string getFullHostname();

        /*
         * Get the canononical name of the host.
         */
        static std::string getHostname();
    };

} // namespace TA_Base_Core

#endif // !defined (__Hostname_h__)
