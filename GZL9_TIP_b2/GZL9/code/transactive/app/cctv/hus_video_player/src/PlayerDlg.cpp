
// HusVideoPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PlayerDlg.h"
#include "HusParam.h"
#include <tlhelp32.h>

#define WM_SHOW_HIDE_WINDOW (WM_USER + 1001)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
// CHusVideoPlayerDlg dialog
namespace
{
	bool IsProcessRunning(const char* processName)
	{
	    bool exists = false;
	    PROCESSENTRY32 entry;
	    entry.dwSize = sizeof(PROCESSENTRY32);

	    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	    if (Process32First(snapshot, &entry))
	        while (Process32Next(snapshot, &entry))
	            if (!_stricmp(entry.szExeFile, processName))
	                exists = true;

	    CloseHandle(snapshot);
	    return exists;
	}
}

extern HusParam gHusParam;
extern bool gLaunchByDisplayManager;

CHusVideoPlayerDlg::CHusVideoPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHusVideoPlayerDlg::IDD, pParent), m_timerID(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHusVideoPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACTIVEXCLIENTCTRL1, m_ax);
}

BEGIN_MESSAGE_MAP(CHusVideoPlayerDlg, CDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SHOW_HIDE_WINDOW, OnShowOrHideWindow)
END_MESSAGE_MAP()


// CHusVideoPlayerDlg message handlers

BOOL CHusVideoPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_timerID = SetTimer(1, 1000, NULL);

	fitWindow();

	m_ax.SetsiteID(gHusParam.getSiteID().c_str());
	m_ax.SetsiteIP(gHusParam.getSiteIP().c_str());
	m_ax.SetuserName(gHusParam.getUserName().c_str());
	m_ax.Setpwd(gHusParam.getPassword().c_str());
	m_ax.SetclientIP(gHusParam.getClientIP().c_str());
	m_ax.SetclientPort(gHusParam.getClientPort().c_str());

	m_ax.Login();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHusVideoPlayerDlg::OnCancel()
{

}

void CHusVideoPlayerDlg::OnOK()
{

}

void CHusVideoPlayerDlg::OnDestroy()
{
	KillTimer(m_timerID);
    CDialog::OnDestroy();
}

void CHusVideoPlayerDlg::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
	fitWindow();
}

void CHusVideoPlayerDlg::OnClose()
{
	gLaunchByDisplayManager ? ShowWindow(SW_HIDE) : ExitProcess(0);
}

void CHusVideoPlayerDlg::OnTimer( UINT nIDEvent )
{
	if(gLaunchByDisplayManager && !IsProcessRunning("DisplayManager.exe"))
	{
		ExitProcess(0);
	}
}

LRESULT CHusVideoPlayerDlg::OnShowOrHideWindow( WPARAM wParam, LPARAM lParam )
{
	if(wParam == 1)
	{
		ShowWindow(SW_SHOW);
		SetForegroundWindow();
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
	return 0;
}

void CHusVideoPlayerDlg::fitWindow()
{
	if(IsWindow(m_ax.GetSafeHwnd()))
	{
		RECT clientRect;
		GetClientRect(&clientRect);
		m_ax.Setwidth(clientRect.right-clientRect.left);
		m_ax.Setheight(clientRect.bottom-clientRect.top);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.