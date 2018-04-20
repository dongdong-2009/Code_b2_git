// WtSvrTst2.h : main header file for the WTSVRTST2 application
//

#if !defined(AFX_WTSVRTST2_H__47BED7A3_6C4F_11D4_80FF_00C04F790F3B__INCLUDED_)
#define AFX_WTSVRTST2_H__47BED7A3_6C4F_11D4_80FF_00C04F790F3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CWtSvrTst2View;

/////////////////////////////////////////////////////////////////////////////
// CWtSvrTst2App:
// See WtSvrTst2.cpp for the implementation of this class
//

class CWtSvrTst2App : public CWinApp
{
public:
	CWtSvrTst2App();
	CWtSvrTst2View	*pView;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWtSvrTst2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWtSvrTst2App)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WTSVRTST2_H__47BED7A3_6C4F_11D4_80FF_00C04F790F3B__INCLUDED_)
