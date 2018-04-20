/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/core/utilities/src/IAlarmClockHelper.h $
  * @author:   Robert Stagg
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
