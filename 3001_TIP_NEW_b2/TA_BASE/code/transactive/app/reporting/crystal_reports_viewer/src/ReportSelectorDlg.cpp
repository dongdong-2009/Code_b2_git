// C:\Perforce\All_Projects\3001_TIP_NEW\TA_BASE\transactive\app\reporting\crystal_reports_viewer\src\ReportSelectorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app\reporting\crystal_reports_viewer\src\ReportSelectorDlg.h"


// ReportSelectorDlg dialog

IMPLEMENT_DYNAMIC(ReportSelectorDlg, CDialog)

ReportSelectorDlg::ReportSelectorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ReportSelectorDlg::IDD, pParent)
{
}

ReportSelectorDlg::~ReportSelectorDlg()
{
}

void ReportSelectorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ReportSelectorDlg, CDialog)
	ON_BN_CLICKED(IDOK, &ReportSelectorDlg::OnBnClickedAts)
	ON_BN_CLICKED(IDCANCEL, &ReportSelectorDlg::OnBnClickedIscs)
END_MESSAGE_MAP()


// ReportSelectorDlg message handlers

void ReportSelectorDlg::OnBnClickedAts()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void ReportSelectorDlg::OnBnClickedIscs()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
