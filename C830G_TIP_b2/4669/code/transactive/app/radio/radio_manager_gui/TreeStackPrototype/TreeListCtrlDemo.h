/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/TreeStackPrototype/TreeListCtrlDemo.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// TreeListCtrlDemo.h : main header file for the TREELISTCTRLDEMO application
//

#if !defined(AFX_TREELISTCTRLDEMO_H__DD5B4639_596F_11D3_9784_000000000000__INCLUDED_)
#define AFX_TREELISTCTRLDEMO_H__DD5B4639_596F_11D3_9784_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrlDemoApp:
// See TreeListCtrlDemo.cpp for the implementation of this class
//

class CTreeListCtrlDemoApp : public CWinApp
{
public:
	CTreeListCtrlDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeListCtrlDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTreeListCtrlDemoApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREELISTCTRLDEMO_H__DD5B4639_596F_11D3_9784_000000000000__INCLUDED_)
