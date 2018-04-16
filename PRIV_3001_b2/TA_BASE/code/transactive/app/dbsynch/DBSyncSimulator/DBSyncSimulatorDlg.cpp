// DBSyncSimulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DBSyncSimulator.h"
#include "DBSyncSimulatorDlg.h"
#include "DBSyncSimCommon.h"

#include <signal.h>
#include "CommonData.h"
#include "CommonDef.h"
#include "UtilityFun.h"	 
#include "GUIUpdateHandler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



using namespace TA_Base_Core;
using namespace TA_DB_Sync;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDBSyncSimulatorDlg dialog




CDBSyncSimulatorDlg::CDBSyncSimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDBSyncSimulatorDlg::IDD, pParent)
	, m_strPort(_T(""))
	, m_nClients(0)
	, m_nStartMsgSeqID(0)
	, m_nEndMsgSeqID(0)
	, m_noOfInsert(0)
	, m_noOfGroup(0)
	, m_noOfPublic(0)
	, m_noOfMMS(0)
	, m_lInsertInterval(0)
	, m_lGroupInterval(0)
	, m_lPublicInterval(0)
	, m_lMMSInterval(0)
	, m_nPacketSize(0)
	, m_lTestDuration(0)
	, m_pDbSyncMonitor(0)
	, m_bIsPaused(false)
	, m_ulRunningTime(0)

{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDBSyncSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DBSYNC, m_dbSyncListCtrl);
	DDX_Text(pDX, IDC_EDT_PORT, m_strPort);
	DDV_MaxChars(pDX, m_strPort, 4);
	DDX_Text(pDX, IDC_EDT_CLIENT_SIZE, m_nClients);
	DDV_MinMaxInt(pDX, m_nClients, 1, 500);
	DDX_Text(pDX, IDC_EDT_START_MSG_SEQ_ID, m_nStartMsgSeqID);
	DDX_Text(pDX, IDC_EDT_END_MSG_SEQ_ID, m_nEndMsgSeqID);
	DDX_Text(pDX, IDC_EDT_NUM_INSERT, m_noOfInsert);
	DDX_Text(pDX, IDC_EDT_NUM_GROUP, m_noOfGroup);
	DDX_Text(pDX, IDC_EDT_NUM_PUBLIC, m_noOfPublic);
	DDX_Text(pDX, IDC_EDT_NUM_MMS, m_noOfMMS);
	DDX_Text(pDX, IDC_EDT_INTERVAL_INSERT, m_lInsertInterval);
	DDX_Text(pDX, IDC_EDT_INTERVAL_GROUP, m_lGroupInterval);
	DDX_Text(pDX, IDC_EDT_INTERVAL_PUBLIC, m_lPublicInterval);
	DDX_Text(pDX, IDC_EDT_INTERVAL_MMS, m_lMMSInterval);
	DDX_Text(pDX, IDC_EDT_PACKAGE_SIZE, m_nPacketSize);
	DDX_Text(pDX, IDC_EDT_TEST_DURATION, m_lTestDuration);
	DDX_Text(pDX, IDC_CBO_QUEUE_NAME, m_strQueueName);
}

BEGIN_MESSAGE_MAP(CDBSyncSimulatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_START, &CDBSyncSimulatorDlg::OnBnClickedBtnStart)
	ON_MESSAGE(WMU_SETUP_CLIENT, OnSetupClients)
	ON_MESSAGE(WMU_CLIENT_STATUS_UPDATE, OnStatusUpdate)
	ON_MESSAGE(WMU_CLIENT_MSGID_UPDATE, OnMgsIDUpdate)
	ON_MESSAGE(WMU_CLIENT_UPDATE, OnClientUpdate)
	ON_BN_CLICKED(IDC_BTN_PRINT_STATUS, &CDBSyncSimulatorDlg::OnBnClickedBtnPrintStatus)
	ON_BN_CLICKED(IDC_BTN_PAUSE, &CDBSyncSimulatorDlg::OnBnClickedBtnPause)
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDBSyncSimulatorDlg message handlers

