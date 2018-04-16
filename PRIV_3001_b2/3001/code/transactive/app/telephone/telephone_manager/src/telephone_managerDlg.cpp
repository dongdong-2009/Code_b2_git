/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/telephone_managerDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// telephone_managerDlg.cpp : implementation file
//

#include "stdafx.h"
#include <afxpriv.h>   //For AfxLoadString
#include "telephone_manager.h"
#include "telephone_managerDlg.h"
#include "LineForwardDialog.h"

#include "bus\generic_gui\src\AbstractDialogGUI.h"
#include "core\utilities\src\taassert.h"
#include "bus\generic_gui\src\HelpLauncher.h"
#include "core\utilities\src\RunParams.h"
#include "core\exceptions\src\UserSettingsException.h"
#include "core\utilities\src\DebugUtil.h"

//using namespace TA_Core::TA_Utilities;
using TA_Bus::TransActiveDialog;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
    ID_LINE_FORWARD_MODE
};

/////////////////////////////////////////////////////////////////////////////
// CTelephone_managerDlg dialog


CTelephone_managerDlg::CTelephone_managerDlg(TA_Bus::IGUIAccess& controlClass)
    : TransActiveDialog(controlClass, CTelephone_managerDlg::IDD)
{
    // {{AFX_DATA_INIT(CTelephone_managerDlg)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTelephone_managerDlg::DoDataExchange(CDataExchange* pDX)
{
    TransActiveDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CTelephone_managerDlg)
    DDX_Control(pDX, IDC_TELEPHONE_CALL_STACK_TAB, m_callstacks);
    DDX_Control(pDX, IDC_TELEPHONE_FACILITIES_TAB, m_facilitiestab);
    // }}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTelephone_managerDlg, TransActiveDialog)
    // {{AFX_MSG_MAP(CTelephone_managerDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CREATE()
    ON_COMMAND(ID_VIEW_TOOLBAR, onViewToolbar)
    ON_COMMAND(ID_VIEW_STATUSBAR, onViewStatusbar)
    ON_COMMAND(ID_HELP_ABOUTTELEPHONECONTROLLER, nnHelpAboutTelephoneController)
    ON_COMMAND(IDC_LINEFORWARD, onLineForward)
    // }}AFX_MSG_MAP
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTelephone_managerDlg message handlers

BOOL CTelephone_managerDlg::OnInitDialog()
{
    CRect windowSize;
    GetWindowRect(&windowSize);
    TA_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minHeight = windowSize.bottom - windowSize.top;
    properties.minWidth = windowSize.right - windowSize.left;
    setResizingProperties(properties);

    TransActiveDialog::OnInitDialog();

    // m_callstacks.SubclassDlgItem(IDC_TELEPHONE_CALL_STACK_TAB,this);
    // m_facilitiestab.SubclassDlgItem(IDC_TELEPHONE_FACILITIES_TAB,this);

    // Set the icon for this dialog.  The framework does this automatically
    // when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    EnableToolBar(true);
    EnableStatusBar(true);

    // Set the resizing properties again now that we have loaded the toolbar and status bar
    GetWindowRect(&windowSize);
    properties.canMaximise = true;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minHeight = windowSize.bottom - windowSize.top;
    properties.minWidth = windowSize.right - windowSize.left;
    setResizingProperties(properties);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTelephone_managerDlg::OnPaint()
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
        TransActiveDialog::OnPaint();
    }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTelephone_managerDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}


void CTelephone_managerDlg::CreateToolbar()
{
    // Create toolbar at the top of the dialog window
    if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
                            | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_toolBar.LoadToolBar(IDR_MAINTOOLBAR))
    {
        m_toolBar.LoadToolBar(IDR_MAINTOOLBAR);
        TRACE0("Failed to create toolbar\n");
    }

    // Add text to the buttons
    // Any exception from these functions will fall through and be caught by
    // the calling member
    AddTextToButton( IDC_LINEFORWARD, "Forward" );
    AddTextToButton( IDC_HELPBUTTON, "Help" );
    AddTextToButton( IDC_CALL,    "Call" );
    AddTextToButton( IDC_END_CALL, "End Call" );
    AddTextToButton( IDC_ANSWER,  "Answer" );

    // m_toolBar.SetButtonStyle( m_toolBar.CommandToIndex( ID_NOW ), TBBS_CHECKBOX );

    m_toolBar.GetToolBarCtrl().SetButtonSize( CSize( 69, 37 ) );
}

void CTelephone_managerDlg::AddTextToButton(UINT buttonId, CString buttonLabel )
{
    int index = m_toolBar.CommandToIndex( buttonId );
    if( index >=0 )
    {
        m_toolBar.SetButtonText( index, buttonLabel );
    }
    else
    {
        TRACE1("Unable to find %s button\n", buttonLabel);
    }
}


int CTelephone_managerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (TransActiveDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create the status bar
    if (!m_wndStatusBar.CreateEx( this, SBT_TOOLTIPS , //| SBARS_SIZEGRIP,
                                  WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
                                  AFX_IDW_TOOLBAR) ||
        !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)) )
    {
        TRACE0("Failed to create status bar\n");
        // return -1;      // fail to create
    }

    m_wndStatusBar.GetStatusBarCtrl().SetTipText( 0, "Ready" );
    m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0),    SBPS_STRETCH, NULL );
//    OnSetMessageString(AFX_IDS_IDLEMESSAGE);

    // Disable the call forward pane
    m_wndStatusBar.SetPaneStyle(1,SBPS_DISABLED);

    // Create the toolbar
    CreateToolbar();

    // Calculate the size of the toolbar and status bar
    CRect rcClientStart;
    CRect rcClientNow;
    GetClientRect(rcClientStart);
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, rcClientNow);
    m_toolBarHeight   = rcClientNow.top;
    m_statusBarHeight = rcClientStart.bottom - rcClientNow.bottom;

    return 0;
}


