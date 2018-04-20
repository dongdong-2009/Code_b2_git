/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/prototype/C830_Incident_Manager/C830_Incident_ManagerDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// C830_Incident_ManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "C830_Incident_Manager.h"
#include "C830_Incident_ManagerDlg.h"


#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NEW_COMMAND     (WM_USER+100)
#define VIEW_COMMAND    (WM_USER+101)
#define DELETE_COMMAND  (WM_USER+102)
#define PRINT_COMMAND   (WM_USER+103)
#define CLOSE_COMMAND   (WM_USER+104)

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
// CC830_Incident_ManagerDlg dialog

CC830_Incident_ManagerDlg::CC830_Incident_ManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CC830_Incident_ManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CC830_Incident_ManagerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CC830_Incident_ManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CC830_Incident_ManagerDlg)
	DDX_Control(pDX, IDC_LIST1, m_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CC830_Incident_ManagerDlg, CDialog)
	//{{AFX_MSG_MAP(CC830_Incident_ManagerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnDblclkList)
    ON_COMMAND(NEW_COMMAND, OnNew)
    ON_COMMAND(VIEW_COMMAND, OnView)
    ON_COMMAND(DELETE_COMMAND, OnDelete)
    ON_COMMAND(PRINT_COMMAND, OnPrint)
    ON_COMMAND(CLOSE_COMMAND, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CC830_Incident_ManagerDlg message handlers

BOOL CC830_Incident_ManagerDlg::OnInitDialog()
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
    CreateToolbar();
    setupList();
    putCrapInList();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CC830_Incident_ManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CC830_Incident_ManagerDlg::OnPaint() 
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
HCURSOR CC830_Incident_ManagerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CC830_Incident_ManagerDlg::CreateToolbar()
{
    // Set Toolbar properties
    CWinApp*    pApp = AfxGetApp();
    UINT        tbarStyle;

    tbarStyle = WS_VISIBLE | WS_CHILD;
    tbarStyle = tbarStyle | CCS_NODIVIDER;
    tbarStyle = tbarStyle | CCS_NOPARENTALIGN;
    tbarStyle = tbarStyle | CCS_NORESIZE;
    tbarStyle = tbarStyle | TBSTYLE_TOOLTIPS;
    tbarStyle = tbarStyle | TBSTYLE_FLAT;
    tbarStyle = tbarStyle | TBSTYLE_TRANSPARENT;

    m_imageList.Create(16,16, ILC_MASK, 10, 20);

    CRect tmpRect;
	CWnd::GetDlgItem(IDC_PAGER_CTRL)->GetWindowRect(tmpRect);
	CWnd::ScreenToClient(tmpRect);

	CWnd::GetDlgItem(IDC_PAGER_CTRL)->DestroyWindow();

    if (!m_toolbar.Create(tbarStyle, tmpRect, this, IDR_TOOLBAR1))
		AfxMessageBox("Failed to create toolbar.");

	const int FONT_SIZE = 100;
	const CString FONT = "Arial Bold";
 
    CFont font;
    font.CreatePointFont(FONT_SIZE,FONT,NULL);
    m_toolbar.SetFont(&font,FALSE);

	m_toolbar.SetButtonWidth(15, 85); // Was 75

	int buttonCount = m_toolbar.GetButtonCount();
	TBBUTTON		tbButton;

	for(int i(buttonCount - 1); i >= 0; --i)
	{
		m_toolbar.DeleteButton(i);
	}

    VERIFY((tbButton.iString = m_toolbar.AddStrings("New")) != -1);
    tbButton.iBitmap = m_imageList.Add(pApp->LoadIcon(IDI_NEW));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON;//| TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = NEW_COMMAND;
    VERIFY(m_toolbar.AddButtons(1, &tbButton));
	
	VERIFY((tbButton.iString = m_toolbar.AddStrings("View")) != -1);
    tbButton.iBitmap = m_imageList.Add(pApp->LoadIcon(IDI_VIEW));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON;// | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = VIEW_COMMAND;
    VERIFY(m_toolbar.AddButtons(1, &tbButton));

	VERIFY((tbButton.iString = m_toolbar.AddStrings("Delete")) != -1);
    tbButton.iBitmap = m_imageList.Add(pApp->LoadIcon(IDI_DELETE));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON;// | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = DELETE_COMMAND;
    VERIFY(m_toolbar.AddButtons(1, &tbButton));

    VERIFY((tbButton.iString = m_toolbar.AddStrings("Print")) != -1);
    tbButton.iBitmap = m_imageList.Add(pApp->LoadIcon(IDI_PRINT));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON; //| TBSTYLE_CHECK// | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = PRINT_COMMAND;
    VERIFY(m_toolbar.AddButtons(1, &tbButton));

	VERIFY((tbButton.iString = m_toolbar.AddStrings("Close")) != -1);
    tbButton.iBitmap = m_imageList.Add(pApp->LoadIcon(IDI_CLOSE));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON;// | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = CLOSE_COMMAND;
    VERIFY(m_toolbar.AddButtons(1, &tbButton));

    m_toolbar.SetImageList(&m_imageList);
    //m_toolbar.SetHotImageList(&m_alarmsTbImageListHot);
}


void CC830_Incident_ManagerDlg::setupList()
{
    m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

    m_list.InsertColumn(0, "ID");
    m_list.InsertColumn(1, "Created");
    m_list.InsertColumn(2, "Title");
    m_list.InsertColumn(3, "Location");
    m_list.InsertColumn(4, "Track");
    m_list.InsertColumn(5, "Train");
    m_list.InsertColumn(6, "Alarm");
    m_list.InsertColumn(7, "Asset");
    m_list.InsertColumn(8, "Subsystem");
    m_list.InsertColumn(9, "Creator");
    m_list.InsertColumn(10, "Profile");
    m_list.InsertColumn(11, "Closed");

    m_list.SetColumnWidth(0, 30);
    m_list.SetColumnWidth(1, 127);
    m_list.SetColumnWidth(2, 306);
    m_list.SetColumnWidth(3, 72);
    m_list.SetColumnWidth(4, 47);
    m_list.SetColumnWidth(5, 51);
    m_list.SetColumnWidth(6, 116);
    m_list.SetColumnWidth(7, 154);
    m_list.SetColumnWidth(8, 81);
    m_list.SetColumnWidth(9, 81);
    m_list.SetColumnWidth(10, 56);
    m_list.SetColumnWidth(11, 125);
}


void CC830_Incident_ManagerDlg::putCrapInList()
{
    m_list.InsertItem(0, "135");
    m_list.SetItemText(0, 1, "02-Jan-04 11:02:23");
    m_list.SetItemText(0, 2, "Driver of train 12 reported smoke in the tunnel");
    m_list.SetItemText(0, 3, "CVS-MLN");
    m_list.SetItemText(0, 4, "Outer");
    m_list.SetItemText(0, 5, "12/125");
    m_list.SetItemText(0, 6, "");
    m_list.SetItemText(0, 7, "");
    m_list.SetItemText(0, 8, "");
    m_list.SetItemText(0, 9, "Rod Marsh");
    m_list.SetItemText(0, 10, "SO");
    m_list.SetItemText(0, 11, "02-Jan-04 12:15:46");

    m_list.InsertItem(1, "134");
    m_list.SetItemText(1, 1, "08-Aug-04 15:32:25");
    m_list.SetItemText(1, 2, "Comms error with RTU");
    m_list.SetItemText(1, 3, "DBG");
    m_list.SetItemText(1, 4, "Inner");
    m_list.SetItemText(1, 5, "05/125");
    m_list.SetItemText(1, 6, "Service Port 3030: Comms Error");
    m_list.SetItemText(1, 7, "CDBG/ECS/TVS/TVF03");
    m_list.SetItemText(1, 8, "ECS");
    m_list.SetItemText(1, 9, "Rod Marsh");
    m_list.SetItemText(1, 10, "SO");
    m_list.SetItemText(1, 11, "");
}

void CC830_Incident_ManagerDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    std::stringstream widths;
    widths << "Column widths are: " << std::endl;
	widths << m_list.GetColumnWidth(0) << " ";
    widths << m_list.GetColumnWidth(1) << " ";
    widths << m_list.GetColumnWidth(2) << " ";
    widths << m_list.GetColumnWidth(3) << " ";
    widths << m_list.GetColumnWidth(4) << " ";
    widths << m_list.GetColumnWidth(5) << " ";
    widths << m_list.GetColumnWidth(6) << " ";
    widths << m_list.GetColumnWidth(7) << " ";
    widths << m_list.GetColumnWidth(8) << " ";
    widths << m_list.GetColumnWidth(9) << " ";
    widths << m_list.GetColumnWidth(10) << " ";
    widths << m_list.GetColumnWidth(11) << " ";

    AfxMessageBox(widths.str().c_str());
	
	*pResult = 0;
}


void CC830_Incident_ManagerDlg::OnNew()
{
    m_incidentSheet.DoModal();
}

void CC830_Incident_ManagerDlg::OnView()
{
    m_stationLogSheet.DoModal();
}

void CC830_Incident_ManagerDlg::OnDelete()
{
    m_incidentSheet.DoModal();
}

void CC830_Incident_ManagerDlg::OnPrint()
{
    m_incidentSheet.DoModal();
}

void CC830_Incident_ManagerDlg::OnClose()
{
    CDialog::OnCancel();
}
