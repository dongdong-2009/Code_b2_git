// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDS ClientSim.h"
#include "ConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog


CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigDlg)
	m_strIpAddress = _T("");
	m_strServicePort = _T("");
	//}}AFX_DATA_INIT
	m_bCheck = false;
	m_paramReader = new TA_IRS_App::CParamReader;
}

CConfigDlg::~CConfigDlg()
{
	if (m_paramReader)
		delete m_paramReader;
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDlg)
	DDX_Control(pDX, IDC_CHK_CHKSUMOVERIDE, m_chkChkSumOverRide);
	DDX_Text(pDX, IDC_EDT_IP, m_strIpAddress);
	DDX_Text(pDX, IDC_EDT_PORT, m_strServicePort);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
	ON_BN_CLICKED(IDC_BTN_UPDATE, OnBtnUpdate)
	ON_BN_CLICKED(IDC_CHK_CHKSUMOVERIDE, OnChkChksumoveride)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg message handlers

void CConfigDlg::attachListener (IConfigServerObserver * ptrListener)
{
	m_ptrListener = ptrListener;
}

void CConfigDlg::OnBtnUpdate() 
{
	// TODO: Add your control notification handler code here
	
	UpdateData ();
	
	if (m_strIpAddress.IsEmpty() || m_strServicePort.IsEmpty())
	{
		::MessageBox (NULL,"Need Valid IP or Port Number", "Error",MB_OK);
		return;
	}
	
	m_ptrListener->notifyServerSettingChange(m_strIpAddress.GetBuffer(0),m_strServicePort.GetBuffer(0));


	m_paramReader->setValueToSection(SECTIONNAME_IP,m_strIpAddress.GetBuffer(0));
	m_paramReader->setValueToSection(SECTIONNAME_PORT,m_strServicePort.GetBuffer(0));
	
}

void CConfigDlg::OnChkChksumoveride() 
{
	// TODO: Add your control notification handler code here
	bool bCheckVal = false;
	int nCheckRes = m_chkChkSumOverRide.GetCheck();
	
	switch (nCheckRes)
	{
		case BST_UNCHECKED:
		case BST_INDETERMINATE:
			bCheckVal = false;
			break;
		case BST_CHECKED:
			bCheckVal = true;
			break;
	}
	m_bCheck = bCheckVal;
	m_ptrListener->notifyCheckSumOverRide(bCheckVal);
}

BOOL CConfigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	switch (m_bCheck)
	{
	case FALSE:
		m_chkChkSumOverRide.SetCheck(BST_UNCHECKED);
		break;
	case TRUE:
		m_chkChkSumOverRide.SetCheck(BST_CHECKED);
		break;
	}
	
	CString strIP = m_paramReader->getValueFromSection(SECTIONNAME_IP).c_str();
	CString strPort = m_paramReader->getValueFromSection(SECTIONNAME_PORT).c_str();
	
	m_strIpAddress = strIP;
	m_strServicePort = strPort;
	
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	UpdateData ();
	
	if (!m_strIpAddress.IsEmpty() && ! m_strServicePort.IsEmpty())
	{
		m_ptrListener->notifyServerSettingChange(m_strIpAddress.GetBuffer(0),m_strServicePort.GetBuffer(0));
	}
	
	CDialog::OnOK();
}
