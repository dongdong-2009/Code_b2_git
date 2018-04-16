// PDS ClientSimDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PDS ClientSim.h"
#include "PDS ClientSimDlg.h"

#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

long CPDSClientSimDlg::m_lStatFileCounter = 0;;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPDSClientSimDlg dialog

CPDSClientSimDlg::CPDSClientSimDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPDSClientSimDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPDSClientSimDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_dwDlgThreadID = GetCurrentThreadId();
	m_paramReader = new TA_IRS_App::CParamReader;
	m_nCounterFrame = 0;
	m_bCheckSumOveride = false;

	m_windowsMessage = FALSE;
	m_trainID = 0;
	
	
}

CPDSClientSimDlg::~CPDSClientSimDlg()
{
	if (m_paramReader)
		delete m_paramReader;

	if (m_socketManager)
	{
		m_socketManager->terminateAndWait();
		delete m_socketManager;
	}
}

void CPDSClientSimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPDSClientSimDlg)
	DDX_Control(pDX, IDC_LST_SENDEVENT, m_lstSendEvent);
	DDX_Control(pDX, IDC_LST_RECV_DATA, m_lstRecvData);
	DDX_Control(pDX, IDC_CMBO_TrainID, m_cmboTrainID);
	DDX_Control(pDX, IDC_EDT_PORT, m_edtPORT);
	DDX_Control(pDX, IDC_EDT_IP, m_edtIP);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPDSClientSimDlg, CDialog)
	//{{AFX_MSG_MAP(CPDSClientSimDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnBeginDownload)
	ON_BN_CLICKED(IDC_BTNREPEAT, OnBtnrepeat)
	ON_CBN_SELCHANGE(IDC_CMBO_TrainID, OnSelchangeCMBOTrainID)
	ON_LBN_DBLCLK(IDC_LST_SENDEVENT, OnDblclkLstSendevent)
	ON_LBN_DBLCLK(IDC_LST_RECV_DATA, OnDblclkLstRecvData)
	ON_BN_CLICKED(IDC_BTN_CONF, OnBtnConf)
	ON_WM_COPYDATA()
	//}}AFX_MSG_MAP
	ON_MESSAGE (WM_UIUPDATE,OnUIUpdate)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPDSClientSimDlg message handlers

BOOL CPDSClientSimDlg::OnInitDialog()
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
	CString strIP = m_paramReader->getValueFromSection(SECTIONNAME_IP).c_str();
	CString strPort = m_paramReader->getValueFromSection(SECTIONNAME_PORT).c_str();

	m_edtIP.SetWindowText(strIP);
	m_edtPORT.SetWindowText(strPort);
	
	m_socketManager = new TA_IRS_App::CUDPSocketManager (strIP.GetBuffer(0),strPort.GetBuffer(0),this,& m_sockClientInfo);
	
	//m_socketManager = new TA_IRS_App::CUDPSocketManager (strIP.GetBuffer(0),strPort.GetBuffer(0),this);
	m_frameRepeatDlg.setListener(this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPDSClientSimDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPDSClientSimDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPDSClientSimDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CPDSClientSimDlg::OnBeginDownload() 
{
	// TODO: Add your control notification handler code here
	
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];
	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	
	strftime (buffer,80,"%m%d%Y-%I%M%S",timeinfo);
	
	m_strFileName.Format("logs\\Begin-Train%s-%s.txt",m_strTrainID.GetBuffer(0),buffer);


	CString strTrainID;
	m_nCounterFrame = 0;
	m_vctrControlFrame.erase (m_vctrControlFrame.begin(),m_vctrControlFrame.end());

	int item = m_cmboTrainID.GetCurSel();
	CString text ("");
	
	if(item != CB_ERR)
	{
		m_cmboTrainID.GetLBText(item,text);
	}
	
	if (0 == text.Compare(""))
	{
		::MessageBox (NULL,"Cannot Proceed, Train ID is Empty","Info Missing",MB_OK);
		return;
	}
	
	char * pszData = new char [1024];
	m_edtIP.GetWindowText(pszData,MAX_PATH);
	
	
	m_vctrControlFrame.push_back(1);
	int nTrainID = 0;

	if ( m_windowsMessage )
	{
		nTrainID = m_trainID;
	}
	else
		nTrainID = atoi (text.GetBuffer(0));
	
	m_vctrControlFrame = m_frameHandler.GetControlFrame(m_vctrControlFrame,nTrainID,BEGIN);
	
	// check sum overide by providing invalid checksum on last two bytes,
	if (m_bCheckSumOveride)
	{
		std::vector <unsigned char>::iterator chkSumIter = m_vctrControlFrame.end();
		m_vctrControlFrame.erase(--chkSumIter);
		chkSumIter = m_vctrControlFrame.end();
		m_vctrControlFrame.erase(--chkSumIter);
	
		m_vctrControlFrame.push_back(0);
		m_vctrControlFrame.push_back(0);
	}

	std::basic_string <unsigned char> bscStrDataBuff;

	bscStrDataBuff.append(m_vctrControlFrame.begin(),m_vctrControlFrame.end());
	
	delete [] pszData;

	int nDataSize = m_vctrControlFrame.size();
	pszData = new char [nDataSize] ;
	
	memset (pszData,0,nDataSize);
	memcpy (pszData,bscStrDataBuff.c_str(),nDataSize);
	
	m_socketManager->writeData(pszData,nDataSize,(struct sockaddr *)&m_sockClientInfo);
	
	UIUpdate (m_vctrControlFrame,FRAME_SEND);

	delete [] pszData;
}

