#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_CTimeTableManagerApp)
#define Transactive_3002_GF_App_Bas_TimeTableManager_CTimeTableManagerApp
#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "app/bas/TimeTableManager/src/resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"
#include "core/utilities/src/DebugUtil.h"

// CTimeTableManagerApp:
// See TimeTableManager.cpp for the implementation of this class
//

class CTimeTableManagerApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	CTimeTableManagerApp();

// Overrides
	public:
	virtual int ExitInstance();

	//clear the singletons object
	static void cleanupSingletons();
// Implementation

	DECLARE_MESSAGE_MAP()
};
#endif //Transactive_3002_GF_App_Bas_TimeTableManager_CTimeTableManagerApp
extern CTimeTableManagerApp theApp;
