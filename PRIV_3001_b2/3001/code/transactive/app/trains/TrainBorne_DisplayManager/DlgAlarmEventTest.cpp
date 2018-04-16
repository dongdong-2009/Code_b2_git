// TrainDetailTest.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "VideoDisplayPresenter.h"
#include "DlgAlarmEventTest.h"
#include "bus/cctv/tvss_agent/tvssmanager/idl/src/TvssManagerCorbaDef.h"
// TrainDetailTest dialog
namespace TA_IRS_App
{
	IMPLEMENT_DYNAMIC(DlgAlarmEventTest, CDialog)

		DlgAlarmEventTest::DlgAlarmEventTest(VideoDisplayPresenter* presenter,CWnd* pParent /*=NULL*/)
		: m_presenter(presenter),CDialog(DlgAlarmEventTest::IDD, pParent)
	{

	}

	DlgAlarmEventTest::~DlgAlarmEventTest()
	{
	}

	void DlgAlarmEventTest::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);

	}


	BEGIN_MESSAGE_MAP(DlgAlarmEventTest, CDialog)
		ON_BN_CLICKED(IDC_VD_BU_AE_ALARM, &DlgAlarmEventTest::OnBnClickedBuSetAlarm)
		ON_BN_CLICKED(IDC_VD_BU_AE_ACKNOWLEAGEALARM, &DlgAlarmEventTest::OnBnClickedBuAcknowleageAlarm)
		ON_BN_CLICKED(IDC_VD_BU_TM_SETSMD, &DlgAlarmEventTest::OnBnClickedBuTVSSCmd)
	END_MESSAGE_MAP()


	// TrainDetailTest message handlers

	void DlgAlarmEventTest::OnBnClickedBuSetAlarm()
	{
		// TODO: Add your control notification handler code here
		UpdateData();
		char alarmStr[TRAINBORNE_ALARM_COUNT+1];
		memset(alarmStr,0,sizeof(alarmStr));
		UINT alarmCtrl;
		for( int alarmIndex=0;alarmIndex<TRAINBORNE_ALARM_COUNT;alarmIndex++)
		{
			alarmCtrl = IDC_VD_CH_AE_ALARM1 + alarmIndex;
			CWnd* pCtrl =GetDlgItem(alarmCtrl);
			CButton* pChecker =(CButton*)pCtrl;
			bool bChecked = pChecker->GetCheck();
			if(bChecked)
				alarmStr[alarmIndex/8] |= (1 <<(alarmIndex%8)) ;
		}
		m_presenter->TrainBorneAlarmEvent(alarmStr);
	}

	void DlgAlarmEventTest::OnBnClickedBuAcknowleageAlarm()
	{
		// TODO: Add your control notification handler code here
		UpdateData();
		CString strSession;
		GetDlgItemText(IDC_VD_ED_AE_SESSIONID,strSession);
		std::string sessionID = strSession.GetBuffer(0);
		CString strEntityKeys;
		GetDlgItemText(IDC_VD_ED_AE_ENTITYKEY,strEntityKeys);
		int pos ,orgpos= 0;
		do 
		{
			pos =strEntityKeys.Find(",",orgpos);
			std::string strEntity = strEntityKeys.Mid(orgpos,pos>0?pos-orgpos:strEntityKeys.GetLength()-orgpos);
			//unsigned long alarmEntityKey = GetDlgItemInt(IDC_VD_ED_AE_ENTITYKEY);
			unsigned long alarmEntityKey = atol(strEntity.c_str());
			m_presenter->TrainBorneAcknowleageAlarm(alarmEntityKey,sessionID.c_str());
			if(pos>=0)
				orgpos=pos+1;
		} while (pos>=0);
		
	}

	void DlgAlarmEventTest::OnBnClickedBuTVSSCmd()
	{
		// TODO: Add your control notification handler code here
		UpdateData();
		CString strCmd,strTrainID,strMonitorID,strInput0,strInoput1,strInput2,strInput3;
		int cmd = GetDlgItemInt(IDC_VD_ED_TM_CMD);
		int trainid = GetDlgItemInt(IDC_VD_ED_TM_TRAINID);
		int monitorid = GetDlgItemInt(IDC_VD_ED_TM_MONITORID);
		int input0 = GetDlgItemInt(IDC_VD_ED_TM_INPUT0);
		int input1 = GetDlgItemInt(IDC_VD_ED_TM_INPUT1);
		int input2 = GetDlgItemInt(IDC_VD_ED_TM_INPUT2);
		int input3 = GetDlgItemInt(IDC_VD_ED_TM_INPUT3);
		TA_Base_Bus::TVSSManagerCorbaDef::TVSSCommand command;
		command.cmdType = (TA_Base_Bus::TVSSManagerCorbaDef::TVSSCommandType)cmd;
		command.trainId = trainid;
		command.monitorId = monitorid;
		command.videoInput[0] = input0;
		command.videoInput[1] = input1;
		command.videoInput[2] = input2;
		command.videoInput[3] = input3;
		m_presenter->OnTVSSCommand((WPARAM)(&command));
	}

	BOOL DlgAlarmEventTest::OnInitDialog()
	{
		CDialog::OnInitDialog();

		// TODO:  Add extra initialization here
		UpdateData(FALSE);
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}
}

