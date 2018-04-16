/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/FZL1/transactive/app/bas/BASAgent/src/PeriodicThread.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/01/19 17:59:10 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */
#ifndef GF_3002_app_bas_BASAgent_PeriodicThread_H
#define GF_3002_app_bas_BASAgent_PeriodicThread_H

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

#endif  // GF_3002_app_bas_BASAgent_PeriodicThread_H
