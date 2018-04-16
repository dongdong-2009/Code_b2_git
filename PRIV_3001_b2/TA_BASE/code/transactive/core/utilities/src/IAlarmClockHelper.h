/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/core/utilities/src/IAlarmClockHelper.h $
  * @author:   Robert Stagg
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class provides a mechanism to call a function after a set period of time asynchronously.
  */
#if !defined(IALARMCLOCKHELPER)
#define IALARMCLOCKHELPER


namespace TA_Base_Core
{
	class AlarmClock;
	class IAlarmClockHelper
	{
    public:
		/**
		 * executeAlarm
		 *
		 * This function is called when the alarm clock goes off
		 */
		 virtual void executeAlarm( AlarmClock* clock ) = 0;
	};
};

#endif // !defined(IALARMCLOCKHELPER)