//******************************
// Manage the tool bar display
//******************************
void CTelephone_managerDlg::onViewToolbar()
{
    bool isVisible = (m_toolBar.IsWindowVisible()==TRUE);
    EnableToolBar(!isVisible);
    m_toolBar.ShowWindow(isVisible ? SW_HIDE : SW_SHOWNA);
     GetMenu()->CheckMenuItem(ID_VIEW_TOOLBAR,isVisible ? MF_UNCHECKED : MF_CHECKED);
    RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);
}


void CTelephone_managerDlg::EnableToolBar(bool bEnable)
{
    CPoint ptOffset;

    ptOffset.x = 0;

    // Check for initial enabling
    if ((bEnable) && (!m_toolBar.IsWindowVisible()))
    {
        // Creating - so move all items down
        ptOffset.y = m_toolBarHeight;
    }
    else if ((!bEnable) && (m_toolBar.IsWindowVisible()))
    {
        // Removing so move all items up
        ptOffset.y = m_toolBarHeight * -1;
    }
    else
    {
        // No change
        return;
    }

    // Move the child controls
    CRect rcChild;
    CWnd* pwndChild = GetWindow(GW_CHILD);
    while (pwndChild)
    {
        if (pwndChild!=&m_toolBar) //&&(pwndChild!=&m_wndStatusBar))
        {
            pwndChild->GetWindowRect(rcChild);
            ScreenToClient(rcChild);
            rcChild.OffsetRect(ptOffset);
            pwndChild->MoveWindow(rcChild, FALSE);
            // pwndChild->MoveWindow(rcChild, TRUE);
        }
        pwndChild = pwndChild->GetNextWindow();
    }

    // Adjust the dialog size
    CRect rcWindow;
    GetWindowRect(rcWindow);
    rcWindow.bottom += ptOffset.y;
    MoveWindow(rcWindow, TRUE);

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
}

/////////////////////////////////////////////////////////////////////////////
// CModelessMain::OnToolTipText
//      Handles TTN_NEEDTEXT message to display tooltips for the toolbar.
//      This code is based on CFrameWnd::OnToolTipText
BOOL CTelephone_managerDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
    ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

    // allow top level routing frame to handle the message
    if (GetRoutingFrame() != NULL)
        return FALSE;

    // need to handle both ANSI and UNICODE versions of the message
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
    TCHAR szFullText[256];
    CString strTipText;
    UINT nID = pNMHDR->idFrom;

    if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
        pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
    {
        // idFrom is actually the HWND of the tool
        nID = ((UINT)(WORD)::GetDlgCtrlID((HWND)nID));
    }

    if (nID != 0) // will be zero on a separator
    {
        AfxLoadString(nID, szFullText);
            // this is the command id, not the button index
        AfxExtractSubString(strTipText, szFullText, 1, '\n');
    }

#ifndef _UNICODE
    if (pNMHDR->code == TTN_NEEDTEXTA)
        lstrcpyn(pTTTA->szText, strTipText,
            (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
    else
        _mbstowcsz(pTTTW->szText, strTipText,
            (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
#else
    if (pNMHDR->code == TTN_NEEDTEXTA)
        _wcstombsz(pTTTA->szText, strTipText,
            (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
    else
        lstrcpyn(pTTTW->szText, strTipText,
            (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
#endif

    *pResult = 0;
    // bring the tooltip window above other popup windows
    ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
        SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

    return TRUE;    // message was handled
}


// ******************************
// Status bar
//*******************************
void CTelephone_managerDlg::onViewStatusbar()
{
    bool isVisible = (m_wndStatusBar.IsWindowVisible()==TRUE);

    m_wndStatusBar.ShowWindow(isVisible ? SW_HIDE : SW_SHOWNA);
    EnableStatusBar(!isVisible);
     GetMenu()->CheckMenuItem(ID_VIEW_STATUSBAR,isVisible ? MF_UNCHECKED : MF_CHECKED);
    RedrawWindow(NULL,NULL,RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);
}


void CTelephone_managerDlg::EnableStatusBar(bool bEnable)
{
    CPoint ptOffset;

    ptOffset.x = 0;

    // Check for initial enabling
    if ((bEnable) && (!m_wndStatusBar.IsWindowVisible()))
    {
        // Creating - so extend dialog
        ptOffset.y = m_statusBarHeight;
    }
    else if ((!bEnable) && (m_wndStatusBar.IsWindowVisible()))
    {
        // Removing so contract dialog
        ptOffset.y = m_statusBarHeight * -1;
    }
    else
    {
        // No change
        return;
    }

    CRect rcWindow;
    GetWindowRect(rcWindow);
    rcWindow.bottom += ptOffset.y;
    MoveWindow(rcWindow, TRUE);

    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
}


void CTelephone_managerDlg::nnHelpAboutTelephoneController()
{
   TA_Bus::HelpLauncher::getInstance().displayAboutBox();
}

void CTelephone_managerDlg::onLineForward()
{
    LineForwardDialog   dlg;
    dlg.DoModal();

    if(dlg.IsForward())
    {
        // This Sets and displays text for the forwarding status.
        m_wndStatusBar.SetPaneText(1,dlg.getStatusString(), TRUE);

        m_wndStatusBar.SetPaneStyle(1, SBPS_NORMAL);

    }
    else //no forwarding - clear the status bar
    {
        m_wndStatusBar.SetPaneStyle(1, SBPS_DISABLED);
    }


}

void CTelephone_managerDlg::OnCancel()
{
    CDialog::OnCancel();
}
