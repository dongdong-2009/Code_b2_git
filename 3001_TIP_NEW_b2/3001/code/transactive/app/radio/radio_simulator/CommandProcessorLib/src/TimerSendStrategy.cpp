/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommanProcessorLib\src\TimerSendStrategy.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/10/23 04:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for responding the train command from the RadioTetraAgent 
  * with the specified time
  */

#include "app/radio/radio_simulator/CommandProcessorLib/src/TimerSendStrategy.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
	TimerSendStrategy::TimerSendStrategy(int trainID, unsigned long millSec)
		: m_trainID(trainID)
		 ,m_timeMilSec(millSec)
	{
		FUNCTION_ENTRY("TimerSendStrategy");
		FUNCTION_EXIT;
	}

	TimerSendStrategy::~TimerSendStrategy()
	{
		FUNCTION_ENTRY("~TimerSendStrategy");
		FUNCTION_EXIT;
	}

	void TimerSendStrategy::waitingTime()
	{
		FUNCTION_ENTRY("waitingTime");

		m_timeWaitSem.timedWait(m_timeMilSec);

		FUNCTION_EXIT;
	}
}