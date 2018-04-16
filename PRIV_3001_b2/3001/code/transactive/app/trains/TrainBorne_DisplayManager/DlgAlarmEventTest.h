#pragma once

// TrainDetailTest dialog
#define TRAINBORNE_ALARM_COUNT		75
namespace TA_IRS_App
{
	class VideoDisplayPresenter;
	class DlgAlarmEventTest : public CDialog
	{
		DECLARE_DYNAMIC(DlgAlarmEventTest)

	public:
		DlgAlarmEventTest(VideoDisplayPresenter* presenter,CWnd* pParent = NULL);   // standard constructor
		virtual ~DlgAlarmEventTest();

		// Dialog Data
		enum { IDD = IDD_VIDEODISPLAYMANAGER_ALARMEVENT_TEST };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		afx_msg void OnBnClickedBuSetAlarm();
		afx_msg void OnBnClickedBuAcknowleageAlarm();
		afx_msg void OnBnClickedBuTVSSCmd();
	private:

		virtual BOOL OnInitDialog();
	private:
		VideoDisplayPresenter* m_presenter;
	};
}

