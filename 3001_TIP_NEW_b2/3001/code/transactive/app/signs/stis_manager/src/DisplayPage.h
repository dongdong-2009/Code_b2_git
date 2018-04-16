/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/DisplayPage.h $
  * @author:  Adam Radics
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2014/05/12 16:16:15 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * The Station display page
  */

#if !defined(DISPLAYPAGE_H)
#define DISPLAYPAGE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"
#include "MessageTypeTab.h"
#include "PidSelectionManager.h"
#include "TimeControlManager.h"
#include "IMessageSelectionListener.h"
#include "RangeNumberEdit.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

#include <vector>
#include <list>
#include <map>
#include <string>


#define PREDEFINED 0
#define FREE_TEXT 1

#define NORMAL_PRIORITY 0
#define EMERGENCY_PRIORITY 1
#define ALL_PRIORITY 2

using TA_Base_Bus::TISAgentAccessFactory;
namespace TA_IRS_App
{
    class STISPredefinedMessages;   // TD11310 ~ added

    class DisplayPage : public CDialog,
                        public IPidSelectionListener,
                        public IMessageSelectionListener
    {

    public:
	    DisplayPage(CWnd* pParent = NULL);	// standard constructor
	    ~DisplayPage();
		
        void InitAll(/*REBProgressManager & mgr*/);

        /** 
          * pidSelectionChanged
          *
          * Gets called when the PID selection is changed.
          *
          * @param pidsInList   true if at least one PID is in the list
          * @param pidSelectionExists   true if one PID in the list is selected
          */
        virtual void pidSelectionChanged(bool pidsInList, bool pidSelectionExists);


        /** 
          * predefinedMessageSelected
          *
          * Called when the predefined message tab is selected.
          * Sets whether a message is selected and if so the details needed to
          * populate certain fields on the display page.
          *
          * This is also called when a message is selected/deselected.
          *
          * @param tabSwitched          only true when the tab has just been switched to
          * @param validMessageSelected
          * @param priority
          * @param repeatInterval
          *
          */
        virtual void predefinedMessageSelected( bool tabSwitched,
                                                bool validMessageSelected,
                                                unsigned short priority = 0,
                                                unsigned short repeatInterval = 0);


        /** 
          * adHocMessageSelected
          *
          * Called when the ad hoc message tab is selected.
          * Sets whether a message has been entered. 
          *
          * This is also called when a message is types/cleared.
          *
          * @param tabSwitched          only true when the tab has just been switched to
          * @param validMessageEntered
          * @param repeatInterval       only used to set the default when the tab is switched
          */
        virtual void adHocMessageSelected( bool tabSwitched,
                                           bool validMessageEntered,
                                           unsigned short repeatInterval );


		//TD 15349
		//zhou yuan++
		bool findAndSelectStationMessage(const std::string& messageName);
		//++zhou yuan


    protected:
        // Dialog Data
	    //{{AFX_DATA(DisplayPage)
	    enum { IDD = IDD_DISPLAY_PAGE };
	    CButton	m_displayButton;
	    CButton	m_clearNormalButton;
	    CButton	m_clearEmergencyButton;
	    CButton	m_clearAllButton;
	    CMessageTypeTab	m_messageTypeTab;
        ColourCombo	m_priority;
        CallbackButton	m_radioTimed;
        CallbackButton	m_radioContinuous;
        CallbackDateTimeCtrl	m_startDate;
	    CallbackDateTimeCtrl	m_startTime;
        CallbackDateTimeCtrl	m_endDate;
	    CallbackDateTimeCtrl	m_endTime;
		CRangeNumberEdit m_repeatInterval;
		CSpinButtonCtrl m_repeatIntervalSpin;
        CStatic	m_startTimeLabel;
	    CStatic	m_repeatLabel;
	    CStatic	m_endTimeLabel;
        PidListCtrl m_PIDList;
        PidGroupCombo	m_pidGroupCombo;
        CallbackButton	m_deleteGroupButton;
	    CallbackButton	m_saveGroupButton;
    	//}}AFX_DATA

	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(DisplayPage)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
	    HICON m_hIcon;

	    // Generated message map functions
	    //{{AFX_MSG(DisplayPage)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnDisplay();
	    afx_msg void OnClearAll();
	    afx_msg void OnClearNormal();
	    afx_msg void OnClearEmergency();
	    afx_msg void OnDestroy();
        afx_msg LRESULT onRightsChanged(WPARAM wParam, LPARAM lParam);
		afx_msg void OnKillFocusRepeatInterval();
	    //}}AFX_MSG
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



    private:
	
		std::string timeToString( const CTime& time );

        /** 
          * submitClearRequest
          *
          * Submits a clear request
          *
          * @param clearType    The name eg All, Normal etc - for UI messages
          * @param lowerPriority    the lower clear priority
          * @param upperPriority    the upper clear priority
          *
          */
        void submitClearRequest( std::string clearType,
                                 unsigned short lowerPriority,
                                 unsigned short upperPriority );
		void updateClearButtonState();

		void updateDisplayButtonState();

		void initRightsCheck();

        bool m_pidsInList;
        bool m_validMessageSelected;
        bool m_canDisplay;
        bool m_canClear;
        bool m_canModifyPidGroups;

        // all PID and PID group logic is encapsulated here
        PidSelectionManager* m_pidSelectionManager;

        // all time control logic is encapsulated here
        TimeControlManager* m_timeControlManager;

        STISPredefinedMessages* m_stisPredefinedMessages;    // TD11310 ~ added

		bool m_canDisplayAdhoc;
		bool m_canDisplayPredefined;
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


} // TA_IRS_App

#endif // !defined(DISPLAYPAGE_H)
