/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/signs/tis_agent_access/src/TTISLibraryVersionMonitor.cpp $
  * @author:  Kent Yip
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * 
  *
  */

#include "TTISLibraryVersionMonitor.h"

namespace TA_IRS_Bus
{
    const std::string TTISLibraryVersionMonitor::CURRENT_TTIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-CurrentTTISLibraryVersion");
    const std::string TTISLibraryVersionMonitor::NEXT_TTIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-NextTTISLibraryVersion");

    TTISLibraryVersionMonitor* TTISLibraryVersionMonitor::s_instance = NULL;
    TA_Base_Core::NonReEntrantThreadLockable TTISLibraryVersionMonitor::singletonLock;
	std::map<std::string, ELibraryVersionPointType> TTISLibraryVersionMonitor::s_dpPostFixNameToType;

	void TTISLibraryVersionMonitor::setDPPostfixNameToType(const std::map<std::string, ELibraryVersionPointType>& dpPostFixNameToType)
	{
		TA_Base_Core::ThreadGuard guard(singletonLock);
		if(NULL == s_instance)
		{
			s_dpPostFixNameToType = dpPostFixNameToType;
		}
	}

    TTISLibraryVersionMonitor& TTISLibraryVersionMonitor::instance()
    {
        {
            TA_Base_Core::ThreadGuard guard(singletonLock);
            if( NULL == s_instance )
            {
				if(s_dpPostFixNameToType.empty())
				{
					s_dpPostFixNameToType[CURRENT_TTIS_STATION_LIBRARY_VERSION_DP_NAME] = CURRENT_VERSION;
					s_dpPostFixNameToType[NEXT_TTIS_STATION_LIBRARY_VERSION_DP_NAME] = NEXT_VERSION;
				}
				s_instance = new TTISLibraryVersionMonitor(s_dpPostFixNameToType);
            }
        }
        return *s_instance;
    }

    void TTISLibraryVersionMonitor::removeInstance()
    {
        TA_Base_Core::ThreadGuard guard(singletonLock);
		if(s_instance->isNoListener())
		{
	        delete s_instance;
	        s_instance = NULL;
		}
    }

    TTISLibraryVersionMonitor::TTISLibraryVersionMonitor( std::map<std::string,ELibraryVersionPointType> dpPostfix )
        : LibraryVersionMonitor( dpPostfix )
    {
    }

    TTISLibraryVersionMonitor::~TTISLibraryVersionMonitor()
    {
    }
} // namespace TA_IRS_Bus

