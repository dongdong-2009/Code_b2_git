#include "TimerMngrHelper.h"

#define TIMERSTOPPED	1
#define TIMEREXPIRED	0

TimerMngrHelper::TimerMngrHelper(ITimerExpire * timerExpire) :m_lTimerId(0),
	 m_timerUtil( TA_Base_Core::SingletonTimerUtil::getInstance() ),
	 m_timerExpire(timerExpire)
	
{
	
}

TimerMngrHelper::~TimerMngrHelper(void)
{
	stopTimer();		
}

void TimerMngrHelper::startTimer (const int & timerValue)
{
	
	stopTimer ();
	
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Starting Timer");

	m_lTimerId = m_timerUtil.startPeriodicTimeOutClock (this, timerValue, true, NULL);

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"Timer Start : Timer ID : %d",m_lTimerId);

}


void TimerMngrHelper::threadedTimerExpired(long timerId, void* userData)
{
	if (m_lTimerId == timerId)
	{
		m_lTimerId = 0;
		m_timerExpire->notifyTimerExpire(ETimerExpire);
	}
}

void TimerMngrHelper::stopTimer ()
{
	if (0 != m_lTimerId)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Stopping Timer");
		
		m_timerUtil.stopPeriodicTimeOutClock(this,m_lTimerId);
		m_lTimerId = 0;

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Timer Stop");
	}
}