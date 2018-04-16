/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommanProcessorLib\src\TimerSendStrategy.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/10/23 04:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for responding the train command from the RadioTetraAgent 
  * with the specified time
  */

#ifndef _TIMER_SEND_STRATEGY_H_
#define _TIMER_SEND_STRATEGY_H_

#include "app/radio/radio_simulator/CommandProcessorLib/src/ITimerResponse.h"
#include "core/timers/src/TimedWaitSemaphore.h"

namespace TA_IRS_App
{
	class TimerSendStrategy : public ITimerResponse
	{
	public:
		TimerSendStrategy(int trainID, unsigned long millSec);
		virtual ~TimerSendStrategy();

		void waitingTime();

	private:
		TA_Base_Core::TimedWaitSemaphore m_timeWaitSem;
		int m_trainID;
		unsigned long m_timeMilSec;
	};
}
#endif