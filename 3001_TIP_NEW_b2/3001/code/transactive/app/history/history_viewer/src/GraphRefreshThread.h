#if !defined(GRAPH_REFRESH_THREAD_H)
#define GRAPH_REFRESH_THREAD_H

/**
  * GraphRefreshThread.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/history_viewer/src/GraphRefreshThread.h $
  * @author:  Anita Lee
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This thread refreshes the graph at a regular interval. It moves the graph forward to the latest
  * time and plots another point (retrieves the last value) at that time for each trend shown in 
  * the graph.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/threads/src/Thread.h"

namespace TA_App
{
    //
    // Forward declaration
    //
    class CHistoryViewerDlg;

    //
    // GraphRefreshThread
    //
    class GraphRefreshThread : public TA_Core::Thread
    {
    public:

        /**
		  * GraphRefreshThread
		  * 
		  * Constructor.
		  *
		  */
        GraphRefreshThread(CHistoryViewerDlg* parentDlg);
        
        /**
		  * ~GraphRefreshThread
		  * 
		  * Destructor.
		  *
		  */
        virtual ~GraphRefreshThread();

        /**
		  * run
		  * 
		  * Calls a method on the main dialog at a regular interval to update the graph.
          *
		  */
        virtual void run();

        /**
		  * terminate
		  * 
		  * Terminate the thread.
		  *
		  */
        virtual void terminate();

    private:

        GraphRefreshThread();
        GraphRefreshThread(const GraphRefreshThread& theThread);
	    GraphRefreshThread& operator=(const GraphRefreshThread&);

        time_t calculateBeginTime();

    public:

        bool isTerminated()
        {
            return m_terminated;
        }

        void setRefreshInterval(int refreshInterval)
        {
            m_refreshInterval = refreshInterval;
        }

    private:

        CHistoryViewerDlg* m_parentProcess;
        int m_refreshInterval;
        bool m_terminated;
        bool m_runFinished;
    };
}


#endif // !defined(GRAPH_REFRESH_THREAD_H)





