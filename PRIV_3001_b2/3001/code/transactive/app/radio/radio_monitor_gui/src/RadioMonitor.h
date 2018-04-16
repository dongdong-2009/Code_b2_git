/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_monitor_gui/src/RadioMonitor.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Main application entry point for the Radio Monitor application
  *
  */

#if !defined(AFX_RadioMonitor_H__BDA71947_87B3_4D13_90C2_5E8F9311AB9D__INCLUDED_)
#define AFX_RadioMonitor_H__BDA71947_87B3_4D13_90C2_5E8F9311AB9D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "app/radio/radio_monitor_gui/src/resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"
#include "bus/generic_gui/src/GenericGUI.h"

namespace TA_IRS_App
{
    /////////////////////////////////////////////////////////////////////////////
    // RadioMonitorApp:
    //
    class RadioMonitorApp : public TA_Base_Bus::TransActiveWinApp
    {
    public:
    	RadioMonitorApp();
        virtual ~RadioMonitorApp();
    
        /**
         * ExitInstance (extension of base class implementation)
         *
         * Performs some additional cleanup (singleton instances etc)
         *
         */
        virtual int ExitInstance();
    
        /**
         * cleanupSingletons
         *
         * Calls removeInstance on singletons in use by application
         * This can be called as many times as application wants to
         * (and in some cases, is known to be called twice when known
         * the singleton destruction can't be postponed, because of 
         * late subscriber destruction issues)
         *
         */ 
        static void cleanupSingletons();
    
        
        /**
         * attemptRestart
         *
         * Attempts to re-launch PA Manager
         *
         */
        void attemptRestart();
    
    // Overrides
    	// ClassWizard generated virtual function overrides
    	//{{AFX_VIRTUAL(CTISManagerApp)
    	public:
    	//}}AFX_VIRTUAL
    
    // Implementation
    
    	//{{AFX_MSG(CTISManagerApp)
    		// NOTE - the ClassWizard will add and remove member functions here.
    		//    DO NOT EDIT what you see in these blocks of generated code !
    	//}}AFX_MSG
    	DECLARE_MESSAGE_MAP()
    };
    
    /////////////////////////////////////////////////////////////////////////////    
    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
    
} // end namespace TA_IRS_App

#endif // !defined(AFX_RadioMonitor_H__BDA71947_87B3_4D13_90C2_5E8F9311AB9D__INCLUDED_)
