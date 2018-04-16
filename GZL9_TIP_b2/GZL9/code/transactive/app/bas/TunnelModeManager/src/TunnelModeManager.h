// TunnelModeManager.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

// CTunnelModeManagerApp:
// See TunnelModeManager.cpp for the implementation of this class
//

class CTunnelModeManagerApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	CTunnelModeManagerApp();

// Overrides
	public:
	 /**
	  * InitInstance
	  *
	  * This method must be implemented for TunnelModeManager to ensure that the mandatory command line 
	  * parameter that determines the BAS dialog type for this application instance (AND hence 
	  * the corresponding application type) should be set as a RunParam before the framework 
	  * demands the application type from TunnelModeManagerGUI.
	  */
	virtual BOOL InitInstance();

	virtual BOOL ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTunnelModeManagerApp theApp;