// TrainSimulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainSimulator.h"
#include "TrainSimulatorDlg.h"
#include "SimulatorConfig.h"
#include "GlobalDataTypes.h"

#include "HeartBeatDlgConfig.h"
#define MAX_TRAIN 150

#include <sstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CTrainSimulatorDlg dialog




CTrainSimulatorDlg::CTrainSimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTrainSimulatorDlg::IDD, pParent)
	, m_controller(NULL)
	, m_FileLogs(NULL)
	, trainSelectedType(0)
	,m_logFlag(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTrainSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOGS, m_listInfo);
	DDX_Control(pDX, IDC_EDIT_LOGS, m_logView);
	//DDX_Control(pDX, IDC_LIST2, m_listSendEvent);
	//DDX_Control(pDX, IDC_LIST3, m_listReceiveData);
	DDX_Control(pDX, IDC_COMBO_STATUS, m_comboStatus);
	DDX_Control(pDX, IDC_COMBO_TRAIN_ID, m_combo_trainID);
	DDX_Control(pDX, IDC_COMBO_TRAIN_MULTIPLE,m_combo_trainIDMultiple);
	DDX_Control(pDX, IDC_COMBO_TIMS_COMMAND, m_TimsCommand);
	DDX_Control(pDX, IDC_STATIC_TO_TRAINID, m_toTrainID);
	DDX_Control(pDX, IDC_COMBO_TSI, m_comboTsiSetup);
	
}

BEGIN_MESSAGE_MAP(CTrainSimulatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_COPYDATA()
	ON_MESSAGE(WM_UPDATE_LOGGER,onUpdateLogView)
	ON_MESSAGE(WM_HEARTBEAT_INFO,OnUIUpdateHeartBeatInfo)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_STARTSERVER, &CTrainSimulatorDlg::OnBnClickedButtonStartserver)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CTrainSimulatorDlg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SEND_STATUS, &CTrainSimulatorDlg::OnBnClickedButtonSendStatus)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_TRAIN_HEARTBETAT, &CTrainSimulatorDlg::OnBnClickedButtonTrainHeartbetat)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_COMBO_TIMS_COMMAND, &CTrainSimulatorDlg::OnCbnSelchangeComboTimsCommand)
	ON_BN_CLICKED(IDC_RADIO_SINGLE_TRAIN, &CTrainSimulatorDlg::OnBnClickedRadioSingleTrain)
	ON_BN_CLICKED(IDC_RADIO_MULTIPLE_TRAINS, &CTrainSimulatorDlg::OnBnClickedRadioMultipleTrains)
	ON_CBN_SELCHANGE(IDC_COMBO_TRAIN_ID, &CTrainSimulatorDlg::OnCbnSelchangeComboTrainId)
	ON_BN_CLICKED(IDC_BUTTON_STATUS_CONFIG, &CTrainSimulatorDlg::OnBnClickedButtonStatusConfig)
	ON_BN_CLICKED(IDC_BUTTON2, &CTrainSimulatorDlg::OnBnClickedButton2)
	ON_CBN_SELCHANGE(IDC_COMBO_TSI, &CTrainSimulatorDlg::OnRadioIdSelectionChange)
END_MESSAGE_MAP()


// CTrainSimulatorDlg message handlers

