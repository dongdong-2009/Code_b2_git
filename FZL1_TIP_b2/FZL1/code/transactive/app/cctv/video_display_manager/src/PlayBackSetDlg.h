#pragma once
#include "afxdtctl.h"
#include "app/cctv/video_display_manager/src/CCTVCommon.h"

// PlayBackSetDlg dialog
namespace TA_IRS_App
{
	class PlayBackSetDlg : public CDialog
	{
		DECLARE_DYNAMIC(PlayBackSetDlg)

	public:
		PlayBackSetDlg(PlayBackTimePair& timeSetPair, CWnd* pParent = NULL);   // standard constructor
		virtual ~PlayBackSetDlg();

	// Dialog Data
		enum { IDD = IDD_DLG_PLAYBACK_SET };

	protected:
		virtual BOOL OnInitDialog();
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnBnClickedOk();
		afx_msg void OnBnClickedCancel();
		CDateTimeCtrl m_startDate;
		CDateTimeCtrl m_endDate;
		CDateTimeCtrl m_startTime;
		CDateTimeCtrl m_endTime;
		PlayBackTimePair& m_timeSetPair;
	};
}