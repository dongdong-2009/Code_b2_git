/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_gui/templates/PropertySheet/src/TemplatePropertySheet.h $
  * @author:  
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  *
  */

#if !defined(AFX_TEMPLATEPROPERTYSHEET_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_)
#define AFX_TEMPLATEPROPERTYSHEET_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// CTemplatePropertySheetApp:
// See TemplatePropertySheet.cpp for the implementation of this class
//

class CTemplatePropertySheetApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	CTemplatePropertySheetApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplatePropertySheetApp)
	public:
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTemplatePropertySheetApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEPROPERTYSHEET_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_)
