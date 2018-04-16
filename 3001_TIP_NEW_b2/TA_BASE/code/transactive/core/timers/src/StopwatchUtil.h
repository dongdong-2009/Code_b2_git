/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/timers/src/StopwatchUtil.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Class for peforming time-based operations with a 
  * millisecond scale.
  *
  */

#ifndef STOPWATCHUTIL_H
#define STOPWATCHUTIL_H

#include <string>
#include <sys/timeb.h>


namespace TA_Base_Core
{

	class StopwatchUtil
	{

	public:

		StopwatchUtil();
		~StopwatchUtil();


		/**
		 * StopwatchUtil::startTiming()
		 *
		 * Initialises timer.
		 *
		 */
		void startTiming();


		/**
		 * StopwatchUtil::stopTiming()
		 *
		 * Stops timer and stores the time difference for average calculations.
		 *
		 * @return the difference between starting and stopping times, measured in 
		 *		   milliseconds.
		 *
		 */
		unsigned long stopTiming();


		/**
		 * StopwatchUtil::timeElapsed()
		 *
		 * Reports back the elasped time since the timer was started, without
		 * stopping or affecting the timer.
		 *
		 * @return the elapsed time since startTiming() was called, measured in
		 *  	   milliseconds.
		 *
		 */	 
		unsigned long timeElapsed();


		/**
		 * StopwatchUtil::getAverageTime()
		 *
		 * Reports back the average time difference between calls to startTiming()
		 * and stopTiming().
		 *
		 * @return the average time difference in milliseconds
		 *
		 */
		double getAverageTime();


		/**
		 * StopwatchUtil::getTotalTime()
		 *
		 * Reports back the total time difference between calls to startTiming()
		 * and stopTiming().
		 *
		 * @return the total time difference in milliseconds
		 *
		 */
		unsigned long getTotalTime();


		/**
		 * StopwatchUtil::stats()
		 *
		 * Reports back a formatted report on various statistics on the timer.
		 *
		 * @return total time, average time and usage count, formatted and organised
		 *		   into hour/minute/seconds
		 *
		 */
		std::string stats();


		/**
		 * StopwatchUtil::reset()
		 *
		 * Initialises running total and usage counter to zero. Does not affect
		 * the current start time.
		 *
		 */
		void reset();

	private:

		struct timeb    m_startTime;
		struct timeb    m_stopTime;

		unsigned long    m_total;
		unsigned long    m_count;
	};

}

#endif    