BOOL CTrainSimulatorDlg::OnInitDialog()
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
	m_listInfo.InsertColumn(0,L"Time",LVCFMT_LEFT,150);
	m_listInfo.InsertColumn(1,L"Title",LVCFMT_LEFT,100);
	m_listInfo.InsertColumn(2,L"Message",LVCFMT_LEFT,500);


	readConfig();
	ResourceInfo infoResource;
	infoResource.HanderWND = this->m_hWnd;
	infoResource.ID = IDC_EDIT_LOGS;

	initializeData();

	m_controller->logViewID(infoResource);
	m_comboStatus.SetCurSel(0);
	m_combo_trainID.SetCurSel(0);
	m_combo_trainIDMultiple.SetCurSel(0);
	m_TimsCommand.SetCurSel(0);

	CButton* pButton = (CButton*)GetDlgItem(IDC_RADIO_SINGLE_TRAIN);
	pButton->SetCheck(1);
	m_toTrainID.ShowWindow(SW_HIDE);
	m_combo_trainIDMultiple.ShowWindow(SW_HIDE);

	
	m_comboTsiSetup.AddString(L"Primary Tsi");
	m_comboTsiSetup.AddString(L"Secondary Tsi");

 	IConfigDataItemPtrType autoSendInfo = m_controller->getUpdateConfForAutoChanging(ERadioIdSetup);
 	RadioIdSetupConfig * autoSendingPtr = (RadioIdSetupConfig * )autoSendInfo.get();
 
	if (autoSendingPtr->getRadioConfigValue())
	{
		m_comboTsiSetup.SetWindowText(L"Primary Tsi");
	}
	else
	{
		m_comboTsiSetup.SetWindowText(L"Secondary Tsi");
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTrainSimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTrainSimulatorDlg::OnPaint()
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
HCURSOR CTrainSimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTrainSimulatorDlg::initializeData()
{
	//Initialize TIMS Command
	std::vector<CString> TimsVecData;
	TimsVecData.push_back(L"End of TTIS Download");
	TimsVecData.push_back(L"Changeover Status");
	TimsVecData.push_back(L"RequestForOccCall");
	TimsVecData.push_back(L"Test Call");

	int tmsCount = 0;
	
	std::vector<CString>::iterator iter = TimsVecData.begin();
	for ( iter; iter != TimsVecData.end(); iter++)
	{
		m_TimsCommand.InsertString(tmsCount++,*iter);
	}
	

	//Initialize 150 Trains selection
	populateTrains(IDC_COMBO_TRAIN_ID);
	

	//Initialize Status Command, by default load the End of TTIS Download status data
	loadStatusData(END_OF_TTIS_DOWNLOAD);
	

}

void CTrainSimulatorDlg::loadStatusData(CommandType command)
{
	switch(command)
	{
	case END_OF_TTIS_DOWNLOAD:
		loadTTISStatusData();
		break;

	case  CHANGEOVER_STATUS:
		loadChangeoverStatusData();
		break;

	case REQUEST_FOR_OCC_CALL:
		//TODO:
		break;

	}
	m_comboStatus.SetCurSel(0);
}

CommandType CTrainSimulatorDlg::getCommandSelected() const
{
	CommandType _command = INVALID_COMMAND;
	int item = m_TimsCommand.GetCurSel();
	switch(item)
	{
	case 0:
		_command = END_OF_TTIS_DOWNLOAD; 
		break;

	case 1:
		_command = CHANGEOVER_STATUS;
		break;

	case 2:
		_command = REQUEST_FOR_OCC_CALL;
		break;

	case 3:
		_command = TEST_CALL;
		break;

	default:
		::MessageBox(this->m_hWnd,L"Invalid Command selected, Please select the correct Command",L"Command Error Message", MB_ICONERROR | MB_OK);
	}

	return _command;

}

void CTrainSimulatorDlg::loadTTISStatusData()
{
	std::vector<CString> strStatus = getTTISCmdStatus();
	std::vector<CString>::iterator iterStat = strStatus.begin();
	int statusCount = 0;

	for ( iterStat; iterStat != strStatus.end(); iterStat++ )
	{
		m_comboStatus.InsertString(statusCount++, *iterStat);
	}
}

void CTrainSimulatorDlg::loadChangeoverStatusData()
{
	std::vector<CString> strStatus = getChangeoverCmdStatus();
	std::vector<CString>::iterator iterStat = strStatus.begin();
	int statusCount = 0;

	for ( iterStat; iterStat != strStatus.end(); iterStat++ )
	{
		m_comboStatus.InsertString(statusCount++, *iterStat);
	}
}

void CTrainSimulatorDlg::OnBnClickedButtonStartserver()
{
	// TODO: Add your control notification handler code here

	CString temp;
	GetDlgItemText(IDC_BUTTON_STARTSERVER,temp);
	CT2CA pszConvertedAnsiString(temp);
	std::string sTemp(pszConvertedAnsiString);

	m_controller->onBtnClick(sTemp,IDC_BUTTON_STARTSERVER);
}

void CTrainSimulatorDlg::setBtnCaption(const std::string &caption)
{
	CString temp(caption.c_str());
	SetDlgItemText(IDC_BUTTON_STARTSERVER,temp);
	
}

void CTrainSimulatorDlg::addListener(TA_IRS_App::IController *controller)
{
	m_controller = controller;

	m_autoSendingConfDlg.addListener(controller);

}

void CTrainSimulatorDlg::updateUI(MessageInfo msgInfo)
{
	switch(msgInfo.ID)
	{
	case IDC_BUTTON_STARTSERVER :
		setBtnCaption(msgInfo.BtnCaption);
		updateListInfo(msgInfo.ListInfoMessage.Title,msgInfo.ListInfoMessage.Message);
		break;

	}
}

void CTrainSimulatorDlg::updateListInfo(const std::string& title, const std::string& msge)
{
	CString mTitle( title.c_str() );
	CString mMsge( msge.c_str() );

	//Get Time
	SYSTEMTIME now;
	GetLocalTime(&now);
	CString strTime;

	strTime.Format(L"%d-%d-%d %d:%d:%d.%d",now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,now.wMilliseconds );

	m_listInfo.InsertItem(0,L"");
	m_listInfo.SetItemText(0,0,strTime);
	m_listInfo.SetItemText(0,1,mTitle);
	m_listInfo.SetItemText(0,2,mMsge);

}
void CTrainSimulatorDlg::OnBnClickedButtonClear()
{
	// TODO: Add your control notification handler code here
	while(m_listInfo.GetItemCount() > 0 )
		m_listInfo.DeleteItem(0);
}

void CTrainSimulatorDlg::readConfig()
{
	SimulatorConfig* config = SimulatorConfig::getInstance();
	WCHAR wbuf[256];
	WCHAR wbufPDSServer[256];
	WCHAR wbufPDSPort[256];

	if ( config->m_ip != "")
	{
		memset(wbuf,0,sizeof(wbuf));
		MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,config->m_ip.c_str(),config->m_ip.length(),wbuf,sizeof(wbuf));
		SetDlgItemText(IDC_EDIT_SERVER,wbuf);
		SetDlgItemInt(IDC_EDIT_PORT,config->m_port);
		
	}
}

