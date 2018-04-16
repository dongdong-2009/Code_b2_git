// OraclePasswordAdministrator.h : main header file for the ORACLEPASSWORDADMINISTRATOR application
//

#if !defined(AFX_ORACLEPASSWORDADMINISTRATOR_H__1FB94A0F_97A7_47ED_9EFE_CCF84296726C__INCLUDED_)
#define AFX_ORACLEPASSWORDADMINISTRATOR_H__1FB94A0F_97A7_47ED_9EFE_CCF84296726C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// COraclePasswordAdministratorApp:
// See OraclePasswordAdministrator.cpp for the implementation of this class
//

class COraclePasswordAdministratorApp : public CWinApp
{
public:
	COraclePasswordAdministratorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COraclePasswordAdministratorApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(COraclePasswordAdministratorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ORACLEPASSWORDADMINISTRATOR_H__1FB94A0F_97A7_47ED_9EFE_CCF84296726C__INCLUDED_)
