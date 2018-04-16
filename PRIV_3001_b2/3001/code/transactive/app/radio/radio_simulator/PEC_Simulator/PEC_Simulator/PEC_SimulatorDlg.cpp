// PEC_SimulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PEC_Simulator.h"
#include "PEC_SimulatorDlg.h"
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


// CPEC_SimulatorDlg dialog




CPEC_SimulatorDlg::CPEC_SimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPEC_SimulatorDlg::IDD, pParent)
	,m_strTrainNumber("")
	,m_strCarNumber("")
	,m_strPECNumber("")
	,m_strPECValue("")
	,m_strPECSource("")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPEC_SimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PEC_TRAIN_NUMBER, m_comboTrainNumber);
	DDX_Control(pDX, IDC_COMBO_PEC_CAR_NUMBER, m_comboCarNumber);
	DDX_Control(pDX, IDC_COMBO_PEC_NUMBER, m_PECNumber);
	DDX_Control(pDX, IDC_COMBO_PEC_VALUE, m_PECValue);
	DDX_Control(pDX, IDC_COMBO_PEC_SOURCE, m_PECSource);
}

BEGIN_MESSAGE_MAP(CPEC_SimulatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEND_COMMAND, &CPEC_SimulatorDlg::OnBnClickedButtonSendCommand)
END_MESSAGE_MAP()


// CPEC_SimulatorDlg message handlers

BOOL CPEC_SimulatorDlg::OnInitDialog()
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
	m_comboTrainNumber.SetCurSel(0);
	m_comboCarNumber.SetCurSel(0);
	m_PECNumber.SetCurSel(0);
	m_PECValue.SetCurSel(0);
	m_PECSource.SetCurSel(0);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPEC_SimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPEC_SimulatorDlg::OnPaint()
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
HCURSOR CPEC_SimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CPEC_SimulatorDlg::OnBnClickedButtonSendCommand()
{
	// TODO: Add your control notification handler code here
	
	//get PEC Information
	getPECInformation();

	//Find the HWnd of TrainSimulator
	HWND hTargetWnd = ::FindWindow(NULL,L"TrainSimulator");

	if ( NULL == hTargetWnd)
	{
		::MessageBox(NULL,L"Unable to find \"TrainSimulator\" window. \n Make sure that the TrainSimulator is running.",L"Error",MB_ICONERROR);
		return;
	}

	std::string dataSend =  m_strTrainNumber+" "+m_strCarNumber+" "+m_strPECNumber+" "+m_strPECValue+" "+m_strPECSource ;

	COPYDATASTRUCT cpd;
	cpd.dwData = 0;
	cpd.cbData = 0;
	cpd.cbData = dataSend.size()+1;
	cpd.lpData = (void*)dataSend.c_str();

	::SendMessage(hTargetWnd,WM_COPYDATA,NULL,reinterpret_cast<LPARAM>(&cpd));
		
}

std::string CPEC_SimulatorDlg::convertToString(const CString &str)
{
	CT2A _convertedString(str);
	std::string _ret(_convertedString);

	return _ret;
}

void CPEC_SimulatorDlg::getPECInformation()
{
	short index;
	CString _trainNumber;

	//Train Number
	index = m_comboTrainNumber.GetCurSel();
	m_comboTrainNumber.GetLBText(index,_trainNumber);

	m_strTrainNumber = convertToString(_trainNumber);

	//Car Number
	CString _carNumber;
	index = m_comboCarNumber.GetCurSel();
	m_comboCarNumber.GetLBText(index,_carNumber);

	m_strCarNumber = convertToString(_carNumber);

	//PEC Number
	CString _pecNumber;
	index = m_PECNumber.GetCurSel();
	m_PECNumber.GetLBText(index,_pecNumber);

	m_strPECNumber = convertToString(_pecNumber);

	//PEC Value
	CString _value;
	index = m_PECValue.GetCurSel();
	m_PECValue.GetLBText(index,_value);

	m_strPECValue = convertToString(_value);

	//PEC Source
	CString _pecSource;
	index = m_PECSource.GetCurSel();
	m_PECSource.GetLBText(index,_pecSource);

	m_strPECSource = convertToString(_pecSource);

}