/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/system_control/control_station/stub/src/Dummy_Process_Monitor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// Dummy_Process_Monitor.h : main header file for the DUMMY_PROCESS_MONITOR application
//

#if !defined(AFX_DUMMY_PROCESS_MONITOR_H__3589DB3E_9592_4DB4_A61B_A9912E2CC63E__INCLUDED_)
#define AFX_DUMMY_PROCESS_MONITOR_H__3589DB3E_9592_4DB4_A61B_A9912E2CC63E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// DummyProcessMonitorApp:
// See Dummy_Process_Monitor.cpp for the implementation of this class
//

class DummyProcessMonitorApp : public CWinApp
{
public:
	DummyProcessMonitorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DummyProcessMonitorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(DummyProcessMonitorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUMMY_PROCESS_MONITOR_H__3589DB3E_9592_4DB4_A61B_A9912E2CC63E__INCLUDED_)
