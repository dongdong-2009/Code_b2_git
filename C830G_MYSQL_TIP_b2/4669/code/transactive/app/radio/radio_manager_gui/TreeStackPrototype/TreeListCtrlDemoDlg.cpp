/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/TreeStackPrototype/TreeListCtrlDemoDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// TreeListCtrlDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TreeListCtrlDemo.h"
#include "TreeListCtrlDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CTreeListCtrlDemoDlg dialog

CTreeListCtrlDemoDlg::CTreeListCtrlDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTreeListCtrlDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTreeListCtrlDemoDlg)
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTreeListCtrlDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTreeListCtrlDemoDlg)
	DDX_Control(pDX, IDC_TREE2, m_HeldTree);
	DDX_Control(pDX, IDC_TREE, m_IncomingTree);
	DDX_Control(pDX, IDC_TREE3, m_CurrentTree);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTreeListCtrlDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CTreeListCtrlDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeListCtrlDemoDlg message handlers

BOOL CTreeListCtrlDemoDlg::OnInitDialog()
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

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Setup the shared image list
	m_il.Create(IDB_FOLDERS, 16, 1, RGB(255, 0, 255));

	// Setup each of the trees (Stacks)
	SetupIncomingStack();
	SetupHeldStack();
	SetupCurrentStack();
	
	// Step one: Window creation (if not subclassed via DDX_Control).
	/*
	m_tree.Create
		(
		WS_BORDER | WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS, 
		CRect(12, 12, 288, 228), 
		this, 
		0x100
		);
	//*/

	// Step two: We insert two columns: Folder name, and folder size


	// Step four:	we insert the tree contents.
	//				The information in the arrays asFolders, asSizes and anLevels
	//				should be obtained as a result of a "process": selecting a
	//				group of records, checking the hard disk, etc.

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTreeListCtrlDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTreeListCtrlDemoDlg::OnPaint() 
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

HCURSOR CTreeListCtrlDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}



void CTreeListCtrlDemoDlg::SetupIncomingStack()
{
	LVCOLUMN	column;

	// Use file row select
	m_IncomingTree.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	// Setup the required column format
	column.mask		= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	column.fmt		= LVCFMT_LEFT;

	// Create the required columns
	column.cx		= 170;
	column.iSubItem	= 0;
	column.pszText	= _T("Time");
	m_IncomingTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 1;
	column.pszText	= _T("Caller");
	m_IncomingTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 2;
	column.pszText	= _T("Number");
	m_IncomingTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 3;
	column.pszText	= _T("Location");
	m_IncomingTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 4;
	column.pszText	= _T("Status");
	m_IncomingTree.InsertColumn(column.iSubItem, &column);

	// Associate image list with tree control 
	m_IncomingTree.SetImageList(&m_il, LVSIL_SMALL);

	// Add some test calls
	CString	asTimes[]     = { "18/07 12:05:34", "18/07 12:06:31" , "18/07 12:06:37", "18/07 12:06:37"};
	CString	asCalls[]     = { "NCH SO"        , "Train 33"       , "CONFERENCE"    , "CVC SO"};
	CString	asNumbers[]	  = { "123-123-123"   , "154-223-344"    , ""              , "213-345-322"};
	CString	asLocations[] = { "DBG"           , "BLV"            , ""              , ""    };
	CString	asStatus[]    = { "Ringing"       , "Ringing"      , "Disconnected"    , "Text"};
    int     iImage[]      = { 1               , 1                , 3               , 1};

	for (int i = 0; i < sizeof(asTimes) / sizeof(asTimes[0]); i++)
	{
		int iItem;
		iItem = m_IncomingTree.InsertItem(i,asTimes[i], iImage[i]);
		m_IncomingTree.SetItemText(iItem, 1, asCalls[i]);
		m_IncomingTree.SetItemText(iItem, 2, asNumbers[i]);
		m_IncomingTree.SetItemText(iItem, 3, asLocations[i]);
		m_IncomingTree.SetItemText(iItem, 4, asStatus[i]);
	}	
}

