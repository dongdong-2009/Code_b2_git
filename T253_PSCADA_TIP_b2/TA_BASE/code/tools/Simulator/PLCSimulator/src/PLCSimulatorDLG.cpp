// PLCSimulatorDlg.cpp
//

#include "stdafx.h"
#include "PLCSimulator.h"
#include "PLCSimulatorDlg.h"
#include "Run.h"
#include "RTUSimulator.h"
#include "RTUAboutDlg.h"
#include "RTUTaskManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CPLCSimulatorDlg::CPLCSimulatorDlg( CWnd* pParent ) : 
CDialog(CPLCSimulatorDlg::IDD, pParent),
m_esetType( BYWORD )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPLCSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_DEVICE, m_devices );
}

BEGIN_MESSAGE_MAP(CPLCSimulatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_MENUSELECT()
	ON_WM_CLOSE()
	ON_WM_COPYDATA()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_lOADCONFIGFILE, OnBnClickedLoad)
	ON_COMMAND(ID_FILE_lOADSTRESSCFG, OnBnClickedLoadStressCfg)
	ON_COMMAND(ID_TOOLBAR_STRESSON, OnBnClickedStressOn)
	ON_COMMAND(ID_TOOLBAR_STRESSOFF, OnBnClickedStressOff)
	ON_COMMAND(ID_TOOLBAR_RTUON, OnBnClickedOn)
	ON_COMMAND(ID_TOOLBAR_RTUOFF, OnBnClickedOff)
	ON_COMMAND(ID_TOOLBAR_RTURELOAD, OnBnClickedReLoad)
	ON_COMMAND(ID_TOOLBAR_HELP, OnBnClickedDisplayHelp)
	ON_COMMAND(ID_TOOLBAR_ABOUT, OnBnClickedDisplayAbout)
	ON_LBN_SELCHANGE(IDC_DEVICE, OnLbnSelchangeDevice)
	ON_CBN_SELCHANGE(IDC_COMBO_PROTOCAL, OnSelchangeComboProtocal)
	ON_BN_CLICKED(IDC_SET, OnBnClickedSet)
	ON_BN_CLICKED(IDC_BYBIT, OnBnClickedBybit)
	ON_BN_CLICKED(IDC_BYBYTE, OnBnClickedBybyte)
	ON_BN_CLICKED(IDC_BYWORD, OnBnClickedByword)
	ON_BN_CLICKED(IDC_BYDWORD, OnBnClickedBydword)
	ON_BN_CLICKED(IDC_RELOADSAVEDATA, OnBnClickedReloadSaveData)
	ON_BN_CLICKED(IDC_LOGTOFILE, OnBnClickedLogtofile)
	ON_BN_CLICKED(IDC_PRIMARY, OnBnClickedPrimary)
	ON_BN_CLICKED(IDC_STANDBY, OnBnClickedStandby)
	ON_BN_CLICKED(IDC_SWITCH, OnBnClickedSwitch)
	ON_BN_CLICKED(IDC_GET, OnBnClickedGet)
	ON_BN_CLICKED(IDC_CHK_CLIENT_ADDRESS, OnBnClickedCheckAddress)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

void CPLCSimulatorDlg::OnClose()
{
	if( IDYES == AfxMessageBox("Are you sure quit the program ?", MB_YESNO) )
	{
		CloseMainWindow();
	}
}

void CPLCSimulatorDlg::CloseMainWindow()
{
	RTUSimulator::getInstance().SaveData();

	OnBnClickedOff();

	ACE::fini();

	CWinThread* pThread = AfxGetThread();

	if (pThread->m_pMainWnd != NULL)
	{
		pThread->m_pMainWnd->DestroyWindow();
	}

	pThread->ExitInstance();
}

BOOL CPLCSimulatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	RTUSimulator::getInstance().registerForUpdate(this);

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

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	((CButton*)GetDlgItem(IDC_BYWORD))->SetCheck(1);

	SetDlgItemText(IDC_STRESSCFG, RTUSimulator::getInstance().GetStressCfgName().c_str());

 	CComboBox* pCombo;
 	pCombo = (CComboBox*)GetDlgItem( IDC_COMBO_PROTOCAL );
    if (C830MODBUS == RTUSimulator::getInstance().getProtocalType())
    {
        pCombo->SetCurSel(1);
    }
    else
    {
        pCombo->SetCurSel(0);
    }
 	
	int pause_output_dis = 0;
	if (RTUSimulator::getInstance().GetPauseOutputDisplay() == "1")
	{
		pause_output_dis = 1;
	}
	((CButton*)GetDlgItem( IDC_PAUSEOUTPUTDISPLAY ))->SetCheck(pause_output_dis);
	int pause_comm_dis = 0;
	if (RTUSimulator::getInstance().GetPauseCommunicationDisplay() == "1")
	{
		pause_comm_dis = 1;
	}
	((CButton*)GetDlgItem( IDC_PAUSECOMMDISPLAY ))->SetCheck(pause_comm_dis);

	// RTUSimulator::getInstance().SetProtocalType( STANDARDMODBUS );
	RTUSimulator::getInstance().SetLogWnd(GetDlgItem(IDC_LOGS));
	RTUSimulator::getInstance().SetOutputWnd(GetDlgItem(IDC_EDIT_OUTPUT));

	EnableDlg();

	ConfigFileUpdated();

	// start plc or stress, if parameter config.
	if (RTUSimulator::getInstance().GetPLCOn())
	{
		OnBnClickedOn();
	}
	if (RTUSimulator::getInstance().GetStressOn())
	{
		OnBnClickedStressOn();
	}

	return TRUE;
}

void CPLCSimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CRTUAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CPLCSimulatorDlg::OnPaint() 
{
	if ( IsIconic() )
	{
		CPaintDC dc(this);
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CPLCSimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPLCSimulatorDlg::OnBnClickedLoad()
{
	char szFilters[]=
		"Config File (*.txt)|*.txt|All Files (*.*)|*.*||";

	char workdir[512];
	memset(workdir, 0, 512);
	_getcwd(workdir, 512);

	CFileDialog fileDlg (TRUE, "txt", "Config.txt",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);

	if( fileDlg.DoModal ()==IDOK )
	{
		CString pathName = fileDlg.GetPathName();
		_chdir(workdir);
		ReSetConfigFile( std::string(pathName) );
	}
}

void CPLCSimulatorDlg::OnBnClickedLoadStressCfg()
{
	char szFilters[]=
		"Config File (*.txt)|*.txt|All Files (*.*)|*.*||";
	
	char workdir[512];
	memset(workdir, 0, 512);
	_getcwd(workdir, 512);
	
	CFileDialog fileDlg (TRUE, "txt", "StressConf.txt",
		OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
	
	if( fileDlg.DoModal ()==IDOK )
	{
		CString pathName = fileDlg.GetPathName();
		_chdir(workdir);
		ReSetStressCfgFile(std::string(pathName));
	}
}


void CPLCSimulatorDlg::ReSetStressCfgFile( std::string cfgFile )
{
	BOOL bStressOn = FALSE;
	CToolBarCtrl &toolBarCtrl= m_WndToolBar.GetToolBarCtrl();
	if (toolBarCtrl.IsButtonHidden(ID_TOOLBAR_STRESSON))	// now stress on.
	{
		bStressOn = TRUE;
		OnBnClickedStressOff();
	}
	
	RTUSimulator::getInstance().SetStressCfgName(cfgFile);
	SetDlgItemText(IDC_STRESSCFG, RTUSimulator::getInstance().GetStressCfgName().c_str());
	
	if (bStressOn)
	{
		OnBnClickedStressOn();
	}
}

void CPLCSimulatorDlg::ReSetConfigFile( std::string cfgFile )
{
	int Num = RTUSimulator::getInstance().GetPortsNum();

	int i = 0;
	for(i = 0; i < Num; ++i)
	{
		RTUTaskManager::getTaskItem(i).thr_mgr()->cancel_task (&RTUTaskManager::getTaskItem(i));
	}

	RTUSimulator::getInstance().InitConfigFile( cfgFile );

	ConfigFileUpdated();
	
	CToolBarCtrl &toolBarCtrl= m_WndToolBar.GetToolBarCtrl();
	if (toolBarCtrl.IsButtonHidden(ID_TOOLBAR_RTUON))	// now plc on.
	{
		OnBnClickedOn();
	}
}

void CPLCSimulatorDlg::OnLbnSelchangeDevice()
{
	CCheckListBox* clb = (CCheckListBox*)GetDlgItem(IDC_DEVICE);
	int index = clb->GetCurSel();
	CString str;
	clb->GetText(index, str);
	clb->SetCheck(index, (1 == clb->GetCheck(index))? 0 : 1);
	RTUSimulator::getInstance().SetDeviceStatus(std::string(str), (1 == clb->GetCheck(index))? true : false);
}

void CPLCSimulatorDlg::OnBnClickedStressOn()
{
	RTUSimulator::getInstance().BgnStress();

	CToolBarCtrl &toolBarCtrl= m_WndToolBar.GetToolBarCtrl();
	toolBarCtrl.HideButton( ID_TOOLBAR_STRESSOFF, false );
	toolBarCtrl.HideButton( ID_TOOLBAR_STRESSON, true );

	CMenu *menu = GetMenu();
	menu->EnableMenuItem(ID_TOOLBAR_STRESSON, MF_GRAYED);
	menu->EnableMenuItem(ID_TOOLBAR_STRESSOFF, MF_ENABLED);
}

void CPLCSimulatorDlg::OnBnClickedStressOff()
{
	RTUSimulator::getInstance().PauseStress();

	CToolBarCtrl &toolBarCtrl= m_WndToolBar.GetToolBarCtrl();
	toolBarCtrl.HideButton( ID_TOOLBAR_STRESSOFF, true );
	toolBarCtrl.HideButton( ID_TOOLBAR_STRESSON, false );

	CMenu *menu = GetMenu();
	menu->EnableMenuItem(ID_TOOLBAR_STRESSON, MF_ENABLED);
	menu->EnableMenuItem(ID_TOOLBAR_STRESSOFF, MF_GRAYED);
}

void CPLCSimulatorDlg::OnBnClickedSet()
{
	CString addr;
	GetDlgItem(IDC_ADDR)->GetWindowText(addr);

	CString val;
	GetDlgItem(IDC_VAL)->GetWindowText(val);	

	int address = 0;
	unsigned int value = 0;
	int bit = 0;
	CString bitstr;

	if(BYBIT == m_esetType)
	{
		if(addr.Find(".") != -1)
		{
			CString addrtmp = addr;
			addr = addrtmp.Left(addrtmp.Find("."));
			bitstr= addrtmp.Mid(addrtmp.Find(".") + 1); 
		}
		else
		{
			AfxMessageBox("Didn't give which bit you want to set!\nWill set bit 0", MB_OK);
		}
	}
	else
	{
		if(addr.Find(".") != -1)
		{
			AfxMessageBox("the .bit is useless unless you set data by bit", MB_OK);
			addr= addr.Left(addr.Find("."));
		}
	}

	if((addr.Find("0X") != -1) || (addr.Find("0x") != -1))
	{
		addr.Delete(0,2);
		address = _tcstoul(addr, 0, 16);
	}
	else
	{
		address = _tcstoul(addr, 0, 10);
	}

	if((bitstr.Find("0X") != -1) || (bitstr.Find("0x") != -1))
	{
		bitstr.Delete(0,2);
		bit = _tcstoul(bitstr, 0, 16);
	}
	else
	{
		bit = _tcstoul(bitstr, 0, 10);
	}

	if((val.Find("0X") != -1) || (val.Find("0x") != -1))
	{
		val.Delete(0,2);
		value = _tcstoul(val, 0, 16);
	}
	else
	{
		value = _tcstoul(val, 0, 10);
	}

	if(!RTUSimulator::getInstance().SetData(address, value, m_esetType, bit))
	{
		AfxMessageBox("Fail!", MB_OK);
	}
	else
	{
		switch( m_esetType )
		{
		case BYBIT:
			{
			char buffer[10] = {0};
			std::string text;
			text = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
			text += " Output: ";
			text += itoa( address, buffer, 10 );
			text += ".";
			memset( buffer, 0, 10 );
			text += itoa( bit, buffer, 10 );
			text += "=";
			text += "0x";
			memset( buffer, 0, 10 );
			text += itoa( value & 0x1, buffer, 16 );
			CString tmp;
			tmp.Insert(0, text.c_str());
			SetStatusBarText( 1, "" );
			SetStatusBarText( 1, tmp );
			};
			break;
		case BYBYTE:
			{
			char buffer[10] = {0};
			std::string text;
			text = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
			text += " Output: ";
			text += itoa( address, buffer, 10 );
			text += "=";
			text += "0x";
			memset( buffer, 0, 10 );
			text += itoa( RTUSimulator::getInstance().GetInternalTableValue( address ) & 0x00ff, buffer, 16 );
			CString tmp;
			tmp.Insert(0, text.c_str());
			SetStatusBarText( 1, "" );
			SetStatusBarText( 1, tmp );
			};
			break;
		case BYWORD:
			{
			char buffer[10] = {0};
			std::string text;
			text = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
			text += " Output: ";
			text += itoa( address, buffer, 10 );
			text += "=";
			text += "0x";
			memset( buffer, 0, 10 );
			text += itoa( RTUSimulator::getInstance().GetInternalTableValue( address ), buffer, 16 );
			CString tmp;
			tmp.Insert(0, text.c_str());
			SetStatusBarText( 1, "" );
			SetStatusBarText( 1, tmp );
			};
			break;
		case BYDWORD:
			{
			char buffer[10] = {0};
			std::string text;
			text = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
			text += " Output: ";
			text += itoa( address, buffer, 10 );
			text += "=";
			text += "0x";
			memset( buffer, 0, 10 );
			text += itoa( RTUSimulator::getInstance().GetInternalTableValue( address ), buffer, 16 );
			text += " ";
			text += itoa( address+1, buffer, 10 );
			text += "=";
			text += "0x";
			memset( buffer, 0, 10 );
			text += itoa( RTUSimulator::getInstance().GetInternalTableValue( address+1 ), buffer, 16 );
			CString tmp;
			tmp.Insert(0, text.c_str());
			SetStatusBarText( 1, "" );
			SetStatusBarText( 1, tmp );	
			};
			break;
		default:
			break;
		}
	}
}

void CPLCSimulatorDlg::OnBnClickedBybit()
{
	m_esetType = BYBIT;
}

void CPLCSimulatorDlg::OnBnClickedBybyte()
{
	m_esetType = BYBYTE;
}

void CPLCSimulatorDlg::OnBnClickedByword()
{
	m_esetType = BYWORD;
}

void CPLCSimulatorDlg::OnBnClickedBydword()
{
	m_esetType = BYDWORD;
}

void CPLCSimulatorDlg::OnBnClickedReloadSaveData()
{
	RTUSimulator::getInstance().ReloadSaveData();
	ProcessPrimaryCheckState();
	ProcessDeviceCheckState();
}

// reload all config files, config.txt, stress file, reaction file. while reload config file, load savedata also.
void CPLCSimulatorDlg::OnBnClickedReLoad()
{
	CString str;
	GetDlgItemText(IDC_CONFIG, str);

	ReSetConfigFile(std::string(str));

	RTUSimulator::getInstance().InitReaction();

	ReSetStressCfgFile(RTUSimulator::getInstance().GetStressCfgName());
}

void CPLCSimulatorDlg::EnableDlg(void)
{
	GetDlgItem(IDC_PRIMARY)->EnableWindow();
	GetDlgItem(IDC_SWITCH )->EnableWindow();
	GetDlgItem(IDC_STANDBY)->EnableWindow();
	GetDlgItem(IDC_DEVICE )->EnableWindow();
	GetDlgItem(IDC_BYBIT  )->EnableWindow();
	GetDlgItem(IDC_BYBYTE )->EnableWindow();
	GetDlgItem(IDC_BYWORD )->EnableWindow();
	GetDlgItem(IDC_BYDWORD)->EnableWindow();
	GetDlgItem(IDC_SET    )->EnableWindow();
	GetDlgItem(IDC_RELOADSAVEDATA )->EnableWindow();
	GetDlgItem(IDC_ADDR   )->EnableWindow();
	GetDlgItem(IDC_VAL    )->EnableWindow();
	GetDlgItem(IDC_GET    )->EnableWindow();
}

void CPLCSimulatorDlg::OnBnClickedLogtofile()
{
	RTUSimulator::getInstance().SetLogToFile( (( (CButton*)GetDlgItem(IDC_LOGTOFILE) )->GetCheck() == 1) ? true: false);
}

void CPLCSimulatorDlg::OnBnClickedOn()
{
	Start();

	CToolBarCtrl &toolBarCtrl= m_WndToolBar.GetToolBarCtrl();
	toolBarCtrl.HideButton( ID_TOOLBAR_RTUOFF, false );
	toolBarCtrl.HideButton( ID_TOOLBAR_RTUON, true );

	CMenu *menu = GetMenu();
	menu->EnableMenuItem(ID_TOOLBAR_RTUON, MF_GRAYED);
	menu->EnableMenuItem(ID_TOOLBAR_RTUOFF, MF_ENABLED);
}

void CPLCSimulatorDlg::OnBnClickedOff()
{
	Pause();

	CToolBarCtrl &toolBarCtrl= m_WndToolBar.GetToolBarCtrl();
	toolBarCtrl.HideButton( ID_TOOLBAR_RTUOFF, true );
	toolBarCtrl.HideButton( ID_TOOLBAR_RTUON, false );

	CMenu *menu = GetMenu();
	menu->EnableMenuItem(ID_TOOLBAR_RTUON, MF_ENABLED);
	menu->EnableMenuItem(ID_TOOLBAR_RTUOFF, MF_GRAYED);
}

void CPLCSimulatorDlg::Start(void)
{
	RTUSimulator & rtuSim = RTUSimulator::getInstance();
	int Num = rtuSim.GetPortsNum();

	for(int i = 0; i < Num; ++i)
	{
		RTUTaskManager::getTaskItem(i).resume();
		rtuSim.resumeClientsOnListenPort(i);
	}
}

void CPLCSimulatorDlg::Pause(void)
{
	RTUSimulator & rtuSim = RTUSimulator::getInstance();
	int Num = rtuSim.GetPortsNum();

	for(int i = 0; i < Num; ++i)
	{
		RTUTaskManager::getTaskItem(i).suspend();
		rtuSim.stopClientsOnListenPort(i);
	}
}

void CPLCSimulatorDlg::DisExtSys(void)
{
	std::map<std::string, int> DeviceName = RTUSimulator::getInstance().GetDeviceMap();
	std::map<std::string, int>::iterator begin = DeviceName.begin();
	std::map<std::string, int>::iterator end = DeviceName.end();

	while(begin != end)
	{
		RTUSimulator::getInstance().SetDeviceStatus(begin->first, false);
		m_devices.SetCheck(m_devices.FindString(0,begin->first.c_str()),0);
		++ begin;
	}

	GetDlgItem(IDC_DEVICE )->EnableWindow(false);
}

void CPLCSimulatorDlg::EnaExtSys(void)
{
	std::map<std::string, int> DeviceName = RTUSimulator::getInstance().GetDeviceMap();
	std::map<std::string, int>::iterator begin = DeviceName.begin();
	std::map<std::string, int>::iterator end = DeviceName.end();

	while(begin != end)
	{
		RTUSimulator::getInstance().SetDeviceStatus(begin->first, true);
		m_devices.SetCheck(m_devices.FindString(0,begin->first.c_str()),1);
		++ begin;
	}

	GetDlgItem(IDC_DEVICE )->EnableWindow();
}

void CPLCSimulatorDlg::OnBnClickedPrimary()
{
	RTUSimulator::getInstance().SetData(6,1,BYBIT,4);
	RTUSimulator::getInstance().SetData(6,0,BYBIT,5);
}

void CPLCSimulatorDlg::OnBnClickedStandby()
{
	RTUSimulator::getInstance().SetData(6,0,BYBIT,4);
	RTUSimulator::getInstance().SetData(6,1,BYBIT,5);
}

void CPLCSimulatorDlg::OnBnClickedSwitch()
{
	if(1 == ((CButton*)GetDlgItem(IDC_SWITCH))->GetCheck())
	{
		RTUSimulator::getInstance().SetData(6,1,BYBIT,3);
	}
	else
	{
		RTUSimulator::getInstance().SetData(6,0,BYBIT,3);
	}
}

void CPLCSimulatorDlg::OnBnClickedGet()
{
	CString addr;
	GetDlgItem(IDC_ADDR)->GetWindowText(addr);

	CString val;
	int address = 0;
	unsigned int value = 0;
	int bit = 0;
	CString bitstr;

	if( BYBIT == m_esetType )
	{
		if(addr.Find(".") != -1)
		{
			CString addrtmp = addr;
			addr = addrtmp.Left(addrtmp.Find("."));
			bitstr= addrtmp.Mid(addrtmp.Find(".") + 1); 
		}
		else
		{
			AfxMessageBox("Didn't give which bit you want to set!\nWill set bit 0", MB_OK);
		}
	}
	else
	{
		if(addr.Find(".") != -1)
		{
			AfxMessageBox("the .bit is useless unless you set data by bit", MB_OK);
			addr= addr.Left(addr.Find(".") + 1);
		}
	}

	if((addr.Find("0X") != -1) || (addr.Find("0x") != -1))
	{
		addr.Delete(0,2);
		address = _tcstoul(addr, 0, 16);
	}
	else
	{
		address = _tcstoul(addr, 0, 10);
	}

	if((bitstr.Find("0X") != -1) || (bitstr.Find("0x") != -1))
	{
		bitstr.Delete(0,2);
		bit = _tcstoul(bitstr, 0, 16);
	}
	else
	{
		bit = _tcstoul(bitstr, 0, 10);
	}

	if( !RTUSimulator::getInstance().GetData(address, value, m_esetType, bit) )
	{
		AfxMessageBox("Fail!", MB_OK);
	}
	else
	{
		val.Format("0X%X", value);
		GetDlgItem(IDC_VAL)->SetWindowText(val);
	}
}

void CPLCSimulatorDlg::OnBnClickedCheckAddress()
{
	if(1 == ((CButton*)GetDlgItem(IDC_CHK_CLIENT_ADDRESS))->GetCheck())
	{
		RTUSimulator::getInstance().setCheckClientRequestAddr(true);
	}
	else
	{
		RTUSimulator::getInstance().setCheckClientRequestAddr(false);
	}
}
void CPLCSimulatorDlg::CreateToolbar()
{
	if (!m_WndToolBar.CreateEx(this, TBSTYLE_FLAT , WS_CHILD | WS_VISIBLE | CBRS_TOP | 
		CBRS_TOOLTIPS | CBRS_FLYBY ) ||
		!m_WndToolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("Failed to create toolbar\n");
	}

	AddTextToButton(ID_FILE_lOADCONFIGFILE, "LoadCfg");
	AddTextToButton(ID_FILE_lOADSTRESSCFG, "StressCfg");
	AddTextToButton(ID_TOOLBAR_STRESSON, "StressOn");
	AddTextToButton(ID_TOOLBAR_STRESSOFF, "StressOff");
	AddTextToButton(ID_TOOLBAR_RTUON, "PLC ON");
	AddTextToButton(ID_TOOLBAR_RTUOFF, "PLC OFF");
	AddTextToButton(ID_TOOLBAR_RTURELOAD, "RELOAD");
	AddTextToButton(ID_TOOLBAR_HELP, "Help");
	AddTextToButton(ID_TOOLBAR_ABOUT, "About");

	m_WndToolBar.GetToolBarCtrl().SetButtonSize(CSize(60,40));	
}

int CPLCSimulatorDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CreateToolbar();
	CreateStatusBar();
	CalculateToolBarStatusBarHeights();

	CToolBarCtrl &toolBarCtrl= m_WndToolBar.GetToolBarCtrl();
	toolBarCtrl.HideButton( ID_TOOLBAR_RTUOFF, true );
	toolBarCtrl.HideButton( ID_TOOLBAR_STRESSOFF, true );

	CMenu *menu = GetMenu();
	menu->EnableMenuItem(ID_TOOLBAR_RTUOFF, MF_GRAYED);
	menu->EnableMenuItem(ID_TOOLBAR_STRESSOFF, MF_GRAYED);
	SetTimer( NULL, 1000, NULL);

	toolBarCtrl.HideButton( ID_TOOLBAR_RTUON, false );
	toolBarCtrl.HideButton( ID_TOOLBAR_STRESSON, false );

	menu->EnableMenuItem(ID_TOOLBAR_RTUON, MF_ENABLED);
	menu->EnableMenuItem(ID_TOOLBAR_STRESSON, MF_ENABLED);

	return 0;
}

void CPLCSimulatorDlg::CreateStatusBar()
{
	if (!m_WndStatusBar.CreateEx(this, SBT_TOOLTIPS, WS_CHILD | WS_VISIBLE  | CBRS_BOTTOM, AFX_IDW_STATUS_BAR) ||
		!m_WndStatusBar.SetIndicators(NULL, 3))
	{
		TRACE0("Failed to create status bar\n");
	}

	m_WndStatusBar.SetPaneInfo(0, m_WndStatusBar.GetItemID(0), SBPS_STRETCH | SBPS_POPOUT | SBPS_NOBORDERS, NULL);
	m_WndStatusBar.SetPaneInfo(1, m_WndStatusBar.GetItemID(1), SBPS_NORMAL, 280);
	m_WndStatusBar.SetPaneInfo(2, m_WndStatusBar.GetItemID(2), SBPS_NORMAL, 120);
	
	SetStatusBarText(0, "Ready");
}

void CPLCSimulatorDlg::AddTextToButton(UINT buttonId, CString buttonLabel )
{
	int index = m_WndToolBar.CommandToIndex(buttonId);

	if( index >=0 )
	{
		m_WndToolBar.SetButtonText(index, buttonLabel);
	}
	else
	{
		TRACE1("Unable to find %s button\n", buttonLabel);
	}
}

void CPLCSimulatorDlg::CalculateToolBarStatusBarHeights()
{
	CRect rcClientStart;
	CRect rcClientNow;
	
	GetClientRect(rcClientStart);
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, 0, reposDefault, rcClientNow);
	
	m_ToolBarHeight   = rcClientNow.top;
	m_StatusBarHeight = rcClientStart.bottom - rcClientNow.bottom;
}

void CPLCSimulatorDlg::SetStatusBarText(int aIndex, const CString &message)
{
	CDC *dc = m_WndStatusBar.GetDC();	

	if (dc != NULL)
	{
		int width; 
		unsigned int a, b;

		m_WndStatusBar.GetPaneInfo(aIndex, a, b, width);
		
		int textExtent		= dc->GetTextExtent(message).cx;
		int spaceExtent		= dc->GetTextExtent(_T(" ")).cx;
		int numSpacesNeeded = ((width - textExtent) / 2) / spaceExtent;
		CString str(message);
		
		for (int i = 1; i <= numSpacesNeeded + 2; i++)
			str.Insert(0, " ");
		m_WndStatusBar.SetPaneText(aIndex, str);
		
		m_WndStatusBar.ReleaseDC(dc);
		dc = NULL;
	}
}

void CPLCSimulatorDlg::OnBnClickedDisplayHelp()
{
	m_HelpDialog.DoModal();
}

void CPLCSimulatorDlg::OnBnClickedDisplayAbout()
{
	m_AboutDialog.DoModal();
}

void CPLCSimulatorDlg::OnTimer(UINT nIDEvent)
{
	CString text;
	text = CTime::GetCurrentTime().Format("%y-%m-%d %H:%M:%S");
	SetStatusBarText(2,text);
	CDialog::OnTimer(nIDEvent);
}

BOOL CPLCSimulatorDlg::OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{			
	UINT nID = pNMHDR->idFrom;
	
	if ((nID == ID_FILE_lOADCONFIGFILE) ||
		(nID == ID_FILE_lOADSTRESSCFG) ||
		(nID == ID_TOOLBAR_STRESSON) ||
		(nID == ID_TOOLBAR_STRESSOFF) ||
		(nID == ID_TOOLBAR_RTUON) ||
		(nID == ID_TOOLBAR_RTUOFF)||
		(nID == ID_TOOLBAR_RTURELOAD) ||
		(nID == ID_TOOLBAR_ABOUT) ||
		(nID == ID_TOOLBAR_HELP))		
	{	
		char szFullText[255];
		AfxLoadString(nID, szFullText);        
		SetStatusBarText(0, szFullText);
		return TRUE;
	} 
	else if ((nID == IDC_STRESSCFG) ||
		(nID == IDC_CONFIG))
	{
		char szFullText[1000];
		AfxLoadString(nID, szFullText);
		SetStatusBarText(0, szFullText);
		return TRUE;
	}
	else
	{
		SetStatusBarText(0, "Ready");
	}			

	return TRUE;
}

void CPLCSimulatorDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu) 
{
	CDialog::OnMenuSelect(nItemID, nFlags, hSysMenu);
	
	TCHAR szFullText[256];
	CString cstStatusText;
	
	if ((nItemID > 3)) // NOTE : Ignore first 3 menu popups
	{
		LoadString(AfxGetResourceHandle(), nItemID, szFullText, 256);	
		AfxExtractSubString(cstStatusText, szFullText, 0, '\n');
		SetStatusBarText(0, cstStatusText);
	} 
	else
	{
		SetStatusBarText(0, "Ready");
	}
}

