/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scheduling/scheduling_manager/src/SchedulingManager.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class inherits from TransActiveWinApp.
  */

#include "app/scheduling/scheduling_manager/src/stdafx.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManager.h"
#include "app/scheduling/scheduling_manager/src/MainFrm.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerDoc.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerView.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

namespace TA_Base_App
{
/////////////////////////////////////////////////////////////////////////////
// CSchedulingManagerApp

BEGIN_MESSAGE_MAP(CSchedulingManagerApp, TransActiveWinApp)
	//{{AFX_MSG_MAP(CSchedulingManagerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CSchedulingManagerApp construction

    CSchedulingManagerApp::CSchedulingManagerApp()
    : TransActiveWinApp( new SchedulingManagerGUI(), "Scheduling Manager" )
    {
	    // TODO: add construction code here,
	    // Place all significant initialization in InitInstance
    }

    /////////////////////////////////////////////////////////////////////////////
    // The one and only CSchedulingManagerApp object

    CSchedulingManagerApp theApp;

    /////////////////////////////////////////////////////////////////////////////
    // CSchedulingManagerApp initialization

    BOOL CSchedulingManagerApp::InitInstance()
    {
	    LoadStdProfileSettings();  // Load standard INI file options (including MRU)

        return TransActiveWinApp::InitInstance();
    }


    /////////////////////////////////////////////////////////////////////////////
    // CSchedulingManagerApp message handlers

};
