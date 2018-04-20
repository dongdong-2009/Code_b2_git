/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/TrainSelectPredefinedPage.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is the page that has the normal and emergency
  * pre-defined messages on it.
  *
  */

#if !defined(AFX_TRAINSELECTPREDEFINEDPAGE_H__B8714389_4685_4FE0_AA92_B4372F9FAF9D__INCLUDED_)
#define AFX_TRAINSELECTPREDEFINEDPAGE_H__B8714389_4685_4FE0_AA92_B4372F9FAF9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "app/signs/ttis_manager/src/IMessageSelectionListener.h"

using TA_Base_Bus::ListCtrlSelNoFocus;

namespace TA_IRS_App
{
    class TTISPredefinedMessages;
}

namespace TA_IRS_App
{

    class CTrainSelectPredefinedPage : public CDialog
    {
    // Construction
    public:
	    CTrainSelectPredefinedPage(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
	    //{{AFX_DATA(CTrainSelectPredefinedPage)
	    enum { IDD = IDD_TRAIN_SELECT_PREDEFINED };
        ListCtrlSelNoFocus	m_normalList;
	    ListCtrlSelNoFocus	m_emergencyList;
	    CEdit	m_languages;
	    CEdit	m_searchEmergency;
	    CEdit	m_searchNormal;
	    CEdit	m_predefinedContent;
	    //}}AFX_DATA


        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CTrainSelectPredefinedPage)
	    public:
	    virtual BOOL PreTranslateMessage(MSG* pMsg);
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(CTrainSelectPredefinedPage)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnUpdateSearchEmergency();
	    afx_msg void OnUpdateSearchNormal();
	    afx_msg void OnClickEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnClickNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT OnUpdateCurrentTTISVersion(WPARAM wParam, LPARAM lParam);
	    afx_msg void OnDestroy();
	    afx_msg void onItemchangedTrainNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void onItemchangedEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult);
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

    public:

        
 	    void Init();

		/** 
          * hasValidSelection
          *
          * test if a message is selected.
          *
          * @return true if a valid pre-defined message is selected
          */
        bool hasValidSelection() const;


        /** 
          * getSelectedMessage
          *
          * Retrieves the selected pre-defined message.
          *
          * @see hasValidSelection
          *
          * @return The selected pre-defined message
          */
        TA_Base_Core::PredefinedMessage getSelectedMessage() const;


        /** 
          * windowShown
          *
          * The owning tab calls this when the window is shown.
          * OnShowWindow is not used because the owning tab has not finished
          * switching over when it is called here.
          */
        void windowShown();

		//TD 15349
		//zhou yuan++
		bool findAndSelectStationMessage(const std::string& messageName);
		void setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener);
		//++zhou yuan

    private:

        /** 
          * populateLists
          *
          * Populates the pre-defined message lists.
          *
          */
	    void populateLists();

        /** 
          * getSelectedMessageData
          *
          * Retrieves the selected pre-defined message.
          *
          *
          * @return The selected pre-defined message (pointer)
          *         Null if nothing valid is selected.
          */
        const TA_Base_Core::PredefinedMessage* getSelectedMessageData() const;

        /** 
          * updatePreviewArea
          *
          * updates the message preview area with the text from the selected message.
          * also updates the languages box.
          *
          */
        void updatePreviewArea();

		//TD 15349
		//zhou yuan++
		bool findAndSelectMessageNameInList(CListCtrl& list, const std::string& messageName);
		void clearCurrentSelection(CListCtrl& list);

		IMessageSelectionListener* m_messageSelectionListener;
		//++zhou yuan

        TTISPredefinedMessages* m_ttisPredefinedMessages;
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_TRAINSELECTPREDEFINEDPAGE_H__B8714389_4685_4FE0_AA92_B4372F9FAF9D__INCLUDED_)
