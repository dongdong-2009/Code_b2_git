#ifndef REMOTE_API_TIMEOUT_THREAD_H_INCLUDED
#define REMOTE_API_TIMEOUT_THREAD_H_INCLUDED

#include "core/utilities/src/IAlarmClockHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_mft_agent/src/ITimerMngr.h"

class RemoteApiTimeoutHelper : public TA_Base_Core::IAlarmClockHelper,
							   public ITimerExpire
{
	virtual void executeAlarm(TA_Base_Core::AlarmClock* clock);
	virtual void notifyTimerExpire(ETimerStatus timerStatus);
};

#endif