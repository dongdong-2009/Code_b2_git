/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_authorise_gui/prototype/Authorise/src/Authorise.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// Authorise.h : main header file for the AUTHORISE application
//

#if !defined(AFX_AUTHORISE_H__3ACA0F68_86B1_460C_8238_1E7AD06DCE8F__INCLUDED_)
#define AFX_AUTHORISE_H__3ACA0F68_86B1_460C_8238_1E7AD06DCE8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAuthoriseApp:
// See Authorise.cpp for the implementation of this class
//

class CAuthoriseApp : public CWinApp
{
public:
	CAuthoriseApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuthoriseApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAuthoriseApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTHORISE_H__3ACA0F68_86B1_460C_8238_1E7AD06DCE8F__INCLUDED_)
