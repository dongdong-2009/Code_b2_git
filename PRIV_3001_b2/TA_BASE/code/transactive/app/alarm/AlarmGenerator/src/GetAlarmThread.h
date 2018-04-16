/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/GetAlarmThread.h,v $
 * @author:  Cameron Rochester
 * @version: $Revision: 1.1.2.2 $
 *
 * Last modification: $Date: 2004/02/18 03:00:43 $
 * Last modified by:  $Author: justine $
 *
 * A thread to call getAlarms() on the Alarm Agent while other threads are attempting to 
 * submit, receive, close, etc.
 */

#if !defined(GETALARMTHREAD_H)
#define GETALARMTHREAD_H

#include "core/threads/src/Thread.h"


#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"
#include "core/corba_named_obj/src/NamedObjRef.h"

using TA_Core::TA_Thread::Thread;
using TA_Core::TA_Alarm::AlarmSequence;

class GetAlarmThread : public Thread
{
public:
    GetAlarmThread(const int sessionId, const bool ackAlarms = false);

    void run();

    void terminate();

private:

    GetAlarmThread(const GetAlarmThread& theGetAlarmThread);
    GetAlarmThread& operator=(const GetAlarmThread&);
   /**
	* Pointer to the AlarmProcessor
	*/
	TA_Core::NamedObjRef<TA_Core::TA_Alarm::AlarmAgentCorbaDef,
		TA_Core::TA_Alarm::AlarmAgentCorbaDef_ptr,
		TA_Core::TA_Alarm::AlarmAgentCorbaDef_var> m_alarmProcessor;

    AlarmSequence* m_alarms;
    bool m_ackAlarms;
    double m_getTime;
    double m_ackTime;
    int m_sessionId;
    TA_Core::TA_Synchronisation::ReEntrantThreadLockable m_instanceLock;
};

#endif // !defined(GETALARMTHREAD_H)