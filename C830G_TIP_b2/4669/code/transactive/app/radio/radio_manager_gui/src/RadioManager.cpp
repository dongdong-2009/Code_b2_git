/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// TestDialogs.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RadioManager.h"
#include "RadioManagerDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "ace/OS.h"

// runparam. When this is set, the call with this id will be selected in the calls list
const char* CRadioManager::RPARAM_CALLID = "CallId";


/////////////////////////////////////////////////////////////////////////////
// CTestDialogsApp

BEGIN_MESSAGE_MAP(CRadioManager, TA_Base_Bus::TransActiveWinApp)
	//{{AFX_MSG_MAP(CRadioManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDialogsApp construction

CRadioManager::CRadioManager()
: TA_Base_Bus::TransActiveWinApp(new RadioManagerGUI(), "Radio Manager" )
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	AfxInitRichEdit();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTestDialogsApp object

CRadioManager theApp;

