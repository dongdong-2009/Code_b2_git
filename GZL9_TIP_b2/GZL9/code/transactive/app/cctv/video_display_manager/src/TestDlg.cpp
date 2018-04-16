// TestDlg.cpp : implementation file
//

#include "TestDlg.h"
#include "DisplayItemManager.h"
#include "Camera.h"
#include "Monitor.h"
#include "Sequence.h"
#include "SwitchAgentCommunicator.h"

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
	DDX_Control(pDX, IDC_PARAM, m_runParam);
	DDX_Control(pDX, IDC_PARAM_VALUE, m_runParamValue);
}


BOOL CTestDlg::OnInitDialog()
{
	return CDialog::OnInitDialog();
}

void CTestDlg::setRunParamUser(TA_Base_Core::RunParamUser* runParamUser)
{
	m_paramUser = runParamUser;
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CTestDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTestDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CTestDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CTestDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CTestDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CTestDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CTestDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CTestDlg::OnBnClickedButton8)
END_MESSAGE_MAP()


// CTestDlg message handlers

void CTestDlg::OnBnClickedButton1()
{
	// TODO: Add your assign camera code
	Camera* c = DisplayItemManager::getInstance().getCamera("TEST_CAM_001");
	try
	{
		SwitchAgentCommunicator::getInstance().assignMonitor(DisplayItemManager::getInstance().getMonitor("OCC_WALL_MON_01"), c);
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Exception", "Exception when assign monitor");
	}
}

void CTestDlg::OnBnClickedButton2()
{
	try
	{
		SwitchAgentCommunicator::getInstance().clearMonitor(DisplayItemManager::getInstance().getMonitor("OCC_WALL_MON_01"));
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Exception", "Exception when clear monitor");
	}
}

void CTestDlg::OnBnClickedButton3()
{
	// TODO: Add your assign seq code
	Sequence* s = DisplayItemManager::getInstance().getSequence("TEST_SEQ_001");
	try
	{
		SwitchAgentCommunicator::getInstance().assignMonitor(DisplayItemManager::getInstance().getMonitor("OCC_WALL_MON_01"), s);
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Exception", "Exception when assign seq");
	}
}

void CTestDlg::OnBnClickedButton4()
{
	Sequence* s = DisplayItemManager::getInstance().getSequence("TEST_SEQ_001");
	Camera* c = DisplayItemManager::getInstance().getCamera("TEST_CAM_001");
	std::list<Camera*> lc;
	lc.push_back(c);
	// TODO: Add your edit seq code
	try
	{
		SwitchAgentCommunicator::getInstance().setSequenceConfig(s, 3, lc, true);
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Exception", "Exception when set seq");
	}
}

void CTestDlg::OnBnClickedButton5()
{
	// TODO: Add your assign camera to local code
}

void CTestDlg::OnBnClickedButton6()
{
	// TODO: Add your pan camera code
	Camera* c = DisplayItemManager::getInstance().getCamera("TEST_CAM_001");
	try
	{
	    SwitchAgentCommunicator::getInstance().panCamera ( c, 1, 1);
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Exception", "Exception when pan camera");
	}
}

void CTestDlg::OnBnClickedButton7()
{
	// TODO: Add your tilt camera code
	Camera* c = DisplayItemManager::getInstance().getCamera("TEST_CAM_001");
	try
	{
	    SwitchAgentCommunicator::getInstance().tiltCamera( c, 1, 1);
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Exception", "Exception when tilt camera");
	}
}

void CTestDlg::OnBnClickedButton8()
{
	// TODO: Add your zoom camera code
	Camera* c = DisplayItemManager::getInstance().getCamera("TEST_CAM_001");
	try
	{
	    SwitchAgentCommunicator::getInstance().zoomCamera( c, 1);
	}
	catch (...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Exception", "Exception when zoom camera");
	}
}

void CTestDlg::OnOK()
{
	CString runParam, runParamValue;
	m_runParam.GetWindowText(runParam);
	m_runParamValue.GetWindowText(runParamValue);
	m_paramUser->onRunParamChange(runParam.GetBuffer(), runParamValue.GetBuffer());
}