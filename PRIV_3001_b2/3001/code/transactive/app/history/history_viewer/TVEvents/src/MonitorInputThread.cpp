/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/TVEvents/src/MonitorInputThread.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#include <string>
#include <iostream>
#include "MonitorInputThread.h"

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