/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioManager.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */


#include "app/radio/radio_manager_gui_icdv4/src/StdAfx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManagerFramework.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioManagerApp

BEGIN_MESSAGE_MAP(CRadioManagerApp, CWinApp)
	//{{AFX_MSG_MAP(CRadioManagerApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioManagerApp construction

CRadioManagerApp::CRadioManagerApp() : TransActiveWinApp( new RadioManagerFramework(),_T("Radio Manager"))
{
    FUNCTION_ENTRY("CRadioManagerApp");
    FUNCTION_EXIT;
}

CRadioManagerApp::~CRadioManagerApp()
{
    FUNCTION_ENTRY("~CRadioManagerApp");
    FUNCTION_EXIT;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRadioManagerApp object

CRadioManagerApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CRadioManagerApp initialization

BOOL CRadioManagerApp::InitInstance()
{
    FUNCTION_ENTRY("InitInstance");
    FUNCTION_EXIT;
	
	return TransActiveWinApp::InitInstance();	// must call this!
}
