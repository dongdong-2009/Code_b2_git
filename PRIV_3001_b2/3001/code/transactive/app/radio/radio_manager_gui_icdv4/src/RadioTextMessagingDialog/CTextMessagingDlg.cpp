/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CTextMessagingDlg.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include <afxpriv.h>
#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CTextMessagingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTextMessagingDlg

IMPLEMENT_DYNAMIC(CTextMessagingDlg, CPropertySheet)

CTextMessagingDlg::CTextMessagingDlg(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CMultiFontPropertySheet(nIDCaption, pParentWnd, iSelectPage),
	m_sendNotifyToParent(false),
	m_blaunchedFromToolbar(true)
{
}

CTextMessagingDlg::CTextMessagingDlg(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CMultiFontPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    m_psh.dwFlags |= PSH_NOAPPLYNOW;
    m_psh.dwFlags &= ~PSH_HASHELP;
	
    AddPage(&m_sendTextPage);
    AddPage(&m_inboxPage);
}

CTextMessagingDlg::~CTextMessagingDlg()
{
	if (m_fntPage.m_hObject)
		VERIFY (m_fntPage.DeleteObject ());
}

BEGIN_MESSAGE_MAP(CTextMessagingDlg, CPropertySheet)
	//{{AFX_MSG_MAP(CTextMessagingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextMessagingDlg message handlers


BOOL CTextMessagingDlg::OnInitDialog() 
{
	CMultiFontPropertySheet::OnInitDialog();

    m_tabCtrlEx.Install(this);

	// by default SendText page is disabled(launched from Toolbar)
	if (m_blaunchedFromToolbar)
	{
		SetActivePage(&m_inboxPage);
        m_tabCtrlEx.EnableItem(0, FALSE);
	}
	else
    {
		m_sendTextPage.EnableWindow(TRUE); 
    }

    CWnd *wnd;

    wnd = GetDlgItem(IDCANCEL);
    if (wnd) wnd->ShowWindow(SW_HIDE);
    wnd = GetDlgItem(IDHELP);
    if (wnd) wnd->ShowWindow(SW_HIDE);

    wnd = GetDlgItem(ID_APPLY_NOW);
    if (wnd) wnd->ShowWindow(SW_HIDE);

    wnd = GetDlgItem(IDOK);
    if (wnd)
    {
        wnd->ShowWindow(SW_HIDE);
    }

    return TRUE;
}


void CTextMessagingDlg::setRadioResource(RadioResource& resource)
{
	m_sendTextPage.setRadioResource(resource);
	
	m_blaunchedFromToolbar = false;
}

