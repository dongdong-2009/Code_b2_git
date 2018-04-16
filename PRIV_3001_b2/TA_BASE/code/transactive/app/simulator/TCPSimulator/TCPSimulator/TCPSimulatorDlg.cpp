// TCPSimulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TCPSimulator.h"
#include "TCPSimulatorDlg.h"
#include "TCPServer.h"
#include "TCPClient.h"
#include "ATSMessageHandler.h"
#include "ATSC95MessageHandler.h"
#include "PointDataTable.h"
#include "SimulatorConfig.h"

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


// CTCPSimulatorDlg dialog




CTCPSimulatorDlg::CTCPSimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTCPSimulatorDlg::IDD, pParent),m_TCPServer(NULL),m_TCPClient(NULL),b_TimerSendRunning(false),b_svrTimerSendRunning(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTCPSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LT_SERVER_INFO, m_LT_ServerInfo);
	DDX_Control(pDX, IDC_LT_CLIENT_INFO, m_LT_ClientInfo);
}

BEGIN_MESSAGE_MAP(CTCPSimulatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_MESSAGE(WM_SERVER_EVENT,OnServerEvent)
	ON_MESSAGE(WM_CLIENT_EVENT,OnClientEvent)
	ON_MESSAGE(WM_SERVER_NOTE,OnServerNote)
	ON_MESSAGE(WM_CLIENT_NOTE,OnClientNote)
	ON_BN_CLICKED(IDC_BT_STARTSERVER, &CTCPSimulatorDlg::OnBnClickedBtStartserver)
	ON_BN_CLICKED(IDC_BT_GETPOINTTABLE, &CTCPSimulatorDlg::OnBnClickedBtGetpointtable)
	ON_BN_CLICKED(IDC_BT_CONNECTSERVER, &CTCPSimulatorDlg::OnBnClickedBtConnectserver)
	ON_BN_CLICKED(IDC_CH_INFOHEX, &CTCPSimulatorDlg::OnBnClickedChInfohex)
	ON_BN_CLICKED(IDC_CH_OUTPUTHEX, &CTCPSimulatorDlg::OnBnClickedChOutputhex)
	ON_BN_CLICKED(IDC_BT_SEND, &CTCPSimulatorDlg::OnBnClickedBtSend)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BT_CLEAR_SERVERINFO, &CTCPSimulatorDlg::OnBnClickedBtClearServerinfo)
	ON_BN_CLICKED(IDC_BT_CLEAR_CLIENTINFO, &CTCPSimulatorDlg::OnBnClickedBtClearClientinfo)
	ON_BN_CLICKED(IDC_BT_SVR_SEND, &CTCPSimulatorDlg::OnBnClickedBtSvrSend)
	ON_NOTIFY(NM_DBLCLK, IDC_LT_SERVER_INFO, &CTCPSimulatorDlg::OnNMDblclkLtServerInfo)
	ON_NOTIFY(NM_DBLCLK, IDC_LT_CLIENT_INFO, &CTCPSimulatorDlg::OnNMDblclkLtClientInfo)
	ON_BN_CLICKED(IDC_CH_SERVER_CRC, &CTCPSimulatorDlg::OnBnClickedChServerCrc)
	ON_BN_CLICKED(IDC_CH_CLIENT_CRC, &CTCPSimulatorDlg::OnBnClickedChClientCrc)
END_MESSAGE_MAP()


// CTCPSimulatorDlg message handlers

