/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/timers/src/ITimeoutCallback.h $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2017/10/31 16:21:54 $
  * Last modified by:	$Author: CM $
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
