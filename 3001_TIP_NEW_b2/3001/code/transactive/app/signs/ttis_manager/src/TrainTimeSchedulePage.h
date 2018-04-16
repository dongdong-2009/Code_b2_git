/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TrainTimeSchedulePage.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is the train time schedule page. It allows editing of train time schedules
  * in the database.
  *
  */

#if !defined(AFX_TRAINTIMESCHEDULEPAGE_H__D88CF840_681D_4C21_B66B_AFE2B9ED3D12__INCLUDED_)
#define AFX_TRAINTIMESCHEDULEPAGE_H__D88CF840_681D_4C21_B66B_AFE2B9ED3D12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainTimeSchedulePage.h : header file
//

#include "EditFreeText.h"
#include "TimeScheduleEntryListCtrl.h"
#include "REBProgressManager.h"
//xufeng++ 2006/11/06 TD14367
#include "CallbackDateTimeCtrl.h"
//++xufeng 2006/11/06 TD14367

#include "core/data_access_interface/tis_agent/src/ITrainTimeSchedule.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"

using TA_Base_Bus::ListCtrlSelNoFocus;

namespace TA_IRS_App
{
    class TTISPredefinedMessages;
    class TrainTimeScheduleManager;
}

namespace TA_IRS_App
{

    class CTrainTimeSchedulePage : public CDialog, public IDateTimeListener
    {
        // Construction
    public:
        CTrainTimeSchedulePage(CWnd* pParent = NULL);   // standard constructor
		
		//xufeng++ 2006/11/06 TD14367
		~CTrainTimeSchedulePage();
		//++xufeng 2006/11/06 TD14367
		
		void Init(REBProgressManager & mgr);
		
        // Dialog Data
        //{{AFX_DATA(CTrainTimeSchedulePage)
	    enum { IDD = IDD_TRAIN_TIME_SCHEDULE };
	    CStatic	m_endLabel;
	    CStatic	m_priorityLabel;
	    CStatic	m_repeatLabel;
	    CStatic	m_startLabel;
        ListCtrlSelNoFocus	m_scheduleLibraryList;
        CEdit	m_scheduleName;
        CallbackDateTimeCtrl	m_endTime;
        CallbackDateTimeCtrl	m_startTime;
        CDateTimeCtrl	m_repeatInterval;
        CComboBox	m_priority;
	    ListCtrlSelNoFocus	m_predefinedList;
        CEdit	m_searchNormal;
        TimeScheduleEntryListCtrl	m_scheduleEntryList;
        CEdit	m_messageContent;
        ListCtrlSelNoFocus	m_freeTextList;
        CButton	m_removeEntry;
	    CButton	m_removeAllEntries;
	    CButton	m_newSchedule;
	    CButton	m_editSchedule;
	    CButton	m_editFreeText;
	    CButton	m_deleteSchedule;
	    CButton	m_copySchedule;
        CButton	m_addEntry;
        CButton	m_save;
	//}}AFX_DATA

		//xufeng++ 2006/11/06 TD14367
		
        /** 
          * dateTimeChanged
          *
          * Gets called when a date/time is changed.
          *
          * @param control   the control that was changed
          */
        virtual void dateTimeChanged(CallbackDateTimeCtrl* control);
        
		/** 
          * getStartTime
          *
          * gets the start time HHMM
          *
          * @return the start time. HHMM.
          */
        CTime getStartTime() const;


        /** 
          * getEndTime
          *
          * gets the end time HHMM
          *
          * @return the end time. HHMM.
          */
        CTime getEndTime() const;
		//++xufeng 2006/11/06 TD14367

    private:
        CBitmap m_rightArrowBitmap;
        
        
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CTrainTimeSchedulePage)
    public:
        virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL
        
        // Implementation
    protected:
        