BOOL CDBSyncSimulatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	Initialize();

	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL,path,sizeof(path));
	
	SetWindowText(path);
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDBSyncSimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDBSyncSimulatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDBSyncSimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDBSyncSimulatorDlg::OnBnClickedBtnStart()
{
	// TODO: Add your control notification handler code here
	if (NULL != m_pManager && m_pManager->getCurrentState() != TA_Base_Core::Thread::THREAD_STATE_RUNNING)
	{
		m_pManager->start();
		m_pDbSyncMonitor->start();

		SetTimer(TEST_RUNNING_TIMER, TIME_INTERVAL, NULL);
	}

}

void CDBSyncSimulatorDlg::Initialize()
{
	// SetUp ListView
	m_dbSyncListCtrl.Initialize();
	CGUIUpdateHandler::getInstance().setTargetWnd(this->GetSafeHwnd());

	std::string strCfgFile;

	m_pManager = new CDbSynTestClientManager();
	strCfgFile = defconfig_Cfg_FileName;
	m_pManager->setConFigFile(strCfgFile);

	m_pDbSyncMonitor = new CDbSyncTestMonitor(m_pManager);
}

std::string CDBSyncSimulatorDlg::ulong64ToString(TA_AQ_Lib::ulong64 ulData)
{
	std::string strRetVal;
	std::stringstream mystream;

	mystream << ulData;
	strRetVal = mystream.str();

	return strRetVal;
}

LRESULT CDBSyncSimulatorDlg::OnSetupClients(WPARAM wParam, LPARAM lParam)
{
	T_CLIENT_MAP* pClients = reinterpret_cast<T_CLIENT_MAP* > (wParam);

	if (NULL != pClients)
	{
		m_dbSyncListCtrl.SetClientList(*pClients);

		//Display Config Information
		SetDlgItemText(IDC_EDT_PORT, g_CfgSimInfo.m_strCfg_ServerPort.c_str());
		SetDlgItemText(IDC_IP_SERVER, g_CfgSimInfo.m_strCfg_ServerIP.c_str());
		SetDlgItemText(IDC_CBO_QUEUE_NAME, g_CfgSimInfo.m_strCfg_QueueName.c_str());

		SetDlgItemText(IDC_EDT_CLIENT_SIZE, ulong64ToString((*pClients).size()).c_str() );
		SetDlgItemText(IDC_EDT_START_MSG_SEQ_ID, g_CfgSimInfo.m_strCfg_StartSeqNum.c_str());
		SetDlgItemText(IDC_EDT_END_MSG_SEQ_ID, g_CfgSimInfo.m_strCfg_EndSeqNum.c_str());

		SetDlgItemText(IDC_EDT_NUM_INSERT, g_CfgSimInfo.m_strCfg_NumberofInsert.c_str());
		SetDlgItemText(IDC_EDT_NUM_PUBLIC, g_CfgSimInfo.m_strCfg_NumberofPublic.c_str());
		SetDlgItemText(IDC_EDT_NUM_GROUP, g_CfgSimInfo.m_strCfg_NumberofGroup.c_str());
		SetDlgItemText(IDC_EDT_NUM_MMS, g_CfgSimInfo.m_strCfg_NumberofMMS.c_str());

		SetDlgItemText(IDC_EDT_INTERVAL_INSERT, g_CfgSimInfo.m_strCfg_IntervalTimeofInsert.c_str());
		SetDlgItemText(IDC_EDT_INTERVAL_PUBLIC, g_CfgSimInfo.m_strCfg_IntervalTimeofPublic.c_str());
		SetDlgItemText(IDC_EDT_INTERVAL_GROUP, g_CfgSimInfo.m_strCfg_IntervalTimeofGroup.c_str());
		SetDlgItemText(IDC_EDT_INTERVAL_MMS, g_CfgSimInfo.m_strCfg_IntervalTimeofMMS.c_str());

		SetDlgItemText(IDC_EDT_PACKAGE_SIZE, g_CfgSimInfo.m_strCfg_PackageSize.c_str());
		SetDlgItemText(IDC_EDT_TEST_DURATION, g_CfgSimInfo.m_strCfg_TestDuration.c_str());
	}	

	return 0;
}

LRESULT CDBSyncSimulatorDlg::OnStatusUpdate(WPARAM wParam, LPARAM lParam)
{
	ClientUpdate* pUpdate = reinterpret_cast<ClientUpdate* > (wParam);

	if (NULL != pUpdate)
	{
		m_dbSyncListCtrl.ProccessStatusUpdate(pUpdate);
		
	}

	return 0;
}

