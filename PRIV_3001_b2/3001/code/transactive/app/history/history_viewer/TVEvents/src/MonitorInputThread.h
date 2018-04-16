/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/TVEvents/src/MonitorInputThread.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(MONITOR_INPUT_THREAD_H)
#define MONITOR_INPUT_THREAD_H

#include "core/threads/src/Thread.h"

class MonitorInputThread : public TA_Core::Thread
{
public:
    MonitorInputThread() : m_quit(false), m_done(false) {}
    virtual ~MonitorInputThread() {}

    void run();
    
    void terminate()
    {
        m_done = true;
    }

    bool quitCalled()
    {
        return m_quit;
    }

private:

    bool m_quit;
    bool m_done;
};

#endif // MONITOR_INPUT_THREAD_H