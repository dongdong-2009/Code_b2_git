#ifndef RECIEVEALARMTHREAD_H
#define RECIEVEALARMTHREAD_H

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/ReceiveAlarmThread.h,v $
 * @author:  Cameron Rochester
 * @version: $Revision: 1.1.2.1 $
 *
 * Last modification: $Date: 2003/06/23 05:00:14 $
 * Last modified by:  $Author: andrewdc $
 *
 * A thread that will recieve alarms sent across the wire while other threads are submitting.
 * It allows real load to be placed on the Alarm Agent
 */

#include <string>
#include "core/threads/src/Thread.h"
//#include "core/exceptions/src/NameNotFoundException.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"


class ReceiveAlarmThread :
    public TA_Core::TA_Message::SpecialisedMessageSubscriber<TA_Core::TA_Alarm::AlarmMessageCorbaDef>,
	public TA_Core::TA_Thread::Thread
{

public:
	/**
	* constructor
	*/
	ReceiveAlarmThread(const int entityTypeKey,const int totalAlarmCount);

	/**
	* destructor
	*/
	virtual ~ReceiveAlarmThread();
	
	void run();

	void terminate();

	void receiveSpecialisedMessage(const TA_Core::TA_Alarm::AlarmMessageCorbaDef& message);

private:

	/**
	* constructor
	* 
	* disable the copy constructor
	*/
	ReceiveAlarmThread(const ReceiveAlarmThread& theReceiveAlarmThread);

	bool m_running;
    int m_targetValue;
    int m_key;
    int m_receivedAlarmCount;
};

#endif // !defined RECIEVEALARMTHREAD_H