        // Generated message map functions
        //{{AFX_MSG(CTrainTimeSchedulePage)
        virtual BOOL OnInitDialog();
        afx_msg void OnEditFreeText();
        afx_msg void OnAddEntry();
        afx_msg void OnRemoveEntry();
        afx_msg void OnRemoveAllEntries();
        afx_msg void OnSaveSchedule();
        afx_msg void OnNewSchedule();
        afx_msg void OnDeleteSchedule();
        afx_msg void OnEditSchedule();
        afx_msg void OnUpdateSearchNormal();
        afx_msg void OnScheduleNameChange();
        afx_msg void onClickFreeTextList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onItemchangedFreeText(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onClickPredefinedList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onItemchangedPredefinedList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onClickScheduleList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onItemchangedScheduleList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onClickScheduleEntryList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onItemchangedScheduleEntryList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT onUpdateCurrentTTISVersion(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onTimeScheduleChanged(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onRightsChanged(WPARAM wParam, LPARAM lParam);
	    afx_msg void OnDestroy();
	    afx_msg void onCopySchedule();
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
        
        /** 
         * loadTimeSchedules
         *
         * Loads all time schedules from the database, and populates the list
         * of time schedules.
         */
        void loadTimeSchedules();
        
        /** 
         * cleanUpTimeSchedules
         *
         * cleans up all time schedules loaded from the database, and clears the list
         * of time schedules.
         */
        void cleanUpTimeSchedules();
        
        /** 
         * loadPreDefined
         *
         * Loads all pre-defined messages from the database, and populates the list.
         */
        void loadPreDefined();

        /** 
          * setEditMode
          *
          * Sets whether edit mode is active
          *
          * @param editMode true if the current schedule should be opened for editing
          */
        void setEditMode(bool editMode);
        
        /** 
          * enableScheduleManagementButtons
          *
          * enables/disables schedule management buttons based on gui criteria
          */
        void enableScheduleManagementButtons();

        /** 
          * enableScheduleEditingButtons
          *
          * enables/disables schedule editing buttons based on gui criteria
          */
        void enableScheduleEditingButtons();

        /** 
          * enableTimeScheduleFields
          *
          * enables/disables schedule fields based on whether a schedule is loaded or not
          */
        void enableTimeScheduleFields();

        /** 
          * freeTextSelected
          *
          * @return true if a free text message is selected
          */
        bool freeTextSelected();

        /** 
          * predefinedSelected
          *
          * @return true if a predefined message is selected
          */
        bool predefinedSelected();

        /** 
          * scheduleEntrySelected
          *
          * @return true if a schedule entry is selected
          */
        bool scheduleEntrySelected();

        /** 
          * resetToBlankSchedule
          *
          * Resets the page to displaying nothing
          */
        void resetToBlankSchedule();

        /** 
         * populateTimeScheduleData
         *
         * When a time schedule is selected for editing, this populates
         * the fields on the page with it's data.
         *
         * @param newSchedule a pointer to a time schedule object.
         *        Can be either one from the list or a new schedule.
         */
        void populateTimeScheduleData(TA_IRS_Core::ITrainTimeSchedule* newSchedule);
        
        /** 
          * messageSelectionChanged
          *
          * Called when the selection in the free text or predefined list changes.
          */
        void messageSelectionChanged();
        
        /** 
          * scheduleListSelectionChanged
          *
          * Called when the selection in the time schedule list changes.
          */
        void scheduleListSelectionChanged();

        /** 
          * scheduleEntryListSelectionChanged
          *
          * Called when the selection in the time schedule entry list changes.
          */
        void scheduleEntryListSelectionChanged();

        /** 
          * deselectAll
          *
          * deselect all items in the given list
          */
        void deselectAll(CListCtrl* theList);

        /** 
          * selectCurrentSchedule
          *
          * Select the currently loaded time schedule
          */
        void selectCurrentSchedule();


		/**
		  * setPriority
		  * 
		  * Set the default priority and whether the user can change it or if its disabled.
		  * 
		  * @param : newPriority The new priority
		  * @param : canChange true if the priority selection should be enabled
		  * 
		  */
		void setPriority( unsigned short newPriority, bool canChange = true);


		/**
		  * setRepeatInterval
		  * 
		  * Sets the repeat interval, and whether it can be changed by the user.
		  * 
		  * @return void 
		  * @param : repeatInterval The new repeat interval ( 1 to 255)
		  * @param : canChange Whether it can be changed or not.
		  * 
		  */
		void setRepeatInterval( unsigned short repeatInterval, bool canChange = true );


        // The dialog used to edit free text messages
        CEditFreeText m_freeTextDlg;

        // the current schedule being edited
        TA_IRS_Core::ITrainTimeSchedule* m_currentSchedule;

        // access rights
        bool m_canEditSchedule;

        // current mode - view or edit
        bool m_editMode;

		int m_nMaxScheduleMessage;	//CL20137, hongzhi, Schedule message limit on ISCS-MFT

        TTISPredefinedMessages* m_ttisPredefinedMessages;
        TrainTimeScheduleManager* m_trainTimeScheduleManager;

		std::map<int,unsigned long> m_scheduleKeyMap;		//add by hongzhi
    };

} // end TA_IRS_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRAINTIMESCHEDULEPAGE_H__D88CF840_681D_4C21_B66B_AFE2B9ED3D12__INCLUDED_)
