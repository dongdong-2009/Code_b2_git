#pragma once

#include "ITimerMngr.h"
#include "TimerMngrHelper.h"


class CTimerMngr : public ITimerMngr
{
	public:
		
		CTimerMngr(ITimerExpire * timerExpireSubs);
		~CTimerMngr(void);
		
		virtual void timerStart (int nMilliSec);
		virtual void timerStop ();
		virtual ETimerStatus timerStatus () {return ETimerStop;}
	
	private:
		TimerMngrHelper m_timerMngrHelper;
      
};