void CPDSClientSimDlg::connectionEstablish (::sockaddr* clientAddress, std::vector<unsigned char> socketData)
{
	// recieved data
	char * pszData = new char [1023];

	m_fileHandler = fopen (m_strFileName.GetBuffer(0),"a+");
	
	if (!m_fileHandler)
		return;

	int nSize = socketData.size();

	for (int nCounter = 0; nCounter < nSize; nCounter++)
	{	
		if (nCounter != 0)
			sprintf (pszData,"%s %d",pszData,socketData.at (nCounter));
		else
			sprintf (pszData,"%d",socketData.at (nCounter));
		
		if (nCounter > 9 && (nCounter != (nSize - 1) && nCounter != (nSize - 2)))
			fprintf(m_fileHandler,"%c",socketData.at (nCounter));
	}

	fclose (m_fileHandler);
	std::string strData = pszData;

	delete [] pszData;


	if (m_dwDlgThreadID != GetCurrentThreadId())
	{	
		PostMessage (WM_UIUPDATE,(WPARAM)new CString (strData.c_str()),(LPARAM)FRAME_RECV);
	}

	m_nCounterFrame++;
}

void CPDSClientSimDlg::notifyFrameRepeat (std::vector <long> vctrFrameRepeat)
{

	if (vctrFrameRepeat.empty())
		return;

	std::vector <unsigned char> vctrRepeatFrame;
	std::vector <long>::iterator frameIter;
	
	vctrRepeatFrame.push_back(vctrFrameRepeat.size());
	vctrRepeatFrame.push_back(0);
	
	for (int nCounter = 0; nCounter < vctrFrameRepeat.size(); nCounter++ )
	{
	//	CString strFormat;
	//	strFormat.Format("%d",vctrFrameRepeat.at(nCounter));
	//	::MessageBox (NULL,strFormat.GetBuffer(0),"Duplicated",MB_OK);
		
		vctrRepeatFrame.push_back(vctrFrameRepeat.at (nCounter));
	}

	int item = m_cmboTrainID.GetCurSel();
	CString text ("");
	
	if(item != CB_ERR)
	{
		m_cmboTrainID.GetLBText(item,text);
	}

	int nTrainID = atol (text.GetBuffer(0));
	

/*	for (nCounter = 0; nCounter < vctrRepeatFrame.size(); nCounter++ )
	{
		//CString strFormat;
		//strFormat.Format("%d",vctrRepeatFrame.at(nCounter));
		//::MessageBox (NULL,strFormat.GetBuffer(0),"Duplicated",MB_OK);
		
		//vctrRepeatFrame.push_back(vctrFrameRepeat.at (nCounter));
	}
*/
	m_vctrControlFrame = m_frameHandler.GetControlFrame(vctrRepeatFrame,nTrainID,REPEAT);

	for (int nCounter = 0; nCounter < m_vctrControlFrame.size (); nCounter++)
	{
		long lEntry = m_vctrControlFrame.at (nCounter);
	}
	
	UIUpdate(m_vctrControlFrame,FRAME_SEND);
	
	char * pszData = new char [1024];
	int nDataSize = m_vctrControlFrame.size();
	pszData = new char [nDataSize] ;
		

	std::basic_string <unsigned char> bscStrDataBuff;
	bscStrDataBuff.append(m_vctrControlFrame.begin(),m_vctrControlFrame.end());

	memset (pszData,0,nDataSize);
	memcpy (pszData,bscStrDataBuff.c_str(),nDataSize);
	m_socketManager->writeData(pszData,nDataSize,(struct sockaddr *)&m_sockClientInfo);

	
	m_nCounterFrame = 0;

}

LRESULT CPDSClientSimDlg::OnUIUpdate (WPARAM wParam,LPARAM lParam)
{

	long lFrameID = (long)lParam;
	CString * strData = (CString *)wParam;
	
	int nIndex = 0;

	switch (lFrameID)
	{
		case FRAME_RECV:
			nIndex = m_lstRecvData.AddString(* strData);
			m_lstRecvData.SetCurSel(nIndex);
			break;
		case FRAME_SEND:
			nIndex = m_lstSendEvent.AddString(* strData);
			m_lstSendEvent.SetCurSel(nIndex);
			break;
	}
	
	delete strData;

	return 0;
}

