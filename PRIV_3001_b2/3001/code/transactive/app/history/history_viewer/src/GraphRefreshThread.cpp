#if !defined(GRAPH_REFRESH_THREAD_CPP)
#define GRAPH_REFRESH_THREAD_CPP

/**
  * GraphRefreshThread.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/src/GraphRefreshThread.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This thread refreshes the graph at a regular interval. It moves the graph forward to the latest
  * time and plots another point (retrieves the last value) at that time for each trend shown in 
  * the graph.
  *
  */

#include <time.h>

#include "app/history/history_viewer/src/stdafx.h"
#include "app/history/history_viewer/src/GraphRefreshThread.h"
#include "app/history/history_viewer/src/HistoryViewerDlg.h"
#include "app/history/history_viewer/src/globals.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "cots/ACE/5_3/ACE_wrappers/ace/OS.h"

using namespace TA_Core;

namespace TA_App
{
   
    //
    // GraphRefreshThread
    //
    GraphRefreshThread::GraphRefreshThread(CHistoryViewerDlg* parentDlg)
        : m_parentProcess(parentDlg),
          m_terminated(true),
          m_runFinished(false)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "GraphRefreshThread");

        TA_ASSERT(parentDlg != NULL, "Null Pointer");

        std::string refreshInterval = RunParams::getInstance().get(RPARAM_GRAPH_REFRESH_INTERVAL);
        m_refreshInterval = atoi(refreshInterval.c_str());
        
        LOG(SourceInfo, DebugUtil::FunctionExit, "GraphRefreshThread");
    }


    //
    // ~GraphRefreshThread
    //
    GraphRefreshThread::~GraphRefreshThread()
    {
        while(!m_runFinished)
        {
            sleep(50);
        }
    }


    //
    // run
    //
    void GraphRefreshThread::run()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "run");

        m_terminated = false;
        m_runFinished = false;

        time_t beginTime = calculateBeginTime();
        
        time_t timeNow;
        time_t nextTime;

        time(&timeNow);
        while(!m_terminated && timeNow < beginTime)
        {
            Thread::sleep(100);
            time(&timeNow);
        }

        nextTime = timeNow + m_refreshInterval;
        bool firstLoop = true;

        while(!m_terminated)
        {
          
            //time(&timeNow);
            if(!firstLoop)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Calling refreshGraph()");
                m_parentProcess->refreshGraph(timeNow);
            }
            else
            {
                firstLoop = false;
            }

            for(int i = 0; (i < m_refreshInterval*100 && !m_terminated); ++i)
            {
                time(&timeNow);
                if(timeNow < nextTime)
                {
                    Thread::sleep(10);
                }
                else
                {
                    break;
                }
            }

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "End while: timeNow: %lu nextTime: %lu", timeNow, nextTime);

            nextTime += m_refreshInterval;

        }

        m_runFinished = true;
    
        LOG(SourceInfo, DebugUtil::FunctionExit, "run");
    }


    //
    // terminate
    //
    void GraphRefreshThread::terminate()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "terminate");

        m_terminated = true;

        //
        // Ensure that the run() method has finished before ending terminate.
        // This is because this thread can be started again, and we don't want
        // two instances to execute run() at any one time.
        //
        //while(!m_runFinished)
        //{
        //    sleep(100);
        //}

        LOG(SourceInfo, DebugUtil::FunctionExit, "terminate");
    }


    time_t GraphRefreshThread::calculateBeginTime()
    {
        //
        // Get the current time
        //
        struct tm currentTime;
	    time_t theTime;
	    time(&theTime);

        ACE_OS::localtime_r(&theTime, &currentTime);
        int currentTimeHour = currentTime.tm_hour;
        int currentTimeMin = currentTime.tm_min;
        int currentTimeSec = currentTime.tm_sec;

	    const int SODHour = 0;
	    const int SODMinute = 0;
        const int SODSec = 0;
	   
	    // The only difference in the Current time and the Start of Day time would be the 
	    // Hour and Minute attributes. All other attributes should be the same, since it 
	    // is the same day, year, etc. Hence to find the difference, we only need to 
	    // compute the difference between the Hour and Minute by converting them into
	    // seconds first.
        //
	    int SODSeconds = SODHour * 60 * 60 + SODMinute * 60 + SODSec;
	    int currentSeconds = currentTimeHour * 60 * 60 + currentTimeMin * 60 + currentTimeSec;
	    time_t beginProcessTime;

        //
		// Find the number of seconds that has elapsed from the 
		// Start of Day time to the Current time.
        //
		int elapsedSeconds = currentSeconds - SODSeconds;

        //
		// Divide elapsed by 'Sample Time' and truncate to determine
		// how many intervals are within this elapsed time, and which
		// interval we are up to.
        //
		int interval = (int) (elapsedSeconds / m_refreshInterval);

        //
		// Move up to the next interval and convert this back to seconds
        //
		int extraTime = (interval + 1) * m_refreshInterval;

        //
		// Calculate the time (in seconds) at which to begin processing
        //
		beginProcessTime = theTime - (long) elapsedSeconds + (long) extraTime;
	    
        return beginProcessTime;
    }
}

#endif // GRAPH_REFRESH_THREAD_CPP