/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/alarm/alarm_manager/src/AlarmsGui.cpp $
  * @author:  A. Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Defines the class behaviors for the application. Launches the Alarm Manager as
  * a managed application using Generic GUI. 
  */


#include "stdafx.h"
#include "app\alarm\alarm_manager\src\AlarmViewerGUI.h"
#include "app\alarm\alarm_manager\src\AlarmsGui.h"
#include "ace/ACE.h"
#include "core\utilities\src\DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarmsGuiApp

BEGIN_MESSAGE_MAP(CAlarmsGuiApp, TA_Base_Bus::TransActiveWinApp)
	//{{AFX_MSG_MAP(CAlarmsGuiApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmsGuiApp construction

CAlarmsGuiApp::CAlarmsGuiApp()
: TA_Base_Bus::TransActiveWinApp( new TA_Base_App::AlarmViewerGUI(), "Alarm Manager" )
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	LOGBEFORECOTS("ACE::init" );
	ACE::init();
	LOGAFTERCOTS("ACE::init");
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAlarmsGuiApp object

CAlarmsGuiApp theApp;

BOOL CAlarmsGuiApp::InitInstance()
{
	return TransActiveWinApp::InitInstance();
}
