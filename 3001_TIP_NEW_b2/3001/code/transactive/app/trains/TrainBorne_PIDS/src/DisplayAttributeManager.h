/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_TIP/4669/transactive/app/signs/ttis_manager/src/DisplayAttributeManager.h $
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

#pragma once


#include <vector>
#include "core/data_access_interface/tis_agent_4669/IDL/src/TisMessageDataDefinitions.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"
#include "ValidatingEdit.h"

//using TA_Base_Bus::ColourCombo;

namespace TA_IRS_App
{

    class DisplayAttributeManager
    {

    public:


        // represents the type of message
        // to be sent
        enum TimeType
        {
            MESSAGE_PERRUN= 0,
            MESSAGE_CONTINUOUS = 1
        };


        /**
          * DisplayAttributeManager
          *
          * Constructor
          *
          * @param priorityCombo        The priority combo box
          * @param timeTypeContinuous   The continuous radio button
          * @param timeTypeCyclic       The cyclic radio button
          * @param startTime            The start time time control
          * @param endTime              The end time time control
          * @param repeatInterval       The repeat interval time control
          *
          */
        DisplayAttributeManager( CButton& timeTypeContinuous,
								 CButton& timeTypePerrun,
                                 ValidatingEdit& perRun,
								 CStatic& timeTypeContinuousLabel,
								 CStatic& timeTypePerrunLabel );

        ~DisplayAttributeManager();


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
		void onTimeTypePerrun();

		void setContinuousBtnCheck();
		void setPerrunBtnCheck();




        /**
          * getMessageType
          *
          * Gets whether cyclic or continuous are set.
          *
          * @return the message type.
          */
        TimeType getMessageType() const;


		unsigned short getPerrun() const;
		bool isPerrunInRange();


		void setPerrun( unsigned short perrun, bool canChange = true );

    private:

        void timeTypeChanged();


        DisplayAttributeManager() {}

        // The display attribute controls
        CButton* m_timeTypeContinuous;
        CButton* m_timeTypePerrun;
		ValidatingEdit* m_perRun;
        CStatic* m_timeTypeContinuousLabel;
        CStatic* m_timeTypePerrunLabel;
        // this is used to determine which time-selection options
        // should be enabled and disabled
        bool m_perRunEnabled;

        // the time type - cyclic or continuous
        TimeType m_timeType;
		static const unsigned char MIN_PER_RUN;
		static const unsigned char MAX_PER_RUN;
    };

} // TA_IRS_App
