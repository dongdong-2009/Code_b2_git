/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/PeriodicThread.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
#ifndef APP_BAS_BASAgent_PeriodicThread_H
#define APP_BAS_BASAgent_PeriodicThread_H

#include "app/bas/BASAgent/src/ITimetableExecutionProxy.h"
#include "core/threads/src/Thread.h"
#include <string>

namespace TA_IRS_App
{

	class PeriodicThread : public TA_Base_Core::Thread
	{
	public:

        PeriodicThread(int hours, int minutes, TA_IRS_App::ITimetableExecutionProxy& manager);

		virtual ~PeriodicThread();

		virtual void run();

		virtual void terminate();

	private:

		std::string getWeekDay();

		bool isTimeForScheduled();

		volatile bool m_stopRequest;

		int m_scheduleHrs;
		int m_scheduleMin;
        TA_IRS_App::ITimetableExecutionProxy& m_timetableExec;

	};
} //end TA_IRS_App

#endif  // APP_BAS_BASAgent_PeriodicThread_H
