
// HusVideoPlayer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "HusVideoPlayer.h"
#include "PlayerDlg.h"
#include "HusParam.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHusVideoPlayerApp

BEGIN_MESSAGE_MAP(CHusVideoPlayerApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CHusVideoPlayerApp construction

CHusVideoPlayerApp::CHusVideoPlayerApp()
{
    // support Restart Manager
    //m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

// The one and only CHusVideoPlayerApp object

CHusVideoPlayerApp theApp;
HusParam gHusParam;
bool gLaunchByDisplayManager = true;


// CHusVideoPlayerApp initialization

BOOL CHusVideoPlayerApp::InitInstance()
{
    if (m_singleInstanceGuard.isInstanceAlreadyRunning())
    {
        return FALSE;
    }
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();
    WNDCLASS wndcls;
    ::GetClassInfo(NULL, MAKEINTRESOURCE(32770), &wndcls);

    // Set our own class name
    wndcls.lpszClassName = "HusVideoPlayer";

    // Just register the class
    if (!::RegisterClass(&wndcls))
    {
        _ASSERTE(! __FUNCTION__ " Failed to register window class");
        return FALSE;
    }

    std::string cmdLine = GetCommandLine();
    //for debug
    //MessageBox(NULL, cmdLine.c_str(), "", 0);

    std::istringstream iss(cmdLine);
    std::vector<std::string> params;
    std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(params));

    params.erase(params.begin());

    gHusParam.setSiteID(params[0]);
    gHusParam.setSiteIP(params[1]);
    gHusParam.setUserName(params[2]);
    gHusParam.setPassword(params[3]);
    gHusParam.setClientIP(params[4]);
    gHusParam.setClientPort(params[5]);

	if(params.size() > 6 && params[6] == "manually")
	{
		gLaunchByDisplayManager = false;
	}

    AfxEnableControlContainer();

    CHusVideoPlayerDlg* dlg = new CHusVideoPlayerDlg();
	dlg->Create(CHusVideoPlayerDlg::IDD);
    m_pMainWnd = dlg;
	dlg->ShowWindow(gLaunchByDisplayManager ? SW_HIDE : SW_SHOW);

    return TRUE;
}

SingleInstanceGuard::SingleInstanceGuard() : m_hStartEvent(NULL)
{
    m_hStartEvent = CreateEvent(NULL, TRUE, FALSE, "EVENT_START_HUS_VIDEO_PLAYER");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(m_hStartEvent);
        m_hStartEvent = NULL;
    }
}

SingleInstanceGuard::~SingleInstanceGuard()
{
    if (m_hStartEvent)
    {
        CloseHandle(m_hStartEvent);
    }
}

bool SingleInstanceGuard::isInstanceAlreadyRunning()
{
    return m_hStartEvent == NULL;
}
