#pragma once


// CDlgWholeText dialog

class CDlgWholeText : public CDialog
{
	DECLARE_DYNAMIC(CDlgWholeText)

public:
	CDlgWholeText(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWholeText();
	void ShowWholeText(CString strText);
// Dialog Data
	enum { IDD = IDD_DLG_INFO_DETAIL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CString m_WholeText;
	afx_msg void OnBnClickedOk();
};
