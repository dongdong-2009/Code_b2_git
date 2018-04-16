/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_gui/templates/PropertySheet/src/TemplatePropertySheet.cpp $
  * @author:  
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  *
  */

#include "stdafx.h"
#include "TemplatePropertySheet.h"
#include "TemplatePropertySheetDlg.h"
#include "TemplatePropertySheetGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

/////////////////////////////////////////////////////////////////////////////
// CTemplatePropertySheetApp

BEGIN_MESSAGE_MAP(CTemplatePropertySheetApp, TransActiveWinApp)
	//{{AFX_MSG_MAP(CTemplatePropertySheetApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplatePropertySheetApp construction

CTemplatePropertySheetApp::CTemplatePropertySheetApp() : TransActiveWinApp(new TemplatePropertySheetGUI(), "Template Property Sheet")
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTemplatePropertySheetApp object

CTemplatePropertySheetApp theApp;

