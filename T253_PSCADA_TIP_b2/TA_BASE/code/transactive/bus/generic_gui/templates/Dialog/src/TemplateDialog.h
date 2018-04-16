/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/generic_gui/templates/Dialog/src/TemplateDialog.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  *
  */

#if !defined(AFX_TEMPLATEDIALOG_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_)
#define AFX_TEMPLATEDIALOG_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// CTemplateDialogApp:
// See TemplateDialog.cpp for the implementation of this class
//

class CTemplateDialogApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	CTemplateDialogApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateDialogApp)
	public:
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTemplateDialogApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEDIALOG_H__13EF9430_909E_4319_9238_94999F357E69__INCLUDED_)
