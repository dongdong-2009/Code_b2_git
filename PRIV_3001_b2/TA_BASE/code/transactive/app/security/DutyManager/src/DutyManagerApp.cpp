/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/DutyManager/src/DutyManagerApp.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class provides the entry point into the application.
  * It instantiates DutyManagerGUI, but leaves it to the
  * GenericGUI to display.
  *
  */

#include "StdAfx.h"
#include "app/security/DutyManager/src/DutyManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// The one and only DutyManagerApp object

bool g_populated = false;
DutyManagerApp dutyManagerApp;


/////////////////////////////////////////////////////////////////////////////
// DutyManagerApp

BEGIN_MESSAGE_MAP(DutyManagerApp, TA_Base_Bus::TransActiveWinApp)
	//{{AFX_MSG_MAP(DutyManagerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// DutyManagerApp construction/destruction

DutyManagerApp::DutyManagerApp() : 
    TA_Base_Bus::TransActiveWinApp(new DutyManagerGUI, "Rights Manager")
{
    FUNCTION_ENTRY("DutyManagerApp()");

    // Note: Place all significant initialization in InitInstance

    FUNCTION_EXIT;
}


DutyManagerApp::~DutyManagerApp()
{
    FUNCTION_ENTRY("~DutyManagerApp()");
    FUNCTION_EXIT;
}
