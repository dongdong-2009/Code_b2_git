// TrainDetailTest.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgAlarmEventTest.h"
#include "app/cctv/video_display_manager/src/SwitchAgentCommunicator.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDlg.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"
#include "core/utilities/src/RunParams.h"
#include "core/alarm/idl/src/AlarmMessageCorbaDef.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
// TrainDetailTest dialog
using TA_Base_Core::TransactiveException;
namespace TA_IRS_App
{
	IMPLEMENT_DYNAMIC(DlgAlarmEventTest, CDialog)

		DlgAlarmEventTest::DlgAlarmEventTest(std::string locationName,CWnd* pParent /*=NULL*/)
		: CDialog(DlgAlarmEventTest::IDD, pParent),m_videoDisplayDlg((VideoDisplayManagerDlg*)pParent),m_locationName(locationName)
	{
		if(m_locationName == "KOCC")
		{
			m_locationName = "OCC";
		}
		TA_Base_Core::CorbaNameList names = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( 
			"VideoSwitchAgent", m_locationName,true );
		TA_ASSERT(names.size() > 0, "failed get corba name");
		m_videoSwitchObj.setCorbaName(names[0]);
	}

	DlgAlarmEventTest::~DlgAlarmEventTest()
	{
	}

	void DlgAlarmEventTest::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);

	}


	BEGIN_MESSAGE_MAP(DlgAlarmEventTest, CDialog)
		ON_BN_CLICKED(IDC_ALARMTEST_BU_ACKNOWLDGE_ALARM, &DlgAlarmEventTest::OnBnClickedBuAcknowleageAlarm)
		ON_BN_CLICKED(IDC_ALARMTEST_BU_CLOSE_ALARM, &DlgAlarmEventTest::OnBnClickedBuCloseAlarm)
		ON_BN_CLICKED(IDC_ALARMTEST_BU_RUNPARAM_CHANGE, &DlgAlarmEventTest::OnBnClickedBuRunParamChange)
		ON_BN_CLICKED(IDC_ALARMTEST_BU_PROCESS_TRIGGERING_ALARM, &DlgAlarmEventTest::OnBnClickedBuProcessTriggeringAlarm)
	END_MESSAGE_MAP()


	// TrainDetailTest message handlers

	void DlgAlarmEventTest::OnBnClickedBuAcknowleageAlarm()
	{
		// TODO: Add your control notification handler code here
		UpdateData();
		CString strAlarmID ;
		GetDlgItemText(IDC_ALARMTEST_ED_ALARMKEY,strAlarmID);
		unsigned long entityKey =  atoi(strAlarmID);
		try
		{
			TA_IRS_App::SwitchAgentCommunicator::getInstance().acknowledgeAlarm(entityKey);
		}
		catch(TransactiveException& e)
		{
			MessageBox(e.what());
		}


	}

	void DlgAlarmEventTest::OnBnClickedBuCloseAlarm()
	{
	}

	void DlgAlarmEventTest::OnBnClickedBuRunParamChange()
	{
		// TODO: Add your control notification handler code here
		UpdateData();
		CString strRunParamName,strRunParamValue ; 
		GetDlgItemText(IDC_ALARMTEST_ED_RUNPARAM_NAME,strRunParamName);
		GetDlgItemText(IDC_ALARMTEST_ED_RUNPARAM_VALUE,strRunParamValue);
		std::string runParamName = strRunParamName;
		std::string runParamValue = strRunParamValue;
		try
		{
			m_videoDisplayDlg->TestRunParamChange(runParamName,runParamValue);
		}
		catch(TransactiveException& e)
		{
			MessageBox(e.what());
		}
	}

	void DlgAlarmEventTest::OnBnClickedBuProcessTriggeringAlarm()
	{
		// TODO: Add your control notification handler code here
		UpdateData();
		CString strAlarmList ; 
		GetDlgItemText(IDC_ALARMTEST_ED_TRIGGERING_ALARMLIST,strAlarmList);
		bool isOpen = ((CButton*) GetDlgItem(IDC_ALARMTEST_CH_OPEN_TRIGGERINGALARM))->GetCheck();
		std::string alarmList = strAlarmList;
		char* p =(char*)alarmList.c_str();
		
		char chAlarm[1024];
		int stationId,eventId;
		int index =0;
		int alarmCount = 0;
		while(p<(char*)alarmList.c_str()+alarmList.length())
		{
			memset(chAlarm,0,sizeof(chAlarm));
			sscanf_s(p,"%s",chAlarm,sizeof(chAlarm));
			sscanf_s(chAlarm,"%d:%d",&stationId,&eventId);
			p+=strlen(chAlarm)+1;
			alarmCount++;
		}
		p =(char*)alarmList.c_str();
		TA_Base_Bus::SwitchManagerCorbaDef::TriggerAlarmSequence alarmSequence;
		alarmSequence.length(alarmCount);
		while(p<(char*)alarmList.c_str()+alarmList.length())
		{
			memset(chAlarm,0,sizeof(chAlarm));
			sscanf_s(p,"%s",chAlarm,sizeof(chAlarm));
			sscanf_s(chAlarm,"%d:%d",&stationId,&eventId);
			p+=strlen(chAlarm)+1;
			TA_Base_Bus::SwitchManagerCorbaDef::TriggerAlarm triggerAlarm;
			triggerAlarm.stationId = stationId;
			triggerAlarm.eventId = eventId;
			alarmSequence[index++]=triggerAlarm;
		}
		try
		{
			//TA_Base_Bus::VideoSwitchAgentFactory::getInstance().setLocationName("DT17");
			CORBA_CALL(TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getSwitchManager(),processAlarmStack,(alarmSequence,isOpen));
		}
		catch(TransactiveException& e)
		{
			MessageBox(e.what());
		}
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

