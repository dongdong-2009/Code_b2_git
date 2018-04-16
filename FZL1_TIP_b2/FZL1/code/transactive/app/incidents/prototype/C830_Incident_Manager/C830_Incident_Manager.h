/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/incidents/prototype/C830_Incident_Manager/C830_Incident_Manager.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// C830_Incident_Manager.h : main header file for the C830_INCIDENT_MANAGER application
//

#if !defined(AFX_C830_INCIDENT_MANAGER_H__D35420FD_BEF8_4131_99F3_ACADF0528D7E__INCLUDED_)
#define AFX_C830_INCIDENT_MANAGER_H__D35420FD_BEF8_4131_99F3_ACADF0528D7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CC830_Incident_ManagerApp:
// See C830_Incident_Manager.cpp for the implementation of this class
//

class CC830_Incident_ManagerApp : public CWinApp
{
public:
	CC830_Incident_ManagerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CC830_Incident_ManagerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CC830_Incident_ManagerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_C830_INCIDENT_MANAGER_H__D35420FD_BEF8_4131_99F3_ACADF0528D7E__INCLUDED_)
