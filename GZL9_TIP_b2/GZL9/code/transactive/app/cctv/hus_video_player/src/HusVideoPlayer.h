
// HusVideoPlayer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CHusVideoPlayerApp:
// See HusVideoPlayer.cpp for the implementation of this class
//

class SingleInstanceGuard
{
public:
	SingleInstanceGuard();
	~SingleInstanceGuard();
	bool isInstanceAlreadyRunning();
private:
	HANDLE m_hStartEvent;
};

class CHusVideoPlayerApp : public CWinApp
{
public:
	CHusVideoPlayerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
private:
	SingleInstanceGuard m_singleInstanceGuard;
};

extern CHusVideoPlayerApp theApp;
