// C:\Perforce\All_Projects\3001_TIP_NEW\TA_BASE\transactive\app\reporting\crystal_reports_viewer\src\DatabaseConnectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app\reporting\crystal_reports_viewer\src\DatabaseConnectionDlg.h"


// DatabaseConnectionDlg dialog

IMPLEMENT_DYNAMIC(DatabaseConnectionDlg, CDialog)

DatabaseConnectionDlg::DatabaseConnectionDlg(CWnd* pParent /*=NULL*/)
: CDialog(DatabaseConnectionDlg::IDD, pParent)
{

}

DatabaseConnectionDlg::~DatabaseConnectionDlg()
{
}

void DatabaseConnectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATABASEPASSWORD_EDIT, m_passwordEdit);
	DDX_Control(pDX, IDC_USERNAME_EDIT, m_usernameEdit);
}


BEGIN_MESSAGE_MAP(DatabaseConnectionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &DatabaseConnectionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &DatabaseConnectionDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// DatabaseConnectionDlg message handlers

void DatabaseConnectionDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_passwordEdit.GetWindowText(m_password);
	m_usernameEdit.GetWindowText(m_userName);
	OnOK();
}

void DatabaseConnectionDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
