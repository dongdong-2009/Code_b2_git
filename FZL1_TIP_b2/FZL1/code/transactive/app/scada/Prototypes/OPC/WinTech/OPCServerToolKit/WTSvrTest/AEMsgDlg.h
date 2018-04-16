#if !defined(AFX_AEMSGDLG_H__E1DAB78D_F9BE_11D3_80BE_00C04F790F3B__INCLUDED_)
#define AFX_AEMSGDLG_H__E1DAB78D_F9BE_11D3_80BE_00C04F790F3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AEMsgDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAEMsgDlg dialog

class CAEMsgDlg : public CDialog
{
// Construction
public:
	CAEMsgDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAEMsgDlg)
	enum { IDD = IDD_USERAE };
	CString	m_msg;
	int		m_severity;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAEMsgDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAEMsgDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AEMSGDLG_H__E1DAB78D_F9BE_11D3_80BE_00C04F790F3B__INCLUDED_)