void CTreeListCtrlDemoDlg::SetupHeldStack()
{
	LVCOLUMN	column;

	// Use file row select
	m_HeldTree.SetExtendedStyle(LVS_EX_FULLROWSELECT);

	// Setup the required column format
	column.mask		= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	column.fmt		= LVCFMT_LEFT;

	// Create the required columns
	column.cx		= 170;
	column.iSubItem	= 0;
	column.pszText	= _T("Time");
	m_HeldTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 1;
	column.pszText	= _T("Caller");
	m_HeldTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 2;
	column.pszText	= _T("Number");
	m_HeldTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 3;
	column.pszText	= _T("Location");
	m_HeldTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 4;
	column.pszText	= _T("Status");
	m_HeldTree.InsertColumn(column.iSubItem, &column);

	// Associate image list with tree control 
	m_HeldTree.SetImageList(&m_il, LVSIL_SMALL);

	// Add some test calls
	CString	asTimes[]     = { "18/07 12:05:34", "18/07 12:06:31" };
	CString	asCalls[]     = { "DCC PSCO"      , "Train 22"       };
	CString	asNumbers[]	  = { "123-123-123"   , "154-223-312"    };
	CString	asLocations[] = { "DCC"           , "CVC"            };
	CString	asStatus[]    = { "Held"          , "Held"           };
    int     iImage[]      = { 1               , 1                };

	for (int i = 0; i < sizeof(asTimes) / sizeof(asTimes[0]); i++)
	{
		int iItem;
		iItem = m_HeldTree.InsertItem(i,asTimes[i], iImage[i]);
		m_HeldTree.SetItemText(iItem, 1, asCalls[i]);
		m_HeldTree.SetItemText(iItem, 2, asNumbers[i]);
		m_HeldTree.SetItemText(iItem, 3, asLocations[i]);
		m_HeldTree.SetItemText(iItem, 4, asStatus[i]);
	}	
}