BOOL CTCPSimulatorDlg::OnInitDialog()
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
	ReadConfig();
	((CButton*)GetDlgItem(IDC_CH_INFOHEX))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_CH_OUTPUTHEX))->SetCheck(TRUE);
	m_LT_ServerInfo.InsertColumn(0,L"Time",LVCFMT_LEFT,130);
	m_LT_ServerInfo.InsertColumn(1,L"Title",LVCFMT_LEFT,80);
	m_LT_ServerInfo.InsertColumn(2,L"Message",LVCFMT_LEFT,460);

	m_LT_ClientInfo.InsertColumn(0,L"Time",LVCFMT_LEFT,130);
	m_LT_ClientInfo.InsertColumn(1,L"Title",LVCFMT_LEFT,80);
	m_LT_ClientInfo.InsertColumn(2,L"Message",LVCFMT_LEFT,460);
	if(SimulatorConfig::getInstance()->m_bIsUDP)
	{
		GetDlgItem(IDC_BT_CONNECTSERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_SEND)->EnableWindow(TRUE) ;
		GetDlgItem(IDC_BT_SVR_SEND)->EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTCPSimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTCPSimulatorDlg::OnPaint()
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
HCURSOR CTCPSimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTCPSimulatorDlg::ReadConfig(bool bUpdateConnectIP)
{
	//m_config.ReadConfig();
	SimulatorConfig* config = SimulatorConfig::getInstance();
	WCHAR wbuf[256];
	
	if(config->m_ip!="")
	{
		memset(wbuf,0,sizeof(wbuf));
		MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,config->m_ip.c_str(),config->m_ip.length(),wbuf,sizeof(wbuf));
		SetDlgItemText(IDC_ED_IP,wbuf);
		SetDlgItemInt(IDC_ED_PORT,config->m_port);
	}
	if(bUpdateConnectIP && config->m_connectIp!="")
	{
		memset(wbuf,0,sizeof(wbuf));
		MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,config->m_connectIp.c_str(),config->m_connectIp.length(),wbuf,sizeof(wbuf));
		SetDlgItemText(IDC_ED_CONNECT_IP,wbuf);
		SetDlgItemInt(IDC_ED_CONNECT_PORT,config->m_connectPort);
	}
	if(config->m_clientCommandFile!="")
	{
		m_clientCommandList.clear();
		m_currentCommandIndex = 0;
		FILE * cmdFile = fopen(config->m_clientCommandFile.c_str(),"rt");
		if(NULL==cmdFile)
		{
			AfxMessageBox(L"Can not open client command file");
		}
		else
		{
			char cmdLine[1024];
			int index=0;
			while(true)
			{
				memset(cmdLine,0,sizeof(cmdLine));
				if(NULL==fgets(cmdLine,sizeof(cmdLine),cmdFile))
					break;
				m_clientCommandList[index]=string(cmdLine);
				index++;
			}
		}
	}
	if(config->m_serverCommandFile!="")
	{
		m_serverCommandList.clear();
		m_currentSvrCommandIndex = 0;
		FILE * cmdFile = fopen(config->m_serverCommandFile.c_str(),"rt");
		if(NULL==cmdFile)
		{
			AfxMessageBox(L"Can not open server command file");
		}
		else
		{
			char cmdLine[1024];
			int index=0;
			while(true)
			{
				memset(cmdLine,0,sizeof(cmdLine));
				if(NULL==fgets(cmdLine,sizeof(cmdLine),cmdFile))
					break;
				m_serverCommandList[index]=string(cmdLine);
				index++;
			}
		}
	}

	memset(wbuf,0,sizeof(wbuf));
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,config->m_typeMsgHandler.c_str(),config->m_typeMsgHandler.length(),wbuf,sizeof(wbuf));
	SetDlgItemText(IDC_ST_HANDLER,wbuf);
	CString strWndText;
	strWndText.Format(L"%s-Simulator",wbuf);
	SetWindowText(strWndText);
}

bool CTCPSimulatorDlg::StartServer()
{
	if(m_TCPServer==NULL)
		m_TCPServer = new TCPServer(this->m_hWnd);
	
	m_TCPServer->SetHex( static_cast<bool>(((CButton*)GetDlgItem(IDC_CH_INFOHEX))->GetCheck() ));
	return m_TCPServer->StartServer(SimulatorConfig::getInstance()->m_ip ,SimulatorConfig::getInstance()->m_port,SimulatorConfig::getInstance()->m_typeMsgHandler);

}

void CTCPSimulatorDlg::StopServer()
{
	m_TCPServer->StopServer();
}

