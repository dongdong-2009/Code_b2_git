/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/OthersStackDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_OTHERSSTACKDLG_H__BAD21D9A_24BE_4710_8ADB_57AA68E0A4C6__INCLUDED_)
#define AFX_OTHERSSTACKDLG_H__BAD21D9A_24BE_4710_8ADB_57AA68E0A4C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OthersStackDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// OthersStackDlg dialog

class OthersStackDlg : public CDialog
{
// Construction
public:
	OthersStackDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(OthersStackDlg)
	enum { IDD = IDD_STACK_OTHERS };
	CListCtrl	m_othersCallStack;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OthersStackDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(OthersStackDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OTHERSSTACKDLG_H__BAD21D9A_24BE_4710_8ADB_57AA68E0A4C6__INCLUDED_)
