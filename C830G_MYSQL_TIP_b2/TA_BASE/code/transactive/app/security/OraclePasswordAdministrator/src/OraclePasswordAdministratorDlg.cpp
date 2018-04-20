// OraclePasswordAdministratorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OraclePasswordAdministrator.h"
#include "OraclePasswordAdministratorDlg.h"
#include "DataBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAX_LENGTH 1024
static BOOL FirstTimeConnect = TRUE;
const static CString CONNECTION_FILE = ".//connectInfo.ini";
const static CString CONNECT_SECTION = "ISCS-ORACLE";
const static CString DEFAULT_PORT_VALUE = "1521";

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
// COraclePasswordAdministratorDlg dialog

COraclePasswordAdministratorDlg::COraclePasswordAdministratorDlg(CUpdateDatabaseDlg* updateDlg, CWnd* pParent /*=NULL*/)
	: CDialog(COraclePasswordAdministratorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COraclePasswordAdministratorDlg)
	m_passwordValue = _T("");
	m_usernameValue = _T("");
	m_servernameValue = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_updateDlg = updateDlg;
}

void COraclePasswordAdministratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COraclePasswordAdministratorDlg)
	DDX_Control(pDX, IDC_STATIC_SERVER_NAME, m_servernameStatic);
	DDX_Control(pDX, IDC_EDIT_SERVER_NAME, m_servernameEdit);
	DDX_Control(pDX, IDCANCEL, m_cancelBtn);
	DDX_Control(pDX, IDC_BTN_CLEAR, m_clearBtn);
	DDX_Control(pDX, IDC_STATIC_PASSWORD, m_passwordStatic);
	DDX_Control(pDX, IDC_STATIC_USERNAME, m_usernameStatic);
	DDX_Control(pDX, IDC_STATIC_CONNECT, m_connectGroupBox);
	DDX_Control(pDX, IDC_STATIC_LOGO, m_logoPicture);
	DDX_Control(pDX, IDOK, m_okBtn);
	DDX_Control(pDX, IDC_EDIT_USERNAME, m_usernameEdit);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_passwordEdit);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_passwordValue);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_usernameValue);
	DDX_Text(pDX, IDC_EDIT_SERVER_NAME, m_servernameValue);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(COraclePasswordAdministratorDlg, CDialog)
	//{{AFX_MSG_MAP(COraclePasswordAdministratorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CLEAR, OnBtnClear)
	ON_EN_CHANGE(IDC_EDIT_USERNAME, OnChangeEditUsername)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, OnChangeEditPassword)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_SERVER_NAME, OnChangeEditServerName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COraclePasswordAdministratorDlg message handlers

BOOL COraclePasswordAdministratorDlg::OnInitDialog()
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

	fixedPosition();

	// TODO: Add extra initialization here
	initConnectionInfo();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void COraclePasswordAdministratorDlg::fixedPosition()
{
	//SetWindowPos(NULL,0,0,505,365,SWP_NOMOVE);

	CRect picRect;
	m_logoPicture.GetWindowRect(picRect);

	CRect dlgRect;
	GetWindowRect(dlgRect);

	//dlgRect.right = picRect.right + 5;
	//MoveWindow(dlgRect);

	picRect.top = dlgRect.top;
	picRect.bottom = picRect.top + 70;
	picRect.left = dlgRect.left;
	picRect.right = dlgRect.right - 5;
	m_logoPicture.MoveWindow(picRect);

	CenterWindow();
	UpdateData(FALSE);
}

void COraclePasswordAdministratorDlg::initConnectionInfo()
{
	//char currentDir[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,currentDir);

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	
	hFind = FindFirstFile(".//connectInfo.ini", &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) 
	{	
		GetPrivateProfileString(CONNECT_SECTION,"ServerName",NULL,m_servernameValue.GetBuffer(MAX_LENGTH),MAX_LENGTH,CONNECTION_FILE);
		GetPrivateProfileString(CONNECT_SECTION,"Username",NULL,m_usernameValue.GetBuffer(MAX_LENGTH),MAX_LENGTH,CONNECTION_FILE);

		UpdateData(FALSE);
	} 
}

void COraclePasswordAdministratorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void COraclePasswordAdministratorDlg::OnPaint() 
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
HCURSOR COraclePasswordAdministratorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void COraclePasswordAdministratorDlg::OnBtnClear() 
{
	// TODO: Add your control notification handler code here
	m_servernameEdit.SetWindowText(_T(""));
	m_usernameEdit.SetWindowText(_T(""));
	m_passwordEdit.SetWindowText(_T(""));

	UpdateData(false);
}

void COraclePasswordAdministratorDlg::OnOK() 
{
	// TODO: Add extra validation here
	HRESULT hr;
	
	try
	{			
		DataBase db(m_usernameValue, m_passwordValue, m_servernameValue);
		hr = db._DB_Connect();
		if( hr != OCI_SUCCESS )
		{
			AfxMessageBox("Connect to database failed.");
			exit(0);
		}
		else
		{
			//AfxMessageBox("Connect to database successful.");
			
			::WritePrivateProfileString(CONNECT_SECTION,"ServerName",m_servernameValue,CONNECTION_FILE);
			::WritePrivateProfileString(CONNECT_SECTION,"Username",m_usernameValue,CONNECTION_FILE);

			if (m_updateDlg != NULL)
			{
				m_updateDlg->m_servername = m_servernameValue;
				m_updateDlg->m_username = m_usernameValue;
				m_updateDlg->m_password = m_passwordValue;
			}
			
		}

	}
	catch(exception e)
	{
		CString errMsg = e.what();
		AfxMessageBox(errMsg);
		exit(0);
	}

	CDialog::OnOK();
}

BOOL COraclePasswordAdministratorDlg::EnableOkButton(EDITCONTROL editControl)
{
	switch (editControl)
	{
	case SERVERNAME:
		m_servernameEdit.GetWindowText(m_servernameValue);

	case USERNAME:
		m_usernameEdit.GetWindowText(m_usernameValue);
		break;

	case PASSWORD:
		if (FirstTimeConnect == TRUE)
		{
			m_servernameEdit.GetWindowText(m_servernameValue);
			m_usernameEdit.GetWindowText(m_usernameValue);
		}
		m_passwordEdit.GetWindowText(m_passwordValue);

		FirstTimeConnect = FALSE;
		break;

	default:
		AfxMessageBox("Ok Button enable/disable only rely on servername/port/schema/username/password edit control", MB_OK);
		break;
	}
	
	if ( m_servernameValue.IsEmpty() == TRUE
		|| m_usernameValue.IsEmpty() == TRUE
		|| m_passwordValue.IsEmpty() == TRUE)
	{
		return FALSE;
	} 
	else
	{
		return TRUE;
	}
}

void COraclePasswordAdministratorDlg::OnChangeEditServerName() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_okBtn.EnableWindow(EnableOkButton(SERVERNAME));
}


void COraclePasswordAdministratorDlg::OnChangeEditUsername() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_okBtn.EnableWindow(EnableOkButton(USERNAME));
}

void COraclePasswordAdministratorDlg::OnChangeEditPassword() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	m_okBtn.EnableWindow(EnableOkButton(PASSWORD));
}

void COraclePasswordAdministratorDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	//::CoUninitialize();

	CDialog::OnClose();
}

HBRUSH COraclePasswordAdministratorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_CONNECT)
	{
		pDC->SetTextColor(RGB(0, 0, 255));
     }
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

