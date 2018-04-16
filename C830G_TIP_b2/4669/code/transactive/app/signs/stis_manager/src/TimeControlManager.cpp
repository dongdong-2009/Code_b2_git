/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/TimeControlManager.cpp $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This handles the time and priority controls
  * on the display page.
  *
  */


#include "StdAfx.h"
#include "TimeControlManager.h"
#include "ITimeTypeListener.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_App
{
    // This is the biggest repeat interval
    static const unsigned short MAX_REPEAT_INTERVAL = 999;

    static const DWORD COLOR_RED = RGB(255, 0, 0);

    using TA_Base_Core::DebugUtil;

    TimeControlManager::TimeControlManager(ColourCombo& priorityCombo,
 	                                       CallbackButton& timeTypeContinuous,
                                           CallbackButton& timeTypeCyclic,
                                           CallbackDateTimeCtrl& startDate,
                                           CallbackDateTimeCtrl& startTime,
                                           CallbackDateTimeCtrl& endDate,
	                                       CallbackDateTimeCtrl& endTime,
	                                       CDateTimeCtrl& repeatInterval,
 	                                       CStatic& startTimeLabel,
                                           CStatic& endTimeLabel,
	                                       CStatic& repeatIntervalLabel,
                                           ITimeTypeListener& timeTypeListener)
      : m_priorityCombo(priorityCombo),
	    m_timeTypeContinuous(timeTypeContinuous),
        m_timeTypeCyclic(timeTypeCyclic),
        m_startDate(startDate),
        m_startTime(startTime),
        m_endDate(endDate),
	    m_endTime(endTime),
	    m_repeatInterval(repeatInterval),
        m_startTimeLabel(startTimeLabel),
        m_endTimeLabel(endTimeLabel),
	    m_repeatIntervalLabel(repeatIntervalLabel),
        m_repeatIntervalEnabled(true),
        m_timeType(MESSAGE_CYCLIC),
        m_timeTypeListener(timeTypeListener)
    {
		FUNCTION_ENTRY("TimeControlManager::TimeControlManager()");
        // do the initial GUI setup

        // set the repeat interval range (0 to 999 minutes)
        CTime lowerTime(1971, 1, 1, 0, 0, 0, 0);
        // set the repeat interval
        unsigned short hours = MAX_REPEAT_INTERVAL / 60;
        unsigned short minutes = MAX_REPEAT_INTERVAL % 60;
        CTime temp(1971, 1, 1, hours, minutes, 0, 0);
        CTime upperTime(1971, 1, 1, hours, minutes, 0, 0);
        m_repeatInterval.SetRange(&lowerTime, &upperTime);

        // emergency priorities are red
        m_priorityCombo.mapItemDataToColour(1, COLOR_RED);
        m_priorityCombo.mapItemDataToColour(2, COLOR_RED);
        m_priorityCombo.mapItemDataToColour(3, COLOR_RED);

        m_timeTypeContinuous.setCallback(this);
        m_timeTypeCyclic.setCallback(this);
        m_startDate.setCallback(this);
        m_startTime.setCallback(this);
        m_endDate.setCallback(this);
	    m_endTime.setCallback(this);

        // default repeat interval
        setRepeatInterval(0);

        // blank everything out to start
        blankAndDisableTimeAndPriority();

		FUNCTION_EXIT;
    }
    

    TimeControlManager::~TimeControlManager()
    {
        m_timeTypeContinuous.setCallback(NULL);
        m_timeTypeCyclic.setCallback(NULL);
        m_startDate.setCallback(NULL);
        m_startTime.setCallback(NULL);
        m_endDate.setCallback(NULL);
	    m_endTime.setCallback(NULL);
    }


    void TimeControlManager::buttonPressed(CallbackButton* button)
    {
        if (button == &m_timeTypeContinuous)
        {
            onTimeTypeContinuous();
            m_timeTypeListener.timeTypeChanged(TimeControlManager::MESSAGE_CONTINUOUS);
        }
        else if (button == &m_timeTypeCyclic)
        {
            onTimeTypeCyclic();
            m_timeTypeListener.timeTypeChanged(TimeControlManager::MESSAGE_CYCLIC);
        }
    }


    void TimeControlManager::onTimeTypeContinuous()
    {
        m_timeType = MESSAGE_CONTINUOUS;
        m_timeTypeCyclic.SetCheck(0);
        m_timeTypeContinuous.SetCheck(1);
        timeTypeChanged();
    }


	void TimeControlManager::onTimeTypeCyclic()
    {
        m_timeType = MESSAGE_CYCLIC;
        m_timeTypeContinuous.SetCheck(0);
        m_timeTypeCyclic.SetCheck(1);
        timeTypeChanged();
    }


    void TimeControlManager::onTimeTypeRatis()
    {
        m_timeType = MESSAGE_RATIS;
        m_timeTypeContinuous.SetCheck(0);
        m_timeTypeCyclic.SetCheck(1);
        timeTypeChanged();
    }

    void TimeControlManager::dateTimeChanged(CallbackDateTimeCtrl* control)
    {
        // this is used to make sure the end time is never before the start time
        // and the end time hasnt passed

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


    void TimeControlManager::enableTimeControls( bool timeTypeEnabled /* = true */)
    {
        if ( timeTypeEnabled )
        {
            // enable the controls
            m_timeTypeCyclic.EnableWindow(TRUE);
            m_timeTypeContinuous.EnableWindow(TRUE);
        }
        else
        {
            // disable the time type controls
            m_timeTypeCyclic.EnableWindow(FALSE);
            m_timeTypeContinuous.EnableWindow(FALSE);
        }

        // manually trigger the update
        timeTypeChanged();
    }


    void TimeControlManager::setPriority( unsigned short newPriority,
                                          bool canChange /* = true */)
    {
        // enable the control only if the priority can be changed
        m_priorityCombo.EnableWindow(canChange);

        // see where the priority falls
        TimeType timeTypeNeeded;
        int selectionIndex = 0;
        // 0 to 3 emergency . continuous
        if ( (newPriority > 0) && (newPriority < 4) )
        {
            timeTypeNeeded = MESSAGE_CONTINUOUS;
            selectionIndex = newPriority - 1;
        }
        // 4 to 6 normal . cyclic
        else if ( (newPriority > 3) && (newPriority < 6) )
        {
            timeTypeNeeded = MESSAGE_CYCLIC;
            selectionIndex = newPriority - 4;
        }
        else if ( newPriority == 6 )
        {
            timeTypeNeeded = MESSAGE_RATIS;
            selectionIndex = newPriority - 1;
        }
        // 7 to 8 normal . cyclic
        else if ( (newPriority > 6) && (newPriority < 9) )
        {
            timeTypeNeeded = MESSAGE_CYCLIC;
            selectionIndex = newPriority - 5;
        }
        // invalid priority
        else
        {
            // log the error
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Invalid priority given: %d. Check the pre-defined message library, and default TTIS attributes.",
                         newPriority);

            // leave it alone
            return;
        }

        // if it is different to the current time type
        // change time types
        if (timeTypeNeeded != m_timeType)
        {
            switch (timeTypeNeeded)
            {
            case MESSAGE_CYCLIC:
                onTimeTypeCyclic();
                break;
            case MESSAGE_CONTINUOUS:
                onTimeTypeContinuous();
                break;
            case MESSAGE_RATIS:
                onTimeTypeRatis();
                break;
            }
        }

        // set the priority
        m_priorityCombo.SetCurSel(selectionIndex);
    }


    void TimeControlManager::setRepeatInterval( unsigned short repeatInterval,
                                                     bool canChange /* = true */)
    {
        if (repeatInterval > MAX_REPEAT_INTERVAL)
        {
            repeatInterval = MAX_REPEAT_INTERVAL;
        }

        // set the repeat interval
        unsigned short hours = repeatInterval / 60;
        unsigned short minutes = repeatInterval % 60;

        CTime temp(1971, 1, 1, hours, minutes, 0, 0);
        m_repeatInterval.SetTime(&temp);

        // the repeat interval is only enabled if this is true, and the current
        // time type is cyclic
        m_repeatIntervalEnabled = canChange;
        m_repeatInterval.EnableWindow( m_repeatIntervalEnabled &&
                                        (m_timeType == MESSAGE_CYCLIC) );
        m_repeatIntervalLabel.EnableWindow( m_repeatIntervalEnabled &&
                                             (m_timeType == MESSAGE_CYCLIC) );
    }


    void TimeControlManager::blankAndDisableTimeAndPriority()
    {
        // disable time controls
        enableTimeControls(false);

        // select continuous
        setPriority(1, false);

        // clear priority
        m_priorityCombo.SetCurSel(-1);
    }



    unsigned short TimeControlManager::getPriority() const
    {
        int selected = m_priorityCombo.GetCurSel();
    
        return static_cast<unsigned short>(m_priorityCombo.GetItemData(selected));
    }


    TimeControlManager::TimeType TimeControlManager::getMessageType() const
    {
        return m_timeType;
    }


    CTime TimeControlManager::getStartTime() const
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


    CTime TimeControlManager::getEndTime() const
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


    unsigned short TimeControlManager::getRepeatInterval() const
    {
        CTime repeatTime;
        m_repeatInterval.GetTime(repeatTime);

        return ( repeatTime.GetHour() * 60 ) + ( repeatTime.GetMinute() );
    }



    void TimeControlManager::setMessagePriorityType(MessagePriorityType prioritiesEnabled)
    {
        // try to restore the currently selected priority
        // if the new selection has that priotity in it
        int selected = m_priorityCombo.GetCurSel();
        CString selectedString;
        if ( selected != CB_ERR )
        {
            m_priorityCombo.GetLBText( selected, selectedString );
        }

        // empty the list
        m_priorityCombo.ResetContent();

        switch ( prioritiesEnabled )
        {
        case NORMAL_PRIORITY_ONLY:
            m_priorityCombo.AddString("Normal - 4");
	        m_priorityCombo.AddString("Normal - 5");
        //	m_priorityCombo.AddString("Normal - 6");
	        m_priorityCombo.AddString("Normal - 7");
	        m_priorityCombo.AddString("Normal - 8");

            m_priorityCombo.SetItemData(0, 4);
            m_priorityCombo.SetItemData(1, 5);
            m_priorityCombo.SetItemData(2, 7);
            m_priorityCombo.SetItemData(3, 8);
            break;
        case EMERGENCY_PRIORITY_ONLY:
            m_priorityCombo.AddString("Emergency - 1");
	        m_priorityCombo.AddString("Emergency - 2");
	        m_priorityCombo.AddString("Emergency - 3");

            m_priorityCombo.SetItemData(0, 1);
            m_priorityCombo.SetItemData(1, 2);
            m_priorityCombo.SetItemData(2, 3);
            break;
        case ALL_PRIORITIES:
            m_priorityCombo.AddString("Emergency - 1");
	        m_priorityCombo.AddString("Emergency - 2");
	        m_priorityCombo.AddString("Emergency - 3");
            m_priorityCombo.AddString("Normal - 4");
	        m_priorityCombo.AddString("Normal - 5");
        	m_priorityCombo.AddString("Normal - 6");
	        m_priorityCombo.AddString("Normal - 7");
	        m_priorityCombo.AddString("Normal - 8");

            m_priorityCombo.SetItemData(0, 1);
            m_priorityCombo.SetItemData(1, 2);
            m_priorityCombo.SetItemData(2, 3);
            m_priorityCombo.SetItemData(3, 4);
            m_priorityCombo.SetItemData(4, 5);
            m_priorityCombo.SetItemData(5, 6);
            m_priorityCombo.SetItemData(6, 7);
            m_priorityCombo.SetItemData(7, 8);
            break;
        }

        // restore the selected priority if possible
        if ( selected != CB_ERR )
        {
            selected = m_priorityCombo.FindString( -1, selectedString.GetBuffer(0) );
        }

        // if nothing was selected or the previos selection wasnt found
        if ( selected == CB_ERR )
        {
            // select 0
            selected = 0;
        }

        m_priorityCombo.SetCurSel(selected);
    }


    void TimeControlManager::timeTypeChanged()
    {
        switch (m_timeType)
        {
        case MESSAGE_CYCLIC:
            // cyclic radio button selected
	        m_startTimeLabel.EnableWindow(TRUE);
	        m_endTimeLabel.EnableWindow(TRUE);
            m_startDate.EnableWindow(TRUE);
            m_startTime.EnableWindow(TRUE);
	        m_endDate.EnableWindow(TRUE);
            m_endTime.EnableWindow(TRUE);

            // the repeat interval adjuster should only be enabled
            // if it is ok to do so
            m_repeatIntervalLabel.EnableWindow(m_repeatIntervalEnabled);
	        m_repeatInterval.EnableWindow(m_repeatIntervalEnabled);

            // show the time again
            m_startDate.SetFormat(_T(" dd-MMM-yy"));
            m_startTime.SetFormat(_T(" HH:mm:ss"));
            m_endDate.SetFormat(_T(" dd-MMM-yy"));
            m_endTime.SetFormat(_T(" HH:mm:ss"));
            m_repeatInterval.SetFormat(_T("  HH:mm"));

            // set the priority to normal only
            setMessagePriorityType(NORMAL_PRIORITY_ONLY);

            break;

        case MESSAGE_CONTINUOUS:
            // continuous radio button selected
            m_startTimeLabel.EnableWindow(FALSE);
            m_endTimeLabel.EnableWindow(FALSE);
            m_repeatIntervalLabel.EnableWindow(FALSE);

            m_startDate.EnableWindow(FALSE);
            m_startTime.EnableWindow(FALSE);
            m_endDate.EnableWindow(FALSE);
            m_endTime.EnableWindow(FALSE);
            m_repeatInterval.EnableWindow(FALSE);

            // clear the fields (hide the time)
            m_repeatInterval.SetFormat(_T(" "));
            m_startDate.SetFormat(_T(" "));
            m_startTime.SetFormat(_T(" "));
            m_endDate.SetFormat(_T(" "));
            m_endTime.SetFormat(_T(" "));

            // set the priority to emergency only
            setMessagePriorityType(EMERGENCY_PRIORITY_ONLY);

            break;

        case MESSAGE_RATIS:
            // Ratis tab selected
	        m_startTimeLabel.EnableWindow(TRUE);
	        m_endTimeLabel.EnableWindow(TRUE);
            m_startDate.EnableWindow(TRUE);
            m_startTime.EnableWindow(TRUE);
	        m_endDate.EnableWindow(TRUE);
            m_endTime.EnableWindow(TRUE);
            m_repeatIntervalLabel.EnableWindow(FALSE);
	        m_repeatInterval.EnableWindow(FALSE);

            // show the time again
            m_startDate.SetFormat(_T(" dd-MMM-yy"));
            m_startTime.SetFormat(_T(" HH:mm:ss"));
            m_endDate.SetFormat(_T(" dd-MMM-yy"));
            m_endTime.SetFormat(_T(" HH:mm:ss"));
            m_repeatInterval.SetFormat(_T(" "));

            // set the priority to normal only
            setMessagePriorityType(ALL_PRIORITIES);

            break;

        }
    }

	void TimeControlManager::setRatisClear( bool clearSelected )
	{
		if( clearSelected )
		{
	        m_startTimeLabel.EnableWindow( FALSE );
	        m_endTimeLabel.EnableWindow( FALSE );
            m_startDate.EnableWindow( FALSE );
            m_startTime.EnableWindow( FALSE );
	        m_endDate.EnableWindow( FALSE );
            m_endTime.EnableWindow( FALSE );
			//hongran++ TD17568
			m_priorityCombo.EnableWindow(FALSE);
			//++hongran TD17568

		}
		else
		{
	        m_startTimeLabel.EnableWindow( TRUE );
	        m_endTimeLabel.EnableWindow( TRUE );
            m_startDate.EnableWindow( TRUE );
            m_startTime.EnableWindow( TRUE );
	        m_endDate.EnableWindow( TRUE );
            m_endTime.EnableWindow( TRUE );
			//hongran++ TD17568
			m_priorityCombo.EnableWindow(TRUE);
			//++hongran TD17568
		}
	}

} // TA_IRS_App

