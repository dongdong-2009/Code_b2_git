/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/src/HistoryViewer.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
