/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioHistoryPage/CRadioHistoryPage.h $
  * @author:  zou chunzhong
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */

#ifndef RADIO_HISTORY_PAGE_H_INCLUDED
#define RADIO_HISTORY_PAGE_H_INCLUDED


#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"

#include <vector>

class CRadioHistoryPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CRadioHistoryPage)

	public:
		
		CRadioHistoryPage(RECT* parentRect = NULL);
		~CRadioHistoryPage();


		//{{AFX_DATA(CRadioHistoryPage)
		enum { IDD = IDD_HISTORY_PAGE };
		//}}AFX_DATA

		/**
		 * Number of entries in the history list(to be initialized at application startup);
		 */
		void setHistoryLimit(unsigned int number);
		

	//{{AFX_VIRTUAL(CRadioHistoryPage)
	protected:		
		virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	
	protected:
		//{{AFX_MSG(CRadioHistoryPage)
		virtual BOOL OnInitDialog(); 
		afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg LRESULT onNotifiedEditMode(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedOperationMode(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedAuditEventOccurred(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceAssignment(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedResourceDeAssignment(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedInboundCall(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedSelectSpeakerSelected(WPARAM wParam, LPARAM unused);
		afx_msg LRESULT onNotifiedSelectSpeakerUnselected(WPARAM wParam, LPARAM unused);
		afx_msg LRESULT onNotifiedMakePrivateCall(WPARAM wParam, LPARAM unused);
		afx_msg LRESULT onNotifiedListeningCall(WPARAM wParam, LPARAM unused);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()


	private:
		/**
		 *	Returns the RadioResourceType given the CString type
		 */
		RadioResourceType getResourceType(CString sType);

		std::string timeToString(const time_t t);

		CString getTypeText(RadioResourceType type);
		
		void updateHistory(RadioResource& resource, const char* strAction, bool isText=false);

		/*
		 * This function is added for Predefined Text Message updates
		 * Fixed TD#14297
		 */
		void updateHistory(const char* description, const char* strAction);

		void setup();

	private:

		TA_Base_Bus::ListCtrlSelNoFocus	m_HistoryListCtrl;

		std::vector<RadioResource> m_resourceList;
		int m_historyLimit;

        RECT m_parentRect;
};

#endif
