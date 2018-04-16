// PDS ClientSim.h : main header file for the PDS CLIENTSIM application
//

#if !defined(AFX_PDSCLIENTSIM_H__DAFD640C_4D9D_428C_8E7D_8D2BE2E6C436__INCLUDED_)
#define AFX_PDSCLIENTSIM_H__DAFD640C_4D9D_428C_8E7D_8D2BE2E6C436__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPDSClientSimApp:
// See PDS ClientSim.cpp for the implementation of this class
//

class CPDSClientSimApp : public CWinApp
{
public:
	CPDSClientSimApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPDSClientSimApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPDSClientSimApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PDSCLIENTSIM_H__DAFD640C_4D9D_428C_8E7D_8D2BE2E6C436__INCLUDED_)
