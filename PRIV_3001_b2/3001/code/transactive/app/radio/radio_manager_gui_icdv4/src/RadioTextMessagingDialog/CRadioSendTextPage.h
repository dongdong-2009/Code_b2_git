/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CRadioSendTextPage.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_RADIOSENDTEXTPAGE_H__49928B4E_FAB5_45AE_95FA_7B207E4C1B96__INCLUDED_)
#define AFX_RADIOSENDTEXTPAGE_H__49928B4E_FAB5_45AE_95FA_7B207E4C1B96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/radio/radio_manager_gui_icdv4/src/RadioUserDefinedEvents.h" 

#include "bus/radio/radio_bus/src/RadioTypes.h"


#include <vector>
#include <string>

#pragma warning (disable : 4786)

/////////////////////////////////////////////////////////////////////////////
// CRadioSendTextPage dialog

class CRadioSendTextPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CRadioSendTextPage)

	// Construction  
	public:
		CRadioSendTextPage();
		~CRadioSendTextPage();

		// Dialog Data
			//{{AFX_DATA(CRadioSendTextPage)
		enum { IDD = IDD_RADIO_SEND_TEXT_PAGE };
		CStatic m_staticCharSetStr;
		CStatic	m_staticCharsUsed;
		CStatic	m_staticCharsAvailable;
		CListBox	m_msgSelectionLB;
		CEdit	m_textEditBox;
		CButton	m_saveButton;
		CButton	m_removeButton;
		CButton	m_cancelButton;
		CButton	m_addButton;
		CButton	m_predefinedButton;
		CButton	m_adHocButton;
		CButton	m_closeButton;
		CButton	m_sendButton;
		//}}AFX_DATA


	// Overrides
		//{{AFX_VIRTUAL(CRadioSendTextPage)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	public:
		
		void setRadioResource(RadioResource& resource);


	// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(CRadioSendTextPage)
		virtual BOOL OnInitDialog();
		afx_msg void onAdHocButtonClicked();
		afx_msg void onPredefinedButtonClicked();
		afx_msg void OnAddButtonClicked();
		afx_msg void OnCancelButtonClicked();
		afx_msg void OnCloseButtonClicked();
		afx_msg void OnRemoveButtonClicked();
		afx_msg void OnSaveButtonClicked();
		afx_msg void OnSendButtonClicked();
		afx_msg LRESULT onNotifiedPredefineMessageAdded(WPARAM pdetails, LPARAM unused);
		afx_msg LRESULT onNotifiedPredefineMessageModified(WPARAM pdetails, LPARAM unused);
		afx_msg LRESULT onNotifiedPredefineMessageRemoved(WPARAM pdetails, LPARAM unused);
		afx_msg void OnSelectionChange();
	afx_msg void OnChangeTextEdit();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		
		void setAdHoc(bool pEnable);
        void setMessageText(const std::string& msgText);
        void updateTextCount();


	// Attributes
	private:
		void refreshListBox();
		/**
		 * This will be a CString transfer that hooks into a CStatic that will display the
		 * number of Unicode characters used and number of bytes available to text
		 * messages. See the 4677P26040101-Radio_Manager_Design_Note.DND for notes.
		 */
		CString m_textCount;

		CString m_CharSetText;
		/**
		 * For languages such as English that can be expressed using only the ASCII 
		 * character set (8-bit character coding scheme) the number of characters used/available 
		 * will be 140. For languages such as Chinese that needs to be expressed using the Unicode 
		 * standard (16-bit character coding scheme), the number of characters used/available 
		 * will be a maximum of 70.
		 */
		static const int MAX_ASCII_CHAR_SET;
		static const int MAX_CHINESE_CHAR_SET;

	protected:
		/**
		 * Contains the list of predefined message types.
		 */
		std::vector<PredefinedMessage> m_predefinedMsgList;
		/**
		 * The RadioResource to send the Text messages to
		 */
		RadioResource m_RadioResource;
		
		bool m_bModifyRights;
		bool m_bAddRights;
};


#endif 
