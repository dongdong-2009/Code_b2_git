// DlgDataValidator.cpp : implementation file
//

#include "stdafx.h"
#include "PDS ClientSim.h"
#include "DlgDataValidator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgDataValidator dialog


CDlgDataValidator::CDlgDataValidator(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgDataValidator::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgDataValidator)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgDataValidator::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgDataValidator)
	DDX_Control(pDX, IDC_EDT_DATA, m_edtDataFrame);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgDataValidator, CDialog)
	//{{AFX_MSG_MAP(CDlgDataValidator)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgDataValidator message handlers


void CDlgDataValidator::setDataFrame(CString strData)
{
	m_stringData = strData;
	
}

BOOL CDlgDataValidator::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_edtDataFrame.SetWindowText(m_stringData.GetBuffer(0));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
