/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/cctv/video_display_manager/src/Monitor.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#include "Monitor.h"
#include "core/utilities/src/FunctionEntry.h"

namespace TA_IRS_App
{

    TrainBorneMonitor::TrainBorneMonitor( unsigned long entityKey,
                      const std::string& name,
                      const std::string& description,
					  const std::string& addr,
                      unsigned int displayOrder,
					  unsigned long consoleKey,
					  bool isOverallMonitor)
      : VideoOutput(entityKey, name, description),
	    m_address(addr),
		m_displayOrder(displayOrder),
		m_consoleKey(consoleKey),
		m_isConsoleMonitor((consoleKey>0&&!isOverallMonitor)?true:false),
		m_isOverallMonitor(isOverallMonitor)
    {
        FUNCTION_ENTRY( "Monitor" );
        FUNCTION_EXIT;
    }


    bool TrainBorneMonitor::isConsoleMonitor()
    {
        FUNCTION_ENTRY( "isConsoleMonitor" );

        // no setter - no need to lock
        FUNCTION_EXIT;
        return m_isConsoleMonitor;
    }

	bool TrainBorneMonitor::isOverAllMonitor()
	{
		FUNCTION_ENTRY( "isOverAllMonitor" );

        // no setter - no need to lock
        FUNCTION_EXIT;
        return m_isOverallMonitor;
	}

	std::string TrainBorneMonitor::getAddress()
	{
		FUNCTION_ENTRY( "getDisplayOrder" );

        // no setter - no need to lock
        FUNCTION_EXIT;
		return m_address;
	}

	unsigned int TrainBorneMonitor::getDisplayOrder()
	{
		FUNCTION_ENTRY( "getDisplayOrder" );

        // no setter - no need to lock
        FUNCTION_EXIT;
		return m_displayOrder;
	}

	unsigned long TrainBorneMonitor::getConsoleKey()
	{
		FUNCTION_ENTRY( "getConsoleKey" );

        // no setter - no need to lock
        FUNCTION_EXIT;
		return m_consoleKey;
	}
}

