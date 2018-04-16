/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CRadioInboxPage.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(AFX_RADIOINBOXPAGE_H__01394A34_AD92_446D_A973_3CA918AD2C36__INCLUDED_)
#define AFX_RADIOINBOXPAGE_H__01394A34_AD92_446D_A973_3CA918AD2C36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RadioInboxPage.h : header file
//

#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "core/message/src/CommsMessageSender.h"

#include <vector>

#pragma warning (disable : 4786)
/////////////////////////////////////////////////////////////////////////////
// CRadioInboxPage dialog

class CRadioInboxPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CRadioInboxPage)


	public:

		CRadioInboxPage();
		~CRadioInboxPage();


		//{{AFX_DATA(CRadioInboxPage)
		enum { IDD = IDD_RADIO_INBOX_PAGE };
		CEdit	m_msgTextEL;
		//}}AFX_DATA

		

		//{{AFX_VIRTUAL(CRadioInboxPage)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL



	protected:

		//{{AFX_MSG(CRadioInboxPage)
		virtual BOOL OnInitDialog();
		afx_msg LRESULT onTextMessageReceived(WPARAM wParam, LPARAM lParam);
		afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClose();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:

		void refreshInbox();
		/*
		 * Methods to populate historical messages in inbox		
		 */
		std::string timeToString(const time_t t);

		/*
		 * Send notification to CallBanner that message received is read
		 * Fix for TD#14311
		 */
		void sendNotification(const MessageInbox& message);
	
	private:

		unsigned long m_entityKey;
		unsigned long m_entitySubsystem;
		unsigned long m_entityLocation;
	// Attributes
	protected:

		std::vector<MessageInbox>  m_msgHistoryList;


		TA_Base_Bus::ListCtrlSelNoFocus m_msgHistoryLB;

		// TD#14311
		TA_Base_Core::CommsMessageSender* m_commsSender;
};


#endif // !defined(AFX_RADIOINBOXPAGE_H__01394A34_AD92_446D_A973_3CA918AD2C36__INCLUDED_)
