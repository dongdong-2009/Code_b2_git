/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/trending/trend_viewer/send_events/src/MonitorInputThread.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#include <string>
#include <iostream>
#include "app/trending/trend_viewer/send_events/src/MonitorInputThread.h"

void MonitorInputThread::run()
{
    std::string input("");
    while(!m_done)
    {
        std::cin >> input;
        if(input == "quit")
        {
            m_quit = true;
            m_done = true;
        }
    }
}