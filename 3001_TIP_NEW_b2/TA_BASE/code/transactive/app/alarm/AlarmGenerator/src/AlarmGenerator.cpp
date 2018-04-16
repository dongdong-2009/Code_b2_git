/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/AlarmGenerator.cpp,v $
  * @author:  Andrew Del Carlo
  * @version: $Revision: 1.1.2.2.60.2 $
  *
  * Last modification: $Date: 2005/05/25 03:02:15 $
  * Last modified by:  $Author: derrickl $
  *
  * Last modification: $Date: 2005/05/25 03:02:15 $
  * Last modified by:  $Author: derrickl $
  * 
  * AlarmGenerator.cpp : Generate alarm based on the those types defined in the database
  *                      Alarm generated will be submitted to the alarm agent for actions.
  *                      This simulator also allows user to close, open and ack existing active alarms.
  */


#include "stdafx.h"

#include "AlarmGenerator.h"
#include "AlarmGeneratorDlg.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"

#include "agDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarmGeneratorApp

BEGIN_MESSAGE_MAP(CAlarmGeneratorApp, CWinApp)
	//{{AFX_MSG_MAP(CAlarmGeneratorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmGeneratorApp construction

CAlarmGeneratorApp::CAlarmGeneratorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAlarmGeneratorApp object

CAlarmGeneratorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAlarmGeneratorApp initialization

BOOL CAlarmGeneratorApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	agDebugInit();
	agDebug("APP->AlarmGenerator Started.");
	TA_Base_Core::RunParams::getInstance().parseCmdLine(m_lpCmdLine);
	TA_Base_Core::RunParams::getInstance().set( RPARAM_LOCATIONKEY, "1" );
	CAlarmGeneratorDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		TA_Base_Core::CorbaUtil::getInstance().shutdown();
	}
	else if (nResponse == IDCANCEL)
	{
		TA_Base_Core::CorbaUtil::getInstance().shutdown();
	}

	agDebug("APP->AlarmGenerator Exited.");

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
