/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/TelephoneApp.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Responsible for creating the TelephoneManagerFramework class and creating and
  * displaying the CTelephoneManagerDlg class.
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"
#include "app/telephone/telephone_manager/src/TelephoneManagerFramework.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TelephoneApp

BEGIN_MESSAGE_MAP(TelephoneApp, TransActiveWinApp)
	//{{AFX_MSG_MAP(TelephoneApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

TelephoneApp::TelephoneApp()
    : TA_Base_Bus::TransActiveWinApp(new TelephoneManagerFramework(), "Telephone Manager")
{
    FUNCTION_ENTRY( "TelephoneApp" );
    FUNCTION_EXIT;
}


TelephoneApp::~TelephoneApp()
{
    FUNCTION_ENTRY( "~TelephoneApp" );
    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// The one and only TelephoneApp object
TelephoneApp theApp;
