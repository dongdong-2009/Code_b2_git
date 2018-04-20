/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_authorise_gui/prototype/Authorise/src/AuthoriseCallDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_AUTHORISECALLDLG_H__C155B561_F93D_45FA_ADF9_964E3FEAB081__INCLUDED_)
#define AFX_AUTHORISECALLDLG_H__C155B561_F93D_45FA_ADF9_964E3FEAB081__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuthoriseCallDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAuthoriseCallDlg dialog

class CAuthoriseCallDlg : public CDialog
{
// Construction
public:
	CAuthoriseCallDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAuthoriseCallDlg)
	enum { IDD = IDD_DIALOG1 };
	CString	m_type;
	CString	m_source;
	CString	m_destination;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuthoriseCallDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAuthoriseCallDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTHORISECALLDLG_H__C155B561_F93D_45FA_ADF9_964E3FEAB081__INCLUDED_)
