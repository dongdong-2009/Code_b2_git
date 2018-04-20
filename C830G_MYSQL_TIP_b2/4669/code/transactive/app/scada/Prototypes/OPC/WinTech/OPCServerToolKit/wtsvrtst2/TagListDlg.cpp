// TagListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WtSvrTst2.h"
#include "TagListDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTagListDlg dialog


CTagListDlg::CTagListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTagListDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTagListDlg)
	m_booltype = FALSE;
	m_floattype = FALSE;
	m_inttype = FALSE;
	m_strtype = FALSE;
	m_updatefrequency = 0;
	m_dohashing = FALSE;
	m_dolistupdates = FALSE;
	//}}AFX_DATA_INIT
}


void CTagListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTagListDlg)
	DDX_Check(pDX, IDC_BOOLTYPE, m_booltype);
	DDX_Check(pDX, IDC_FLOATTYPE, m_floattype);
	DDX_Check(pDX, IDC_INTTYPE, m_inttype);
	DDX_Check(pDX, IDC_STRTYPE, m_strtype);
	DDX_Text(pDX, IDC_UPDATE, m_updatefrequency);
	DDV_MinMaxInt(pDX, m_updatefrequency, 1, 30);
	DDX_Check(pDX, IDC_HASH, m_dohashing);
	DDX_Check(pDX, IDC_LISTUPDATES, m_dolistupdates);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTagListDlg, CDialog)
	//{{AFX_MSG_MAP(CTagListDlg)
	ON_BN_CLICKED(IDC_RANDOM, OnRandom)
	ON_BN_CLICKED(IDC_INCREMENTAL, OnIncremental)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTagListDlg message handlers

BOOL CTagListDlg::OnInitDialog() 
{
	CButton	*pRadio;
	CComboBox	*pCombo;

	CDialog::OnInitDialog();
	
	if (m_randomupdates)
		pRadio = (CButton *)GetDlgItem (IDC_RANDOM);
	else
		pRadio = (CButton *)GetDlgItem (IDC_INCREMENTAL);
	pRadio->SetCheck(1);

	pCombo = (CComboBox *)GetDlgItem (IDC_COUNT);
	switch (m_tagcount)
		{
		case 20000:	 pCombo->SetCurSel(0); break;
		case 10000:	 pCombo->SetCurSel(1); break;
		case 5000:	 pCombo->SetCurSel(2); break;
		case 2000:	 pCombo->SetCurSel(3); break;
		default:	 pCombo->SetCurSel(4); break;
		}

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTagListDlg::OnRandom() 
{
	m_randomupdates = TRUE;	
}

void CTagListDlg::OnIncremental() 
{
	m_randomupdates = FALSE;	
}

void CTagListDlg::OnOK() 
{
	CComboBox	*pCombo;
	int			i;

	pCombo = (CComboBox *)GetDlgItem (IDC_COUNT);
	i = pCombo->GetCurSel();
	switch (i)
		{
		case 0: m_tagcount = 20000;  break;
		case 1: m_tagcount = 10000;  break;
		case 2: m_tagcount = 5000;  break;
		case 3: m_tagcount = 2000;  break;
		case 4: m_tagcount = 1000;  break;
		case 5: m_tagcount = 100;  break;
		case 6: m_tagcount = 10;  break;
		default: m_tagcount = 1000;  break;
		}

	CDialog::OnOK();
}
