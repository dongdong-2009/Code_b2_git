#pragma once
#include "resource.h"
#include "app/bas/BASModeManager/src/CommonDef.h"
Namespace_TA_IRS_APP_Begin
// CSystemControlTypeDlg dialog
class CSystemControlTypeDlg : public CDialog
{
	DECLARE_DYNAMIC(CSystemControlTypeDlg)

public:
    CSystemControlTypeDlg(const std::string& subSystemName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSystemControlTypeDlg();

    unsigned long getSystemControlValue();

// Dialog Data
	enum { IDD = IDD_DIALOG_SYSTEM_MODE_TYPE };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedHelp();
    afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()
private:
    void setSystemControlValue(unsigned long value);

    CComboBox m_systemControlType;
    CEdit m_subSystemName;
    CEdit m_attribute;

    std::string m_strSubSystemName;
    unsigned long m_systemControlValue;
};
Namespace_TA_IRS_APP_End
