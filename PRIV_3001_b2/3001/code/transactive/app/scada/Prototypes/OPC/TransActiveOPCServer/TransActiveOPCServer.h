/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/TransActiveOPCServer.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// TransActiveOPCServer.h : main header file for the TRANSACTIVEOPCSERVER application
//

#if !defined(AFX_TRANSACTIVEOPCSERVER_H__04684DCB_D433_4374_8CEB_400D2E748BDE__INCLUDED_)
#define AFX_TRANSACTIVEOPCSERVER_H__04684DCB_D433_4374_8CEB_400D2E748BDE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

class CTransActiveOPCServerDlg;

/////////////////////////////////////////////////////////////////////////////
// CTransActiveOPCServerApp:
// See TransActiveOPCServer.cpp for the implementation of this class
//

class CTransActiveOPCServerApp : public CWinApp
{
public:
	CTransActiveOPCServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransActiveOPCServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	CTransActiveOPCServerDlg * m_dlg;

// Implementation

	//{{AFX_MSG(CTransActiveOPCServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSACTIVEOPCSERVER_H__04684DCB_D433_4374_8CEB_400D2E748BDE__INCLUDED_)
