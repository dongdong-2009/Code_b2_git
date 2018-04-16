/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/TelephoneApp.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Responsible for creating the TelephoneManagerFramework class and creating and
  * displaying the CTelephoneManagerDlg class.
  */

#if !defined(TelephoneApp_E80BC983_54D4_444f_987B_BC1FF434C11E__INCLUDED_)
#define TelephoneApp_E80BC983_54D4_444f_987B_BC1FF434C11E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "app/telephone/telephone_manager/src/resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// TelephoneApp:
// See telephone_manager.cpp for the implementation of this class
//

class TelephoneApp : public TA_Base_Bus::TransActiveWinApp
{

public:
	TelephoneApp();
	virtual ~TelephoneApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TelephoneApp)
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(TelephoneApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(TelephoneApp_E80BC983_54D4_444f_987B_BC1FF434C11E__INCLUDED_)
