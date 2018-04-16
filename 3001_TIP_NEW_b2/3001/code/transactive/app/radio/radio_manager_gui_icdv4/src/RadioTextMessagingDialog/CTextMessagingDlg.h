/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CTextMessagingDlg.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * This dialog is used to:
 * - Send text (SDS) messages to the selected radio resource
 * - Read incoming messages
 */

#if !defined(AFX_TAPROPERTYSHEET_H__F59F5AFF_C911_4D4E_B0CF_58AEAFE5B8EC__INCLUDED_)
#define AFX_TAPROPERTYSHEET_H__F59F5AFF_C911_4D4E_B0CF_58AEAFE5B8EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextMessagingDlg.h : header file
//

#include "bus/mfc_extensions/src/multi_font_property_sheet/MultiFontPropertySheet.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CRadioInboxPage.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CRadioSendTextPage.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioTextMessageFacade.h"
#include "app/radio/radio_manager_gui_icdv4/src/TabCtrlEx.h"


/////////////////////////////////////////////////////////////////////////////
// CTextMessagingDlg

class CTextMessagingDlg : public CMultiFontPropertySheet 
{
	protected:
		DECLARE_DYNAMIC(CTextMessagingDlg)

	// Construction
	public:
		CTextMessagingDlg(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
		CTextMessagingDlg(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

		virtual ~CTextMessagingDlg();

		/**
		 * setParentToReceiveNotifyMessages (simple accessor)
		 */    
		void setParentToReceiveNotifyMessages(bool bSet) { m_sendNotifyToParent = bSet; }

		void setRadioResource(RadioResource& resource);
   

	public:

	    // Overrides
		//{{AFX_VIRTUAL(CTextMessagingDlg)
		public:
		virtual BOOL OnInitDialog();
		protected:
		//}}AFX_VIRTUAL



	protected:
		//{{AFX_MSG(CTextMessagingDlg)
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()


	// Attributes
	private:

		bool m_blaunchedFromToolbar;

		bool m_sendNotifyToParent;

        CTabCtrlEx m_tabCtrlEx;

	protected:
		
		CRadioSendTextPage m_sendTextPage;

		CRadioInboxPage m_inboxPage;
};



#endif 
