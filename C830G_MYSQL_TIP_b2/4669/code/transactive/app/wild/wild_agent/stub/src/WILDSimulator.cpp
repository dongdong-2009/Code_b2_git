/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/wild/wild_agent/stub/src/WILDSimulator.cpp $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */

#include "stdafx.h"
#include "WILDSimulator.h"
#include "WILDSimulatorDlg.h"
#include "WILDSimulatorGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

/////////////////////////////////////////////////////////////////////////////
// CWILDSimulatorApp

BEGIN_MESSAGE_MAP(CWILDSimulatorApp, TransActiveWinApp)
	//{{AFX_MSG_MAP(CWILDSimulatorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWILDSimulatorApp construction

CWILDSimulatorApp::CWILDSimulatorApp() : TransActiveWinApp(new WILDSimulatorGUI(), "Application Name")
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWILDSimulatorApp object

CWILDSimulatorApp theApp;

