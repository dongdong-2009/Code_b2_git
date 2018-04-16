/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/Monitor.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#include "Monitor.h"
#include "core/utilities/src/FunctionEntry.h"

namespace TA_IRS_App
{

    Monitor::Monitor( unsigned long entityKey,
                      const std::string& name,
                      const std::string& description,
                      bool isConsoleMonitor )
      : VideoOutput(entityKey, name, description),
        m_isConsoleMonitor(isConsoleMonitor)
    {
        FUNCTION_ENTRY( "Monitor" );
        FUNCTION_EXIT;
    }


    bool Monitor::isConsoleMonitor()
    {
        FUNCTION_ENTRY( "isConsoleMonitor" );

        // no setter - no need to lock
        FUNCTION_EXIT;
        return m_isConsoleMonitor;
    }


}

