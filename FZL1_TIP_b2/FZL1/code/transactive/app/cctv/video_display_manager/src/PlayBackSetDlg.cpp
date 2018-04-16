// PlayBackSetDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "app/cctv/video_display_manager/src/Resource.h"
#include "app/cctv/video_display_manager/src/PlayBackSetDlg.h"


namespace TA_IRS_App
{
	// PlayBackSetDlg dialog

	IMPLEMENT_DYNAMIC(PlayBackSetDlg, CDialog)

	PlayBackSetDlg::PlayBackSetDlg(PlayBackTimePair& timeSetPair, CWnd* pParent /*=NULL*/)
		: CDialog(PlayBackSetDlg::IDD, pParent),
		  m_timeSetPair(timeSetPair)
	{

	}

	PlayBackSetDlg::~PlayBackSetDlg()
	{
	}

	void PlayBackSetDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_PLAYBACK_START_DATE, m_startDate);
		DDX_Control(pDX, IDC_PLAYBACK_END_DATE, m_endDate);
		DDX_Control(pDX, IDC_PLAYBACK_START_TIME, m_startTime);
		DDX_Control(pDX, IDC_PLAYBACK_STOP_TIME, m_endTime);
	}


	BEGIN_MESSAGE_MAP(PlayBackSetDlg, CDialog)
		ON_BN_CLICKED(IDOK, &PlayBackSetDlg::OnBnClickedOk)
		ON_BN_CLICKED(IDCANCEL, &PlayBackSetDlg::OnBnClickedCancel)
	END_MESSAGE_MAP()


	BOOL PlayBackSetDlg::OnInitDialog()
	{
		CDialog::OnInitDialog();
		m_startTime.SetFormat("tt  h:mm:ss");
		m_endTime.SetFormat("tt  h:mm:ss");
		return TRUE;
	}
	// PlayBackSetDlg message handlers

	void PlayBackSetDlg::OnBnClickedOk()
	{
		CTime startDate, startTime, endDate, endTime;
		m_startDate.GetTime(startDate);
		m_startTime.GetTime(startTime);
		m_endDate.GetTime(endDate);
		m_endTime.GetTime(endTime);

        CTime startDateAndTime(startDate.GetYear(), startDate.GetMonth(), startDate.GetDay(),
            startTime.GetHour(), startTime.GetMinute(), startTime.GetSecond());
        CTime endDateAndTime(endDate.GetYear(), endDate.GetMonth(), endDate.GetDay(),
            endTime.GetHour(), endTime.GetMinute(), endTime.GetSecond());
        CTime currentDateAndTime = CTime::GetCurrentTime();

        if(startDateAndTime >= endDateAndTime)
        {
            MessageBox("开始时间必须小于结束时间.");
            return;
        }

        if(endDateAndTime >= currentDateAndTime)
        {
            MessageBox("结束时间必须小于当前时间.");
            return;
        }

		m_timeSetPair.startTime.year = startDateAndTime.GetYear();
		m_timeSetPair.startTime.month = startDateAndTime.GetMonth();
		m_timeSetPair.startTime.day = startDateAndTime.GetDay();
		m_timeSetPair.startTime.hour = startDateAndTime.GetHour();
		m_timeSetPair.startTime.minute = startDateAndTime.GetMinute();
		m_timeSetPair.startTime.second = startDateAndTime.GetSecond();

		m_timeSetPair.endTime.year = endDateAndTime.GetYear();
		m_timeSetPair.endTime.month = endDateAndTime.GetMonth();
		m_timeSetPair.endTime.day = endDateAndTime.GetDay();
		m_timeSetPair.endTime.hour = endDateAndTime.GetHour();
		m_timeSetPair.endTime.minute = endDateAndTime.GetMinute();
		m_timeSetPair.endTime.second = endDateAndTime.GetSecond();

		// TODO: fill up the time set pair struct
		OnOK();
	}

	void PlayBackSetDlg::OnBnClickedCancel()
	{
		// TODO: Add your control notification handler code here
		OnCancel();
	}
}