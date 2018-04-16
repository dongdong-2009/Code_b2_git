/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/signs/pids_manager/src/StationPidManagerApp.cpp $
  * @author:  Wu Min Tao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif
#include "app/signs/pids_manager/src/StdAfx.h"
#include "app/signs/pids_manager/src/StationPidManagerApp.h"
#include "app/signs/pids_manager/src/StationPidManagerGUI.h"
#include "app/signs/pids_manager/src/GwxDisplayWrapper.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/TransActiveMessage.h" //TD18416 AVteam
////#include "stdafx.h"
////#include "StationPidManagerApp.h"
////#include "PidManagerDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{

BEGIN_MESSAGE_MAP(StationPidManagerApp,  TA_Base_Bus::TransActiveWinApp)
	//{{AFX_MSG_MAP(StationPidManagerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	//ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

StationPidManagerApp::StationPidManagerApp()
	:TransActiveWinApp(new StationPidManagerGUI(), "PIDS Manager"){
    FUNCTION_ENTRY("Constructor");
	//	AfxMessageBox("Construct entered");
    FUNCTION_EXIT;
	
}


StationPidManagerApp::~StationPidManagerApp()
{
}

BOOL StationPidManagerApp::InitInstance()
{
    createSingletons();
	return TransActiveWinApp::InitInstance();
}

int StationPidManagerApp::ExitInstance()
{
    destroySingletons();
    return TransActiveWinApp::ExitInstance(); 

    // Clean up all static instances that we may (or may not have) used
    // It is expected this will already have been called (before servants 
    // de-activated etc) - call again just in case
}

void StationPidManagerApp::destroySingletons()
{
    GwxDisplayWrapper::destroyInstance();

    //PaDataManager::destoryInstance();
}

void StationPidManagerApp::createSingletons()
{
    GwxDisplayWrapper::createInstance();

    //PaDataManager::createInstance();
}

//TD18416 AVteam++
//BOOL StationPidManagerApp::InitInstance()
//{
//	FUNCTION_ENTRY("InitInstance");
//	
//	//BOOL result = TransActiveWinApp::InitInstance();
//	//CPidManagerDialog dlg;
//
//	//m_pMainWnd = &dlg;
//	//SetDialogBkColor(RGB(192, 192, 192));
//	StationPidManagerGUI* pGui = new StationPidManagerGUI();
//	pGui->createApplicationObject();
//	int nResponse = IDOK;
//
//	FUNCTION_EXIT;
//	return nResponse;
//}
//TD18416 ++AVteam
/////////////////////////////////////////////////////////////////////////////
// The one and only StationPidManagerApp object
StationPidManagerApp theApp;
}