void CPLCSimulatorDlg::OnSelchangeComboProtocal() 
{
	// TODO: Add your control notification handler code here
	CComboBox* pCombo;
	pCombo = (CComboBox*)GetDlgItem( IDC_COMBO_PROTOCAL );

	switch ( pCombo->GetCurSel() )
	{
	case 0:
		RTUSimulator::getInstance().SetProtocalType( STANDARDMODBUS );
		break;
	case 1:
		RTUSimulator::getInstance().SetProtocalType( C830MODBUS );
		break;
	}
}

BOOL CPLCSimulatorDlg::OnCopyData( CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct )
{
	std::string strData((char*) pCopyDataStruct->lpData);
	//AfxMessageBox(strData.c_str());
	unsigned int indx = strData.find(" ");
	if (std::string::npos == indx)
	{
		indx = strData.find("\n");
	}
	std::string cmd;
	std::string ParaStr;
	if (std::string::npos != indx)
	{
		cmd = strData.substr(0, indx);
		ParaStr = strData.substr(indx);
	}
	if (cmd == "ReloadStressConfig" || cmd == "AddStressConfig")
	{
		indx = strData.find(STRESS_RPARAM);
		if (std::string::npos == indx)
		{
			return FALSE;
		}
		indx = strData.find("=", indx);
		if (std::string::npos == indx)
		{
			return FALSE;
		}
		std::string stress_file;
		++indx;
		if ('\"' == strData.c_str()[indx])
		{
			stress_file = strData.substr(indx, strData.find("\"", indx) - indx);
		}
		else
		{
			unsigned int endIndx = strData.find(" ", indx);
			if (std::string::npos == endIndx)
			{
				endIndx = strData.find("\n", indx);
			}
			if (std::string::npos == endIndx)
			{
				endIndx = strData.length();
			}
			stress_file = strData.substr(indx, endIndx - indx);
		}
		if (cmd == "AddStressConfig")
		{
			stress_file = RTUSimulator::getInstance().GetStressCfgName() + ";" + stress_file;
		}
		ReSetStressCfgFile(stress_file);
	}
	else if(cmd == "Kill")
	{
		//SendMessage(WM_CLOSE, 0, 0);
		//exit(0);
		CloseMainWindow();
	}
	else if (cmd == "StressPlcOnOffOp")
	{
		indx = strData.find(STRESSON_PARAM);
		if (std::string::npos != indx)
		{
			indx = strData.find("=", indx);
			if (std::string::npos != indx)
			{
				++indx;
				if ('1' == strData.c_str()[indx])
				{
					OnBnClickedStressOn();
				}
				else
				{
					OnBnClickedStressOff();
				}
			}
		}
		indx = strData.find(PLCON_PARAM);
		if (std::string::npos != indx)
		{
			indx = strData.find("=", indx);
			if (std::string::npos != indx)
			{
				++indx;
				if ('1' == strData.c_str()[indx])
				{
					OnBnClickedOn();
				}
				else
				{
					OnBnClickedOff();
				}
			}
		}
	}
	else if (cmd == "ClearStressConfig")
	{
		ReSetStressCfgFile("");
	}
	else if (cmd == "SetDataString")
	{
		SetDataString(ParaStr);
	}
	return TRUE;
}

