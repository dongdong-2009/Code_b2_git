/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/system_control/control_station/stub/src/Dummy_Process_Monitor.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// Dummy_Process_Monitor.cpp : Defines the class behaviors for the application.
//

#pragma warning (disable:4786)

#include "stdafx.h"
#include "Dummy_Process_Monitor.h"
#include "Dummy_Process_MonitorDlg.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DummyProcessMonitorApp

BEGIN_MESSAGE_MAP(DummyProcessMonitorApp, CWinApp)
	//{{AFX_MSG_MAP(DummyProcessMonitorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DummyProcessMonitorApp construction

DummyProcessMonitorApp::DummyProcessMonitorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only DummyProcessMonitorApp object

DummyProcessMonitorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// DummyProcessMonitorApp initialization

BOOL DummyProcessMonitorApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

    TA_Base_Core::RunParams::getInstance().parseCmdLine(::GetCommandLine());

	DummyProcessMonitorDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
