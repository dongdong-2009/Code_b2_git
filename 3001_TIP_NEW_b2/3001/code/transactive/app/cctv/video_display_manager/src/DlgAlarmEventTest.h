#pragma once

// TrainDetailTest dialog
#include "core/naming/src/namedobject.h"
#include "bus/cctv/video_switch_agent/IDL/src/SwitchManagerCorbaDef.h"

namespace TA_IRS_App
{
	typedef TA_Base_Core::NamedObject< TA_Base_Bus::SwitchManagerCorbaDef,
		TA_Base_Bus::SwitchManagerCorbaDef_ptr,
		TA_Base_Bus::SwitchManagerCorbaDef_var >		VideoSwitchNamedObject;
	class VideoDisplayManagerDlg;
	class DlgAlarmEventTest : public CDialog
	{
		DECLARE_DYNAMIC(DlgAlarmEventTest)

	public:
		DlgAlarmEventTest(std::string locationName,CWnd* pParent = NULL);   // standard constructor
		virtual ~DlgAlarmEventTest();

		// Dialog Data
		enum { IDD = IDD_DLG_ALARM_TEST };

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

		DECLARE_MESSAGE_MAP()
	public:
		
		afx_msg void OnBnClickedBuAcknowleageAlarm();
		afx_msg void OnBnClickedBuCloseAlarm();
		afx_msg void OnBnClickedBuRunParamChange();
		afx_msg void OnBnClickedBuProcessTriggeringAlarm();
	private:

		virtual BOOL OnInitDialog();
	private:
		std::string m_locationName;
		VideoSwitchNamedObject m_videoSwitchObj;
		VideoDisplayManagerDlg* m_videoDisplayDlg;
	};
}

