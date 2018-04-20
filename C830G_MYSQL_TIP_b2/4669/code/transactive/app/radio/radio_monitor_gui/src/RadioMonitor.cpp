/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/src/RadioMonitor.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Main application entry point for the Radio Monitor application
  *
  */

#include "app/radio/radio_monitor_gui/src/stdafx.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitor.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorGUI.h"
#include "bus/generic_gui/src/IGUIApplication.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_IRS_App
{
    
    /////////////////////////////////////////////////////////////////////////////
    // RadioMonitorApp
    
    BEGIN_MESSAGE_MAP(RadioMonitorApp, TA_Base_Bus::TransActiveWinApp)
    	//{{AFX_MSG_MAP(RadioMonitorApp)
    		// NOTE - the ClassWizard will add and remove mapping macros here.
    		//    DO NOT EDIT what you see in these blocks of generated code!
    	//}}AFX_MSG
    END_MESSAGE_MAP()
    
    /////////////////////////////////////////////////////////////////////////////
    // RadioMonitorApp construction
    
    RadioMonitorApp::RadioMonitorApp()
    : TA_Base_Bus::TransActiveWinApp( new RadioMonitorGUI(), "Radio Monitor" )
    {
    	// Place all significant initialization in InitInstance
    }
    
    RadioMonitorApp::~RadioMonitorApp()
    {
    }
    
    
    int RadioMonitorApp::ExitInstance()
    {
        int result = TransActiveWinApp::ExitInstance(); 
    
        // Clean up all static instances that we may (or may not have) used
        // It is expected this will already have been called (before servants 
        // de-activated etc) - call again just in case
        cleanupSingletons();
    
        return result;
    }
    
    
    void RadioMonitorApp::cleanupSingletons()
    {
        CachedConfig::removeInstance();
        RadioMonitorErrorHandler::removeInstance();
    }
    
    
    /////////////////////////////////////////////////////////////////////////////
    // The one and only RadioMonitorApp object
    
    RadioMonitorApp theApp;

} // end namespace TA_IRS_App
