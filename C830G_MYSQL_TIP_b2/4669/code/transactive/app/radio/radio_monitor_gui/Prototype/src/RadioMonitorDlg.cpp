/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/Prototype/src/RadioMonitorDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// RadioMonitorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RadioMonitor.h"
#include "RadioMonitorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioMonitorDlg dialog

CRadioMonitorDlg::CRadioMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRadioMonitorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRadioMonitorDlg)
	m_searchEdit = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRadioMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadioMonitorDlg)
	DDX_Control(pDX, IDC_SEARCH_LIST, m_SearchList);
	DDX_Control(pDX, IDC_SUBSCRIBER_TREE, m_DirectoryTree);
	DDX_Control(pDX, IDC_CALL_LIST, m_callList);
	DDX_Control(pDX, IDC_MONITOR_LIST, m_monitorList);
	DDX_Text(pDX, IDC_EDIT1, m_searchEdit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRadioMonitorDlg, CDialog)
	//{{AFX_MSG_MAP(CRadioMonitorDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioMonitorDlg message handlers

BOOL CRadioMonitorDlg::OnInitDialog()
{
	m_searchEdit = "Fred";


	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// Setup the subscriber tree
	{
		HTREEITEM hItem = m_DirectoryTree.InsertItem("Users");
		HTREEITEM hSubItem = m_DirectoryTree.InsertItem("OCC",hItem);
				   m_DirectoryTree.InsertItem("CS",hSubItem);
				   m_DirectoryTree.InsertItem("MCO",hSubItem);
				   m_DirectoryTree.InsertItem("PSCO",hSubItem);
				   m_DirectoryTree.InsertItem("SCO",hSubItem);
				   m_DirectoryTree.InsertItem("SO",hSubItem);
				   m_DirectoryTree.InsertItem("TCO",hSubItem);

		hSubItem = m_DirectoryTree.InsertItem("DCR",hItem);
				   m_DirectoryTree.InsertItem("DCO",hSubItem);
				   m_DirectoryTree.InsertItem("PSCO",hSubItem);

		hSubItem = m_DirectoryTree.InsertItem("PSC",hItem);
				   m_DirectoryTree.InsertItem("BLV",hSubItem);
				   m_DirectoryTree.InsertItem("CVC",hSubItem);
				   m_DirectoryTree.InsertItem("DBG",hSubItem);
				   m_DirectoryTree.InsertItem("MLM",hSubItem);
				   m_DirectoryTree.InsertItem("MSM",hSubItem);
				   m_DirectoryTree.InsertItem("NCH",hSubItem);


	   hItem = m_DirectoryTree.InsertItem("Trains");
				m_DirectoryTree.InsertItem("TRAIN#01",hItem);
				m_DirectoryTree.InsertItem("TRAIN#03",hItem);
				m_DirectoryTree.InsertItem("TRAIN#09",hItem);
				m_DirectoryTree.InsertItem("TRAIN#14",hItem);
				m_DirectoryTree.InsertItem("TRAIN#22",hItem);
				m_DirectoryTree.InsertItem("TRAIN#52",hItem);

		hItem = m_DirectoryTree.InsertItem("Groups");
				m_DirectoryTree.InsertItem("Portables",hItem);
				m_DirectoryTree.InsertItem("OCC",hItem);
				m_DirectoryTree.InsertItem("DCR",hItem);
				m_DirectoryTree.InsertItem("PSC",hItem);
				m_DirectoryTree.InsertItem("Trains",hItem);
		
		hItem = m_DirectoryTree.InsertItem("Dynamic Group");
				m_DirectoryTree.InsertItem("CVC",hItem);
				m_DirectoryTree.InsertItem("OCC",hItem);
				m_DirectoryTree.InsertItem("DCR",hItem);
	}

	// Setup example search matches
	m_SearchList.InsertColumn(0,"ID" );
	m_SearchList.InsertColumn(1,"TSI" );
//	m_SearchList.InsertColumn(2,"Location" );
	m_SearchList.SetColumnWidth(0,100);
	m_SearchList.SetColumnWidth(1,100);
	m_SearchList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	int iRet;

	iRet = m_SearchList.InsertItem(0,"Fred Smith");
    m_SearchList.SetItemText(iRet,1,"6724");
	iRet = m_SearchList.InsertItem(1,"Fred Jones");
    m_SearchList.SetItemText(iRet,1,"2432");
	iRet = m_SearchList.InsertItem(2,"Fred Wedlock");
    m_SearchList.SetItemText(iRet,1,"9874");
	iRet = m_SearchList.InsertItem(3,"Fred Dibnah");
    m_SearchList.SetItemText(iRet,1,"9843 7374 666");
	iRet = m_SearchList.InsertItem(4,"John Fredrick");
    m_SearchList.SetItemText(iRet,1,"3444");
	//m_SearchList.SetCurSel(0);

	// Setup monitor list
	m_monitorList.AddString("Operator #1");
	m_monitorList.AddString("Operator #2");
	m_monitorList.AddString("Operator #3");
	m_monitorList.AddString("Operator #4");
	m_monitorList.AddString("Fred Dibnah");
	m_monitorList.AddString("Train 082/0234");

	m_monitorList.SetCurSel(0);

	// Setup call list
	m_callList.InsertColumn(0,"Source");
	m_callList.InsertColumn(1,"Destination");
	m_callList.InsertColumn(2,"Type");
	m_callList.SetColumnWidth(0,150);
	m_callList.SetColumnWidth(1,150);
	m_callList.SetColumnWidth(2,150);
	m_callList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	{
		int hItem = m_callList.InsertItem(0,"Operator #1");
				m_callList.SetItemText(hItem,1,"DBG SO");
				m_callList.SetItemText(hItem,2,"SINGLE");
		hItem = m_callList.InsertItem(0,"Operator #2");
				m_callList.SetItemText(hItem,1,"Talk Group 1");
				m_callList.SetItemText(hItem,2,"CONFERENCE");
		hItem = m_callList.InsertItem(0,"Operator #3");
				m_callList.SetItemText(hItem,1,"Fred Smith");
				m_callList.SetItemText(hItem,2,"SINGLE");
		hItem = m_callList.InsertItem(0,"Operator #4");
				m_callList.SetItemText(hItem,1,"Train 034/1234");
				m_callList.SetItemText(hItem,2,"SINGLE");
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRadioMonitorDlg::OnPaint() 
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
HCURSOR CRadioMonitorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
