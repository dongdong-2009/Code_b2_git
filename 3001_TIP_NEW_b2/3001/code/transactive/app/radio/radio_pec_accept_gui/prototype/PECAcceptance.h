/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/radio/radio_pec_accept_gui/prototype/PECAcceptance.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// PECAcceptance.h : main header file for the PECACCEPTANCE application
//

#if !defined(AFX_PECACCEPTANCE_H__6BF6AB49_D9D9_4471_A717_0B1C22C5B165__INCLUDED_)
#define AFX_PECACCEPTANCE_H__6BF6AB49_D9D9_4471_A717_0B1C22C5B165__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPECAcceptanceApp:
// See PECAcceptance.cpp for the implementation of this class
//

class CPECAcceptanceApp : public CWinApp
{
public:
	CPECAcceptanceApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPECAcceptanceApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPECAcceptanceApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PECACCEPTANCE_H__6BF6AB49_D9D9_4471_A717_0B1C22C5B165__INCLUDED_)
