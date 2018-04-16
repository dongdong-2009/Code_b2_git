/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/Monitor.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/11/18 19:40:33 $
  * Last modified by:  $Author: huang.wenbo $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#include "Monitor.h"
#include "core/utilities/src/FunctionEntry.h"

namespace TA_IRS_App
{

    Monitor::Monitor(unsigned long entityKey,
                     const std::string& name,
                     const std::string& description,
                     bool isConsoleMonitor,
                     unsigned long consoleKey,
                     int order,
					 const std::string& address)
        : VideoOutput(entityKey, name, description),
          m_isConsoleMonitor(isConsoleMonitor),
          m_consoleKey(consoleKey),
          m_order(order),
		  m_address(address)
    {
        FUNCTION_ENTRY("Monitor");
        FUNCTION_EXIT;
    }


    bool Monitor::isConsoleMonitor()
    {
        FUNCTION_ENTRY("isConsoleMonitor");
        // no setter - no need to lock
        FUNCTION_EXIT;
        return m_isConsoleMonitor;
    }

	bool Monitor::isQuadMonitor()
	{
		return m_order % 5 != 0;
	}

	unsigned long Monitor::getConsoleKey()
    {
        FUNCTION_ENTRY("getConsoleKey");
        // no setter - no need to lock
        FUNCTION_EXIT;
        return m_consoleKey;
    }

    int Monitor::getDisplayOrder()
    {
        FUNCTION_ENTRY("getConsoleKey");
        // no setter - no need to lock
        FUNCTION_EXIT;
        return m_order;
    }

	std::string Monitor::getAddress()
	{
        FUNCTION_ENTRY("getAddress");
        FUNCTION_EXIT;
		return m_address;
	}
}
