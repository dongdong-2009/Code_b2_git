/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_agent/src/TSAPINotificationThread.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// TSAPINotificationThread.cpp : implementation file
//

#include "stdafx.h"
#include "telephone_agent.h"
#include "TelephoneSwitch.h"
#include "HiddenDialog.h"
#include "TSAPINotificationThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TSAPINotificationThread

IMPLEMENT_DYNCREATE(TSAPINotificationThread, CWinThread)
TSAPINotificationThread::TSAPINotificationThread()
{
	m_switchSet = false;
	m_hiddenDialogCreated = false;
	m_switch = NULL;
}

TSAPINotificationThread::~TSAPINotificationThread()
{
}

BOOL TSAPINotificationThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int TSAPINotificationThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(TSAPINotificationThread, CWinThread)
	//{{AFX_MSG_MAP(TSAPINotificationThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TSAPINotificationThread message handlers

int TSAPINotificationThread::Run() 
{
	while(!m_switchSet)
	{
		Sleep(10);
	}
	m_hiddenDialog = new CHiddenDialog(m_switch);
	m_hiddenDialog->setThread(this);
	m_hiddenDialog->Create(IDD_HIDDEN_DIALOG);
//	m_hiddenDialog->ShowWindow(SW_SHOW);
//	m_hiddenDialogCreated = true;
//	return m_hiddenDialog->DoModal();
	return CWinThread::Run();
}

HWND TSAPINotificationThread::getWindowHandle()
{
	while(!m_hiddenDialogCreated)
	{
		Sleep(1000);
	}
	return m_hiddenDialog->m_hWnd;
}

void TSAPINotificationThread::setSwitch(TA_App::TelephoneSwitch* telephoneSwitch)
{
	m_switch = telephoneSwitch;
	m_switchSet = true;
}

void TSAPINotificationThread::setHiddenDialogCreated()
{
	m_hiddenDialogCreated = true;
}