void CTCPSimulatorDlg::ServerSendMsg()
{
	CString strInput;
	GetDlgItemText(IDC_ED_SVR_OUTPUT,strInput);
	char cBuf[1024*60];
	memset(cBuf,0,sizeof(cBuf));
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,strInput.GetString(),wcslen(strInput.GetString()),cBuf,sizeof(cBuf),NULL,NULL);
	m_TCPServer->Send(string(cBuf));
}

bool CTCPSimulatorDlg::ConnectServer()
{
	if(m_TCPClient==NULL)
		m_TCPClient = new TCPClient(this->m_hWnd);
	CString cstrConnectIp;
	int    connectPort;
	char cBuf[512];
	memset(cBuf,0,sizeof(cBuf));
	GetDlgItemText(IDC_ED_CONNECT_IP,cstrConnectIp);
	connectPort=GetDlgItemInt(IDC_ED_CONNECT_PORT);
	m_TCPClient->SetHex( ((CButton*)GetDlgItem(IDC_CH_OUTPUTHEX))->GetCheck() );
	//wcscpy(wcBuf,cstrConnectIp.GetString());
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,cstrConnectIp.GetString(),wcslen(cstrConnectIp.GetString()),cBuf,sizeof(cBuf),NULL,NULL);
	return m_TCPClient->Connect(string(cBuf) ,connectPort);
}

void CTCPSimulatorDlg::DisconnectServer()
{
	m_TCPClient->StopConnect();
	if( b_TimerSendRunning)
	{
		OnBnClickedBtSend();
	}
}

void CTCPSimulatorDlg::SendMsg()
{
	if(m_TCPClient==NULL)
	{
		if(SimulatorConfig::getInstance()->m_bIsUDP)
			m_TCPClient = new TCPClient(this->m_hWnd);
		else
			return;
	}
	CString cstrConnectIp;
	int    connectPort;
	char cBuf[512];
	memset(cBuf,0,sizeof(cBuf));
	GetDlgItemText(IDC_ED_CONNECT_IP,cstrConnectIp);
	connectPort=GetDlgItemInt(IDC_ED_CONNECT_PORT);
	m_TCPClient->SetHex( ((CButton*)GetDlgItem(IDC_CH_OUTPUTHEX))->GetCheck() );
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,cstrConnectIp.GetString(),wcslen(cstrConnectIp.GetString()),cBuf,sizeof(cBuf),NULL,NULL);
	//return m_TCPClient->Connect(string(cBuf) ,connectPort);

	CString cstrOutput;
	char    chOutput[1024*60];
	memset(chOutput,0,sizeof(chOutput));
	GetDlgItemText(IDC_ED_OUTPUT,cstrOutput);
	cstrOutput.Replace('#','\n');
	WideCharToMultiByte(CP_ACP,WC_COMPOSITECHECK,cstrOutput.GetString(),wcslen(cstrOutput.GetString()),chOutput,sizeof(chOutput),NULL,NULL);
	DeleteFile(L"TCPInputRecord");

	m_TCPClient->Send(string(chOutput),string(cBuf),connectPort);
}

void CTCPSimulatorDlg::RefreshPointTable()
{
	if(SimulatorConfig::getInstance()->m_typeMsgHandler != "ATS" &&
		SimulatorConfig::getInstance()->m_typeMsgHandler != "ATSC95")
		return;
	PointDataTable dt;
	string strData = dt.getPointData(SimulatorConfig::getInstance()->m_pointTableFileName);
	if(strData == "")
		return;
	if(SimulatorConfig::getInstance()->m_typeMsgHandler == "ATS")
	{
		ATSMessageHandler* msgHandler = dynamic_cast<ATSMessageHandler*>(m_TCPServer->getMessageHandler());
		msgHandler->setDataPointTable(strData);
	}
	else if(SimulatorConfig::getInstance()->m_typeMsgHandler == "ATSC95")
	{
		ATSC95MessageHandler* msgHandler = dynamic_cast<ATSC95MessageHandler*>(m_TCPServer->getMessageHandler());
		msgHandler->setDataPointTable(strData);
	}
}

void CTCPSimulatorDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if(m_TCPServer!=NULL)
	{
		if(m_TCPServer->IsUDPLooping())
		{
			AfxMessageBox(L"Please stop server at first");
			return;
		}
		delete m_TCPServer;
		m_TCPServer = NULL;
	}
	if(m_TCPClient!=NULL)
	{
		delete m_TCPClient;
		m_TCPClient = NULL;
	}
	SimulatorConfig::clearInstance();
	CDialog::OnClose();
}

LRESULT CTCPSimulatorDlg::OnServerEvent(WPARAM wParam,LPARAM lParam)
{
	if(WSAGETSELECTEVENT(lParam) == FD_CLOSE)
	{
		GetDlgItem(IDC_BT_SVR_SEND)->EnableWindow(FALSE);
	//	SetDlgItemText(IDC_BT_STARTSERVER,L"Start Server");
		if( b_svrTimerSendRunning)
		{
			OnBnClickedBtSvrSend();
		}
	}
	return m_TCPServer->OnSocketEvent(wParam,lParam);
}

LRESULT CTCPSimulatorDlg::OnClientEvent(WPARAM wParam,LPARAM lParam)
{
	if(WSAGETSELECTEVENT(lParam) == FD_CLOSE)
	{
		GetDlgItem(IDC_BT_SEND)->EnableWindow(FALSE);
		SetDlgItemText(IDC_BT_CONNECTSERVER,L"Connect");
		if( b_TimerSendRunning)
		{
			OnBnClickedBtSend();
		}
	}
	return m_TCPClient->OnSocketEvent(wParam,lParam);
}

LRESULT CTCPSimulatorDlg::OnServerNote(WPARAM wParam,LPARAM lParam)
{
	string* caption = (string*)wParam;
	string* message = (string*)lParam;
	SYSTEMTIME now;
	GetLocalTime(&now);
	CString strTime;
	WCHAR wcItem[1024*60];
	int len = message->length();
	strTime.Format(L"%d-%d-%d %d:%d:%d.%d",now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,now.wMilliseconds);
	m_LT_ServerInfo.InsertItem(0,L"");
	m_LT_ServerInfo.SetItemText(0,0,strTime);
	memset(wcItem,0,sizeof(wcItem));
	
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,caption->c_str() ,caption->length(),wcItem,caption->length());
	CString strMsg(wcItem);
	m_LT_ServerInfo.SetItemText(0,1,wcItem);
	memset(wcItem,0,sizeof(wcItem));
	int wLen = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,message->c_str(),message->length(),NULL,0);
	MultiByteToWideChar(CP_ACP,MB_COMPOSITE,message->c_str(),message->length(),wcItem,message->length());
	wcItem[wLen]=0x0;
	m_LT_ServerInfo.SetItemText(0,2,wcItem);
	return 0l;
}

LRESULT CTCPSimulatorDlg::OnClientNote(WPARAM wParam,LPARAM lParam)
{
	string* caption = (string*)wParam;
	string* message = (string*)lParam;
	SYSTEMTIME now;
	GetLocalTime(&now);
	CString strTime;
	WCHAR wcItem[1024*60];
	strTime.Format(L"%d-%d-%d %d:%d:%d.%d",now.wYear,now.wMonth,now.wDay,now.wHour,now.wMinute,now.wSecond,now.wMilliseconds);
	m_LT_ClientInfo.InsertItem(0,L"");
	m_LT_ClientInfo.SetItemText(0,0,strTime);
	memset(wcItem,0,sizeof(wcItem));
	long len = caption->length();
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,caption->c_str(),caption->length(),wcItem,caption->length());
	m_LT_ClientInfo.SetItemText(0,1,wcItem);
	memset(wcItem,0,sizeof(wcItem));
	int wLen = MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,message->c_str(),message->length(),NULL,0);
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,message->c_str(),message->length(),wcItem,wLen);
	wcItem[wLen]=0x0;
	m_LT_ClientInfo.SetItemText(0,2,wcItem);
	return 0l;
}

