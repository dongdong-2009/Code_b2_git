// BasParameterManager.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "app/bas/BasParameterManager/src/BasParameterManager.h"
#include "app/bas/BasParameterManager/src/BasParameterManagerDlg.h"
#include "app/bas/BasParameterManager/src/BasParameterManagerGUI.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBasParameterManagerApp

BEGIN_MESSAGE_MAP(CBasParameterManagerApp, TA_Base_Bus::TransActiveWinApp)
	ON_COMMAND(ID_HELP, &TA_Base_Bus::TransActiveWinApp::OnHelp)
END_MESSAGE_MAP()


// CBasParameterManagerApp construction

CBasParameterManagerApp::CBasParameterManagerApp()
:TA_Base_Bus::TransActiveWinApp(new TA_IRS_App::BasParameterManagerGUI(), "设备参数管理器")
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBasParameterManagerApp object

CBasParameterManagerApp theApp;


// CBasParameterManagerApp initialization
int CBasParameterManagerApp::ExitInstance()
{
	return TA_Base_Bus::TransActiveWinApp::ExitInstance();
}