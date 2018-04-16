// src\SystemControlTypeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/BASModeManager/src/SystemControlTypeDlg.h"
#include "bus/generic_gui/src/HelpLauncher.h"

Namespace_TA_IRS_APP_Begin
// CSystemControlTypeDlg dialog
IMPLEMENT_DYNAMIC(CSystemControlTypeDlg, CDialog)
CSystemControlTypeDlg::CSystemControlTypeDlg(const std::string& subSystemName, CWnd* pParent /*=NULL*/)
	: CDialog(CSystemControlTypeDlg::IDD, pParent),
    m_strSubSystemName(subSystemName)
{
    
}

CSystemControlTypeDlg::~CSystemControlTypeDlg()
{
}

void CSystemControlTypeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SYSTEM_MODE_TYPE, m_systemControlType);
    DDX_Control(pDX, IDC_DEVICE, m_subSystemName);
    DDX_Control(pDX, IDC_ATTRIBUTE, m_attribute);
}


BEGIN_MESSAGE_MAP(CSystemControlTypeDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
    ON_BN_CLICKED(ID_HELP, OnBnClickedHelp)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CSystemControlTypeDlg message handlers
BOOL CSystemControlTypeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    std::map<unsigned long, std::string> mapSystemControlType = BASManagerUtility::getAllSystemControlModeValues();
    for (std::map<unsigned long, std::string>::iterator it = mapSystemControlType.begin(); it != mapSystemControlType.end(); it++)
    {
        int index = m_systemControlType.InsertString(m_systemControlType.GetCount(), _T(it->second.c_str()));
        m_systemControlType.SetItemData(index, it->first);
    }
    m_subSystemName.SetWindowText(m_strSubSystemName.c_str());
    m_subSystemName.SetReadOnly();
    m_attribute.SetWindowText(_T("系统控制方式设置"));
    m_attribute.SetReadOnly();
	return TRUE;
}

void CSystemControlTypeDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

void CSystemControlTypeDlg::OnBnClickedHelp()
{
    TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
}

void CSystemControlTypeDlg::OnBnClickedOk()
{
    setSystemControlValue(m_systemControlType.GetItemData(m_systemControlType.GetCurSel()));

    OnOK();
}

void CSystemControlTypeDlg::setSystemControlValue(unsigned long value)
{
    m_systemControlValue = value;
}

unsigned long CSystemControlTypeDlg::getSystemControlValue()
{
    return m_systemControlValue;
}

Namespace_TA_IRS_APP_End
