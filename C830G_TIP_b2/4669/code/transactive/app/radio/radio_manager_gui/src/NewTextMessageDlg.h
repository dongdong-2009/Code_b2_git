/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/NewTextMessageDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_NEWTEXTMESSAGEDLG_H__0C8ACD92_FF5E_432C_804F_C657C48EA18C__INCLUDED_)
#define AFX_NEWTEXTMESSAGEDLG_H__0C8ACD92_FF5E_432C_804F_C657C48EA18C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewTextMessageDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewTextMessageDlg dialog

class CNewTextMessageDlg : public CDialog
{
// Construction
public:
	CNewTextMessageDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewTextMessageDlg)
	enum { IDD = IDD_NEW_TEXT_MESSAGE };
	CString	m_SDSShortName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewTextMessageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    virtual BOOL OnInitDialog();

	// Generated message map functions
	//{{AFX_MSG(CNewTextMessageDlg)
	afx_msg void onChangeSDSName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWTEXTMESSAGEDLG_H__0C8ACD92_FF5E_432C_804F_C657C48EA18C__INCLUDED_)