LRESULT CDBSyncSimulatorDlg::OnMgsIDUpdate(WPARAM wParam, LPARAM lParam)
{
	ClientUpdate* pUpdate = reinterpret_cast<ClientUpdate* > (wParam);

	if (NULL != pUpdate)
	{
		m_dbSyncListCtrl.ProccessMgsIDUpdate(pUpdate);
	}

	return 0;
}


std::string CDBSyncSimulatorDlg::u64tostr(TA_AQ_Lib::ulong64 u64Val)
{
	std::string strRet;
	unsigned nDigit = 0;
	while (u64Val >= 10)
	{
		nDigit = u64Val - (u64Val / 10) * 10;
		nDigit |= 0x30;

		strRet.insert(0, 1, nDigit);
		u64Val = u64Val / 10;
	}
	nDigit = u64Val | 0x30;
	strRet.insert(0, 1, nDigit);

	return strRet;
}

LRESULT CDBSyncSimulatorDlg::OnClientUpdate(WPARAM wParam, LPARAM lParam)
{
	T_ClientInfo_Map* pUpdate = reinterpret_cast<T_ClientInfo_Map* > (wParam);

	if (NULL != pUpdate)
	{
		TA_AQ_Lib::ulong64 ulDeqTotal = 0;
		TA_AQ_Lib::ulong64 ulEnqTotal = 0;

		m_dbSyncListCtrl.ProcessClientUpdate(*pUpdate);

		m_dbSyncListCtrl.GetTotalDeqEnq(ulDeqTotal, ulEnqTotal);

// 		this->SetDlgItemText(IDC_EDT_DEQ_TOTAL, ulong64ToString(ulDeqTotal).c_str());
// 		this->SetDlgItemText(IDC_EDT_ENQ_TOTAL, ulong64ToString(ulEnqTotal).c_str());
		
		this->SetDlgItemText(IDC_EDT_DEQ_TOTAL, u64tostr(ulDeqTotal).c_str());
		this->SetDlgItemText(IDC_EDT_ENQ_TOTAL, u64tostr(ulEnqTotal).c_str());

	}

	return 0;
}

void CDBSyncSimulatorDlg::OnBnClickedBtnPrintStatus()
{
	// TODO: Add your control notification handler code here
	if (NULL != m_pManager)
	{
		m_pManager->printStatus();
	}
}

void CDBSyncSimulatorDlg::OnBnClickedBtnPause()
{
	// TODO: Add your control notification handler code here
	if (NULL != m_pManager)
	{
		if (m_bIsPaused)
		{
			m_pManager->continueWork();
			this->SetDlgItemText(IDC_BTN_PAUSE, "Pause");
			m_bIsPaused = false;
			KillTimer(TEST_RUNNING_TIMER);
		}
		else
		{
			m_pManager->pauseWork();
			this->SetDlgItemText(IDC_BTN_PAUSE, "Continue");
			m_bIsPaused = true;
			SetTimer(TEST_RUNNING_TIMER, TIME_INTERVAL, NULL);
		}
		
	}
}

void CDBSyncSimulatorDlg::OnDestroy()
{
	//::MessageBox(NULL, "begin DBSyncSimulator OnDestroy", "DBSyncSimulator", MB_OK);
	CDialog::OnDestroy();
	KillTimer(TEST_RUNNING_TIMER);

	if (m_pDbSyncMonitor != NULL)
	{
		m_pDbSyncMonitor->terminateAndWait();
		delete m_pDbSyncMonitor;
	}

	CGUIUpdateHandler::releaseInstance();

	if (NULL != m_pManager)
	{
		m_pManager->terminateAndWait();
		delete m_pManager;
	}
	//::MessageBox(NULL, "end DBSyncSimulator OnDestroy", "DBSyncSimulator", MB_OK);

	// TODO: Add your message handler code here
}

void CDBSyncSimulatorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case TEST_RUNNING_TIMER:
		{
			m_ulRunningTime++;
			time_t t_temp = m_ulRunningTime;
			tm* ptm = gmtime(&t_temp);

			char szTime[256];
			strftime (szTime,256,"%H:%M:%S", ptm);
			CString strTime = szTime;

			SetDlgItemText(IDC_EDT_RUNNING_TIME, strTime.GetBuffer(0));
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}
