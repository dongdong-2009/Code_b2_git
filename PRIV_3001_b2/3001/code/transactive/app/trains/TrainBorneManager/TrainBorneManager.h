// TrainBorneManager.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "bus/generic_gui/src/TransActiveWinApp.h"
// CTrainBorneManagerApp:
// See TrainBorneManager.cpp for the implementation of this class
//
namespace TA_IRS_App
{
class CTrainBorneManagerApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	CTrainBorneManagerApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTrainBorneManagerApp theApp;
}
