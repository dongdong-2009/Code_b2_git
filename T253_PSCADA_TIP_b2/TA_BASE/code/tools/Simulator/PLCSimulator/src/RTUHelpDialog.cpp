// RTUHelpDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PLCSimulator.h"
#include "RTUHelpDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTUHelpDialog dialog


CRTUHelpDialog::CRTUHelpDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRTUHelpDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRTUHelpDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRTUHelpDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRTUHelpDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRTUHelpDialog, CDialog)
	//{{AFX_MSG_MAP(CRTUHelpDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRTUHelpDialog message handlers

BOOL CRTUHelpDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CString str = "When you set data,";
	str += "\r\n";
	str += "if input HEX,";
	str += "\r\n";
	str += "need to use 0X prefix.";
	SetDlgItemText( IDC_EDIT_HELP, str);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
