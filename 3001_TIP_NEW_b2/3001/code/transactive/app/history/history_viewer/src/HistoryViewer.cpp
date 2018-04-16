/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/history_viewer/src/HistoryViewer.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#include "stdafx.h"
#include "HistoryViewer.h"
#include "HistoryViewerGUI.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#include "cots/ACE/5_3/ACE_wrappers/ace/OS.h"

namespace TA_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerApp

        BEGIN_MESSAGE_MAP(CHistoryViewerApp, TA_Bus::TransActiveWinApp)
        //{{AFX_MSG_MAP(CHistoryViewerApp)
	        // NOTE - the ClassWizard will add and remove mapping macros here.
	        //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerApp construction

    CHistoryViewerApp::CHistoryViewerApp()
    : TA_Bus::TransActiveWinApp(new TA_App::HistoryViewerGUI, "Trend Viewer")
    {
        // TODO: add construction code here,
        // Place all significant initialization in InitInstance
    }

    /////////////////////////////////////////////////////////////////////////////
    // The one and only CHistoryViewerApp object

    CHistoryViewerApp theApp;

} // TA_History
