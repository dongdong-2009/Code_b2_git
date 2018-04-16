#include "TimerMngr.h"

CTimerMngr::CTimerMngr(ITimerExpire * timerExpireSubs) :
	m_timerMngrHelper (timerExpireSubs)
{
}

CTimerMngr::~CTimerMngr(void)
{
	m_timerMngrHelper.stopTimer();
}

void CTimerMngr::timerStart (int nMilliSec)
{
	m_timerMngrHelper.startTimer(nMilliSec);
}

void CTimerMngr::timerStop ()
{
	m_timerMngrHelper.stopTimer();
}
