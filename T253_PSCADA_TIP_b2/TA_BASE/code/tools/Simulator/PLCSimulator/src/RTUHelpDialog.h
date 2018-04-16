#if !defined(AFX_HELPDIALOG_H__A349824E_E2BC_4611_99AC_669AB7A91B05__INCLUDED_)
#define AFX_HELPDIALOG_H__A349824E_E2BC_4611_99AC_669AB7A91B05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RTUHelpDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRTUHelpDialog dialog

class CRTUHelpDialog : public CDialog
{
// Construction
public:
	CRTUHelpDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRTUHelpDialog)
	enum { IDD = IDD_DIALOG_HELP };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTUHelpDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRTUHelpDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HELPDIALOG_H__A349824E_E2BC_4611_99AC_669AB7A91B05__INCLUDED_)
