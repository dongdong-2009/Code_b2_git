/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/CIncidentManagerApp.cpp $
  * @author:  Song Toan
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * CIncidentManagerApp inherits from TransActiveWinApp. Object of 
  * CIncidentManagerApp provides member functions for initializing 
  * and running the application
  */
#include "core/utilities/src/FunctionEntry.h"
#include "app/incidents/incident_manager/src/stdafx.h"
#include "app/incidents/incident_manager/src/resource.h"
#include "app/incidents/incident_manager/src/CIncidentManagerApp.h"
#include "app/incidents/incident_manager/src/CMainFrame.h"
#include "app/incidents/incident_manager/src/IncidentManagerDoc.h"
#include "app/incidents/incident_manager/src/CIncidentManagerListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Bus;
/////////////////////////////////////////////////////////////////////////////
// CIncidentManagerApp

namespace TA_IRS_App
{	
	BEGIN_MESSAGE_MAP( CIncidentManagerApp, TA_Base_Bus::TransActiveWinApp )
		//{{AFX_MSG_MAP(CIncidentManagerApp)
		//}}AFX_MSG_MAP
		// Standard file based document commands
		//	ON_COMMAND(ID_FILE_NEW, TransActiveWinApp::OnFileNew)
		//	ON_COMMAND(ID_FILE_OPEN, TransActiveWinApp ::OnFileOpen)
		// Standard print setup command
		//	ON_COMMAND(ID_FILE_PRINT_SETUP, TransActiveWinApp::OnFilePrintSetup)
		END_MESSAGE_MAP()
		
		/////////////////////////////////////////////////////////////////////////////
		// CIncidentManagerApp construction
		
		CIncidentManagerApp::CIncidentManagerApp()
		:TransActiveWinApp( new IncidentManagerFramework(),	// IGUIApplication* gui
		_T("Incident Manager"))		// const CString& applicationName
		
	{
		FUNCTION_ENTRY("CIncidentManagerApp");
		FUNCTION_EXIT;
	}
	
	CIncidentManagerApp::~CIncidentManagerApp()
	{
		FUNCTION_ENTRY("~CIncidentManagerApp");
		FUNCTION_EXIT;
	}
	/////////////////////////////////////////////////////////////////////////////
	// The one and only CIncidentManagerApp object
	
	CIncidentManagerApp theApp;
	
	/////////////////////////////////////////////////////////////////////////////
	// CIncidentManagerApp initialization
	
	BOOL CIncidentManagerApp::InitInstance()
	{
		FUNCTION_ENTRY("InitInstance");
		FUNCTION_EXIT;
		return TransActiveWinApp::InitInstance();	// must call this!
	}
	
	/////////////////////////////////////////////////////////////////////////////
	// CIncidentManagerApp message handlers
}// end TA_IRS_App
