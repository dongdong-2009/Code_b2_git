/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_agent/src/HiddenDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// HiddenDialog.cpp : implementation file
//

#include "stdafx.h"
#include "telephone_agent.h"
#include "HiddenDialog.h"
#include "TSAPINotificationThread.h"
#include "TelephoneSwitch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHiddenDialog dialog


CHiddenDialog::CHiddenDialog(TA_App::TelephoneSwitch* telephoneSwitch,CWnd* pParent /*=NULL*/)
	: CDialog(CHiddenDialog::IDD, pParent), m_telephoneSwitch(telephoneSwitch)
{
	//{{AFX_DATA_INIT(CHiddenDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CHiddenDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHiddenDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHiddenDialog, CDialog)
	//{{AFX_MSG_MAP(CHiddenDialog)
  ON_MESSAGE(WM_ACSEVENT, OnACSEvent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHiddenDialog message handlers

LRESULT CHiddenDialog::OnACSEvent (WPARAM wParam, LPARAM lParam) 
{
//	std::cout << "Event Received\n";
	//get the switch to deal with this
	m_telephoneSwitch->getEventFromSwitchBlocking();
	return 0;
}


BOOL CHiddenDialog::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
//	std::cout << "Message Received" << pMsg->message << "\n";
	
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CHiddenDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_thread->setHiddenDialogCreated();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
