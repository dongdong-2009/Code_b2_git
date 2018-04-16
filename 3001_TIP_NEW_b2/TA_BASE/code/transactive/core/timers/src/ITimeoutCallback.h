/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/timers/src/ITimeoutCallback.h $
  * @author:	HoaVu
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2012/02/06 16:15:14 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  * This interface is used as a mean to notify user of TimerUtil when the registered
  * timeout period has expired so that appropriated action can be executed.
  *
  */

#ifndef TIMEOUT_CALLBACK_H
#define TIMEOUT_CALLBACK_H


namespace TA_Base_Core
{

	class ITimeoutCallback
	{

	public:

		virtual ~ITimeoutCallback(){};

		/**
		*
		* timerExpired
		*
		* To be implemented by users of TimerUtil.  This operation will be called
		* when the registered timeout period has expired.
		*
		*/
		virtual void timerExpired(long timerId, void* userData) = 0;
	};

}

#endif
