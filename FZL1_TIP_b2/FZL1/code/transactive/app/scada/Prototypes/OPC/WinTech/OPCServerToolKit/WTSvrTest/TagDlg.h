#if !defined(AFX_TAGDLG_H__C99DAAF0_2AEB_11D3_80DF_0060975858BE__INCLUDED_)
#define AFX_TAGDLG_H__C99DAAF0_2AEB_11D3_80DF_0060975858BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// TagDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTagDlg dialog

class CTagDlg : public CDialog
{
// Construction
public:
	CTagDlg(CWnd* pParent = NULL);   // standard constructor

	VARIANT	m_value;
	DWORD	m_ReadAccess, m_WriteAccess;

	CButton	*pArrayLen;
	CStatic *pLenTxt;

// Dialog Data
	//{{AFX_DATA(CTagDlg)
	enum { IDD = IDD_ADDTAG };
	CString	m_name;
	CString	m_description;
	CString	m_units;
	BOOL	m_doArray;
	int		m_arraylength;
	//}}AFX_DATA

	float	m_alarms[4];
	DWORD	m_severity[4];
	BOOL	m_enabled[4];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTagDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTagDlg)
	afx_msg void OnAlarms();
	virtual BOOL OnInitDialog();
	afx_msg void OnBoolean();
	afx_msg void OnFloat();
	afx_msg void OnInteger();
	afx_msg void OnString();
	virtual void OnOK();
	afx_msg void OnArray();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAGDLG_H__C99DAAF0_2AEB_11D3_80DF_0060975858BE__INCLUDED_)
