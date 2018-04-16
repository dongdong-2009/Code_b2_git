#include "stdafx.h"
#include   "resource.h"
// DatabaseConnectionDlg dialog

class DatabaseConnectionDlg : public CDialog
{
	DECLARE_DYNAMIC(DatabaseConnectionDlg)

public:
	DatabaseConnectionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~DatabaseConnectionDlg();

	// Dialog Data
	enum { IDD = IDD_DATABASE_DIALOG };
	CEdit               m_passwordEdit;
	CEdit               m_usernameEdit;
	CString				m_password;
	CString				m_userName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();


};
