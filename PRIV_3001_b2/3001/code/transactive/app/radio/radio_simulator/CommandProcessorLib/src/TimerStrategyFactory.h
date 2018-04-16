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

#ifndef _TIMER_STRATEGY_FACTORY_H_
#define _TIMER_STRATEGY_FACTORY_H_

#include "app/radio/radio_simulator/CommandProcessorLib/src/ITimerResponse.h"

namespace TA_IRS_App
{
	class TimerStrategyFactory
	{
	public:
		static ITimerResponse* makeTimerStrategy(int trainID, int strategyType,unsigned long millSec);

	private:
		/**
		 * Disable default, copy and assignment constructor
		 */
		TimerStrategyFactory(){};
		TimerStrategyFactory(const TimerStrategyFactory& ){};
		TimerStrategyFactory& operator = (const TimerStrategyFactory& ){};

	};
}
#endif