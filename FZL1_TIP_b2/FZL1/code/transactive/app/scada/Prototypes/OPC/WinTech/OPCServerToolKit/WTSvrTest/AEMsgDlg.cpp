// AEMsgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WTSvrTest.h"
#include "AEMsgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAEMsgDlg dialog


CAEMsgDlg::CAEMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAEMsgDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAEMsgDlg)
	m_msg = _T("");
	m_severity = 0;
	//}}AFX_DATA_INIT
}


void CAEMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAEMsgDlg)
	DDX_Text(pDX, IDC_MESSAGE, m_msg);
	DDX_Text(pDX, IDC_SEVERITY, m_severity);
	DDV_MinMaxInt(pDX, m_severity, 1, 1000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAEMsgDlg, CDialog)
	//{{AFX_MSG_MAP(CAEMsgDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAEMsgDlg message handlers
