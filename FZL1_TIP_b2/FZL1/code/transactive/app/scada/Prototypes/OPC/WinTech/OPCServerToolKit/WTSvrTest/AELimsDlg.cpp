// AELimsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WTSvrTest.h"
#include "AELimsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAELimsDlg dialog


CAELimsDlg::CAELimsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAELimsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAELimsDlg)
	m_hi_enable = FALSE;
	m_hihi_enable = FALSE;
	m_lo_enable = FALSE;
	m_lolo_enable = FALSE;
	m_hihi_level = 0.0f;
	m_hi_level = 0.0f;
	m_lo_level = 0.0f;
	m_lolo_level = 0.0f;
	m_hihi_severity = 0;
	m_hi_severity = 0;
	m_lolo_severity = 0;
	m_lo_severity = 0;
	//}}AFX_DATA_INIT
}


void CAELimsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAELimsDlg)
	DDX_Check(pDX, IDC_HICHK, m_hi_enable);
	DDX_Check(pDX, IDC_HIHICHK, m_hihi_enable);
	DDX_Check(pDX, IDC_LOCHK, m_lo_enable);
	DDX_Check(pDX, IDC_LOLOCHK, m_lolo_enable);
	DDX_Text(pDX, IDC_HIHILEVEL, m_hihi_level);
	DDX_Text(pDX, IDC_HILEVEL, m_hi_level);
	DDX_Text(pDX, IDC_LOLEVEL, m_lo_level);
	DDX_Text(pDX, IDC_LOLOLEVEL, m_lolo_level);
	DDX_Text(pDX, IDC_HIHISEV, m_hihi_severity);
	DDX_Text(pDX, IDC_HISEV, m_hi_severity);
	DDX_Text(pDX, IDC_LOLOSEV, m_lolo_severity);
	DDX_Text(pDX, IDC_LOSEV, m_lo_severity);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAELimsDlg, CDialog)
	//{{AFX_MSG_MAP(CAELimsDlg)
	ON_BN_CLICKED(IDC_LOCHK, OnLochk)
	ON_BN_CLICKED(IDC_LOLOCHK, OnLolochk)
	ON_BN_CLICKED(IDC_HICHK, OnHichk)
	ON_BN_CLICKED(IDC_HIHICHK, OnHihichk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAELimsDlg message handlers

BOOL CAELimsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	pLimits[0] = (CEdit *)GetDlgItem(IDC_LOLOLEVEL);
	pLimits[1] = (CEdit *)GetDlgItem(IDC_LOLEVEL);
	pLimits[2] = (CEdit *)GetDlgItem(IDC_HILEVEL);
	pLimits[3] = (CEdit *)GetDlgItem(IDC_HIHILEVEL);

	pSeverities[0] = (CEdit *)GetDlgItem(IDC_LOLOSEV);
	pSeverities[1] = (CEdit *)GetDlgItem(IDC_LOSEV);
	pSeverities[2] = (CEdit *)GetDlgItem(IDC_HISEV);
	pSeverities[3] = (CEdit *)GetDlgItem(IDC_HIHISEV);

	pChk[0] = (CButton *)GetDlgItem(IDC_LOLOCHK);
	pChk[1] = (CButton *)GetDlgItem(IDC_LOCHK);
	pChk[2] = (CButton *)GetDlgItem(IDC_HICHK);
	pChk[3] = (CButton *)GetDlgItem(IDC_HIHICHK);

	pLimits[0]->EnableWindow(m_lolo_enable);
	pSeverities[0]->EnableWindow(m_lolo_enable);

	pLimits[1]->EnableWindow(m_lo_enable);
	pSeverities[1]->EnableWindow(m_lo_enable);

	pLimits[2]->EnableWindow(m_hi_enable);
	pSeverities[2]->EnableWindow(m_hi_enable);

	pLimits[3]->EnableWindow(m_hihi_enable);
	pSeverities[3]->EnableWindow(m_hihi_enable);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAELimsDlg::OnLolochk() 
{
	if (pChk[0]->GetCheck() == 1)
		m_lolo_enable = TRUE;
	else
		m_lolo_enable = FALSE;

	pLimits[0]->EnableWindow(m_lolo_enable);
	pSeverities[0]->EnableWindow(m_lolo_enable);

}

void CAELimsDlg::OnLochk() 
{
	if (pChk[1]->GetCheck() == 1)
		m_lo_enable = TRUE;
	else
		m_lo_enable = FALSE;

	pLimits[1]->EnableWindow(m_lo_enable);
	pSeverities[1]->EnableWindow(m_lo_enable);
}

void CAELimsDlg::OnHichk() 
{
	if (pChk[2]->GetCheck() == 1)
		m_hi_enable = TRUE;
	else
		m_hi_enable = FALSE;

	pLimits[2]->EnableWindow(m_hi_enable);
	pSeverities[2]->EnableWindow(m_hi_enable);
}

void CAELimsDlg::OnHihichk() 
{
	if (pChk[3]->GetCheck() == 1)
		m_hihi_enable = TRUE;
	else
		m_hihi_enable = FALSE;

	pLimits[3]->EnableWindow(m_hihi_enable);
	pSeverities[3]->EnableWindow(m_hihi_enable);
}
