/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommanProcessorLib\src\ITimerResponse.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/10/23 04:05:25 $
  * Last modified by:  $Author: builder $
  *
  * Interface use for Timer Response Strategy
  */

#ifndef _ITIMER_STRATEGY_H_
#define _ITIMER_STRATEGY_H_

namespace TA_IRS_App
{
	class ITimerResponse
	{
	public:
		virtual void waitingTime() = 0;
	};
}
#endif