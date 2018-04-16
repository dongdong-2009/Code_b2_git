/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/DirectorySearchDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(AFX_DIRECTORYSEARCHDIALOG_H__33CA3ED7_8829_11D7_B17D_0050BAB1D931__INCLUDED_)
#define AFX_DIRECTORYSEARCHDIALOG_H__33CA3ED7_8829_11D7_B17D_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirectorySearchDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DirectorySearchDialog dialog

class DirectorySearchDialog : public CDialog
{
// Construction
public:
	DirectorySearchDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(DirectorySearchDialog)
	enum { IDD = IDD_FACILITY_SEARCH };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DirectorySearchDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(DirectorySearchDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTORYSEARCHDIALOG_H__33CA3ED7_8829_11D7_B17D_0050BAB1D931__INCLUDED_)
