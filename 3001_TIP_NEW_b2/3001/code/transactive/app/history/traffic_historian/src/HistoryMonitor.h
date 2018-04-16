/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/traffic_historian/src/HistoryMonitor.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
//
// HistoryMonitor.h
//
// Author: Craig Grant 22/01/2001
//
// Defines the HistoryMonitor class which monitors the TrafficDetectors and
// has them save the data when the sampling period is up.
//

// KT 19/02/02: The Historian no longer averages the received samples 
// over a long period.  It just logs received data for the statistics
// it is interested in straight to the database.  Therefore, this
// class is no longer required.

/*
#ifndef HISTORYMONITOR_H_INCLUDED
#define HISTORYMONITOR_H_INCLUDED

#include <vector>
#include <string>
#include "MiThreadLockable.h"
#include "MiThread.h"


class HistoryMonitor : public MiThread
{
public:
    HistoryMonitor();

    // Override pure virtual from JTCThread
    virtual void run();
    void terminate();

private:
    bool m_Done; // Used to implement termination method

    //
    // This class was originally derived from a thread and a thread
    // guard, even though it is only a thread.  The thread guard has
    // been made a member variable because that is where it really
    // belongs.
    //
    MiThreadLockable m_securityGuard;
};

#endif  // HISTORYMONITOR_H_INCLUDED
*/