// OraclePasswordAdministratorDlg.h : header file
//

#if !defined(AFX_ORACLEPASSWORDADMINISTRATORDLG_H__EEF74BD6_140D_487D_8498_39437A1ABCD8__INCLUDED_)
#define AFX_ORACLEPASSWORDADMINISTRATORDLG_H__EEF74BD6_140D_487D_8498_39437A1ABCD8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// COraclePasswordAdministratorDlg dialog
#include "UpdateDatabaseDlg.h"

class COraclePasswordAdministratorDlg : public CDialog
{
// Construction
public:
	COraclePasswordAdministratorDlg(CUpdateDatabaseDlg* updateDlg, CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(COraclePasswordAdministratorDlg)
	enum { IDD = IDD_ORACLEPASSWORDADMINISTRATOR_DIALOG };
	CStatic	m_servernameStatic;
	CEdit	m_servernameEdit;
	CButton	m_cancelBtn;
	CButton	m_clearBtn;
	CStatic	m_passwordStatic;
	CStatic	m_usernameStatic;
	CButton	m_connectGroupBox;
	CStatic	m_logoPicture;
	CButton	m_okBtn;
	CEdit	m_usernameEdit;
	CEdit	m_passwordEdit;
	CString	m_passwordValue;
	CString	m_usernameValue;
	CString	m_servernameValue;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COraclePasswordAdministratorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(COraclePasswordAdministratorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnClear();
	virtual void OnOK();
	afx_msg void OnChangeEditUsername();
	afx_msg void OnChangeEditPassword();
	afx_msg void OnClose();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnChangeEditServerName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CUpdateDatabaseDlg* m_updateDlg;

	enum EDITCONTROL
	{
		SERVERNAME = 0,
		USERNAME,
		PASSWORD
	};

	void initConnectionInfo();
	BOOL EnableOkButton(EDITCONTROL editControl);

	void fixedPosition();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ORACLEPASSWORDADMINISTRATORDLG_H__EEF74BD6_140D_487D_8498_39437A1ABCD8__INCLUDED_)
