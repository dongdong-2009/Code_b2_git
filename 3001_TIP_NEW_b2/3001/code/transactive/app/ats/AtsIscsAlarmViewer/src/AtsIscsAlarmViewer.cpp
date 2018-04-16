/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsIscsAlarmViewer/src/AtsIscsAlarmViewer.cpp $
  * @author:  A. Lee
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Defines the class behaviors for the application. Launches the Integrated Alarm Viewer as
  * a managed application using Generic GUI. 
  */


#include "stdafx.h"
#include "app/ats/AtsIscsAlarmViewer/src/AtsIscsAlarmViewer.h"
#include "app/ats/AtsIscsAlarmViewer/src/AtsIscsAlarmViewerGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarmsGuiApp

BEGIN_MESSAGE_MAP(CAtsIscsAlarmViewerApp, TA_Base_Bus::TransActiveWinApp)
	//{{AFX_MSG_MAP(CAtsIscsAlarmViewerApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmsGuiApp construction

CAtsIscsAlarmViewerApp::CAtsIscsAlarmViewerApp()
: TA_Base_Bus::TransActiveWinApp( new TA_IRS_App::AtsIscsAlarmViewerGUI(), "ATSISCS Alarm Viewer" )
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAtsIscsAlarmViewerApp object

CAtsIscsAlarmViewerApp theApp;
