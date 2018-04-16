/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/DutyManager.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// DutyManager.h : main header file for the DUTYMANAGER application
//

#if !defined(AFX_DUTYMANAGER_H__EFFB7D9B_D634_408A_A301_14B51BF04E56__INCLUDED_)
#define AFX_DUTYMANAGER_H__EFFB7D9B_D634_408A_A301_14B51BF04E56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDutyManagerApp:
// See DutyManager.cpp for the implementation of this class
//

class CDutyManagerApp : public CWinApp
{
public:
	CDutyManagerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDutyManagerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDutyManagerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUTYMANAGER_H__EFFB7D9B_D634_408A_A301_14B51BF04E56__INCLUDED_)
