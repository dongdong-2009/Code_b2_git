/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_gui/templates/Document/src/TemplateDocument.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  *
  */

#if !defined(AFX_TEMPLATEDOCUMENT_H__A05580F7_39A8_4098_9B49_9E27CC0F3949__INCLUDED_)
#define AFX_TEMPLATEDOCUMENT_H__A05580F7_39A8_4098_9B49_9E27CC0F3949__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "bus/generic_gui/src/TransActiveWinApp.h"

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentApp:
// See TemplateDocument.cpp for the implementation of this class
//

class CTemplateDocumentApp : public TA_Base_Bus::TransActiveWinApp
{
public:
	CTemplateDocumentApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateDocumentApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CTemplateDocumentApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEDOCUMENT_H__A05580F7_39A8_4098_9B49_9E27CC0F3949__INCLUDED_)
