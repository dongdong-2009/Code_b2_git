/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/templates/Document/src/TemplateDocument.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */

#include "stdafx.h"
#include "TemplateDocument.h"

#include "MainFrm.h"
#include "TemplateDocumentDoc.h"
#include "TemplateDocumentView.h"
#include "TemplateDocumentGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentApp

BEGIN_MESSAGE_MAP(CTemplateDocumentApp, TransActiveWinApp)
	//{{AFX_MSG_MAP(CTemplateDocumentApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentApp construction

CTemplateDocumentApp::CTemplateDocumentApp()
: TransActiveWinApp( new TemplateDocumentGUI(), "Template Document" )
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTemplateDocumentApp object

CTemplateDocumentApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentApp initialization

BOOL CTemplateDocumentApp::InitInstance()
{
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

    return TransActiveWinApp::InitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentApp message handlers

