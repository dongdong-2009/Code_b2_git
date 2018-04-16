/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/DisplayAttributeManager.cpp $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This handles the time and priority controls
  * on the display page.
  *
  */


#include "StdAfx.h"
#include "DisplayAttributeManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_App
{
    // This is the biggest repeat interval to fit into
    // the 1 byte that the protocol allows.
    static const unsigned short MAX_REPEAT_INTERVAL = 255;

    static const DWORD COLOR_RED = RGB(255, 0, 0);

    using TA_Base_Core::DebugUtil;

    // xufeng++ 2006/11/06 TD14367
    /*
    DisplayAttributeManager::DisplayAttributeManager(ColourCombo& priorityCombo,
                                                      CButton& timeTypeContinuous,
                                                     CButton& timeTypeCyclic,
                                                     CDateTimeCtrl& startTime,
                                                     CDateTimeCtrl& endTime,
                                                     CDateTimeCtrl& repeatInterval,
                                                      CStatic& startTimeLabel,
                                                     CStatic& endTimeLabel,
                                                     CStatic& repeatIntervalLabel)
    */
    // ++xufeng 2006/11/06 TD14367
    DisplayAttributeManager::DisplayAttributeManager(ColourCombo& priorityCombo,
                                                      CButton& timeTypeContinuous,
                                                     CButton& timeTypeCyclic,
                                                     CallbackDateTimeCtrl& startTime,
                                                     CallbackDateTimeCtrl& endTime,
                                                     CDateTimeCtrl& repeatInterval,
                                                      CStatic& startTimeLabel,
                                                     CStatic& endTimeLabel,
                                                     CStatic& repeatIntervalLabel)
      : m_priorityCombo(&priorityCombo),
        m_timeTypeContinuous(&timeTypeContinuous),
        m_timeTypeCyclic(&timeTypeCyclic),
        m_startTime(&startTime),
        m_endTime(&endTime),
        m_repeatInterval(&repeatInterval),
        m_startTimeLabel(&startTimeLabel),
        m_endTimeLabel(&endTimeLabel),
        m_repeatIntervalLabel(&repeatIntervalLabel),
        m_repeatIntervalEnabled(true),
        m_timeType(MESSAGE_CYCLIC),
        m_messagePrioritesEnabled(NORMAL_PRIORITY_ONLY)
    {
        FUNCTION_ENTRY( "DisplayAttributeManager" );

        // do the initial GUI setup

        // set the repeat interval range (0 to 255 minutes)
        CTime lowerTime(1971, 1, 1, 0, 0, 0, 0);
        // set the repeat interval
        unsigned short hours = MAX_REPEAT_INTERVAL / 60;
        unsigned short minutes = MAX_REPEAT_INTERVAL - (hours * 60);
        CTime temp(1971, 1, 1, hours, minutes, 0, 0);
        CTime upperTime(1971, 1, 1, hours, minutes, 0, 0);
        m_repeatInterval->SetRange(&lowerTime, &upperTime);

        // emergency priorities are red
        m_priorityCombo->mapItemDataToColour(1, COLOR_RED);
        m_priorityCombo->mapItemDataToColour(2, COLOR_RED);
        m_priorityCombo->mapItemDataToColour(3, COLOR_RED);

        // xufeng++ 2006/11/06 TD14367
        m_startTime->setCallback(this);
        m_endTime->setCallback(this);
        // ++xufeng 2006/11/06 TD14367

        // default repeat interval
        setRepeatInterval(0);

        // blank everything out to start
        blankAndDisableTimeAndPriority();

        FUNCTION_EXIT;
    }


    DisplayAttributeManager::~DisplayAttributeManager()
    {
        FUNCTION_ENTRY( "~DisplayAttributeManager" );

        // xufeng++ 2006/11/06 TD14367
        m_startTime->setCallback(NULL);
        m_endTime->setCallback(NULL);
        // ++xufeng 2006/11/06 TD14367

        FUNCTION_EXIT;
    }


    void DisplayAttributeManager::onTimeTypeContinuous()
    {
        FUNCTION_ENTRY( "onTimeTypeContinuous" );

        m_timeType = MESSAGE_CONTINUOUS;
        m_timeTypeCyclic->SetCheck(0);
        m_timeTypeContinuous->SetCheck(1);
        timeTypeChanged();

        FUNCTION_EXIT;
    }


    void DisplayAttributeManager::onTimeTypeCyclic()
    {
        FUNCTION_ENTRY( "onTimeTypeCyclic" );

        m_timeType = MESSAGE_CYCLIC;
        m_timeTypeContinuous->SetCheck(0);
        m_timeTypeCyclic->SetCheck(1);
        timeTypeChanged();

        FUNCTION_EXIT;
    }


    // xufeng++ 2006/11/06 TD14367
    void DisplayAttributeManager::dateTimeChanged(CallbackDateTimeCtrl* control)
    {
        FUNCTION_ENTRY( "dateTimeChanged" );

        // this is used to make sure the end time is never before the start time
        // and the end time hasnt passed

        // firstly, get the start, end, and current times
        CTime startTime = getStartTime();
        CTime endTime = getEndTime();
        CTime currentTime = CTime::GetCurrentTime();

//        // if the start time is before the current time, set it to the current time
//        if (startTime < currentTime)
//        {
//            startTime = currentTime;
//            m_startTime->SetTime(&startTime);
//        }
//
//        // if the end time is before the start time, set it to the start time
//        if (endTime < startTime)
//        {
//            endTime = startTime;
//            m_endTime->SetTime(&endTime);
//        }

		  //zhongjie++ TD17917
         m_startTime->SetTime(&startTime);
	
		// if the end time is before the start time, set it to the start time
		if (endTime<startTime)
		{
             endTime = startTime;
            m_endTime->SetTime(&endTime);
		}
		else
		{
			m_endTime->SetTime(&endTime);
		}
         //zhongjie++ TD17917

        // xufeng++ 2006/11/09 TD17546
        if (startTime.GetHour() == 3 && startTime.GetMinute() == 0)
        {
//			startTime = currentTime;
//          m_startTime->SetTime(&startTime);
		    CTimeSpan incTime(0,0,1,0);//TD17917
            startTime+=incTime;
            m_startTime->SetTime(&startTime);
        }
        if(endTime.GetHour() == 3 && endTime.GetMinute() == 0)
        {
            endTime = startTime;
            m_endTime->SetTime(&endTime);
        }
        // ++xufeng 2006/11/09 TD17546

        FUNCTION_EXIT;
    }


    // ++xufeng 2006/11/06 TD14367

    void DisplayAttributeManager::enableTimeControls( bool timeTypeEnabled /* = true */)
    {
        FUNCTION_ENTRY( "enableTimeControls" );

        if ( timeTypeEnabled )
        {
            // enable the controls
            m_timeTypeCyclic->EnableWindow(TRUE);
            m_timeTypeContinuous->EnableWindow(TRUE);
        }
        else
        {
            // disable the time type controls
            m_timeTypeCyclic->EnableWindow(FALSE);
            m_timeTypeContinuous->EnableWindow(FALSE);
        }

        // manually trigger the update
        timeTypeChanged();

        FUNCTION_EXIT;
    }


    void DisplayAttributeManager::setPriority( unsigned short newPriority,
                                               bool canChange /* = true */)
    {
        FUNCTION_ENTRY( "setPriority" );

        // enable the control only if the priority can be changed
        m_priorityCombo->EnableWindow(canChange);

        // if the priority is 6, make it 7.
        if (newPriority == 6)
        {
            newPriority = 7;
        }

        // see where the priority falls
        TimeType timeTypeNeeded;
        int selectionIndex = 0;
        // 0 to 3 emergency -> continuous
        if ( (newPriority > 0) && (newPriority < 4) )
        {
            timeTypeNeeded = MESSAGE_CONTINUOUS;
            selectionIndex = newPriority - 1;
        }
        // 4 to 6 normal -> cyclic
        else if ( (newPriority > 3) && (newPriority < 6) )
        {
            timeTypeNeeded = MESSAGE_CYCLIC;
            selectionIndex = newPriority - 4;
        }
        // 7 to 8 normal -> cyclic
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
            FUNCTION_EXIT;
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
            }
        }

        // set the priority
        m_priorityCombo->SetCurSel(selectionIndex);

        FUNCTION_EXIT;
    }


    void DisplayAttributeManager::setRepeatInterval( unsigned short repeatInterval,
                                                     bool canChange /* = true */)
    {
        FUNCTION_ENTRY( "setRepeatInterval" );

        if (repeatInterval > MAX_REPEAT_INTERVAL)
        {
            repeatInterval = MAX_REPEAT_INTERVAL;
        }

        // set the repeat interval
        unsigned short hours = repeatInterval / 60;
        unsigned short minutes = repeatInterval - (hours * 60);

        CTime temp(1971, 1, 1, hours, minutes, 0, 0);
        m_repeatInterval->SetTime(&temp);

        // the repeat interval is only enabled if this is true, and the current
        // time type is cyclic
        m_repeatIntervalEnabled = canChange;
        m_repeatInterval->EnableWindow( m_repeatIntervalEnabled &&
                                        (m_timeType == MESSAGE_CYCLIC) );
        m_repeatIntervalLabel->EnableWindow( m_repeatIntervalEnabled &&
                                             (m_timeType == MESSAGE_CYCLIC) );

        FUNCTION_EXIT;
    }


    void DisplayAttributeManager::blankAndDisableTimeAndPriority()
    {
        FUNCTION_ENTRY( "blankAndDisableTimeAndPriority" );

        // disable time controls
        enableTimeControls(false);

        // select continuous
        setPriority(1, false);

        // clear priority
        m_priorityCombo->SetCurSel(-1);

        FUNCTION_EXIT;
    }


    unsigned short DisplayAttributeManager::getPriority() const
    {
        FUNCTION_ENTRY( "getPriority" );

        int selected = m_priorityCombo->GetCurSel();

        // if all priorities are being shown
        // or if only the emergency priorities are being shown
        // add 1 to give it the right priority
        if ( m_messagePrioritesEnabled == EMERGENCY_PRIORITY_ONLY )
        {
            selected++;
        }

        // if only the normal priorities are being shown
        // add 4 to give it the right priority
        if ( m_messagePrioritesEnabled == NORMAL_PRIORITY_ONLY )
        {
            selected += 4;
        }


        // if applicable, add 1 for the missing entry - 6.
        if ( selected > 5 )
        {
            selected++;
        }

        FUNCTION_EXIT;
        return static_cast<unsigned short>(selected);
    }


    DisplayAttributeManager::TimeType DisplayAttributeManager::getMessageType() const
    {
        FUNCTION_ENTRY( "getMessageType" );
        FUNCTION_EXIT;
        return m_timeType;
    }


    CTime DisplayAttributeManager::getStartTime() const
    {
        FUNCTION_ENTRY( "getStartTime" );

        CTime startTime;

        m_startTime->GetTime(startTime);

        FUNCTION_EXIT;
        return startTime;
    }


    CTime DisplayAttributeManager::getEndTime() const
    {
        FUNCTION_ENTRY( "getEndTime" );

        CTime endTime;

        m_endTime->GetTime(endTime);

        FUNCTION_EXIT;
        return endTime;
    }


    unsigned short DisplayAttributeManager::getRepeatInterval() const
    {
        FUNCTION_ENTRY( "getRepeatInterval" );

        CTime repeatTime;
        m_repeatInterval->GetTime(repeatTime);

        FUNCTION_EXIT;
        return ( repeatTime.GetHour() * 60 ) + ( repeatTime.GetMinute() );
    }


    void DisplayAttributeManager::setMessagePriorityType(MessagePriorityType prioritiesEnabled)
    {
        FUNCTION_ENTRY( "setMessagePriorityType" );

        // save the setting
        m_messagePrioritesEnabled = prioritiesEnabled;

        // try to restore the currently selected priority
        // if the new selection has that priotity in it
        int selected = m_priorityCombo->GetCurSel();
        CString selectedString;
        if ( selected != CB_ERR )
        {
            m_priorityCombo->GetLBText( selected, selectedString );
        }

        // empty the list
        m_priorityCombo->ResetContent();

        switch ( prioritiesEnabled )
        {
        case NORMAL_PRIORITY_ONLY:
            m_priorityCombo->AddString("Normal - 4");
            m_priorityCombo->AddString("Normal - 5");
        // m_priorityCombo->AddString("Normal - 6"); Note if 6 is allowed, change getPriority()
            m_priorityCombo->AddString("Normal - 7");
            m_priorityCombo->AddString("Normal - 8");

            m_priorityCombo->SetItemData(0, 4);
            m_priorityCombo->SetItemData(1, 5);
            m_priorityCombo->SetItemData(2, 7);
            m_priorityCombo->SetItemData(3, 8);
            break;
        case EMERGENCY_PRIORITY_ONLY:
            m_priorityCombo->AddString("Emergency - 1");
            m_priorityCombo->AddString("Emergency - 2");
            m_priorityCombo->AddString("Emergency - 3");

            m_priorityCombo->SetItemData(0, 1);
            m_priorityCombo->SetItemData(1, 2);
            m_priorityCombo->SetItemData(2, 3);
            break;
        }

        // restore the selected priority if possible
        if ( selected != CB_ERR )
        {
            selected = m_priorityCombo->FindString( -1, selectedString.GetBuffer(0) );
        }

        // if nothing was selected or the previos selection wasnt found
        if ( selected == CB_ERR )
        {
            // select 0
            selected = 0;
        }

        m_priorityCombo->SetCurSel(selected);

        FUNCTION_EXIT;
    }


    void DisplayAttributeManager::timeTypeChanged()
    {
        FUNCTION_ENTRY( "timeTypeChanged" );

        switch (m_timeType)
        {
        case MESSAGE_CYCLIC:
            // cyclic radio button selected
            m_startTimeLabel->EnableWindow(TRUE);
            m_endTimeLabel->EnableWindow(TRUE);
            m_startTime->EnableWindow(TRUE);
            m_endTime->EnableWindow(TRUE);

            // the repeat interval adjuster should only be enabled
            // if it is ok to do so
            m_repeatIntervalLabel->EnableWindow(m_repeatIntervalEnabled);
            m_repeatInterval->EnableWindow(m_repeatIntervalEnabled);

            // show the time again
            m_startTime->SetFormat(_T("  HH:mm"));
            m_endTime->SetFormat(_T("  HH:mm"));
            m_repeatInterval->SetFormat(_T("  HH:mm"));

            // set the priority to normal only
            setMessagePriorityType(NORMAL_PRIORITY_ONLY);

            break;

        case MESSAGE_CONTINUOUS:
            // continuous radio button selected
            m_startTimeLabel->EnableWindow(FALSE);
            m_endTimeLabel->EnableWindow(FALSE);
            m_repeatIntervalLabel->EnableWindow(FALSE);

            m_startTime->EnableWindow(FALSE);
            m_endTime->EnableWindow(FALSE);
            m_repeatInterval->EnableWindow(FALSE);

            // clear the fields (hide the time)
            m_repeatInterval->SetFormat(_T(" "));
            m_startTime->SetFormat(_T(" "));
            m_endTime->SetFormat(_T(" "));

            // set the priority to emergency only
            setMessagePriorityType(EMERGENCY_PRIORITY_ONLY);

            break;
        }

        FUNCTION_EXIT;
    }


} // TA_IRS_App

