/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/signs/tis_agent_access/src/STISLibraryVersionMonitor.h $
  * @author:  Kent Yip
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/04/19 20:02:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This is used to keep track of the STIS predefined message library
  * version at each location
  *
  */

#ifndef STISLIBRARYVERSIONMONITOR_H
#define STISLIBRARYVERSIONMONITOR_H

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "bus/signs/tis_agent_access/src/LibraryVersionMonitor.h"

#include <string>
#include <map>

namespace TA_IRS_Bus
{

    /**
     * Class that will listen for datapoint updates on datapoints that store
     * STIS version data.
     *
     * This class essentially stores the applicable datapoint postfix and
     * informs the super class of the nature of each postfix (current or
     * next).
     */
    class STISLibraryVersionMonitor : public LibraryVersionMonitor
    {
    public:
        const static std::string CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME;
        const static std::string NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME;
        const static std::string CURRENT_STIS_STATION_STIS_LIBRARY_VERSION_DP_NAME;
        const static std::string NEXT_STIS_STATION_STIS_LIBRARY_VERSION_DP_NAME;
		static void setDPPostfixNameToType(const std::map<std::string, ELibraryVersionPointType>& dpPostFixNameToType);

    public:
        /** Singleton access method. */
        static STISLibraryVersionMonitor& instance();

        static void removeInstance();

    private:
        /** Constructor. */
        STISLibraryVersionMonitor( std::map<std::string,ELibraryVersionPointType> dpPostfix );
        /** Destructor. */
        virtual ~STISLibraryVersionMonitor();


		static std::map<std::string, ELibraryVersionPointType> s_dpPostFixNameToType;

        /** The Singleton instance. */
        static STISLibraryVersionMonitor* s_instance;
        /** Threadlock for singleton access. */
        static TA_Base_Core::NonReEntrantThreadLockable singletonLock;
    };

} // namespace TA_IRS_Bus

#endif // STISLIBRARYVERSIONMONITOR_H