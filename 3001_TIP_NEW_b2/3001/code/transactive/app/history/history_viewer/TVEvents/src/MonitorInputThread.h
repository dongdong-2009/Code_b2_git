/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/history_viewer/TVEvents/src/MonitorInputThread.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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