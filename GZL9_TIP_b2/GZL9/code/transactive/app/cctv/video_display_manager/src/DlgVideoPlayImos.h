#pragma once
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/imos_public.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/imos_def.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/sdk_def.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/imos_errcode.h"
#include "ImosCtrl.h"
#include "Resource.h"
#include "IMonitorAssigner.h"
#include <vector>
#include <string>

// Some macro for convenient usage
#define LOG_ERRORCODE_ERROR(errorCode) \
    if((errorCode) != ERR_COMMON_SUCCEED)\
    {\
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Call imos function return error, errorcode is:%u",(errorCode));\
    }

#define LOG_ERRORCODE_DEBUG(errorCode) \
    if((errorCode) != ERR_COMMON_SUCCEED)\
    {\
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Call imos function return error, errorcode is:%u",(errorCode));\
    }

#define LOG_CALLED_INFO(funcName) \
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Function "#funcName" was called")

#ifdef LOCAL_TEST
#define IMOS_CALL(call) ((TA_Base_Core::RunParams::getInstance().get("ImosReturnValue") == "succeed") ? ERR_COMMON_SUCCEED : ERR_COMMON_FAIL)
#else
#define IMOS_CALL(call) call
#endif

// DlgVideoPlayImos dialog
namespace TA_IRS_App
{
    class Monitor;
    class VideoInput;
    class IMonitorAssignChangeObserver;

    class DlgVideoPlayImos : public CDialog,
        public IMonitorAssigner
    {
		protected:
            DECLARE_DYNAMIC(DlgVideoPlayImos)
        public:
            DlgVideoPlayImos(CWnd* pParent);
            virtual ~DlgVideoPlayImos();
            virtual bool assignMonitor(TA_IRS_App::Monitor* monitor, TA_IRS_App::VideoInput* input) = 0;
            void setMonitorAssignChangedObserver(IMonitorAssignChangeObserver& assignChangedObserver);
            enum { IDD = IDD_VIDEO_PLAY_IMOS };
        protected:
			virtual void OnCancel();
			virtual void OnOk();
            virtual BOOL OnInitDialog();
            virtual void CheckShowOrHideWindow() = 0;
            virtual void DoDataExchange(CDataExchange* pDX);
            afx_msg void OnDestroy();
			afx_msg void OnTimer(UINT_PTR timerId);
			afx_msg void OnSize(UINT nType, int cx, int cy);
			afx_msg LRESULT OnFullScreenRequst(WPARAM wParam, LPARAM lParam);
            DECLARE_MESSAGE_MAP()

            bool setLayoutToQuad();
            bool setLayoutToSingle();
            bool initImosCtrl();
            ImosCtrl m_imosCtrl;
            std::string m_serverIp;
            std::string m_userName;
            std::string m_userPassword;
            unsigned long m_serverPort;
			unsigned long m_currentLayOut;
			IMonitorAssignChangeObserver* m_monitorAssignChangeOb;
			DECLARE_EVENTSINK_MAP()
			void eventCurLayoutImos(unsigned long ulCurLayout);
		private:
			UINT_PTR m_timer;
    };

    // Wrap start and stop ptzctrl
    class PTZCtrlStartStop
    {
        public:
            PTZCtrlStartStop(ImosCtrl& imosCtrl, LPCTSTR camCode);
            ~PTZCtrlStartStop();
            unsigned long getErrorCode();
        private:
            TCHAR m_camCode[IMOS_RES_CODE_LEN];
            unsigned long m_errorCode;
            ImosCtrl& m_imosCtrl;
    };
}