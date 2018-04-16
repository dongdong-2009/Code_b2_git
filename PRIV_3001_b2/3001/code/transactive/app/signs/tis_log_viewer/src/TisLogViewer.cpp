/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_log_viewer/src/TisLogViewer.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  */

#include "stdafx.h"
#include "TisLogViewer.h"
#include "TisLogViewerDlg.h"
#include "TisLogViewerGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

namespace TA_IRS_App
{

/////////////////////////////////////////////////////////////////////////////
// TisLogViewerApp

BEGIN_MESSAGE_MAP(TisLogViewerApp, TransActiveWinApp)
    // {{AFX_MSG_MAP(TisLogViewerApp)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        // DO NOT EDIT what you see in these blocks of generated code!
    // }}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TisLogViewerApp construction

TisLogViewerApp::TisLogViewerApp()
 : TransActiveWinApp(new TisLogViewerGUI(), "Tis Log Viewer"),
   m_refreshInterval(0)
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}


unsigned long TisLogViewerApp::getRefreshIntervalInSecs()
{
    return m_refreshInterval;
}


void TisLogViewerApp::setRefreshIntervalInSecs( unsigned long interval )
{
    m_refreshInterval = interval;
}


/////////////////////////////////////////////////////////////////////////////
// The one and only TisLogViewerApp object

TisLogViewerApp theApp;


} // TA_App