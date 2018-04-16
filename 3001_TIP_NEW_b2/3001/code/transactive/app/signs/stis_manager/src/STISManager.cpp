/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/STISManager.cpp $
  * @author:  Robin Aschcroft
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2014/06/13 16:43:25 $
  * Last modified by:  $Author: huang.wenbo $
  *
  *
  */

#include "stdafx.h"
#include "STISManager.h"
#include "STISManagerGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
	const char* const appName = "TIS Manager";

    BEGIN_MESSAGE_MAP(CSTISManagerApp,  TA_Base_Bus::TransActiveWinApp)
	    //{{AFX_MSG_MAP(CSTISManagerApp)
		    // NOTE - the ClassWizard will add and remove mapping macros here.
		    //    DO NOT EDIT what you see in these blocks of generated code!
	    //}}AFX_MSG
	    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CSTISManagerApp construction
	bool userClose = false;
	static LPTOP_LEVEL_EXCEPTION_FILTER defaultAppCrashHandler = NULL;

	static _CRT_REPORT_HOOK defaultDbgErrorHook = NULL;

	static int DbgErrorReportHook(int reportType, char *message, int *returnValue)
	{
		if((!userClose) && (defaultDbgErrorHook != NULL))
		{
			return defaultDbgErrorHook(reportType, message, returnValue);
		}
		return 1;
	}

	static LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)  
	{     
		if((!userClose) && (defaultAppCrashHandler != NULL))
		{
			return defaultAppCrashHandler(pException);
		}
	    return EXCEPTION_EXECUTE_HANDLER;  
	}  

    CSTISManagerApp::CSTISManagerApp()
    : TA_Base_Bus::TransActiveWinApp( new CSTISManagerGUI(), appName )
    {
		defaultAppCrashHandler = SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
#ifdef _DEBUG
		defaultDbgErrorHook = _CrtSetReportHook(DbgErrorReportHook);
#endif
    }

	BOOL CSTISManagerApp::InitInstance()
	{
		free((void*)m_pszAppName);
		m_pszAppName = _strdup(appName);
		free((void*)afxCurrentAppName);
		afxCurrentAppName = _strdup(appName);
		// Call SystemParametersInfo, make dialog show underline on button text, on matter whether user press the ALT
		BOOL value = TRUE;
        SystemParametersInfo(SPI_SETKEYBOARDCUES, 0, (PVOID)&value, 0);
		return TransActiveWinApp::InitInstance();
	}
    /////////////////////////////////////////////////////////////////////////////
    // The one and only CSTISManagerApp object

    CSTISManagerApp theApp;

    /////////////////////////////////////////////////////////////////////////////
    // CSTISManagerApp initialization

    /*BOOL CSTISManagerApp::InitInstance()
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

	    CSTISManagerDlg dlg;
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
    } */


} // TA_IRS_App
