// TunnelModeManager.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManager.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTunnelModeManagerApp

BEGIN_MESSAGE_MAP(CTunnelModeManagerApp, TA_Base_Bus::TransActiveWinApp)
	ON_COMMAND(ID_HELP, &TA_Base_Bus::TransActiveWinApp::OnHelp)
END_MESSAGE_MAP()


// CTunnelModeManagerApp construction

CTunnelModeManagerApp::CTunnelModeManagerApp()
:TA_Base_Bus::TransActiveWinApp(new TA_IRS_App::TunnelModeManagerGUI(), "Tunnel Mode Manager")
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CTunnelModeManagerApp object

CTunnelModeManagerApp theApp;

BOOL CTunnelModeManagerApp::InitInstance()
{
	// Ensures that the mandatory command-line parameter that determines the BAS dialog type 
	// for this application instance (AND hence the corresponding application type)
	// should be set as a RunParam before the framework demands the application type from 
	// TunnelModeManagerGUI
	std::string cmdLine(m_lpCmdLine);
	TA_Base_Core::RunParams::getInstance().parseCmdLine( cmdLine.c_str() );

	// now let the TransActive framework kick in.
	return TransActiveWinApp::InitInstance();
}

BOOL CTunnelModeManagerApp::ExitInstance()
{
	return TA_Base_Bus::TransActiveWinApp::ExitInstance();
}
