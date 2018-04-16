#pragma once

#include <string>

// TextEditDialog dialog

class AFX_EXT_CLASS TextEditDialog : public CDialog
{
	DECLARE_DYNAMIC(TextEditDialog)

public:
	TextEditDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~TextEditDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
    CEdit m_edtContent;

// Implementation
protected:
    // Generated message map functions
    afx_msg virtual BOOL OnInitDialog();
    afx_msg virtual void OnOK();

	DECLARE_MESSAGE_MAP()

public:
    std::string Title;
    std::string Content;
    unsigned int LimitTextLength;
};
