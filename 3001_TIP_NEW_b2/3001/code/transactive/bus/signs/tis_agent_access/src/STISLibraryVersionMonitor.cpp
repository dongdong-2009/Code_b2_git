/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/signs/tis_agent_access/src/STISLibraryVersionMonitor.cpp $
  * @author:  Kent Yip
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/04/19 20:02:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * 
  *
  */

#include "STISLibraryVersionMonitor.h"

namespace TA_IRS_Bus
{
    const std::string STISLibraryVersionMonitor::CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-CurrentSTISLibraryVersion");
    const std::string STISLibraryVersionMonitor::NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME(".TIS.ISCS.ISCS.aiiTISC-NextSTISLibraryVersion");
    const std::string STISLibraryVersionMonitor::CURRENT_STIS_STATION_STIS_LIBRARY_VERSION_DP_NAME(".TIS.STIS.SEV.aiiTISC-CurrentSTISLibraryVersion");
    const std::string STISLibraryVersionMonitor::NEXT_STIS_STATION_STIS_LIBRARY_VERSION_DP_NAME(".TIS.STIS.SEV.aiiTISC-NextSTISLibraryVersion");

    STISLibraryVersionMonitor* STISLibraryVersionMonitor::s_instance = NULL;
    TA_Base_Core::NonReEntrantThreadLockable STISLibraryVersionMonitor::singletonLock;
	std::map<std::string, ELibraryVersionPointType> STISLibraryVersionMonitor::s_dpPostFixNameToType;

	void STISLibraryVersionMonitor::setDPPostfixNameToType(const std::map<std::string, ELibraryVersionPointType>& dpPostFixNameToType)
	{
        TA_Base_Core::ThreadGuard guard(singletonLock);
		if(NULL == s_instance)
		{
			s_dpPostFixNameToType = dpPostFixNameToType;
		}
	}

    STISLibraryVersionMonitor& STISLibraryVersionMonitor::instance()
    {
        {
            TA_Base_Core::ThreadGuard guard(singletonLock);
            if( NULL == s_instance )
            {
				if(s_dpPostFixNameToType.empty())
				{
	                s_dpPostFixNameToType[CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME] = CURRENT_ISCS_VERSION;
	                s_dpPostFixNameToType[NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME] = NEXT_ISCS_VERSION;
	                s_dpPostFixNameToType[CURRENT_STIS_STATION_STIS_LIBRARY_VERSION_DP_NAME] = CURRENT_STIS_VERSION;
	                s_dpPostFixNameToType[NEXT_STIS_STATION_STIS_LIBRARY_VERSION_DP_NAME] = NEXT_STIS_VERSION;
				}
                s_instance = new STISLibraryVersionMonitor(s_dpPostFixNameToType);
            }
        }
        return *s_instance;
    }

    void STISLibraryVersionMonitor::removeInstance()
    {
        TA_Base_Core::ThreadGuard guard(singletonLock);
		if(s_instance->isNoListener())
		{
	        delete s_instance;
	        s_instance = NULL;
		}
    }

    STISLibraryVersionMonitor::STISLibraryVersionMonitor( std::map<std::string,ELibraryVersionPointType> dpPostfix )
        : LibraryVersionMonitor( dpPostfix )
    {
    }

    STISLibraryVersionMonitor::~STISLibraryVersionMonitor()
    {
    }
} // namespace TA_IRS_Bus

