// BasParameterManager.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CBasParameterManagerApp:
// See BasParameterManager.cpp for the implementation of this class
//
#include "bus/generic_gui/src/TransActiveWinApp.h"
class CBasParameterManagerApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	CBasParameterManagerApp();

	// Overrides
	public:
	virtual int ExitInstance();
	// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CBasParameterManagerApp theApp;