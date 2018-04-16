// TimeTableManager.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "app/bas/TimeTableManager/src/TimeTableManager.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerMainDlg.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerGUI.h"
#include "app/bas/TimeTableManager/src/CommonDefine.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTimeTableManagerApp

BEGIN_MESSAGE_MAP(CTimeTableManagerApp, TA_Base_Bus::TransActiveWinApp)
	ON_COMMAND(ID_HELP, &TA_Base_Bus::TransActiveWinApp::OnHelp)
END_MESSAGE_MAP()


// CTimeTableManagerApp construction
using namespace TA_IRS_App;
CTimeTableManagerApp::CTimeTableManagerApp()
:TA_Base_Bus::TransActiveWinApp(new TimeTableManagerGUI(), "TimeTable Manager")
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTimeTableManagerApp object

CTimeTableManagerApp theApp;

int CTimeTableManagerApp::ExitInstance()
{
	FUNCTION_ENTRY("ExitInstance");
	int result = TA_Base_Bus::TransActiveWinApp::ExitInstance(); 

	// Clean up all static instances that we may (or may not have) used
	// It is expected this will already have been called (before servants 
	// de-activated etc) - call again just in case
	cleanupSingletons();
	FUNCTION_EXIT;
	return result;
}

void CTimeTableManagerApp::cleanupSingletons()
{
	FUNCTION_ENTRY("cleanupSingletons");
	FUNCTION_EXIT;
}