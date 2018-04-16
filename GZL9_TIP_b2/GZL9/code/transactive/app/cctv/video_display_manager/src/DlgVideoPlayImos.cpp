// DlgVideoPlay.cpp : implementation file
//

#include "stdafx.h"
#include "DlgVideoPlayImos.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/mw_ptzlib_pub.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "CCTVCommon.h"
#include <boost/bind.hpp>

using TA_Base_Core::DebugUtil;

namespace
{
	BOOL CALLBACK EnumChildProc(
	    _In_  HWND hwnd,
	    _In_  LPARAM lParam
	)
	{
	    EnableWindow(hwnd, false);
	    return true;
	}
}

namespace TA_IRS_App
{
    IMPLEMENT_DYNAMIC(DlgVideoPlayImos, CDialog)

    DlgVideoPlayImos::DlgVideoPlayImos(CWnd* pParent)
        : CDialog(DlgVideoPlayImos::IDD, pParent),
          m_timer(0),
          m_currentLayOut(4) //default layout
    {
    }

    DlgVideoPlayImos::~DlgVideoPlayImos()
    {
    }

    void DlgVideoPlayImos::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_IMOS_MEDIAOCXCTRL1, m_imosCtrl);
    }


    BEGIN_MESSAGE_MAP(DlgVideoPlayImos, CDialog)
        //{{AFX_MSG_MAP(CDlgVideoDisplay)
        ON_WM_CLOSE()
        ON_WM_DESTROY()
        ON_WM_TIMER()
		ON_WM_SIZE()
		ON_MESSAGE(FULLSCREEN_REQUEST, OnFullScreenRequst)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void DlgVideoPlayImos::OnCancel()
    {
    }

    void DlgVideoPlayImos::OnOk()
    {
    }

	LRESULT DlgVideoPlayImos::OnFullScreenRequst(WPARAM wParam, LPARAM lParam)
	{
		// If window in full screen then normal, else fullscreen
		if(IsZoomed())
		{
			ModifyStyle(0, WS_CAPTION);
			ShowWindow(SW_SHOWNORMAL);
		}
		else
		{
			ModifyStyle( WS_CAPTION,0);
			ShowWindow(SW_SHOWMAXIMIZED);
		}
		return 0;
	}

	void DlgVideoPlayImos::OnSize(UINT nType, int cx, int cy)
	{
		CDialog::OnSize(nType, cx, cy);
		if(::IsWindow(m_imosCtrl.GetSafeHwnd()))
		{
			RECT clientRect;
			this->GetClientRect(&clientRect);
			m_imosCtrl.MoveWindow(&clientRect);
		}
	}

    void DlgVideoPlayImos::OnTimer(UINT_PTR timerId)
    {
        // imosctrl init timer
        if((timerId == m_timer) && (true == initImosCtrl()))
        {
            KillTimer(m_timer);
            m_timer = 0;
            return;
        }
        CDialog::OnTimer(timerId);
    }

    void DlgVideoPlayImos::OnDestroy()
    {
        if(0 != m_timer)
        {
            KillTimer(m_timer);
        }
        IMOS_CALL(m_imosCtrl.IMOSAX_UnregOCX());
        CDialog::OnDestroy();
    }

    BOOL DlgVideoPlayImos::OnInitDialog()
    {
        CDialog::OnInitDialog();
        // Get imos parameters
        TA_Base_Core::IEntityDataList entities;
        do
        {
            try
            {
                unsigned long locationKey = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str() , NULL, 10);
                entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::VideoSwitchAgent::getStaticType(), locationKey);
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Got exception while get VideoSwitchAgent entity.", "");
                break;
            }
            if(1 == entities.size())
            {
                try
                {
                    TA_Base_Core::VideoSwitchAgent entity(entities[0]->getKey());
                    m_serverIp = entity.getCCTVServerIPAddress();
                    m_userName = entity.getCCTVServerUserName();
                    m_userPassword = entity.getCCTVServerPassword();
                    m_serverPort = entity.getCCTVServerPort();
                }
                catch(...)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "Got exception while get IMOS related parameters value.", "");
                    break;
                }
            }
            for(TA_Base_Core::IEntityDataList::size_type i = 0; i < entities.size(); ++i)
            {
                delete entities[i];
            }
            if(false == initImosCtrl())
            {
                m_timer = SetTimer(1, 1000, 0);
            }
        }
        while(0);
        return TRUE;
    }

    bool DlgVideoPlayImos::initImosCtrl()
    {
        // Init imos active ctrl
        unsigned long errorCode = 0;
        bool success = false;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Server ip:%s, Server port:%lu, User name:%s, User password:%s",
			m_serverIp.c_str(), m_serverPort, m_userName.c_str(), m_userPassword.c_str());
        errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_InitOCX(m_serverIp.c_str(), m_serverPort, m_userName.c_str(), m_userPassword.c_str(), true));
        LOG_ERRORCODE_ERROR(errorCode);
        if(ERR_COMMON_SUCCEED == errorCode)
        {
            // Disable all child window all imos ctrl to disable contextmenu of imos
            EnumChildWindows(m_imosCtrl.m_hWnd, EnumChildProc, reinterpret_cast<LPARAM>(this));
            success = true;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Init imos ocx ctrl successfully.");
        }
        return success;
    }

    void DlgVideoPlayImos::setMonitorAssignChangedObserver(IMonitorAssignChangeObserver& assignChangedObserver)
    {
        m_monitorAssignChangeOb = &assignChangedObserver;
    }

    bool DlgVideoPlayImos::setLayoutToQuad()
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "setLayoutToQuad");
        bool success = true;
        if(m_currentLayOut != 4)
        {
            unsigned long errorCode = 0;
            errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_ChangeLayout(4));
            LOG_ERRORCODE_ERROR(errorCode);
            success = (ERR_COMMON_SUCCEED == errorCode);
        }
        return success;
    }

    bool DlgVideoPlayImos::setLayoutToSingle()
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "setLayoutToSingle");
        bool success = true;
        if(m_currentLayOut != 1)
        {
            unsigned long errorCode = 0;
            errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_ChangeLayout(1));
            LOG_ERRORCODE_ERROR(errorCode);
            success = (ERR_COMMON_SUCCEED == errorCode);
        }
        return success;
    }
    // DlgVideoPlayImos message handlers
    BEGIN_EVENTSINK_MAP(DlgVideoPlayImos, CDialog)
    ON_EVENT(DlgVideoPlayImos, IDC_IMOS_MEDIAOCXCTRL1, 16, DlgVideoPlayImos::eventCurLayoutImos, VTS_UI4)
    END_EVENTSINK_MAP()

    void DlgVideoPlayImos::eventCurLayoutImos(unsigned long ulCurLayout)
    {
        // TODO: Add your message handler code here
        m_currentLayOut = ulCurLayout;
    }

    PTZCtrlStartStop::PTZCtrlStartStop(ImosCtrl& imosCtrl, LPCTSTR camCode)
        : m_imosCtrl(imosCtrl),
          m_errorCode(0)
    {
        strncpy(m_camCode, camCode, IMOS_RES_CODE_LEN);
        m_errorCode = IMOS_CALL(m_imosCtrl.IMOSAX_StartPtzCtrl(m_camCode));
    }

    PTZCtrlStartStop::~PTZCtrlStartStop()
    {
        if(ERR_COMMON_SUCCEED == m_errorCode)
        {
            IMOS_CALL(m_imosCtrl.IMOSAX_StopPtzCtrl(m_camCode));
            LOG_ERRORCODE_ERROR(m_errorCode);
        }
    }

    unsigned long PTZCtrlStartStop::getErrorCode()
    {
        return m_errorCode;
    }
}