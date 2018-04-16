/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/FZL1_TIP/FZL1/transactive/app/bas/BASAgent/src/PeriodicThread.cpp $
  * @author:	Raymond Pau
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2015/01/26 16:54:00 $
  * Last modified by:	$Author: qi.huang $
  *
  * Description:
  *
  *
  */
#include "app/bas/BASAgent/src/ITimetableExecutionProxy.h"
#include "app/bas/BASAgent/src/PeriodicThread.h"
#include <time.h>
#include "core/utilities/src/DebugUtil.h"
#include "app/bas/BASAgent/src/TimetableManager.h"

namespace TA_IRS_App
{

    PeriodicThread::PeriodicThread(int hours, int minutes, TA_IRS_App::ITimetableExecutionProxy& manager)
		: 
		m_scheduleHrs(hours),
		m_scheduleMin(minutes),
		m_timetableExec(manager)
	{
		FUNCTION_ENTRY("PeriodicThread");
        if (m_scheduleHrs < 0 && m_scheduleHrs > 23 )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Schedule Hour must be within 0 ~ 23");
        }

        if (m_scheduleMin < 0 && m_scheduleMin > 59 )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Schedule Hour must be within 0 ~ 59");
        }

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Schedule BAS Timetable at %d:%d", m_scheduleHrs, m_scheduleMin);

		FUNCTION_EXIT;
	}


	PeriodicThread::~PeriodicThread()
	{
		FUNCTION_ENTRY("~PeriodicThread");
		FUNCTION_EXIT;
	}

	void PeriodicThread::terminate()
	{
		FUNCTION_ENTRY("terminate");

		m_stopRequest = true;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"PeriodicThread::terminate() called");

		FUNCTION_EXIT;
	}

	void PeriodicThread::run()
	{
		FUNCTION_ENTRY("run");
		m_stopRequest = false;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "PeriodicThread::run() called");
		
        short sleepCounter = 60;

		while(!m_stopRequest)
		{
			if( m_timetableExec.isAgentRunningControlMode())
			{
				if (isTimeForScheduled())
				{
					std::string weekday = getWeekDay();
					if (!weekday.empty())
					{
						m_timetableExec.executeScheduleTimeTable(weekday);
					}					
				}
			}

            sleepCounter = 60;

            while (!m_stopRequest&& sleepCounter--) // scan each minute
            {
                sleep(1000);	
            }

		}

		FUNCTION_EXIT;
	}

	std::string PeriodicThread::getWeekDay()
	{
		FUNCTION_ENTRY("getWeekDay");
	
		std::string ret("");

		struct tm* newtime;
		time_t long_time;

		time(&long_time);
		newtime = localtime(&long_time);

		switch(newtime->tm_wday)
		{
		case 0:
			ret = "SUNDAY";
			break;
			
		case 1:		//Monday
		case 2:		//Tuesday
		case 3:		//Wednesday
		case 4:		//Thursday
			ret = "WORKDAYS";
			break;

		case 5:		//Friday
			ret = "FRIDAY";
			break;

		case 6:		//Saturday
			ret = "SATURDAY";
			break;

		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Do not have this weekday!");
			break;
		}

		FUNCTION_EXIT;
		return ret;
	}

	bool PeriodicThread::isTimeForScheduled()
	{
		FUNCTION_ENTRY("isTimeForScheduled");
		
		time_t now;
		time(&now);
		struct tm *newtime;
		newtime = localtime(&now);

		return (newtime->tm_hour == m_scheduleHrs && newtime->tm_min == m_scheduleMin);

		FUNCTION_EXIT;
	}

}
