
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_TIP/4669/transactive/app/signs/stis_manager/src/TimeScheduleTimeControl.cpp $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:15:01 $
  * Last modified by:  $Author: grace.koh $
  *
  * This handles the time and priority controls
  * on the display page.
  *
  */


#include "StdAfx.h"
#include "TimeScheduleTimeControl.h"
#include "RangeNumberEdit.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include <iomanip>


namespace TA_IRS_App
{
    // This is the biggest repeat interval
    using TA_Base_Core::DebugUtil;
	const unsigned short MIN_REPEAT_INTERVAL = 1;
	const unsigned short MAX_REPEAT_INTERVAL = 60;
	const unsigned short NORMAL_7_VALUE = 7;

    TimeScheduleTimeControl::TimeScheduleTimeControl(CComboBox& priorityCombo,
                                           CallbackDateTimeCtrl& startDate,
                                           CallbackDateTimeCtrl& startTime,
                                           CallbackDateTimeCtrl& endDate,
	                                       CallbackDateTimeCtrl& endTime,
	                                       CRangeNumberEdit& repeatInterval
										   )
      : m_priorityCombo(priorityCombo),
        m_startDate(startDate),
        m_startTime(startTime),
        m_endDate(endDate),
	    m_endTime(endTime),
	    m_repeatInterval(repeatInterval)
    {
		FUNCTION_ENTRY("TimeScheduleTimeControl::TimeScheduleTimeControl()");
        // do the initial GUI setup


        CTime maxDate(2099, 12, 31, 0, 0, 0, 0);
		m_startDate.SetRange(NULL, &maxDate);
		m_endDate.SetRange(NULL, &maxDate);

        m_startDate.setCallback(this);
        m_startTime.setCallback(this);
        m_endDate.setCallback(this);
	    m_endTime.setCallback(this);

		FUNCTION_EXIT;
    }
    

    TimeScheduleTimeControl::~TimeScheduleTimeControl()
    {
        m_startDate.setCallback(NULL);
        m_startTime.setCallback(NULL);
        m_endDate.setCallback(NULL);
	    m_endTime.setCallback(NULL);
    }

	void TimeScheduleTimeControl::Init()
	{

		//set repeat interval time format
		m_repeatInterval.setValueRange(MIN_REPEAT_INTERVAL, MAX_REPEAT_INTERVAL);

		//init priority
		m_priorityCombo.AddString(_T("Normal - 7"));
		m_priorityCombo.AddString(_T("Normal - 8"));
	}

	void TimeScheduleTimeControl::setDefaultValue()
	{
		// Set default priority 7
		setPriority(NORMAL_7_VALUE);

		// Set default repeat interval 1 minute
		m_repeatInterval.setDefaultValue(MIN_REPEAT_INTERVAL);
		setRepeatInterval(MIN_REPEAT_INTERVAL);
	}

    void TimeScheduleTimeControl::dateTimeChanged(CallbackDateTimeCtrl* control)
    {
        // this is used to make sure the end time is never before the start time
        // and the end time has not passed

        // firstly, get the start, end, and current times
        CTime startTime = getStartTime();
        CTime endTime = getEndTime();
        CTime currentTime = CTime::GetCurrentTime();

        // if the start time is before the current time, set it to the current time
        if (startTime < currentTime)
        {
            startTime = currentTime;

            m_startDate.SetTime(&startTime);
            m_startTime.SetTime(&startTime);
        }

        // if the end time is before the start time, set it to the start time
        if (endTime < startTime)
        {
            endTime = startTime;

            m_endDate.SetTime(&endTime);
            m_endTime.SetTime(&endTime);
        }


    }

    void TimeScheduleTimeControl::setPriority( unsigned short newPriority )
    {
        int selectionIndex = 0;
		if( NORMAL_7_VALUE + 1 == newPriority)
		{
			//should be normal 8
			selectionIndex = 1;
		}

        m_priorityCombo.SetCurSel(selectionIndex);
    }


    void TimeScheduleTimeControl::setRepeatInterval( unsigned short repeatInterval )
    {
        if (repeatInterval > MAX_REPEAT_INTERVAL)
        {
            repeatInterval = MAX_REPEAT_INTERVAL;
        }

        m_repeatInterval.setValue(repeatInterval);
    }

    unsigned short TimeScheduleTimeControl::getPriority() const
    {
        int selected = m_priorityCombo.GetCurSel();
    
        return static_cast<unsigned short>(selected + NORMAL_7_VALUE);
    }


    CTime TimeScheduleTimeControl::getStartTime() const
    {
        CTime startTime;
        m_startTime.GetTime(startTime);

        CTime startDate;
        m_startDate.GetTime(startDate);

        CTime combinedTime(startDate.GetYear(),
                           startDate.GetMonth(),
                           startDate.GetDay(),
                           startTime.GetHour(),
                           startTime.GetMinute(),
                           startTime.GetSecond());

        return combinedTime;
    }

	void TimeScheduleTimeControl::setStartTime(CTime startTime)
	{
		m_startDate.SetTime(&startTime);
		m_startTime.SetTime(&startTime);
	}


    CTime TimeScheduleTimeControl::getEndTime() const
    {
        CTime endTime;
        m_endTime.GetTime(endTime);

        CTime endDate;
        m_endDate.GetTime(endDate);

        CTime combinedTime(endDate.GetYear(),
                           endDate.GetMonth(),
                           endDate.GetDay(),
                           endTime.GetHour(),
                           endTime.GetMinute(),
                           endTime.GetSecond());

        return combinedTime;
    }

	void TimeScheduleTimeControl::setEndTime(CTime endTime)
	{
		m_endDate.SetTime(&endTime);
		m_endTime.SetTime(&endTime);
	}

    unsigned short TimeScheduleTimeControl::getRepeatInterval() const
    {
		return m_repeatInterval.getValue();
    }

	std::string TimeScheduleTimeControl::convertCTimeToString(CTime time)
	{
		std::stringstream ss;
		ss << time.GetYear();
		ss << std::setw(2) << std::setfill('0') << time.GetMonth();
		ss << std::setw(2) << std::setfill('0') << time.GetDay();
		ss << std::setw(2) << std::setfill('0') << time.GetHour();
		ss << std::setw(2) << std::setfill('0') << time.GetMinute();
		ss << std::setw(2) << std::setfill('0') << time.GetSecond();
		return ss.str();
	}

	CTime TimeScheduleTimeControl::convertStringToCTime(const std::string time)
	{
		//if time string not legal, return current time
		if(14 != time.size())
		{
			return CTime::GetCurrentTime();
		}
		int year, month, day, hour, min, sec;
		std::stringstream ss;
		//get year
		ss << time.substr(0, 4);
		ss >> year;
		ss.clear();
		ss.str("");
		//get month
		ss << time.substr(4, 2);
		ss >> month;
		ss.clear();
		ss.str("");
		//get day
		ss << time.substr(6, 2);
		ss >> day;
		ss.clear();
		ss.str("");
		//get hour
		ss << time.substr(8, 2);
		ss >> hour;
		ss.clear();
		ss.str("");
		//get min
		ss << time.substr(10, 2);
		ss >> min;
		ss.clear();
		ss.str("");
		//get sec
		ss << time.substr(12, 2);
		ss >> sec;
		return CTime(year, month, day, hour, min, sec);
	}

	unsigned short TimeScheduleTimeControl::getValidMinRepeatInterval()
	{
		return MIN_REPEAT_INTERVAL;
	}

	unsigned short TimeScheduleTimeControl::getValidMaxRepeatInterval()
	{
		return MAX_REPEAT_INTERVAL;
	}
} // TA_IRS_App

