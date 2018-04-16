#if !defined(AFX_DELIMITDLG_H__97C67671_7C35_11D5_882B_204C4F4F5020__INCLUDED_)
#define AFX_DELIMITDLG_H__97C67671_7C35_11D5_882B_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DelimitDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDelimitDlg dialog

class CDelimitDlg : public CDialog
{
// Construction
public:
	CDelimitDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDelimitDlg)
	enum { IDD = IDD_DELIMITERDLG };
	CString	m_delimiter;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDelimitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDelimitDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DELIMITDLG_H__97C67671_7C35_11D5_882B_204C4F4F5020__INCLUDED_)
