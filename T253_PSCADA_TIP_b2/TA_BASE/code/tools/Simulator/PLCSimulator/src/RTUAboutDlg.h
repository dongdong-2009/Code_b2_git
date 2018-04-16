#if !defined(AFX_MYABOUTDLG_H__E2C5AA90_B7B7_4986_98B7_6D2BDA34B9E1__INCLUDED_)
#define AFX_MYABOUTDLG_H__E2C5AA90_B7B7_4986_98B7_6D2BDA34B9E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RTUAboutDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRTUAboutDlg dialog

class CRTUAboutDlg : public CDialog
{
// Construction
public:
	CRTUAboutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRTUAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTUAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRTUAboutDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYABOUTDLG_H__E2C5AA90_B7B7_4986_98B7_6D2BDA34B9E1__INCLUDED_)
