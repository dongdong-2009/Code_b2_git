// BASManager.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "app/bas/BASModeManager/src/BASManager.h"
#include "app/bas/BASModeManager/src/BASManagerDlg.h"
#include "app/bas/BASModeManager/src/BASManagerGUI.h"
#include "core/utilities/src/RunParams.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBASManagerApp
BEGIN_MESSAGE_MAP(CBASManagerApp, TA_Base_Bus::TransActiveWinApp)
	ON_COMMAND(ID_HELP, &TA_Base_Bus::TransActiveWinApp::OnHelp)
END_MESSAGE_MAP()


// CBASManagerApp Constructor
CBASManagerApp::CBASManagerApp()
: TransActiveWinApp( new TA_IRS_App::BASManagerGUI(), "BAS Mode Manager" )
{

}


// CBASManagerApp object
CBASManagerApp theApp;
