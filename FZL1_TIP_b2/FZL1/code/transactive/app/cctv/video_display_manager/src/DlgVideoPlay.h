#pragma once
#include <string>
#include "app/cctv/video_display_manager/src/resource.h"
#include "app/cctv/video_display_manager/src/QcapDlg.h"

// CDlgVideoPlay dialog
namespace TA_IRS_App
{
    class Monitor;
    class VideoInput;
	class VirtualMonitorAssigner;

    class CDlgVideoPlay : public QcapDlg
    {
            DECLARE_DYNAMIC(CDlgVideoPlay)
        public:
            CDlgVideoPlay(VirtualMonitorAssigner& virtualMonitorAssigner,
						  float xyRate = 0.0,
						  CWnd* pParent = NULL);
            virtual ~CDlgVideoPlay();
            enum { IDD = IDD_VIDEO_PLAY };
        protected:
            virtual BOOL OnInitDialog();
            virtual void DoDataExchange(CDataExchange* pDX);
			afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
			afx_msg void OnClose();
            afx_msg void OnDestroy();
			afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
			afx_msg void OnSize(UINT nType, int cx, int cy);
			afx_msg BOOL OnEraseBkgnd(CDC* pDC);
			VirtualMonitorAssigner& m_virtualMonitorAssigner;
			CStatic m_videoWnd;
			float m_xyRate; // Value of height divide width
            DECLARE_MESSAGE_MAP()
    };
}