// AutoAlarmsOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "alarmgenerator.h"
#include "AutoAlarmsOptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AutoAlarmsOptionDlg dialog


AutoAlarmsOptionDlg::AutoAlarmsOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AutoAlarmsOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(AutoAlarmsOptionDlg)
	m_nCount = 0;
	m_nInterval = 0;
	m_nThreadCount = 0;
	//}}AFX_DATA_INIT
}


void AutoAlarmsOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AutoAlarmsOptionDlg)
	DDX_Text(pDX, IDC_EDIT_COUNT, m_nCount);
	DDV_MinMaxLong(pDX, m_nCount, 1, 999999999);
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_nInterval);
	DDV_MinMaxLong(pDX, m_nInterval, 0, 9999999);
	DDX_Text(pDX, IDC_EDIT_THREAD_COUNT, m_nThreadCount);
	DDV_MinMaxLong(pDX, m_nThreadCount, 1, 256);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AutoAlarmsOptionDlg, CDialog)
	//{{AFX_MSG_MAP(AutoAlarmsOptionDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AutoAlarmsOptionDlg message handlers

void AutoAlarmsOptionDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData();
	
	CDialog::OnOK();
}

BOOL AutoAlarmsOptionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_nCount = 30000;
	m_nInterval = 1000;
	m_nThreadCount = 100;
	UpdateData(FALSE);
	GetDlgItem(IDC_EDIT_COUNT)->SetFocus();
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
