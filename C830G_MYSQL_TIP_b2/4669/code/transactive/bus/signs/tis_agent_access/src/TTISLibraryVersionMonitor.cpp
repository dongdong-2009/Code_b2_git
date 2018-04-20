/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/signs/tis_agent_access/src/TTISLibraryVersionMonitor.cpp $
  * @author:  Kent Yip
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * 
  *
  */

#include "TTISLibraryVersionMonitor.h"

namespace TA_IRS_Bus
{
    const std::string TTISLibraryVersionMonitor::CURRENT_TTIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-CurrentTTISLibraryVersion");
    const std::string TTISLibraryVersionMonitor::NEXT_TTIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-NextTTISLibraryVersion");

    TTISLibraryVersionMonitor* TTISLibraryVersionMonitor::instance = NULL;
    TA_Base_Core::NonReEntrantThreadLockable TTISLibraryVersionMonitor::singletonLock;

    TTISLibraryVersionMonitor& TTISLibraryVersionMonitor::getTTISLibraryVersionMonitor()
    {
        {
            TA_Base_Core::ThreadGuard guard(singletonLock);
            if( NULL == instance )
            {
                std::map<std::string,ELibraryVersionPointType> libraries;
                libraries[CURRENT_TTIS_STATION_LIBRARY_VERSION_DP_NAME] = CURRENT_VERSION;
                libraries[NEXT_TTIS_STATION_LIBRARY_VERSION_DP_NAME] = NEXT_VERSION;
                instance = new TTISLibraryVersionMonitor( libraries );
            }
        }
        return *instance;
    }

    void TTISLibraryVersionMonitor::removeTTISLibraryVersionMonitor()
    {
        TA_Base_Core::ThreadGuard guard(singletonLock);

        delete instance;
        instance = NULL;
    }

    TTISLibraryVersionMonitor::TTISLibraryVersionMonitor( std::map<std::string,ELibraryVersionPointType> dpPostfix )
        : LibraryVersionMonitor( dpPostfix )
    {
    }

    TTISLibraryVersionMonitor::~TTISLibraryVersionMonitor()
    {
    }
} // namespace TA_IRS_Bus

