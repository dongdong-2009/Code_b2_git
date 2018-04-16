/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/AlarmGenerator.h,v $
  * @author:  Andrew Del Carlo
  * @version: $Revision: 1.1.2.1 $
  *
  * Last modification: $Date: 2003/06/23 05:00:14 $
  * Last modified by:  $Author: andrewdc $
  * 
  * AlarmGenerator : Defines the class behaviors for the application.
  */

#if !defined(AFX_ALARMGENERATOR_H__B1926A89_8DE4_45B6_8E9D_9BFD8AAC9B4E__INCLUDED_)
#define AFX_ALARMGENERATOR_H__B1926A89_8DE4_45B6_8E9D_9BFD8AAC9B4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAlarmGeneratorApp:
// See AlarmGenerator.cpp for the implementation of this class
//

class CAlarmGeneratorApp : public CWinApp
{
public:
	CAlarmGeneratorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmGeneratorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAlarmGeneratorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMGENERATOR_H__B1926A89_8DE4_45B6_8E9D_9BFD8AAC9B4E__INCLUDED_)
