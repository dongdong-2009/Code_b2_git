/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/sound/sound_manager/stub/src/ConnectionPointTest.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// ConnectionPointTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ConnectionPointTest.h"
#include "ConnectionPointTestDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ConnectionPointTestApp

BEGIN_MESSAGE_MAP(ConnectionPointTestApp, CWinApp)
	//{{AFX_MSG_MAP(ConnectionPointTestApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ConnectionPointTestApp construction

ConnectionPointTestApp::ConnectionPointTestApp()
: m_soundManager(this)
{
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only ConnectionPointTestApp object

ConnectionPointTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// ConnectionPointTestApp initialization

BOOL ConnectionPointTestApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	ConnectionPointTestDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

// Event handlers
void ConnectionPointTestApp::handleCategorySilenced(const std::string& category, bool isSilenced)
{
    TRACE2("%s category %s\n", category.c_str(), (isSilenced ? "silenced" : "unsilenced"));
}

void ConnectionPointTestApp::handleAllSilenced(bool isSilenced)
{
    TRACE1("All categories %s\n", (isSilenced ? "silenced" : "unsilenced"));
}