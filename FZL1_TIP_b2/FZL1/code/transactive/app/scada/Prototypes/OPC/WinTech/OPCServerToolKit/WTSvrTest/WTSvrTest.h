// WTSvrTest.h : main header file for the WTSVRTEST application
//

#if !defined(AFX_WTSVRTEST_H__E1407F92_FCB2_11D2_80D5_0060975858BE__INCLUDED_)
#define AFX_WTSVRTEST_H__E1407F92_FCB2_11D2_80D5_0060975858BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestApp:
// See WTSvrTest.cpp for the implementation of this class
//
class CWTSvrTestView;

class CWTSvrTestApp : public CWinApp
{
public:
	CWTSvrTestApp();

	CWTSvrTestView	*pView;

	WCHAR	*WSTRFromCString(const CHAR *buf);
	void	WSTRFree(WCHAR * c);
	void	FormatVariant(VARIANT *pValue, char *buf, int bufsize);
	void	FormatIntArray(VARIANT *pValue, char *buf, int bufsize);
	void	FormatFltArray(VARIANT *pValue, char *buf, int bufsize);
	void	FormatBoolArray(VARIANT *pValue, char *buf, int bufsize);
	void	FormatBstrArray(VARIANT *pValue, char *buf, int bufsize);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWTSvrTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWTSvrTestApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WTSVRTEST_H__E1407F92_FCB2_11D2_80D5_0060975858BE__INCLUDED_)
