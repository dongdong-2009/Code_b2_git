#pragma once
#include "afxwin.h"
#include "app/cctv/video_display_manager/src/QcapDlg.h"
#include "app/cctv/video_display_manager/src/resource.h"

// PlayBackDlg dialog

namespace TA_IRS_App
{
	class VirtualMonitorAssigner;
	class PlayBackDlg : public QcapDlg
	{
		DECLARE_DYNAMIC(PlayBackDlg)

	public:
		PlayBackDlg(VirtualMonitorAssigner& virtualMonitorAssigner, CWnd* pParent = NULL);   // standard constructor
		virtual ~PlayBackDlg();
		virtual void stop();
		virtual void play();
	// Dialog Data
		enum { IDD = IDD_PLAY_BACK_DLG };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL OnInitDialog();
		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnDestroy();
		afx_msg void OnClose();
		afx_msg void OnBnClickedOk();
		afx_msg void OnBnClickedCancel();
		afx_msg void OnBnClickedBtnPlaybackHelp();
		afx_msg void OnBnClickedBtnPlaybackForward();
		afx_msg void OnBnClickedBtnPlaybackPlay();
		afx_msg void OnBnClickedBtnPlaybackRewind();
	private:
		enum PlayBtnStatus
		{
			PLAY,
			PAUSE
		};
		void updatePlayBtnStatus(PlayBtnStatus status);
		CButton m_btnRewind;
		CButton m_btnPlay;
		CButton m_btnForward;
		CStatic m_playBtnDescription;
		HBITMAP m_bitmapPause;
		HBITMAP m_bitmapPlay;
		VirtualMonitorAssigner& m_virtualMonitorAssigner;
	};
}