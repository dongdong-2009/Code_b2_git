// TrainBorneManager.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TrainBorneManager.h"
#include "TrainBorneManagerDlg.h"

#include "TrainBorneGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTrainBorneTestApp
namespace TA_IRS_App
{
// CTrainBorneManagerApp

BEGIN_MESSAGE_MAP(CTrainBorneManagerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTrainBorneManagerApp construction

bool userClose = false;
static LPTOP_LEVEL_EXCEPTION_FILTER defaultAppCrashHandler = NULL;

static LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)  
{     
	if((!userClose) && (defaultAppCrashHandler != NULL))
	{
		return defaultAppCrashHandler(pException);
	}
    return EXCEPTION_EXECUTE_HANDLER;  
}  

CTrainBorneManagerApp::CTrainBorneManagerApp()
: TA_Base_Bus::TransActiveWinApp( new TrainBorneGUI(), "TrainBorne" )
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	defaultAppCrashHandler = SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
}


// The one and only CTrainBorneManagerApp object

CTrainBorneManagerApp theApp;


// CTrainBorneManagerApp initialization

BOOL CTrainBorneManagerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
/*	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CTrainBorneManagerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
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
*/
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	BOOL result = TA_Base_Bus::TransActiveWinApp::InitInstance();
	return result;
}

int CTrainBorneManagerApp::ExitInstance()
{
	int result = TA_Base_Bus::TransActiveWinApp::ExitInstance(); 
    return result;
}
}