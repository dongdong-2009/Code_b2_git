// DlgWholeText.cpp : implementation file
//

#include "stdafx.h"
#include "TCPSimulator.h"
#include "DlgWholeText.h"


// CDlgWholeText dialog

IMPLEMENT_DYNAMIC(CDlgWholeText, CDialog)

CDlgWholeText::CDlgWholeText(CWnd* pParent /*=NULL*/)
: CDialog(CDlgWholeText::IDD, pParent)
{

}

CDlgWholeText::~CDlgWholeText()
{
}

void CDlgWholeText::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgWholeText, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgWholeText::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgWholeText message handlers

BOOL CDlgWholeText::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetDlgItemText(IDC_ED_WHOLETEXT,m_WholeText);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWholeText::ShowWholeText(CString strText)
{
	m_WholeText = strText;
	//if(!IsWindowVisible())
	DoModal();
	//SetDlgItemText(IDC_ED_WHOLETEXT,strText);
}

void CDlgWholeText::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}
