// PLCSimulator.h
//

#pragma once
#ifndef __AFXWIN_H__
	#error
#endif

#include "resource.h"

class CPLCSimulatorApp : public CWinApp
{
public:
	CPLCSimulatorApp();
	virtual BOOL InitInstance();
	DECLARE_MESSAGE_MAP()
};
