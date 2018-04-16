/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/telephone_manager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// telephone_manager.h : main header file for the TELEPHONE_MANAGER application
//

#if !defined(AFX_TELEPHONE_MANAGER_H__57855046_8806_11D7_B17D_0050BAB1D931__INCLUDED_)
#define AFX_TELEPHONE_MANAGER_H__57855046_8806_11D7_B17D_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
//#include "code\bus\generic_gui\src\TransActiveWinApp.h"
#include "bus\generic_gui\src\TransActiveWinApp.h"
/////////////////////////////////////////////////////////////////////////////
// CTelephone_managerApp:
// See telephone_manager.cpp for the implementation of this class
//

class CTelephone_managerApp : public TA_Bus::TransActiveWinApp
{
public:
	CTelephone_managerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTelephone_managerApp)
	public:
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTelephone_managerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TELEPHONE_MANAGER_H__57855046_8806_11D7_B17D_0050BAB1D931__INCLUDED_)
