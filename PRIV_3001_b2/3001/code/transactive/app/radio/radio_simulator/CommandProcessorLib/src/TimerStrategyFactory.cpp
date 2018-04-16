/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommanProcessorLib\src\TimerStrategyFactory.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/10/23 04:05:25 $
  * Last modified by:  $Author: builder $
  *
  * Interface use for creating object of a strategy
  */

#include "app/radio/radio_simulator/CommandProcessorLib/src/TimerStrategyFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/TimerSendStrategy.h"

namespace TA_IRS_App
{
	ITimerResponse* TimerStrategyFactory::makeTimerStrategy(int trainID, int strategyType,unsigned long millSec)
	{
		FUNCTION_ENTRY("makeTimerStrategy");

		ITimerResponse* m_timerResponse = NULL;

		switch (strategyType)
		{
		case 0:
			m_timerResponse = new TimerSendStrategy(trainID,millSec);
			break;
		}

		FUNCTION_EXIT;
		return m_timerResponse;
	}
}