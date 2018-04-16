#if !defined(AFX_TAGLISTDLG_H__47BED7B8_6C4F_11D4_80FF_00C04F790F3B__INCLUDED_)
#define AFX_TAGLISTDLG_H__47BED7B8_6C4F_11D4_80FF_00C04F790F3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TagListDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTagListDlg dialog

class CTagListDlg : public CDialog
{
// Construction
public:
	CTagListDlg(CWnd* pParent = NULL);   // standard constructor
	int		m_tagcount;
	BOOL	m_randomupdates;
// Dialog Data
	//{{AFX_DATA(CTagListDlg)
	enum { IDD = IDD_CREATETAGS };
	BOOL	m_booltype;
	BOOL	m_floattype;
	BOOL	m_inttype;
	BOOL	m_strtype;
	int		m_updatefrequency;
	BOOL	m_dohashing;
	BOOL	m_dolistupdates;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTagListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTagListDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRandom();
	afx_msg void OnIncremental();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TAGLISTDLG_H__47BED7B8_6C4F_11D4_80FF_00C04F790F3B__INCLUDED_)