LRESULT CTrainSimulatorDlg::onUpdateLogView(WPARAM wparm, LPARAM lparam)
{

	LogInfo* ptr = reinterpret_cast<LogInfo*>(wparm);

	LogInfo _localCopy;
	_localCopy.LogMessage = ptr->LogMessage;
	_localCopy.Time = ptr->Time;

	//Clean ptr
	delete ptr;
	ptr = NULL;

	std::string _log = _localCopy.Time + ":" +  _localCopy.LogMessage;

	CString temp;

	m_logView.GetWindowText(temp);
	_log += "\r\n";

	int _countLine = m_logView.GetLineCount();

// 	if ( temp.GetLength() >= 7000 )
// 	{
// 		temp.Delete(0,temp.GetLength()-1);
// 	}

	//Disable for testing purposes only


	if ( _countLine >= 15 )
	{
		m_logView.SetSel(0,-1);
		m_logView.Clear();
		temp = "";
	}

	CString _logView(_log.c_str());
	temp.Insert(0,_logView);

	m_logView.SetWindowText(temp);

	CT2CA pszConvertedAnsiString(temp);

	std::string _logFile(pszConvertedAnsiString);


	
	//Check if Log Status is enable
	if (SimulatorConfig::getInstance()->m_logStatus)
	{
		updateLogFile(_log);
	}
	

	return 0l;
}

void CTrainSimulatorDlg::updateLogFile(const std::string& log)
{

	if ( NULL != m_FileLogs )
	{
		m_FileLogs->Write(log.c_str(),log.size());

		if ( m_FileLogs->GetPosition() >= 0xFFFFF ) //2^20 bytes for 1mb size;
		{
			m_FileLogs->Flush();
			m_FileLogs->Close();

			delete m_FileLogs;
			m_FileLogs = NULL;

			m_logFlag++;

		}
	}
	else
	{
		std::ostringstream _strFileName;
		_strFileName << "TrainSimulator_" << m_logFlag << ".log";
		CString _fileName(_strFileName.str().c_str());
		LPCTSTR strLPC = _fileName;

		m_FileLogs = new CFile(strLPC,CFile::modeCreate|CFile::shareDenyNone|CFile::modeWrite);
		m_FileLogs->Write(log.c_str(),log.size());
	}
}

