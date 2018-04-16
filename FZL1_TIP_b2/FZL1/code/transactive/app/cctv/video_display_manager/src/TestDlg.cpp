// TestDlg.cpp : implementation file
//

#include "TestDlg.h"
#include "DisplayItemManager.h"
#include "Camera.h"
#include "Monitor.h"
#include "Sequence.h"
#include "SDKCommunicator.h"
#include "core/utilities/src/RunParams.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/message/types/VisualAlarms_MessageTypes.h"

// CTestDlg dialog
using namespace TA_IRS_App;

IMPLEMENT_DYNAMIC(CTestDlg, CDialog)
CTestDlg::CTestDlg(CWnd* pParent /*= NULL*/):CDialog(CTestDlg::IDD, pParent)
{

}

CTestDlg::~CTestDlg()
{
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PARAM_NAME, m_runParam);
	DDX_Control(pDX, IDC_PARAM_VALUE, m_runParamValue);
	DDX_Control(pDX, IDC_PARAM_AND_VALUE, m_runPramAndValue);
	DDX_Control(pDX, IDC_EDIT_ALARM_ENTITYKEY_ACK, m_alarmEntityKeyAck);
	DDX_Control(pDX, IDC_EDIT_ALARM_ENTITYKEY_SUBMIT, m_alarmEntitykeySubmit);
}


BOOL CTestDlg::OnInitDialog()
{
	return CDialog::OnInitDialog();
}

// CTestDlg message handlers

void CTestDlg::OnCancel()
{
}

void CTestDlg::OnOK()
{
	CString runParam, runParamValue, runParamAndValue;
	m_runParam.GetWindowText(runParam);
	m_runParamValue.GetWindowText(runParamValue);
	m_runPramAndValue.GetWindowText(runParamAndValue);
	if(runParamAndValue.IsEmpty())
	{
		TA_Base_Core::RunParams::getInstance().set(LPCSTR(runParam), LPCSTR(runParamValue));
	}
	else
	{
		std::string s = LPCSTR(runParamAndValue);
		std::string::size_type i = s.find('=');
		if(i == std::string::npos)
		{
			return;
		}
		std::string n = s.substr(0, i);
		std::string v = s.substr(i+1);
		TA_Base_Core::RunParams::getInstance().set(n.c_str(), v.c_str());
	}
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
ON_BN_CLICKED(IDC_BUTTON_ACK_ALARM, &CTestDlg::OnBnClickedButtonAckAlarm)
ON_BN_CLICKED(IDC_BUTTON_SUBMIT_ALARM, &CTestDlg::OnBnClickedButtonSubmitAlarm)
END_MESSAGE_MAP()

void CTestDlg::OnBnClickedButtonSubmitAlarm()
{
	// TODO: Add your control notification handler code here
	CString t;
	m_alarmEntitykeySubmit.GetWindowText(t);
	unsigned long key;
	std::stringstream ss;
	ss << LPCTSTR(t);
	ss >> key;
	try
	{
		TA_Base_Core::IEntityData* alarmEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(key);
	    TA_Base_Core::DescriptionParameters dp;
	    TA_Base_Core::NameValuePair pair1("Event", "A test alarm");
	    dp.push_back(&pair1);
		TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarm(TA_Base_Core::VisualAlarms::CctvTriggeringAlarm,
			    alarmEntity->getKey(),
				alarmEntity->getTypeKey(),
	            dp,
				alarmEntity->getName(),
				alarmEntity->getLocation(),
				alarmEntity->getSubsystem());
	}
	catch (...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error occur when rising alarm");
	}
}

void CTestDlg::OnBnClickedButtonAckAlarm()
{
	// TODO: Add your control notification handler code here
	CString t;
	m_alarmEntityKeyAck.GetWindowText(t);
	unsigned long key;
	std::stringstream ss;
	ss << LPCTSTR(t);
	ss >> key;
	TA_IRS_App::SDKCommunicator::getInstance().acknowledgeAlarm(key);
}
