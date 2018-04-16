/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/TrainDisplayPage.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The train display page.
  *
  */

#if !defined(AFX_TRAINDISPLAYPAGE_H__8AEB3BB0_157B_4544_BBC1_497DEB014E8F__INCLUDED_)
#define AFX_TRAINDISPLAYPAGE_H__8AEB3BB0_157B_4544_BBC1_497DEB014E8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "TrainDisplayTab.h"
#include "IPidSelectionListener.h"
#include "app/signs/ttis_manager/src/IMessageSelectionListener.h"
#include "REBProgressManager.h"

#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisableEx.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"
#include "bus/mfc_extensions/src/toggleButton/toggleButton.h"


using TA_Base_Bus::ColourCombo;
using TA_IRS_Bus::TrainListCtrl;
using TA_IRS_Bus::TrainGroupComboBox;
using TA_Base_Bus::CTabCtrlWithDisableEx;


namespace TA_IRS_Bus
{
    class MFCTrainSelectorGUI;
}

namespace TA_IRS_App
{
    class PidSelectionManager;
    class DisplayAttributeManager;
    class DisplayRequestManager;
}

namespace TA_IRS_App
{

/////////////////////////////////////////////////////////////////////////////
// CTrainDisplayPage dialog

    class CTrainDisplayPage : public CDialog,
                              public IPidSelectionListener,
                              public TA_IRS_Bus::ITrainSelectionListener,
                              public IMessageSelectionListener
    {

    // Construction
    public:
        CTrainDisplayPage(CWnd* pParent = NULL);   // standard constructor


    public:

        void Init(REBProgressManager & mgr);

        /**
          * pidSelectionChanged
          *
          * Gets called when the PID selection is changed. (IPidSelectionListener)
          *
          * @param pidSelectionExists   true if at least one PID is selected
          */
        virtual void pidSelectionChanged(bool pidSelectionExists);


        /**
          * trainSelectionChanged
          *
          * Gets called when the train selection is changed. (ITrainSelectionListener)
          *
          * @param trainsAreSelected   true if at least one train is selected
          */
        virtual void trainSelectionChanged(bool trainsAreSelected);


        /**
          * messageSelectionChanged
          *
          * Gets called when the message selection is changed.
          *
          * @param trainsAreSelected   true if at least one train is selected
          */
        virtual void messageSelectionChanged(bool messageIsSelected);


        /**
          * getDisplayAttributeManager
          *
          * Gets the display attribute manager used to manipulate
          * time and priority controls on this page.
          * Shouldnt be kept, but gotten when needed.
          *
          * @return the display attribute manager pointer. If NULL,
          *         this window hasnt finished setting up.
          */
        virtual DisplayAttributeManager* getDisplayAttributeManager();

        // TD 15349
        // zhou yuan++
        bool findAndSelectStationMessage(const std::string& messageName);
        // ++zhou yuan

    // Dialog Data
    private:

        // {{AFX_DATA(CTrainDisplayPage)
        enum { IDD = IDD_TRAIN_DISPLAY };
        CTabCtrlWithDisableEx m_trainSelectionTab;
        CStatic    m_inboundLabel;
        CStatic    m_outboundLabel;
        TrainListCtrl    m_outboundTrainList;
        TrainListCtrl    m_inboundTrainList;
        CButton    m_clearAllTrains;
        CButton    m_selectAllTrains;
        CButton    m_allInboundTrains;
        CButton    m_allOutboundTrains;
        TrainGroupComboBox    m_trainGroupCombo;
        CButton    m_saveTrainGroup;
        CButton    m_deleteTrainGroup;
        CButton    m_clearAllMessages;
        CButton    m_clearEmergencyMessages;
        CButton    m_clearNormalMessages;
        CButton    m_clearPriority6;
        CButton    m_display;
        CButton    m_selectEntireTrain;
        CButton    m_clearPids;
        // TES799 ~ modified (start)
        ToggleButton    m_allCar3;
        ToggleButton    m_allCar2;
        ToggleButton    m_allCar1;
        // TES799 ~ modified (end)
        ToggleButton    m_car3PID4;
        ToggleButton    m_car3PID3;
        ToggleButton    m_car3PID2;
        ToggleButton    m_car3PID1;
        ToggleButton    m_car2PID4;
        ToggleButton    m_car2PID3;
        ToggleButton    m_car2PID2;
        ToggleButton    m_car2PID1;
        ToggleButton    m_car1PID4;
        ToggleButton    m_car1PID3;
        ToggleButton    m_car1PID2;
        ToggleButton    m_car1PID1;
        ColourCombo    m_priority;
        CButton    m_timeTypeContinuous;
        CButton    m_timeTypeCyclic;
        CallbackDateTimeCtrl    m_endTime;
        CDateTimeCtrl    m_repeatInterval;
        CallbackDateTimeCtrl    m_startTime;
        CStatic    m_endTimeLabel;
        CStatic    m_startTimeLabel;
        CStatic    m_repeatIntervalLabel;
        CTrainDisplayTab    m_trainDisplayTab;
        // }}AFX_DATA


        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CTrainDisplayPage)
        public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CTrainDisplayPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnAllCar1();
        afx_msg void OnAllCar2();
        afx_msg void OnAllCar3();
        afx_msg void OnPIDButtonClicked();
        afx_msg void OnCar1PID1Clicked();
        afx_msg void OnCar1PID2Clicked();
        afx_msg void OnCar1PID3Clicked();
        afx_msg void OnCar1PID4Clicked();
        afx_msg void OnCar2PID1Clicked();
        afx_msg void OnCar2PID2Clicked();
        afx_msg void OnCar2PID3Clicked();
        afx_msg void OnCar2PID4Clicked();
        afx_msg void OnCar3PID1Clicked();
        afx_msg void OnCar3PID2Clicked();
        afx_msg void OnCar3PID3Clicked();
        afx_msg void OnCar3PID4Clicked();
        afx_msg void OnEntireTrain();
        afx_msg void OnClearPidSelection();
        afx_msg void OnDisplay();
        afx_msg void OnClearAll();
        afx_msg void OnClearPriority6();
        afx_msg void OnClearNormal();
        afx_msg void OnClearEmergency();
        afx_msg void onTimeTypeContinuous();
        afx_msg void onTimeTypeCyclic();
        afx_msg void OnDestroy();
        afx_msg LRESULT onUpdateInboundList(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onUpdateOutboundList(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onUpdateSelectionCombo(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onRightsChanged(WPARAM wParam, LPARAM lParam);
        afx_msg void onAllInbound();
        afx_msg void onAllOutbound();
        afx_msg void onAllTrains();
        afx_msg void onClearAllTrains();
        afx_msg void onDeleteTrainGroup();
        afx_msg void onSaveTrainGroup();
        afx_msg void onSelchangeTrainSelectionTab(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

        /**
         * OnOK
         *
         * This method has been implemented to hide accidental calls when
         * the Enter key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnOK();

        /**
         * OnCancel
         *
         * This method has been implemented to hide accidental calls when
         * the ESC key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnCancel();

         // TES799 ~ added methods
        /**
          * updateAllCarXState
          *
          * These functions update the state of the 'all car' button
          * when one of the PID button in the car is clicked.
          */
        void updateAllCar1State();
        void updateAllCar2State();
        void updateAllCar3State();

    private:

        void setupTrainSelectionComponent();
        void cleanpTrainSelectionComponent();

        // train selection component
        TA_IRS_Bus::TrainSelectorGUI* m_trainSelectorGUI;
        TA_IRS_Bus::MFCTrainSelectorGUI* m_mfcTrainSelector;
        TA_IRS_Bus::TrainSelector* m_trainSelector;

        // manages the PID selection logic
        PidSelectionManager* m_pidSelectionManager;

        // manages the display attribute logic (time and priority controls)
        DisplayAttributeManager* m_displayAttributeManager;

        // manages clear and display buttons
        DisplayRequestManager* m_displayRequestManager;


        // used to enable/disable buttons
        bool m_trainSelected;
        bool m_messageSelected;
        bool m_pidSelected;

        // true if the user has rights to do the following
        bool m_canSendFreeTextDisplayRequest;
        bool m_canSendPredefinedDisplayRequest;
        bool m_canSendClearRequest;

    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_TRAINDISPLAYPAGE_H__8AEB3BB0_157B_4544_BBC1_497DEB014E8F__INCLUDED_)
