#include "stdafx.h"
#include "ModbusVideoPlayerControl.h"
#include "Configuration.h"
#include "core/utilities/src/DebugUtil.h"
#include <string>
#include <boost/lexical_cast.hpp>

#define WM_SHOW_HIDE_WINDOW (WM_USER + 1001)

using namespace std;
using namespace TA_IRS_App;
using namespace TA_Base_Core;

static bool sShouldRunning = true;
static HANDLE sDaemonThreadHandle = NULL;

string exePath()
{
    char buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    string::size_type pos = string(buffer).find_last_of("\\/");
    return string(buffer).substr(0, pos);
}

void showPlayer()
{
    CWnd* pWnd = CWnd::FindWindow("HusVideoPlayer", NULL);
    if (pWnd != NULL)
    {
        //pWnd->SendMessage(WM_SHOW_HIDE_WINDOW, 1, 0);
        pWnd->PostMessage(WM_SHOW_HIDE_WINDOW, 1, 0);
    }

}

void hidePlayer()
{
    CWnd* pWnd = CWnd::FindWindow("HusVideoPlayer", NULL);
    if (pWnd != NULL)
    {
        //pWnd->SendMessage(WM_SHOW_HIDE_WINDOW, 0, 0);
        pWnd->PostMessage(WM_SHOW_HIDE_WINDOW, 0, 0);
    }
}

void launchModbusVideoPlayer()
{
    if (isModbusVideoPlayerAlreadyRunning())
    {
        return;
    }

    TA_Base_Core::IConsole* console = CCTVConfiguration::getinstance().getConsole();
    string siteID, siteIP, userName, password, clientIP, clientPort;
    try
    {
        siteID = console->getCCTVSiteID();
        clientIP = console->getCCTVClientIP();
        clientPort = boost::lexical_cast<std::string>(console->getCCTVClientPort());
        userName = console->getCCTVServerUserName();
        password = console->getCCTVServerPassword();
        siteIP = console->getCCTVServerIPAddress();
    }
    catch (...)
    {
    }
    char args[MAX_PATH];
#ifndef _DEBUG
    sprintf(args, "HusVideoPlayer.exe %s %s %s %s %s %s", siteID.c_str(), siteIP.c_str(), userName.c_str(), password.c_str(), clientIP.c_str(), clientPort.c_str());
#else
    sprintf(args, "HusVideoPlayer.exe %s %s %s %s %s %s", "0789c5e4-3f88-4cd7-bb82-f3717b03da37", "10.9.1.10", "ISCS", "ISCS", "180.81.30.177", "5700");
#endif

    string currentPath = exePath();
    string exeFullName = currentPath + "\\HusVideoPlayer.exe";
    // Construct the WIN32 start process information

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Launch the application using CreateProcess
    if (0 == CreateProcess(exeFullName.c_str(),
                           args,
                           NULL,
                           NULL,
                           FALSE,
                           DETACHED_PROCESS,
                           NULL,
                           currentPath.c_str(),
                           &si,
                           &pi)
       )
    {
        // If CreateProcess returns 0 then it means it failed. Get the error message
        // and throw an exception
        char msg[1000];
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      GetLastError(),
                      MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_DEFAULT), // Default language
                      msg,
                      0,
                      NULL
                     );

        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, msg);
    }
    else
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

bool isModbusVideoPlayerAlreadyRunning()
{
	CWnd* pWnd = CWnd::FindWindow("HusVideoPlayer", NULL);
	if (pWnd != NULL)
	{
		return true;
	}

	return false;

    /*HANDLE event = CreateEvent(NULL, TRUE, FALSE, "EVENT_START_HUS_VIDEO_PLAYER");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(event);
        return true;
    }
    else if (event != NULL)
    {
        CloseHandle(event);
        return false;
    }
    return false;*/
}

DWORD WINAPI startDaemonThread(LPVOID param)
{
	static bool firstRunning = true;
	if(firstRunning)
	{
		firstRunning = false;
		sDaemonThreadHandle = CreateThread(NULL, 0, &startDaemonThread, NULL, 0, NULL);
		return 0;
	}
	while(sShouldRunning)
	{
		launchModbusVideoPlayer();
		if(sShouldRunning)
		{
			Sleep(2000);
		}
	}
	return 0;
}

void terminateDaemonThread()
{
	sShouldRunning = false;
	WaitForSingleObject(sDaemonThreadHandle, INFINITE);
}