void CPLCSimulatorDlg::SetDeviceCheckState( const char* pDeviceName, int nValue )
{
	m_devices.SetCheck(m_devices.FindString(0, pDeviceName), nValue);
}

void CPLCSimulatorDlg::ProcessPrimaryCheckState()
{
	unsigned int statusValue = 0;
	if (RTUSimulator::getInstance().GetData(6, statusValue, BYWORD))
	{
		((CButton*)GetDlgItem(IDC_SWITCH))->SetCheck(0x0008 & statusValue);
		((CButton*)GetDlgItem(IDC_PRIMARY))->SetCheck(0x0010 & statusValue);
		((CButton*)GetDlgItem(IDC_STANDBY))->SetCheck(0x0020 & statusValue);
	}
}

void CPLCSimulatorDlg::ProcessDeviceCheckState()
{
	std::map<std::string, int>::iterator mbgn = RTUSimulator::getInstance().GetDeviceMap().begin();
	std::map<std::string, int>::iterator mend = RTUSimulator::getInstance().GetDeviceMap().end();

	while(mbgn != mend)
	{
		m_devices.SetCheck(
			m_devices.FindString(0, mbgn->first.c_str()),
			RTUSimulator::getInstance().IsDeviceValid(mbgn->first.c_str())
			);
		++mbgn;
	}
}

