#pragma once
#include "Resource.h"
#include "activexclientctrl1.h"
#include "IWindowShowOrHide.h"
#include <vector>
#include <string>

// DlgVideoPlayModbus dialog
namespace TA_IRS_App
{
	class IMonitorAssignChangeObserver;
    class DlgVideoPlayModbus : public CDialog, public IWindowShowOrHide
    {
		protected:
            DECLARE_DYNAMIC(DlgVideoPlayModbus)
        public:
            DlgVideoPlayModbus(CWnd* pParent);
            virtual ~DlgVideoPlayModbus();
			virtual void show();
			virtual void hide();
            enum { IDD = IDD_VIDEO_PLAY_MODBUS };
        protected:
			virtual void OnCancel();
			virtual void OnOk();
            virtual BOOL OnInitDialog();
            virtual void DoDataExchange(CDataExchange* pDX);
			void fitWindow();
			afx_msg void OnTimer(UINT_PTR timerId);
            afx_msg void OnDestroy();
			afx_msg void OnSize(UINT nType, int cx, int cy);
			afx_msg void OnClose();
            DECLARE_MESSAGE_MAP()

			IMonitorAssignChangeObserver* m_monitorAssignChangeOb;
		private:
			CActivexclientctrl1 m_ax;
			HHOOK m_mouseHook;
			std::string m_clientIP;
			unsigned long m_clientPort;
			std::string m_siteIP;
			std::string m_userName;
			std::string m_password;
			std::string m_siteID;
			UINT_PTR m_timer;
    };
}