void CTCPSimulatorDlg::OnBnClickedBtStartserver()
{
	// TODO: Add your control notification handler code here
	CString strButton;
	GetDlgItemText(IDC_BT_STARTSERVER,strButton);
	if(strButton.Compare(L"Start Server")==0)
	{
		ReadConfig(false);
		
		if(StartServer())
		{
			GetDlgItem(IDC_BT_SVR_SEND)->EnableWindow(TRUE);
			SetDlgItemText(IDC_BT_STARTSERVER,L"Stop Server");
		}
		RefreshPointTable();
		if(SimulatorConfig::getInstance()->m_bIsUDP)
			m_TCPServer->UDPReadLoop();
		
	}
	else
	{
		StopServer();
		GetDlgItem(IDC_BT_SVR_SEND)->EnableWindow(FALSE);
		SetDlgItemText(IDC_BT_STARTSERVER,L"Start Server");
	}
}

void CTCPSimulatorDlg::OnBnClickedBtSvrSend()
{
	// TODO: 在此添加控件通知处理程序代码
	if( ((CButton*)GetDlgItem(IDC_CH_SVR_TIMER))->GetCheck())
	{
		CString strBTName;
		GetDlgItemText(IDC_BT_SVR_SEND,strBTName);
		if(strBTName.Compare(L"Send")==0)
		{
			int nElapse = GetDlgItemInt(IDC_ED_SVR_TIMER_SEND);
			if(nElapse<=0)
			{
				AfxMessageBox(L"Please set time interval at first");
				return;
			}
			SetTimer(TIMER_SERVER_SEND,nElapse,NULL);
			b_svrTimerSendRunning = true;
			SetDlgItemText(IDC_BT_SVR_SEND,L"Stop Send");
			GetDlgItem(IDC_CH_SVR_TIMER)->EnableWindow(FALSE);
		}
		else
		{	
			KillTimer(TIMER_SERVER_SEND);
			b_svrTimerSendRunning = false;
			SetDlgItemText(IDC_BT_SVR_SEND,L"Send");
			GetDlgItem(IDC_CH_SVR_TIMER)->EnableWindow(TRUE);
		}
	}
	else
		ServerSendMsg();
}

void CTCPSimulatorDlg::OnBnClickedBtGetpointtable()
{
	// TODO: Add your control notification handler code here
	SimulatorConfig* config = SimulatorConfig::getInstance();
	config->ReadExtConfig();
	m_TCPServer->getMessageHandler()->SetExtConfig();
	RefreshPointTable();
}

void CTCPSimulatorDlg::OnBnClickedBtConnectserver()
{
	// TODO: Add your control notification handler code here
	CString strButton;
	GetDlgItemText(IDC_BT_CONNECTSERVER,strButton);
	if(strButton.Compare(L"Connect")==0)
	{
		if(ConnectServer())
		{
			GetDlgItem(IDC_BT_SEND)->EnableWindow(TRUE) ;
			SetDlgItemText(IDC_BT_CONNECTSERVER,L"Disconnect");
		}
	}
	else
	{
		DisconnectServer();
		
		GetDlgItem(IDC_BT_SEND)->EnableWindow(FALSE);
		SetDlgItemText(IDC_BT_CONNECTSERVER,L"Connect");
	}
	
}

void CTCPSimulatorDlg::OnBnClickedChInfohex()
{
	// TODO: Add your control notification handler code here
	if(m_TCPServer!=NULL)
		m_TCPServer->SetHex( static_cast<bool>(((CButton*)GetDlgItem(IDC_CH_INFOHEX))->GetCheck() ));
}

void CTCPSimulatorDlg::OnBnClickedChOutputhex()
{
	// TODO: Add your control notification handler code here
	if(m_TCPClient!=NULL)
		m_TCPClient->SetHex( static_cast<bool>(((CButton*)GetDlgItem(IDC_CH_OUTPUTHEX))->GetCheck() ));
}

