#if !defined(AFX_AUTOALARMSOPTIONDLG_H__67E6DEB2_2A4C_4850_9FC1_2EE01349C85B__INCLUDED_)
#define AFX_AUTOALARMSOPTIONDLG_H__67E6DEB2_2A4C_4850_9FC1_2EE01349C85B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AutoAlarmsOptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AutoAlarmsOptionDlg dialog

class AutoAlarmsOptionDlg : public CDialog
{
// Construction
public:
	AutoAlarmsOptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(AutoAlarmsOptionDlg)
	enum { IDD = IDD_AUTO_ALARMS_OPTION };
	long	m_nCount;
	long	m_nInterval;
	long	m_nThreadCount;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AutoAlarmsOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AutoAlarmsOptionDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOALARMSOPTIONDLG_H__67E6DEB2_2A4C_4850_9FC1_2EE01349C85B__INCLUDED_)
