/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/TimeControlManager.h $
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


#if !defined(TimeControlManager_H)
#define TimeControlManager_H

#include "CallbackButton.h"
#include "CallbackDateTimeCtrl.h"

#include <vector>
#include "core/data_access_interface/tis_agent_4669/IDL/src/TisMessageDataDefinitions.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"

using TA_Base_Bus::ColourCombo;

namespace TA_IRS_App
{
    class ITimeTypeListener;


    class TimeControlManager : public IButtonListener,
                               public IDateTimeListener
    {

    public:


        // represents the type of message
        // to be sent
        enum TimeType
        {
            MESSAGE_CYCLIC = 0,
            MESSAGE_CONTINUOUS = 1,
            MESSAGE_RATIS = 2
        };


        /** 
          * TimeControlManager
          *
          * Constructor
          *
          * @param priorityCombo        The priority combo box
          * @param timeTypeContinuous   The continuous radio button
          * @param timeTypeCyclic       The cyclic radio button
          * @param startDate            The start date time control
          * @param startTime            The start time time control
          * @param endDate              The end date time control
          * @param endTime              The end time time control
          * @param repeatInterval       The repeat interval time control
          * @param timeTypeListener     Object to notify when the time type changes
          *
          */
        TimeControlManager( ColourCombo& priorityCombo,
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
                            ITimeTypeListener& timeTypeListener);
        

        ~TimeControlManager();


        /** 
          * getPriority
          *
          * gets the priority from the drop down priority box
          *
          * @return the priority from 1 to 8
          */
        unsigned short getPriority() const;


        /** 
          * getMessageType
          *
          * Gets whether cyclic or continuous are set.
          *
          * @return the message type.
          */
        TimeType getMessageType() const;


        /** 
          * getStartTime
          *
          * gets the start time
          *
          * @return the start time.
          */
        CTime getStartTime() const;


        /** 
          * getEndTime
          *
          * gets the end time
          *
          * @return the end time.
          */
        CTime getEndTime() const;


        /** 
          * getRepeatInterval
          *
          * Gets the repeat interval (in seconds).
          *
          * @return the repeat interval.
          */
        unsigned short getRepeatInterval() const;


        /** 
          * enableTimeControls
          *
          * Enables or disables the time selection controls
          *
          * @param timeTypeEnabled  whether the time type selection can be enabled
          *
          */
        void enableTimeControls( bool timeTypeEnabled = true);


        /** 
          * setPriority
          *
          * Set the default priority and whether the user can change it or if its disabled.
          * The message type will change to cyclic/continuous based on the given priority.
          *
          * @param newPriority  The new priority
          * @param canChange    true if the priority selection should be enabled.
          *
          */
        void setPriority( unsigned short newPriority, bool canChange = true);


        /** 
          * setRepeatInterval
          *
          * Sets the repeat interval, and whether it can be changed by the user.
          * If it cannot be changed it will be disabled even when in cyclic mode.
          *
          * @param repeatInterval   The new repeat interval ( 1 to 999)
          * @param canChange        Whether it can be changed or not.
          *
          */
        void setRepeatInterval( unsigned short repeatInterval, bool canChange = true );


        /** 
          * blankAndDisableTimeAndPriority
          *
          * This is called when nothing is selected on the pre-defined page.
          * It sets the message type to continuous, and blanks out the time controls.
          * It blanks out the priority and clears the priority box.
          *
          */
        void blankAndDisableTimeAndPriority();


        /** 
          * buttonPressed
          *
          * Gets called when a button is pressed.
          *
          * @param button   the button that was pressed
          */
        virtual void buttonPressed(CallbackButton* button);


        /** 
          * dateTimeChanged
          *
          * Gets called when a date/time is changed.
          *
          * @param control   the control that was changed
          */
        virtual void dateTimeChanged(CallbackDateTimeCtrl* control);

		void setRatisClear( bool isClearMessage );


    private:

        TimeControlManager();

        enum MessagePriorityType
        {
            NORMAL_PRIORITY_ONLY = 0,
            EMERGENCY_PRIORITY_ONLY = 1,
            ALL_PRIORITIES = 2
        };


        /** 
          * onTimeTypeContinuous
          *
          * Called when the time type is set to cyclic,
          * (Button press)
          */
        void onTimeTypeContinuous();


        /** 
          * onTimeTypeCyclic
          *
          * Called when the time type is set to continuous,
          * (Button press)
          */
	    void onTimeTypeCyclic();


        /** 
          * onTimeTypeRatis
          *
          * Called when the time type is set to RATIS,
          */
	    void onTimeTypeRatis();

        /**
          * setMessagePriorityType
          *
          * Set what priorities are available in the priority selection combo
          *
          * @param prioritiesEnabled    all, normal only, or emergency only.
          */
        void setMessagePriorityType(MessagePriorityType prioritiesEnabled);


        /** 
          * timeTypeChanged
          *
          * enables/disables controls based on whether cyclic or
          * continuous is selected.
          */
        void timeTypeChanged();


        // The display attribute controls
        ColourCombo& m_priorityCombo;
	    CallbackButton& m_timeTypeContinuous;
        CallbackButton& m_timeTypeCyclic;
        CallbackDateTimeCtrl& m_startDate;
        CallbackDateTimeCtrl& m_startTime;
        CallbackDateTimeCtrl& m_endDate;
	    CallbackDateTimeCtrl& m_endTime;
	    CDateTimeCtrl& m_repeatInterval;
        CStatic& m_startTimeLabel;
        CStatic& m_endTimeLabel;
	    CStatic& m_repeatIntervalLabel;

        // this is used to determine which time-selection options 
        // should be enabled and disabled
        bool m_repeatIntervalEnabled;

        // the time type - cyclic or continuous
        TimeType m_timeType;

        // Object to be notified when the time type changes.
        ITimeTypeListener& m_timeTypeListener;
    };

} // TA_IRS_App

#endif // !defined(TimeControlManager_H)