void CPDSClientSimDlg::UIUpdate (std::vector <unsigned char> vctrData, long lFrameID)
{
	char * pszData = new char [1023];
	
	for (int nCounter = 0; nCounter < vctrData.size(); nCounter++)
	{	
		if (nCounter != 0)
			sprintf (pszData,"%s %d",pszData,vctrData.at (nCounter));
		else
			sprintf (pszData,"%d",vctrData.at (nCounter));
	}
	
	int nIndex = 0;
	switch (lFrameID)
	{
	case FRAME_RECV:
		nIndex = m_lstRecvData.AddString(pszData);
		m_lstRecvData.SetCurSel(nIndex);
		break;
	case FRAME_SEND:
		nIndex = m_lstSendEvent.AddString(pszData);
		m_lstSendEvent.SetCurSel(nIndex);
		break;
	}

	delete [] pszData;
}


void CPDSClientSimDlg::OnBtnrepeat() 
{
	// TODO: Add your control notification handler code here
	//if (m_nCounterFrame != m_nTotalFrame)
	//	return;
	
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];
	
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	
	strftime (buffer,80,"%m%d%Y-%I%M%S",timeinfo);

	m_strFileName.Format("logs\\Repeat-Train%s-%s.txt",m_strTrainID.GetBuffer(0),buffer);

	m_frameRepeatDlg.SetMaxFrame(m_nCounterFrame);
	m_frameRepeatDlg.DoModal();

}	

void CPDSClientSimDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	char * pszIP = new char [1023];
	char * pszPort = new char [1023];
	m_edtIP.GetWindowText(pszIP,MAX_PATH);
	m_edtPORT.GetWindowText(pszPort,MAX_PATH);
	
	m_paramReader->setValueToSection(SECTIONNAME_IP,pszIP);
	m_paramReader->setValueToSection(SECTIONNAME_PORT,pszPort);
	
	delete [] pszIP;
	delete [] pszPort;

	CDialog::OnOK();
}

void CPDSClientSimDlg::OnSelchangeCMBOTrainID() 
{
	// TODO: Add your control notification handler code here
	int item = m_cmboTrainID.GetCurSel();
	CString text ("");
	
	if(item != CB_ERR)
	{
		m_cmboTrainID.GetLBText(item,text);
	}
	
	m_strTrainID = text;
	//m_strFileName.Format("Data-Train %s.txt",text.GetBuffer(0));

}	

void CPDSClientSimDlg::OnDblclkLstSendevent() 
{
	// TODO: Add your control notification handler code here
	char * pszData = new char [1023];

	m_lstSendEvent.GetText(m_lstSendEvent.GetCurSel(),pszData);
	
	CString strData = pszData;
	
	delete [] pszData;

	m_dataValidator.setDataFrame(strData);
	m_dataValidator.DoModal();

}

void CPDSClientSimDlg::OnDblclkLstRecvData() 
{
	// TODO: Add your control notification handler code here
	char * pszData = new char [1023];

	m_lstRecvData.GetText(m_lstRecvData.GetCurSel(),pszData);
	
	CString strData = pszData;
	
	delete [] pszData;
	
	m_dataValidator.setDataFrame(strData);
	m_dataValidator.DoModal();
}

void CPDSClientSimDlg::OnBtnConf() 
{
	// TODO: Add your control notification handler code here
	m_configDlg.attachListener(this);
	m_configDlg.DoModal();
}

void CPDSClientSimDlg::notifyServerSettingChange (std::string strIpAddress, std::string strServicePort)
{
	if (m_socketManager)
	{
		m_socketManager->terminateAndWait();
		delete m_socketManager;
	}

	m_socketManager = new TA_IRS_App::CUDPSocketManager (strIpAddress.c_str(),strServicePort.c_str(),this,& m_sockClientInfo);

	m_edtIP.SetWindowText(strIpAddress.c_str());
	m_edtPORT.SetWindowText(strServicePort.c_str());

}

void CPDSClientSimDlg::notifyCheckSumOverRide (const BOOL & bStatus)
{
	m_bCheckSumOveride = bStatus;
}

BOOL CPDSClientSimDlg::OnCopyData(CWnd *pWnd, COPYDATASTRUCT *pCopyDataStruct)
{
	::MessageBox(NULL,_T("Received Data"),_T("Message"), MB_ICONINFORMATION);
	m_windowsMessage = TRUE;
	m_trainID = (unsigned char)pCopyDataStruct->lpData;
	OnBeginDownload();
	return 0l;
}