/*
void CTreeListCtrlDemoDlg::SetupCurrentStack()
{
	LVCOLUMN	column;

	// Setup the required column format
	column.mask		= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	column.fmt		= LVCFMT_LEFT;

	// Create the required columns
	column.cx		= 170;
	column.iSubItem	= 0;
	column.pszText	= _T("Time");
	m_CurrentTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 1;
	column.pszText	= _T("Call");
	m_CurrentTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 2;
	column.pszText	= _T("Number");
	m_CurrentTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 3;
	column.pszText	= _T("Location");
	m_CurrentTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 4;
	column.pszText	= _T("Status");
	m_CurrentTree.InsertColumn(column.iSubItem, &column);

	// Associate image list with tree control 
	m_CurrentTree.SetImageList(&m_il, LVSIL_SMALL);

	// Add some test calls
	CString	asTimes[]     = { "18/07 12:02:30", "18/07 12:02:54" , "18/07 12:04:22"};
	CString	asCalls[]     = { "DBG SO"        , "Train 55"       , "MLN SO Talking"};
	CString	asNumbers[]	  = { "123-123-123"   , "154-223-344"    , ""};
	CString	asLocations[] = { "DBG"           , "BLV"            , ""};
	CString	asStatus[]    = { "Ringing"       , "Connected"      , "CONFERENCE"};
    int     iImage[]      = { 1               , 2                , 3};

	for (int i = 0; i < sizeof(asTimes) / sizeof(asTimes[0]); i++)
	{
		int iItem;
		iItem = m_CurrentTree.AddItem(asTimes[i], iImage[i], 0);
		m_CurrentTree.SetItemText(iItem, 1, asCalls[i]);
		m_CurrentTree.SetItemText(iItem, 2, asNumbers[i]);
		m_CurrentTree.SetItemText(iItem, 3, asLocations[i]);
		m_CurrentTree.SetItemText(iItem, 4, asStatus[i]);
	}	

	// Add some group call details
	CString	asCalls2[]     = { "BLV SO"        , "MLN SO"        , "Train 77"};
	CString	asNumbers2[]	  = { "123-123-555"   , "154-223-333", "333-999-222"};
	CString	asLocations2[] = { "BLV"           , "MLN"           , "MSM"};
	CString	asStatus2[]    = { "Listening"       , "Talking"      , "Disconected"};

	for (i = 0; i < sizeof(asTimes) / sizeof(asTimes[0]); i++)
	{
		int iItem;
		iItem = m_CurrentTree.AddItem(asCalls2[i], 0, 1);
		m_CurrentTree.SetItemText(iItem, 2, asNumbers2[i]);
		m_CurrentTree.SetItemText(iItem, 3, asLocations2[i]);
		m_CurrentTree.SetItemText(iItem, 4, asStatus2[i]);
	}	
}

*/
void CTreeListCtrlDemoDlg::SetupCurrentStack()
{
	LVCOLUMN	column;

	// Setup the required column format
	column.mask		= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	column.fmt		= LVCFMT_LEFT;

	// Create the required columns
	column.cx		= 170;
	column.iSubItem	= 0;
	column.pszText	= _T("Call");
	m_CurrentTree.InsertColumn(column.iSubItem, &column);


	column.cx		= 75;
	column.iSubItem	= 1;
	column.pszText	= _T("Number");
	m_CurrentTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 2;
	column.pszText	= _T("Location");
	m_CurrentTree.InsertColumn(column.iSubItem, &column);

	column.cx		= 75;
	column.iSubItem	= 3;
	column.pszText	= _T("Status");
	m_CurrentTree.InsertColumn(column.iSubItem, &column);

	// Associate image list with tree control 
	m_CurrentTree.SetImageList(&m_il, LVSIL_SMALL);

	// Add some test calls
	CString	asTimes[]     = { "18/07 12:02:30", "18/07 12:02:54" , "18/07 12:04:22"};
	CString	asCalls[]     = { ""        , ""       , ""};
	CString	asNumbers[]	  = { ""   , ""    , ""};
	CString	asLocations[] = { ""           , ""            , ""};
	CString	asStatus[]    = { "MLN Talking"       , "CVC Talking"      , "Listening"};
    int     iImage[]      = { 1               , 2                , 3};

	for (int i = 0; i < sizeof(asTimes) / sizeof(asTimes[0]); i++)
	{
		int iItem;
		iItem = m_CurrentTree.AddItem(asTimes[i], iImage[i], 0);
		m_CurrentTree.SetItemText(iItem, 1, asStatus[i]);
		m_CurrentTree.SetItemText(iItem, 2, asNumbers[i]);
		m_CurrentTree.SetItemText(iItem, 3, asLocations[i]);

		if (i==1)
		{
			// Add some group call details
			CString	asCalls2[]     = { "BLV SO"        , "MLN SO"        , "Train 77"};
			CString	asNumbers2[]	  = { "123-123-775"   , "154-223-883", "333-999-112"};
			CString	asLocations2[] = { "BLV"           , "MLN"           , "MSM"};
			CString	asStatus2[]    = { "Listening"       , "Talking"      , "Disconected"};

			for (int i2 = 0; i2 < sizeof(asTimes) / sizeof(asTimes[0]); i2++)
			{
				int iItem;
				iItem = m_CurrentTree.AddItem(asCalls2[i2], 0, 1);
				m_CurrentTree.SetItemText(iItem, 1, asNumbers2[i2]);
				m_CurrentTree.SetItemText(iItem, 2, asLocations2[i2]);
				m_CurrentTree.SetItemText(iItem, 3, asStatus2[i2]);
			}
		}
		else if (i==2)
		{
			CString	asCalls3[]     = { "MSM SO"        , "CVC SO"        , "Train 21"};
			CString	asNumbers3[]	  = { "123-123-555"   , "154-223-333", "333-999-222"};
			CString	asLocations3[] = { "MSM"           , "CVC"           , "DBG"};
			CString	asStatus3[]    = { "Listening"       , "Talking"      , "Disconected"};

			for (int i3 = 0; i3 < sizeof(asTimes) / sizeof(asTimes[0]); i3++)
			{
				int iItem;
				iItem = m_CurrentTree.AddItem(asCalls3[i3], 0, 1);
				m_CurrentTree.SetItemText(iItem, 1, asNumbers3[i3]);
				m_CurrentTree.SetItemText(iItem, 2, asLocations3[i3]);
				m_CurrentTree.SetItemText(iItem, 3, asStatus3[i3]);
			}
		}
		else 
		{
			CString	asCalls4[]     = { "NCH SO"        , "DBG SO"        , "Train 50"};
			CString	asNumbers4[]	  = { "123-123-445"   , "154-223-553", "333-999-444"};
			CString	asLocations4[] = { "NCH"           , "DBG"           , "CVC"};
			CString	asStatus4[]    = { "Listening"       , "Talking"      , "Disconected"};

			for (int i4 = 0; i4 < sizeof(asTimes) / sizeof(asTimes[0]); i4++)
			{
				int iItem;
				iItem = m_CurrentTree.AddItem(asCalls4[i4], 0, 1);
				m_CurrentTree.SetItemText(iItem, 1, asNumbers4[i4]);
				m_CurrentTree.SetItemText(iItem, 2, asLocations4[i4]);
				m_CurrentTree.SetItemText(iItem, 3, asStatus4[i4]);
			}
		}
	}
}
