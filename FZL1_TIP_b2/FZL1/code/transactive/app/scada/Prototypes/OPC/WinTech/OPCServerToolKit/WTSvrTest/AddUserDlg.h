#if !defined(AFX_ADDUSERDLG_H__CB564880_6D84_11D6_8A07_000476323C57__INCLUDED_)
#define AFX_ADDUSERDLG_H__CB564880_6D84_11D6_8A07_000476323C57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AddUserDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAddUserDlg dialog

class CAddUserDlg : public CDialog
{
// Construction
public:
	CAddUserDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAddUserDlg)
	enum { IDD = IDD_NEWUSER };
	CString	m_group;
	DWORD	m_accessrights;
	CString	m_user;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddUserDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddUserDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADDUSERDLG_H__CB564880_6D84_11D6_8A07_000476323C57__INCLUDED_)