void CPLCSimulatorDlg::ConfigFileUpdated()
{
	SetDlgItemText(IDC_ADDRESS, RTUSimulator::getInstance().GetAddress().c_str());
	SetDlgItemText(IDC_POLLPORT, RTUSimulator::getInstance().GetPollPort().c_str());
	SetDlgItemText(IDC_CMDPORT, RTUSimulator::getInstance().GetCmdPort().c_str());
	SetDlgItemText(IDC_IDENTITY, RTUSimulator::getInstance().GetIdentification().c_str());
	SetDlgItemText(IDC_LOC, RTUSimulator::getInstance().GetLocation().c_str());
	SetDlgItemText(IDC_CONFIG, RTUSimulator::getInstance().GetCfgName().c_str());
	
	std::string appName = RTUSimulator::getInstance().GetLocation();
	appName.append("-PLCSimulator");
	SetWindowText(appName.c_str());
		
	int Num = RTUSimulator::getInstance().GetPortsNum();

	int i = 0;
	for(i = 0; i < Num; ++i)
	{
		RTUTaskManager::getTaskItem(i).SetID(i);
		int res = RTUTaskManager::getTaskItem(i).activate(
			THR_NEW_LWP | THR_JOINABLE | THR_SUSPENDED, 
			1, 0, ACE_DEFAULT_THREAD_PRIORITY, -1, 0, 0, 0, 0, 0
			);
	}

	m_devices.ResetContent();
	m_devices.SetCheckStyle( BS_CHECKBOX );

	std::map<std::string, int>::iterator mbgn = RTUSimulator::getInstance().GetDeviceMap().begin();
	std::map<std::string, int>::iterator mend = RTUSimulator::getInstance().GetDeviceMap().end();

	while(mbgn != mend)
	{
		m_devices.AddString(mbgn->first.c_str());
		++mbgn;
	}
	ProcessPrimaryCheckState();
	ProcessDeviceCheckState();
}

void CPLCSimulatorDlg::SetDataString(std::string str)
{
	int sta = 0, end = 0, len = str.length();
	bool bFindEqual = false;
	std::string address;
	while(sta < len)
	{
		if (' ' == str[sta])
		{
			++sta;
			end = sta;
		}
		if (',' == str[end] || ';' == str[end] || '\n' == str[end] || '=' == str[end] || end == len)
		{
			std::string tempStr(str.substr(sta, end - sta));
			if ('=' == str[end])
			{
				bFindEqual = true;
				address = tempStr;
			}
			else
			{
				bFindEqual = false;
				if ("" != address && "" != tempStr)
				{
					int addr = 0, bit = 0;
					unsigned int v = 0;
					unsigned int dotPos = address.find(".", 0);
					SETTYPE setType = BYWORD;
					if(std::string::npos != dotPos)
					{
						setType = BYBIT;
						bit = atoi(address.substr(dotPos + 1).c_str());
					}
					addr = atoi(address.substr(0, dotPos).c_str());
					v = strtoul(tempStr.c_str(), 0, 10);
					RTUSimulator::getInstance().SetData(addr, v, setType, bit);
				}
			}
			sta = end + 1;
			end = sta;
		}
		++end;
	}
}
