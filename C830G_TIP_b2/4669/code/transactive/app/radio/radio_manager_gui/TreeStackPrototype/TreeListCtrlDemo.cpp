/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/TreeStackPrototype/TreeListCtrlDemo.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// TreeListCtrlDemo.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TreeListCtrlDemo.h"
#include "TreeListCtrlDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrlDemoApp

BEGIN_MESSAGE_MAP(CTreeListCtrlDemoApp, CWinApp)
	//{{AFX_MSG_MAP(CTreeListCtrlDemoApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrlDemoApp construction

CTreeListCtrlDemoApp::CTreeListCtrlDemoApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTreeListCtrlDemoApp object

CTreeListCtrlDemoApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrlDemoApp initialization

BOOL CTreeListCtrlDemoApp::InitInstance()
{
	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CTreeListCtrlDemoDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
