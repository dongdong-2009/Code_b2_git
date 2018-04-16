/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// PAManager.cpp : Defines the class behaviors for the application.
//

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/GwxDisplayWrapper.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/RunParamListener.h"
#include "app/pa/PAManager/src/PAManagerGUI.h"
#include "app/pa/PAManager/src/PAManager.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PAManagerApp

BEGIN_MESSAGE_MAP(PAManagerApp, TA_Base_Bus::TransActiveWinApp)
//{{AFX_MSG_MAP(PAManagerApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PAManagerApp construction

PAManagerApp::PAManagerApp() : TA_Base_Bus::TransActiveWinApp( new PAManagerGUI(), "PA Manager" )
{
	// Place all significant initialization in InitInstance
}

PAManagerApp::~PAManagerApp()
{
}

BOOL PAManagerApp::InitInstance()
{
    createSingletons();

    return TransActiveWinApp::InitInstance();
}

int PAManagerApp::ExitInstance()
{
    int result = TransActiveWinApp::ExitInstance(); 

    // Clean up all static instances that we may (or may not have) used
    // It is expected this will already have been called (before servants 
    // de-activated etc) - call again just in case
    destroySingletons();

    return result;
}

void PAManagerApp::destroySingletons()
{
	RunParamListener::destroyInstance();

    GwxDisplayWrapper::destroyInstance();

    PaDataManager::destoryInstance();
}

void PAManagerApp::createSingletons()
{
    GwxDisplayWrapper::createInstance();

    PaDataManager::createInstance();

	RunParamListener::createInstance();
}
/////////////////////////////////////////////////////////////////////////////
// The one and only PAManagerApp object

PAManagerApp theApp;