BOOL CTrainSimulatorDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	//Check the Server if it is already started
	checkServerStatus();

	std::string receiveData( (char*) pCopyDataStruct->lpData);
	CString temp(receiveData.c_str());

	updateListInfo("Received Event",receiveData);
	m_controller->pecEvent(receiveData);
	
	return TRUE;
}


BOOL CTrainSimulatorDlg::checkServerStatus()
{
	CButton* _button = reinterpret_cast<CButton*>(GetDlgItem(IDC_BUTTON_STARTSERVER));
	CString _temp;
	_button->GetWindowText(_temp);
	CT2CA convertedString(_temp);
	std::string _caption(convertedString);
	if ( _caption.compare("Start Server") == 0)
	{
		::MessageBox(this->m_hWnd,L"Server was not yet started..",L"Message",MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

void CTrainSimulatorDlg::OnBnClickedButtonSendStatus()
{
	// TODO: Add your control notification handler code here
	if ( FALSE == checkServerStatus())
		return;

	int trainID = getTrainIDSelected();
	int status = 0;
	CommandType _command = getCommandSelected();
	if ( _command == REQUEST_FOR_OCC_CALL )
	{
		status = REQUEST_FOR_OCC_CALL;
	}
	else if ( _command == TEST_CALL )
	{
		status = TEST_CALL;
	}
	else
	{
		status = getStatusSelected();
	}
	
	

	//return if any selected data is incorrect value
	if ( CB_ERR == trainID || CB_ERR == status || INVALID_COMMAND == _command )
		return;

	if ( FALSE == sendStatusMessage(status))
 		return;

	//Send TIMS Command
	
	CString strData;
	
	m_comboTsiSetup.GetWindowText(strData);

	bool bisPrimary = true;

	if (0 == strData.Compare(L"Secondary Tsi"))
	{
		bisPrimary = false;
	}

	if ( false == m_controller->sendTIMSCommand(_command,status,getTrainsSelected(),bisPrimary))
	{
		::MessageBox(NULL,L"No connection has been established, \nmessage will be disregarded.",L"Message",MB_ICONINFORMATION);
	}
}

int CTrainSimulatorDlg::getTrainIDSelected() const
{
	return trainIDSelected(IDC_COMBO_TRAIN_ID);
}

int CTrainSimulatorDlg::getTrainIDMultipleSelected() const
{
	int _ret = 0;
	if ( MAX_TRAIN == getTrainIDSelected())
		_ret = MAX_TRAIN;
	else
		_ret = trainIDSelected(IDC_COMBO_TRAIN_MULTIPLE);
		
	return _ret;
}

std::vector<int> CTrainSimulatorDlg::getTrainsSelected() const
{
	std::vector<int> _trainsSelected;

	//If Multiple Train selected
	if ( 1 == trainSelectedType )
	{
		for ( int item = getTrainIDSelected(); item <= getTrainIDMultipleSelected(); item++)
		{
			_trainsSelected.push_back(item);
		}		
	}
	else
		_trainsSelected.push_back(getTrainIDSelected());

	return _trainsSelected;
}

int CTrainSimulatorDlg::trainIDSelected(int nID) const
{
	CComboBox* cmbox = (CComboBox*)GetDlgItem(nID);
	int _ret = 0;

	int item = cmbox->GetCurSel();
	CString itemData;

	if ( CB_ERR != item )
	{
		cmbox->GetLBText(item,itemData);
		_ret = _ttoi(itemData);
	}
	else
	{
		::MessageBox(this->m_hWnd,L"Wrong trainID inputed, please select the correct train",L"Train Error Message",MB_ICONERROR | MB_OK);
		_ret = -1;
	}
	return _ret;
}



std::vector<CString> CTrainSimulatorDlg::getTTISCmdStatus() const
{
	std::vector<CString> _ret;
	_ret.push_back(L"OK");
	_ret.push_back(L"Failed");
	_ret.push_back(L"Cancelled");
	_ret.push_back(L"Cancelled by OCC");
	return _ret;
}

std::vector<CString> CTrainSimulatorDlg::getChangeoverCmdStatus() const
{
	std::vector<CString> _ret;
	_ret.push_back(L"Stb");
	_ret.push_back(L"Main");
	return _ret;
}

int CTrainSimulatorDlg::getStatusSelected() 
{
	int _ret = 0;

	int item = m_comboStatus.GetCurSel();
	CString itemData;

	if ( CB_ERR != item )
	{
		if ( TRUE == ttisStatusCheck())
		{
			item++;
		}
		_ret = item;
	}
	else
	{
		::MessageBox(this->m_hWnd,L"Wrong Status inputed, please select the correct Status",L"Status Error Message",MB_ICONERROR | MB_OK);
		_ret = -1;
	}
	return _ret;
}

CString CTrainSimulatorDlg::getStatusTextSelected()
{
	CString _ret;
	int item = m_comboStatus.GetCurSel();
	if ( CB_ERR != item )
	{
		m_comboStatus.GetLBText(item,_ret);
	}
	return _ret;
}

BOOL CTrainSimulatorDlg::ttisStatusCheck()
{
	BOOL _ret = FALSE;
	CString _item = getStatusTextSelected();
	if ( END_OF_TTIS_DOWNLOAD == getCommandSelected() &&
		 0 == _item.Compare(getTTISCmdStatus().at(2)) || 
		 0 == _item.Compare(getTTISCmdStatus().at(3)) )
	{
		_ret = TRUE;
	}
	return _ret;
}

BOOL CTrainSimulatorDlg::sendStatusMessage(int status)
{
	BOOL _sendStatus = FALSE;
	CString statusResult = getStatusTextSelected();
	CString _log;

	if ( 0 == trainSelectedType )
	{
		if ( status == REQUEST_FOR_OCC_CALL )
		{
			_log.Format(L"Send TrainID : %d",getTrainIDSelected());
		}
		else
			_log.Format(L"Send TrainID : %d with Status : %s",getTrainIDSelected(),statusResult);
	}
	else
	{
		if ( status == REQUEST_FOR_OCC_CALL )
		{
			_log.Format(L"Send TrainID : %d  to TrainID: %d",getTrainIDSelected(),getTrainIDMultipleSelected());
		}
		else
			_log.Format(L"Send TrainID : %d  to TrainID: %d with Status : %s",getTrainIDSelected(),getTrainIDMultipleSelected(),statusResult);
	}
	

	if ( IDOK == ::MessageBox(this->m_hWnd,_log,L"Status Message",MB_ICONINFORMATION | MB_OKCANCEL) )
	{
		_sendStatus = TRUE;
	}
	
	return _sendStatus;
}

void CTrainSimulatorDlg::OnDestroy()
{
	__super::OnDestroy();

	// TODO: Add your message handler code here
	
	if ( NULL != m_FileLogs )
	{
		m_FileLogs->Close();
		delete m_FileLogs;
		m_FileLogs = NULL;
	}
}

void CTrainSimulatorDlg::OnBnClickedButtonTrainHeartbetat()
{
	// TODO: Add your control notification handler code here
	
	m_dlg.setHeartBeatInfo(m_controller->getTrainHeartBeatInfo());
	//m_dlg.setHeatBeatProcessor(m_controller->getHeartBeatProcessor());
	m_dlg.addObserver(this);
	m_dlg.setCurrentState(m_controller->getCurrentState());
	m_dlg.setCommsObserverStatus(m_controller->getCommsObserverStatus());
	m_dlg.setTrainStrategyMap(m_controller->getTrainStrategyMap());
	m_dlg.DoModal();

}

void CTrainSimulatorDlg::addTrainHeartbeat(int trainID, int delay)
{
	//call the controller to add the train
	m_controller->addTrainHeartBeat(trainID,delay);
}

void CTrainSimulatorDlg::sendStatusTrainHeartBeat(TrainStatus stat, int trainID)
{
	m_controller->sendStatusTrainHeartBeat(stat,trainID);
}

LRESULT CTrainSimulatorDlg::OnUIUpdateHeartBeatInfo(WPARAM wParm, LPARAM lParm)
{
	::PostMessage(m_dlg.m_hWnd,WM_HEARTBEAT,wParm,lParm);
	return 0l;
}

void CTrainSimulatorDlg::addTrainResponseStrategy(const TrainStrategyInfo& info)
{
	m_controller->addTrainResponseStrategy(info);
}

void CTrainSimulatorDlg::removeTrainResponseStrategy(int trainID)
{
	m_controller->removeTrainResponseStrategy(trainID);
}
void CTrainSimulatorDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	__super::OnClose();
}
void CTrainSimulatorDlg::OnCbnSelchangeComboTimsCommand()
{
	// TODO: Add your control notification handler code here
	CommandType _command = getCommandSelected();
	if ( INVALID_COMMAND == _command )
	{
		::MessageBox(this->m_hWnd,L"Invalid Command selected, Please select the correct Command",L"Command Error Message", MB_ICONERROR | MB_OK);
		return;
	}

	if ( _command == REQUEST_FOR_OCC_CALL ||
		 _command == TEST_CALL )
	{
		//Disable status combo box
		m_comboStatus.EnableWindow(FALSE);
	}
	else
	{
		//Disable status combo box
		m_comboStatus.EnableWindow(TRUE);
	}

	//Clear the Status data
	clearData(IDC_COMBO_STATUS);

	//load the Status Data here
	loadStatusData(_command);
}

void CTrainSimulatorDlg::clearData(int nID)
{
	//Delete every item from the status combo box
	CComboBox* cmbox = (CComboBox*)GetDlgItem(nID);

	for (int i= cmbox->GetCount() - 1; i >=0; i--)
	{
		cmbox->DeleteString(i);
	}
}

int CTrainSimulatorDlg::getCommandID(CommandType command) const
{
	int _retCommand = 0;
	switch(command)
	{
	case END_OF_TTIS_DOWNLOAD:
		_retCommand = 39;
		break;

	case CHANGEOVER_STATUS:
		_retCommand = 65;
		break;
	}
	return _retCommand;
}
void CTrainSimulatorDlg::OnBnClickedRadioSingleTrain()
{
	// TODO: Add your control notification handler code here
	trainSelectedType = 0;
	m_toTrainID.ShowWindow(SW_HIDE);
	m_combo_trainIDMultiple.ShowWindow(SW_HIDE);
}

void CTrainSimulatorDlg::OnBnClickedRadioMultipleTrains()
{
	// TODO: Add your control notification handler code here
	trainSelectedType = 1;
	m_toTrainID.ShowWindow(SW_SHOW);
	m_combo_trainIDMultiple.ShowWindow(SW_SHOW);

	//clear the train list first
	clearData(IDC_COMBO_TRAIN_MULTIPLE);

	//then populate new trains
	populateTrains(IDC_COMBO_TRAIN_MULTIPLE);
}

void CTrainSimulatorDlg::OnCbnSelchangeComboTrainId()
{
	// TODO: Add your control notification handler code here
	//clear trainID List
	clearData(IDC_COMBO_TRAIN_MULTIPLE);

	//then populate new trains
	populateTrains(IDC_COMBO_TRAIN_MULTIPLE);
}

void CTrainSimulatorDlg::populateTrains(int nID)
{
	CComboBox* cmbox = (CComboBox*)GetDlgItem(nID);
	int item = 0;
	int index = 0;

	if ( IDC_COMBO_TRAIN_MULTIPLE == nID )
	{
		item = getTrainIDSelected();
	}

	for ( int x = item; x < MAX_TRAIN; x++ )
	{
		CString _str;
		_str.Format(L"%d", x + 1);
		cmbox->InsertString(index++, _str);
	}

	cmbox->SetCurSel(0);
}

void CTrainSimulatorDlg::OnBnClickedButtonStatusConfig()
{
	// TODO: Add your control notification handler code here
	m_dlgTrainStatus.DoModal();
}

void CTrainSimulatorDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	m_autoSendingConfDlg.DoModal();
}


void CTrainSimulatorDlg::OnRadioIdSelectionChange()
{
	
	CString strData;

	m_comboTsiSetup.GetWindowText(strData);

	bool bisPrimary = true;

	if (0 == strData.Compare(L"Secondary Tsi"))
	{
		bisPrimary = false;
	}

	RadioIdSetupInfo info;
	info.bIsUsePrimary = !bisPrimary;

	m_controller->onRadioIdComboSelChange(info);

}