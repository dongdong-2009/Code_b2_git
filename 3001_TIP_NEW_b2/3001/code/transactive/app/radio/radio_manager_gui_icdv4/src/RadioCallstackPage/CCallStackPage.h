/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioCallstackPage/CCallStackPage.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_CALLSTACKPAGE_H__E2CD1158_CCC7_4302_810D_C7172A6540FB__INCLUDED_)
#define AFX_CALLSTACKPAGE_H__E2CD1158_CCC7_4302_810D_C7172A6540FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"

#include "bus/radio/radio_bus/src/RadioTypes.h"

#include "app\radio\radio_manager_gui_icdv4\src\RadioCallstackPage\CallStackFilterList.h"


#include <vector>

#pragma warning (disable : 4786)
/////////////////////////////////////////////////////////////////////////////
// CCallStackPage dialog

class CCallStackPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCallStackPage)

	// Construction
	public:
		CCallStackPage(RECT* parentRect = NULL);
		~CCallStackPage();

	// Dialog Data
		//{{AFX_DATA(CCallStackPage)
	enum { IDD = IDD_CALLSTACK_PAGE };
	//}}AFX_DATA


	// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(CCallStackPage)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(CCallStackPage)
		virtual BOOL OnInitDialog();
		afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg LRESULT onNotifiedResourceAssignment(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceDeAssignment(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedInboundCall(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedInboundCallCanceled(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedInboundPTTStatus(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedInboundPTTStatusReleased(WPARAM wParama, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceActivity(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceEmergency(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceActivityEnded(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceEmergencyAcknowledged(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceEmergencyEnded(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedSelectSpeakerSelected(WPARAM pselectSpeakerData, LPARAM unused);
		afx_msg LRESULT onNotifiedSelectSpeakerUnselected(WPARAM pselectSpeakerData, LPARAM unused);
		afx_msg LRESULT onNotifiedMakePrivateCall(WPARAM pselectSpeakerData, LPARAM unused);
		afx_msg LRESULT onNotifiedListeningCall(WPARAM pselectSpeakerData, LPARAM unused);
		afx_msg LRESULT onNotifiedConnectGroup(WPARAM pselectSpeakerData, LPARAM unused);
		afx_msg LRESULT onNotifiedEditMode(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedOperationMode(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedDatabaseReady(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedPrivateCallEstablished(WPARAM wParam = 0, LPARAM lParam = 0); // TD#14287,14289
		afx_msg LRESULT onNotifiedPrivateCallRinging(WPARAM wParam = 0, LPARAM lParam = 0); // TD#14287
		
		afx_msg LRESULT onNotifiedIncomingDriverCall(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedEstablishedDriverCall(WPARAM wParam = 0, LPARAM lParam = 0);

		
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		CString getStatusTextDescription(RadioResource& resource,unsigned long message, LPARAM pData = 0);

	private:

		/**
		 * {{AFX_MSG(CRadioCallStackListCtrl) }}AFX_MSG
		 */
		void setupListControl();

		void addCall(RadioResource& resource);

		void removeCall(RadioResource& resource);

		// added Status param
		void updateCall(RadioResource& resource, CString statusText = ""); 
		
		CString getTypeText(RadioResourceType type);


	protected:

		CallStackFilterList	m_RadioCallStackListCtrl;
		
	
	private:
		
		std::vector<RadioResource> m_RadioResource;
        RECT m_parentRect;

};

#endif // !defined(AFX_CALLSTACKPAGE_H__E2CD1158_CCC7_4302_810D_C7172A6540FB__INCLUDED_)
