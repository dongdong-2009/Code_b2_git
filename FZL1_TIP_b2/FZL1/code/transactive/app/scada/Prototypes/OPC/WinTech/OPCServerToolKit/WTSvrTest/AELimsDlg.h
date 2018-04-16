#if !defined(AFX_AELIMSDLG_H__99317841_FB83_11D3_833C_204C4F4F5020__INCLUDED_)
#define AFX_AELIMSDLG_H__99317841_FB83_11D3_833C_204C4F4F5020__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AELimsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAELimsDlg dialog

class CAELimsDlg : public CDialog
{
// Construction
public:
	CAELimsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAELimsDlg)
	enum { IDD = IDD_AE_LIMITS };
	BOOL	m_hi_enable;
	BOOL	m_hihi_enable;
	BOOL	m_lo_enable;
	BOOL	m_lolo_enable;
	float	m_hihi_level;
	float	m_hi_level;
	float	m_lo_level;
	float	m_lolo_level;
	DWORD	m_hihi_severity;
	DWORD	m_hi_severity;
	DWORD	m_lolo_severity;
	DWORD	m_lo_severity;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAELimsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CEdit *pLimits[4], *pSeverities[4];
	CButton	*pChk[4];

	// Generated message map functions
	//{{AFX_MSG(CAELimsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLochk();
	afx_msg void OnLolochk();
	afx_msg void OnHichk();
	afx_msg void OnHihichk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AELIMSDLG_H__99317841_FB83_11D3_833C_204C4F4F5020__INCLUDED_)
