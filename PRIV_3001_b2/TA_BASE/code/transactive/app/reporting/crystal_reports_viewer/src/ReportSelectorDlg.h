#include "stdafx.h"
#include   "resource.h"
// ReportSelectorDlg dialog

class ReportSelectorDlg : public CDialog
{
	DECLARE_DYNAMIC(ReportSelectorDlg)

public:
	ReportSelectorDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ReportSelectorDlg();
// Dialog Data
	enum { IDD = IDD_RPTTYPE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAts();
	afx_msg void OnBnClickedIscs();

};
