#pragma once

#include "core/utilities/src/DebugUtil.h"
#include "core\synchronisation\src\ReEntrantThreadLockable.h"
#include "core/timers/src/AbstractThreadedTimeoutUser.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "ITimerMngr.h"


class TimerMngrHelper : public TA_Base_Core::AbstractThreadedTimeoutUser
{
	public:
		
		TimerMngrHelper(ITimerExpire * timerExpire);
		~TimerMngrHelper(void);

		void startTimer (const int & timerValue);
		void stopTimer ();
		virtual void threadedTimerExpired(long timerId, void* userData);

	private:
		 TA_Base_Core::SingletonTimerUtil& m_timerUtil;
		 long m_lTimerId;
		 ITimerExpire * m_timerExpire;


};
