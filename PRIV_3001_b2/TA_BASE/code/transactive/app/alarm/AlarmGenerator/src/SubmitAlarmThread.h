/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/SubmitAlarmThread.h,v $
 * @author:  <Cameron Rochester>
 * @version: $Revision: 1.1.2.2 $
 *
 * Last modification: $Date: 2004/02/18 03:00:43 $
 * Last modified by:  $Author: justine $
 * 
 * This class generates a specified number of alarms, and submits them to the alarm processor
 * as fast as possible. It is a thread so multiple instances of it can be fired off at once.
 */

#ifndef SUBMITALARMTHREAD_H
#define SUBMITALARMTHREAD_H

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

using TA_Core::TA_Thread::Thread;

class SubmitAlarmThread : public Thread
{
public:


    /** 
     * SubmitAlarmThread
     *
     * <description>
     *
     * @return 
     *
     * @param int key
     * @param int alarmCount 
     *
     */
    SubmitAlarmThread(const int key, const int typekey,const int alarmCount = 1000,const int submitDelay = 0): 
      m_alarmCount(alarmCount),m_key(key),m_typeKey(typekey),m_submitDelay(submitDelay) {}

    ~SubmitAlarmThread() {};


    /** 
     * run
     *
     * <description>
     *
     */
    void run();

    void terminate();

    double getTotalSubmitTime();

    double getAverageSubmitTime();

private:
    
    int m_alarmCount;
    int m_key;
    int m_typeKey;
    int m_submitDelay;
    TA_Core::TA_Synchronisation::ReEntrantThreadLockable m_instanceLock;

    double m_totalSubmitTime;
    double m_averageSubmitTime;
    double m_totalCloseTime;
};

#endif // !defined SUBMITALARMTHREAD_H