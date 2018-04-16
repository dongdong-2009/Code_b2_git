/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/TransActiveOPCServer.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// TransActiveOPCServer.cpp : Defines the class behaviors for the application.
//
#pragma warning ( disable:4786 )

#include "stdafx.h"
#include "TransActiveOPCServer.h"
#include "TransActiveOPCServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransActiveOPCServerApp

BEGIN_MESSAGE_MAP(CTransActiveOPCServerApp, CWinApp)
	//{{AFX_MSG_MAP(CTransActiveOPCServerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransActiveOPCServerApp construction

CTransActiveOPCServerApp::CTransActiveOPCServerApp()
:
m_dlg ( 0 )
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTransActiveOPCServerApp object

CTransActiveOPCServerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTransActiveOPCServerApp initialization

BOOL CTransActiveOPCServerApp::InitInstance()
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

	if ( m_dlg == 0 )
	{
		m_dlg = new CTransActiveOPCServerDlg();
	}

	m_pMainWnd = m_dlg;
	int nResponse = m_dlg->DoModal();
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

	// clean up objects of the dlg and the dlg itself
	m_dlg->cleanUp();
	delete m_dlg;
	m_dlg = 0;

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
