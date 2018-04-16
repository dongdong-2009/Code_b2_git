// DlgVideoPlay.cpp : implementation file
//

#include "stdafx.h"
#include "CCTVCommon.h"
#include "Configuration.h"
#include "DlgVideoPlayModbus.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include <boost/bind.hpp>
#include <boost/scoped_array.hpp>
#include <boost/lexical_cast.hpp>

using TA_Base_Core::DebugUtil;

namespace
{
	CActivexclientctrl1* ax = NULL;
	LRESULT CALLBACK mouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
	    if (nCode >= HC_ACTION && wParam == WM_LBUTTONDBLCLK && ax != NULL) 
	    {
			MOUSEHOOKSTRUCT* m = (MOUSEHOOKSTRUCT*)(lParam);
			HWND h = ax->GetSafeHwnd();
			if(IsChild(h, m->hwnd) || h == m->hwnd)
			{
				ax->FullScreen();
			}
	    }

	    return CallNextHookEx(NULL, nCode, wParam, lParam);
	}
}

namespace TA_IRS_App
{
    IMPLEMENT_DYNAMIC(DlgVideoPlayModbus, CDialog)

    DlgVideoPlayModbus::DlgVideoPlayModbus(CWnd* pParent)
        : CDialog(DlgVideoPlayModbus::IDD, pParent),
		  m_mouseHook(NULL),
		  m_timer(0),
		  m_clientPort(0)
    {
    }

    DlgVideoPlayModbus::~DlgVideoPlayModbus()
    {
    }

	void DlgVideoPlayModbus::show()
	{
		ShowWindow(SW_SHOW);
	}

	void DlgVideoPlayModbus::hide()
	{
		ShowWindow(SW_HIDE);
	}

	void DlgVideoPlayModbus::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_ACTIVEXCLIENTCTRL1, m_ax);
    }


    BEGIN_MESSAGE_MAP(DlgVideoPlayModbus, CDialog)
        //{{AFX_MSG_MAP(CDlgVideoDisplay)
        ON_WM_CLOSE()
        ON_WM_DESTROY()
		ON_WM_SIZE()
		ON_WM_TIMER()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void DlgVideoPlayModbus::OnCancel()
    {
    }

    void DlgVideoPlayModbus::OnOk()
    {
    }

	void DlgVideoPlayModbus::OnSize(UINT nType, int cx, int cy)
	{
		CDialog::OnSize(nType, cx, cy);
		fitWindow();
	}

	void DlgVideoPlayModbus::OnClose()
	{
		ShowWindow(SW_HIDE);
	}

	void DlgVideoPlayModbus::OnDestroy()
    {
		if(m_timer != 0)
		{
			KillTimer(m_timer);
		}
		UnhookWindowsHookEx(m_mouseHook);
        CDialog::OnDestroy();
    }

    BOOL DlgVideoPlayModbus::OnInitDialog()
    {
        CDialog::OnInitDialog();

		ax = &m_ax;
		m_mouseHook = SetWindowsHookEx(WH_MOUSE, mouseHookProc, AfxGetInstanceHandle(), GetCurrentThreadId());
		fitWindow();

		TA_Base_Core::IConsole* console = CCTVConfiguration::getinstance().getConsole();
		try
		{
			m_siteID = console->getCCTVSiteID();
			m_clientIP = console->getCCTVClientIP();
			m_clientPort = console->getCCTVClientPort();
			m_userName = console->getCCTVServerUserName();
			m_password = console->getCCTVServerPassword();
			m_siteIP = console->getCCTVServerIPAddress();
		}
		catch (...)
		{
		}
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "SiteIP:%s, ClientIP:%s, UserName:%s, Password:%s, SiteID:%s", m_siteIP.c_str(), m_clientIP.c_str(), m_userName.c_str(), m_password.c_str(), m_siteID.c_str());
		m_ax.SetsiteIP(m_siteIP.c_str());
		m_ax.SetsiteID(m_siteID.c_str());
		m_ax.SetuserName(m_userName.c_str());
		m_ax.Setpwd(m_password.c_str());
		m_ax.SetclientIP(m_clientIP.c_str());
		if(m_clientPort != 0)
		{
			m_ax.SetclientPort(boost::lexical_cast<std::string>(m_clientPort).c_str());
		}
		m_ax.Login();
        m_timer = SetTimer(1, 4000, 0);
        return TRUE;
    }

	void DlgVideoPlayModbus::fitWindow()
	{
		if(IsWindow(m_ax.GetSafeHwnd()))
		{
			RECT clientRect;
			GetClientRect(&clientRect);
			m_ax.Setwidth(clientRect.right-clientRect.left);
			m_ax.Setheight(clientRect.bottom-clientRect.top);
		}
	}

	void DlgVideoPlayModbus::OnTimer( UINT_PTR timerId )
	{
		if(timerId == m_timer)
		{
			m_ax.Login();
		}
        CDialog::OnTimer(timerId);
	}

}