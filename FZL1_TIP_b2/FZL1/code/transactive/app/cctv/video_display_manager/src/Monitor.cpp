/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/Monitor.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#include "app/cctv/video_display_manager/src/Monitor.h"
#include "core/utilities/src/FunctionEntry.h"

namespace TA_IRS_App
{

    Monitor::Monitor(unsigned long entityKey,
                     const std::string& name,
                     const std::string& description,
                     bool isConsoleMonitor,
                     unsigned long consoleKey,
                     int order,
					 const std::string& address,
					 bool isQuad)
        : VideoOutput(entityKey, name, description),
          m_isConsoleMonitor(isConsoleMonitor),
          m_consoleKey(consoleKey),
          m_order(order),
		  m_address(address),
		  m_isVirtualQuadMon(false),
		  m_isQuad(isQuad),
		  m_isSingleWallMonitor(false)
    {
        FUNCTION_ENTRY("Monitor");
        FUNCTION_EXIT;
    }

	Monitor::~Monitor()
	{
	}

    bool Monitor::isConsoleMonitor()
    {
        FUNCTION_ENTRY("isConsoleMonitor");
        FUNCTION_EXIT;
        return m_isConsoleMonitor;
    }

	bool Monitor::isVirtualQuadMonitor()
	{
		return m_isVirtualQuadMon;
	}
	
	bool Monitor::isQuad()
	{
		return m_isQuad;
	}

    unsigned long Monitor::getConsoleKey()
    {
        FUNCTION_ENTRY("getConsoleKey");
        FUNCTION_EXIT;
        return m_consoleKey;
    }

    int Monitor::getDisplayOrder()
    {
        FUNCTION_ENTRY("getConsoleKey");
        FUNCTION_EXIT;
        return m_order;
    }

	std::string Monitor::getAddress()
	{
        FUNCTION_ENTRY("getAddress");
        FUNCTION_EXIT;
		return m_address;
	}

	void Monitor::setSingleWallMonitor( bool isSingle )
	{
		m_isSingleWallMonitor = isSingle;
	}

	bool Monitor::isSingleWallMonitor()
	{
		return m_isSingleWallMonitor;
	}

}
