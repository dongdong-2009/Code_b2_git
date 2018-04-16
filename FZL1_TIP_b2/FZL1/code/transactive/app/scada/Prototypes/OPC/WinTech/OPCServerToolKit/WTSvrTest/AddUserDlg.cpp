// AddUserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WTSvrTest.h"
#include "AddUserDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAddUserDlg dialog


CAddUserDlg::CAddUserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddUserDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddUserDlg)
	m_group = _T("");
	m_accessrights = 0;
	m_user = _T("");
	//}}AFX_DATA_INIT
}


void CAddUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddUserDlg)
	DDX_Text(pDX, IDC_GROUP, m_group);
	DDX_Text(pDX, IDC_RIGHTS, m_accessrights);
	DDX_Text(pDX, IDC_USER, m_user);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddUserDlg, CDialog)
	//{{AFX_MSG_MAP(CAddUserDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddUserDlg message handlers
