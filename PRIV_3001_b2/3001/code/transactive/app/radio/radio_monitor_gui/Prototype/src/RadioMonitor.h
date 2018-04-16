/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_monitor_gui/Prototype/src/RadioMonitor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// RadioMonitor.h : main header file for the RADIOMONITOR application
//

#if !defined(AFX_RADIOMONITOR_H__B06D3353_67BC_4F32_8342_C4C5ABAAB0E8__INCLUDED_)
#define AFX_RADIOMONITOR_H__B06D3353_67BC_4F32_8342_C4C5ABAAB0E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRadioMonitorApp:
// See RadioMonitor.cpp for the implementation of this class
//

class CRadioMonitorApp : public CWinApp
{
public:
	CRadioMonitorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioMonitorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRadioMonitorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOMONITOR_H__B06D3353_67BC_4F32_8342_C4C5ABAAB0E8__INCLUDED_)
