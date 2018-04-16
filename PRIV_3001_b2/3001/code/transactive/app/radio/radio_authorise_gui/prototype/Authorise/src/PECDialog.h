/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_authorise_gui/prototype/Authorise/src/PECDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(AFX_PECDIALOG_H__CE14ABD0_ECC2_42B3_8690_869C740190A9__INCLUDED_)
#define AFX_PECDIALOG_H__CE14ABD0_ECC2_42B3_8690_869C740190A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PECDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPECDialog dialog

class CPECDialog : public CDialog
{
// Construction
public:
	CPECDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPECDialog)
	enum { IDD = IDD_PEC_DIALOG };
	CString	m_type;
	CString	m_source;
	CString	m_destination;
	CString	m_statusBar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPECDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPECDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PECDIALOG_H__CE14ABD0_ECC2_42B3_8690_869C740190A9__INCLUDED_)