void CTCPSimulatorDlg::OnBnClickedBtSend()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_TCPClient->Send("GET news.google.com/?output=rss HTTP/1.1\nAccept-Charset: UTF-8\nHost:news.google.com\n\n");
	
	//return;

	if( ((CButton*)GetDlgItem(IDC_CH_TIMER))->GetCheck())
	{
		CString strBTName;
		GetDlgItemText(IDC_BT_SEND,strBTName);
		if(strBTName.Compare(L"Send")==0)
		{
			int nElapse = GetDlgItemInt(IDC_ED_TIMER_SEND);
			if(nElapse<=0)
			{
				AfxMessageBox(L"Please set time interval at first");
				return;
			}
			SetTimer(TIMER_CLIENT_SEND,nElapse,NULL);
			b_TimerSendRunning = true;
			SetDlgItemText(IDC_BT_SEND,L"Stop Send");
			GetDlgItem(IDC_CH_TIMER)->EnableWindow(FALSE);
		}
		else
		{	
			KillTimer(TIMER_CLIENT_SEND);
			b_TimerSendRunning = false;
			SetDlgItemText(IDC_BT_SEND,L"Send");
			GetDlgItem(IDC_CH_TIMER)->EnableWindow(TRUE);
		}
	}
	
	else
		SendMsg();
}

void CTCPSimulatorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch(nIDEvent)
	{
	case TIMER_CLIENT_SEND:
		if(!m_clientCommandList.empty())
		{
			WCHAR wbuf[2048];
			memset(wbuf,0,sizeof(wbuf));
			MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,m_clientCommandList[m_currentCommandIndex].c_str(),m_clientCommandList[m_currentCommandIndex].length(),wbuf,sizeof(wbuf));
			SetDlgItemText(IDC_ED_OUTPUT,wbuf);
			m_currentCommandIndex++;
			if(m_currentCommandIndex>=m_clientCommandList.size())
				m_currentCommandIndex=0;
		}
		SendMsg();
		break;
	case TIMER_SERVER_SEND:
		if(!m_serverCommandList.empty())
		{
			WCHAR wbuf[2048];
			memset(wbuf,0,sizeof(wbuf));
			MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,m_serverCommandList[m_currentSvrCommandIndex].c_str(),m_serverCommandList[m_currentSvrCommandIndex].length(),wbuf,sizeof(wbuf));
			SetDlgItemText(IDC_ED_SVR_OUTPUT,wbuf);
			m_currentSvrCommandIndex++;
			if(m_currentSvrCommandIndex>=m_serverCommandList.size())
				m_currentSvrCommandIndex=0;
		}
		ServerSendMsg();
		break;
	default:
		break;
	}
	CDialog::OnTimer(nIDEvent);
}

void CTCPSimulatorDlg::OnBnClickedBtClearServerinfo()
{
	// TODO: 在此添加控件通知处理程序代码
	while(m_LT_ServerInfo.GetItemCount()>0)
		m_LT_ServerInfo.DeleteItem(0);
}

void CTCPSimulatorDlg::OnBnClickedBtClearClientinfo()
{
	// TODO: 在此添加控件通知处理程序代码
	while(m_LT_ClientInfo.GetItemCount()>0)
		m_LT_ClientInfo.DeleteItem(0);
}

void CTCPSimulatorDlg::OnNMDblclkLtServerInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int nItem = m_LT_ServerInfo.GetHotItem();
	if(-1 == nItem)
		return;
	CString strText = m_LT_ServerInfo.GetItemText(nItem,2);
	m_DlgWholeText.ShowWholeText(strText);
	*pResult = 0;
}

void CTCPSimulatorDlg::OnNMDblclkLtClientInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int nItem = m_LT_ClientInfo.GetHotItem();
	if(-1 == nItem)
		return;
	CString strText = m_LT_ClientInfo.GetItemText(nItem,2);
	m_DlgWholeText.ShowWholeText(strText);
	*pResult = 0;
}

void CTCPSimulatorDlg::OnBnClickedChServerCrc()
{
	// TODO: Add your control notification handler code here
	if(m_TCPServer!=NULL)
		m_TCPServer->SetCRC( static_cast<bool>(((CButton*)GetDlgItem(IDC_CH_SERVER_CRC))->GetCheck() ));
}

void CTCPSimulatorDlg::OnBnClickedChClientCrc()
{
	// TODO: Add your control notification handler code here
	if(m_TCPClient!=NULL)
		m_TCPClient->SetCRC( static_cast<bool>(((CButton*)GetDlgItem(IDC_CH_CLIENT_CRC))->GetCheck() ));
}
