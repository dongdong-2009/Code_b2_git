#if !defined(AFX_CONFIGDLG_H__B27761EA_456C_4F91_B4B3_EA9B6326204F__INCLUDED_)
#define AFX_CONFIGDLG_H__B27761EA_456C_4F91_B4B3_EA9B6326204F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigDlg.h : header file
//

#include "IConfigServerObserver.h"
#include "ParamReader.h"
/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog

class CConfigDlg : public CDialog
{
// Construction
public:
	CConfigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigDlg();
	void attachListener (IConfigServerObserver * ptrListener);
// Dialog Data
	//{{AFX_DATA(CConfigDlg)
	enum { IDD = IDD_DLG_CONFIG };
	CButton	m_chkChkSumOverRide;
	CString	m_strIpAddress;
	CString	m_strServicePort;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigDlg)
	afx_msg void OnBtnUpdate();
	afx_msg void OnChkChksumoveride();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	IConfigServerObserver * m_ptrListener;
	BOOL					m_bCheck;
	TA_IRS_App::IParamReader * m_paramReader;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDLG_H__B27761EA_456C_4F91_B4B3_EA9B6326204F__INCLUDED_)
