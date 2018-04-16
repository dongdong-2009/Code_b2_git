/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/maintenance_management/job_request_gui/src/JobRequestMgrDlg.cpp $
  * @author:  
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2012/11/27 14:49:38 $
  * Last modified by:  $Author: huangjian $
  * 
  *
  */

#include <sstream>
#include "app/maintenance_management/job_request_gui/src/stdafx.h"
#include "app/maintenance_management/job_request_gui/src/JobRequestMgr.h"
#include "app/maintenance_management/job_request_gui/src/resource.h"
#include "app/maintenance_management/job_request_gui/src/JobRequestMgrDlg.h"


#include "app/maintenance_management/job_request_gui/src/AuditMessageUtility.h"

#include "app/maintenance_management/job_request_gui/src/ATSMMSSubmitMgr.h"

#include "bus/generic_gui/src/IGUIAccess.h"

#include "bus\application_types\src\apptypes.h"

#include "bus/generic_gui/src/TransactiveMessage.h"
#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\DebugUtil.h"

#include "bus\user_settings\src\SettingsMgr.h"
#include "core\exceptions\src\UserSettingsException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "bus/maintenance_management/mmsHelper/src/MMSSubmitManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;
using TA_Base_Bus::TransActiveDialog;

namespace TA_IRS_App
{
	const CString MSG_DLG_TITLE = "�����걨��";
	const int PROB_REPORT_SIZE = 256;
	const int ALARM_MESSAGE_SIZE = 256;
	const int ALARM_REQUESTOR_SIZE = 20;
	const int ALARM_EQUIPMENT_SIZE = 38;

	CJobRequestMgrDlg::CJobRequestMgrDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, 
		const launchConditionsStruct lc, CWnd* pParent /*=NULL*/)
		: TransActiveDialog(genericGUICallback, CJobRequestMgrDlg::IDD, pParent),
		m_jobRequest(0)
	{
		FUNCTION_ENTRY("CJobRequestMgrDlg");

		 m_launchConditions = lc;
		 m_pendingLaunchConditions = m_launchConditions;


		//{{AFX_DATA_INIT(CJobRequestMgrDlg)
		m_assetId = _T("");
		m_requestor = _T("");
		m_problem = _T("");
		m_alarmMessage = _T("");
		m_compDate = 0;
		m_compTime = 0;
		m_dateEnabled = FALSE;
	//}}AFX_DATA_INIT
		


		FUNCTION_EXIT;
	}

	CJobRequestMgrDlg::~CJobRequestMgrDlg()
	{
		FUNCTION_ENTRY("~CJobRequestMgrDlg");
		if(0 != m_jobRequest)
		{
			delete m_jobRequest;
		}

		FUNCTION_EXIT;
	}

	void CJobRequestMgrDlg::init()
	{//TD18095, jianghp, to fix the performance of showing manager application
		try
		{
			//resolve the submitter object
            //m_submitter.setEntityName("MmsJobRequestSubmitter_occ",false);
            //if( m_submitter->_non_existent() ) throw "bugger";

            AuditMessageUtility::getInstance()->setJobRequestGuiKey(getGuiKey());
		}
		catch(...)
		{
			reportFailureAndTerminate("EAM AgentͨѶʧ��", "EAM AgentͨѶʧ��");
			return;
		}
		
		//Manually call the code for launch condition updates
		//so that the parameters are read from the launch conditions
		OnLaunchConditionsChange(0, 0);
	}

	void CJobRequestMgrDlg::DoDataExchange(CDataExchange* pDX)
	{
		TransActiveDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CJobRequestMgrDlg)
		DDX_Control(pDX, IDSUBMIT, m_submitBtn);
		DDX_Control(pDX, IDC_DATETIMEPICKER2, m_targetTimeCtr);
		DDX_Control(pDX, IDC_DATETIMEPICKER1, m_targetDateCtr);
		DDX_Control(pDX, IDC_CHECK1, m_dateEnableBtn);
		DDX_Control(pDX, IDC_ALARMMESSAGE_EDIT, m_alarmMessageCtr);
		DDX_Control(pDX, IDC_REQUESTOR_EDIT, m_requestorCtr);
		DDX_Control(pDX, IDC_EQUIP_EDIT, m_equipmentCtr);
		DDX_Control(pDX, IDC_PROBLEMREPORT_EDIT, m_reportCtr);
		DDX_Text(pDX, IDC_EQUIP_EDIT, m_assetId);
		DDX_Text(pDX, IDC_REQUESTOR_EDIT, m_requestor);
		DDX_Text(pDX, IDC_PROBLEMREPORT_EDIT, m_problem);
		DDX_Text(pDX, IDC_ALARMMESSAGE_EDIT, m_alarmMessage);
		DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER1, m_compDate);
		DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER2, m_compTime);
		DDX_Check(pDX, IDC_CHECK1, m_dateEnabled);
	//}}AFX_DATA_MAP
	}

	BEGIN_MESSAGE_MAP(CJobRequestMgrDlg, TransActiveDialog)
		//{{AFX_MSG_MAP(CJobRequestMgrDlg)
		ON_BN_CLICKED(IDSUBMIT, OnSubmit)
		ON_BN_CLICKED(IDC_CHECK1, OnDateEnableClick)
		ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER1, OnChangeDate)
		ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER2, OnChangeTime)
	//}}AFX_MSG_MAP
		ON_MESSAGE(LAUNCH_CONDITIONS_UPDATE, OnLaunchConditionsChange)
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CJobRequestMgrDlg message handlers

	BOOL CJobRequestMgrDlg::OnInitDialog()
	{
		FUNCTION_ENTRY("OnInitDialog");

		// If necessary this code can go in OnInitDialog before the call to TransActiveDialog::OnInitDialog()
		TA_Base_Bus::ResizingProperties properties;

		CRect rect;
		GetWindowRect( &rect );
		properties.canMaximise = false;
		properties.maxHeight = rect.Height();
		properties.maxWidth = rect.Width();
		properties.minWidth =rect.Width();
		properties.minHeight = rect.Height();
		setResizingProperties(properties);


		TransActiveDialog::OnInitDialog();

		//set static limits for controls:
		m_alarmMessageCtr.SetLimitText(ALARM_MESSAGE_SIZE);
		m_requestorCtr.SetLimitText(ALARM_REQUESTOR_SIZE);
		m_equipmentCtr.SetLimitText(ALARM_EQUIPMENT_SIZE);

		//force this to a top most window. I tried setting the style in the
		//dialog template but that seemed to have no effect...?
		SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

		m_reportCtr.SetFocus();

		FUNCTION_EXIT;
		return FALSE;  // return TRUE  unless you set the focus to a control
	}

	void CJobRequestMgrDlg::OnCancel() 
	{
		FUNCTION_ENTRY("OnCancel");
		TransActiveDialog::DestroyWindow();
	}

	/**
	* buildDate
	* @returns a time_t by combining the date from the m_compDate member
	* and the m_compTime member. Note that these members may not represent the
	* current values set in the controls
	*/
	time_t CJobRequestMgrDlg::buildDate()
	{
		if( !m_dateEnabled)
		{
			return 0;
		}
		FUNCTION_ENTRY("CJobRequestMgrDlg");
		int nYear = m_compDate.GetYear();
		int nMonth = m_compDate.GetMonth();
		int nDay = m_compDate.GetDay();
		int nHour = m_compTime.GetHour();
		int nMin = m_compTime.GetMinute();
		int nSec = m_compTime.GetSecond();

        // ++TD13664 : AZ 21/11/05
		int nDST = -1; // automatically set Daylight Saving Time
        // TD13664++

        CTime combined( nYear, nMonth, nDay, nHour, nMin, nSec, nDST );
		return combined.GetTime();
	}

	//precondition control data variables have been updated
	//
	//return true if date is not enabled or if it is enabled
	//and is set to a value later than now, 
	//else set new min limit to now on control and return false
	bool CJobRequestMgrDlg::validateDate()
	{

		//don't care what the date is set to if it ain't enabled
		if(!m_dateEnabled) return true;

		time_t dateEntered = buildDate();
		time_t now;
		time(&now);
		if(now < dateEntered) return true;

		//bugger - bad date
		//set the min value for the date and time control.
		CTime earliest(now);

		m_targetDateCtr.SetRange( &earliest,0 );
		m_targetTimeCtr.SetRange( &earliest,0 );

		return false;
	}

	void CJobRequestMgrDlg::OnSubmit()
	{
		FUNCTION_ENTRY("OnSubmit");
		//push the data from the GUI to the members 

		UpdateData(TRUE);

		//return true if date is not enabled or if it is enabled
		//and is set to a value later than now, 
		//else set new min limit to now on control and return false
		if(!validateDate())
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_190011,MSG_DLG_TITLE);

			FUNCTION_EXIT;
			return;
		}

		try
		{
			//Set the Job Request fields to the specified values
			m_jobRequest->assetId = (const char*)m_assetId;
			m_jobRequest->targetDate = buildDate();
			m_jobRequest->targetDateEnabled = m_dateEnabled != 0;
			m_jobRequest->problemReport = (const char*)m_problem;

			bool ret = false;
			if (m_launchConditions.isATSAlarm)
			{ // Submit ATS alarms
				ret =  ATSMMSSubmitMgr::getInstance()->getATSSubmitter()->submitJobRequest(*m_jobRequest);
			}
			else
			{
				ret=TA_Base_Bus::MMSSubmitManager::getInstance()->getSubmitter()->submitJobRequest(*m_jobRequest,getSessionId());
				if (ret==false)
				{
					std::string displayStr = "EAM AgentͨѶʧ��.";
					reportFailureAndTerminate(displayStr, displayStr);
				}
				else
				{
					//if it did not throw the submission was a success 
					LogSuccessEvent();
				}
			}
			
			if (ret==true)
			{
				//CDialog::OnOK();	
				TransActiveDialog::DestroyWindow(); // TD18982 - exit from a modeless dialog, use destroywindow()
			}
		
		
		}
		catch(TA_Base_Bus::DependencyFailure& ex)
		{
			std::string disPlayStr = "";
			switch(ex.eType)
			{
			case TA_Base_Bus::DEPENDENCYFAILURE_MMS_SERVER_NO_CONTACTED:
				disPlayStr = "EAM AgentͨѶʧ��";
				break;
			case TA_Base_Bus::DEPENDENCYFAILURE_ALARM_AGENT_NO_CONTACTED:
				disPlayStr = "Alarm AgentͨѶʧ��";
				break;
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_OPERATOR_DETAIL:
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_SPECIFIED_ALARM:
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_SPECIFIED_DATAPOINT:
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_EQUIPMENT_DATA:
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_ALARM_DATA:
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_SESSION:
				disPlayStr = "�޷���ȡ��Ӧ������";
				break;
			default:
				break;
			}
			reportFailureAndTerminate(disPlayStr, disPlayStr);
		}
		catch(TA_Base_Bus::SubmissionDenied& ex)
		{
			std::string displayStr = "";
            switch(ex.eType)
		    {
			case TA_Base_Bus::SUBMISSIONDENIED_NO_PRIVILEGE:
				displayStr = "û��Ȩ��ִ�иö���";
				break;
			case TA_Base_Bus::SUBMISSIONDENIED_DATAPOINT_MMS_INHIBITED:
				displayStr = "�����ݵ��Ӧ�ĸ澯��MMS��ֹ" ;
				break;
			case TA_Base_Bus::SUBMISSIONDENIED_GLOBAL_MMS_INHIBITED:
				displayStr = "ȫ��EAM��ֹ������";
				break;
			case TA_Base_Bus::SUBMISSIONDENIED_ALARM_ALREADY_RAISED:
				displayStr = "�����걨���Ѿ����ɹ��ύ������Ӧ�ĸ澯û�б�����";
				break;
			case TA_Base_Bus::SUBMISSIONDENIED_ALARM_NORMALIZED:
				displayStr = "�澯�ָ�����";
				break;
			case TA_Base_Bus::SUBMISSIONDENIED_ALARM_SUPPRESSED:
				displayStr = "�澯������";
				break;
			case TA_Base_Bus::SUBMISSIONDENIED_NO_DETERMINE_INHIBITION_STATE:
				displayStr = "����ȷ����ֹ��״̬";
				break;
			default:
				break;
		    }
			reportSubmissionDenial(displayStr, displayStr, (ex.rights == false) );
		}
		catch(...)
		{
			std::string displayStr = "EAM AgentͨѶʧ��.";
			reportFailureAndTerminate(displayStr, displayStr);
		}
		
		FUNCTION_EXIT;
		
	}



	void CJobRequestMgrDlg::updateLaunchConditions(const launchConditionsStruct lc)
	{
		FUNCTION_ENTRY("updateLaunchConditions");
		
		m_pendingLaunchConditions = lc;

		if(::IsWindow(m_hWnd))
		{
			//send an message to tell the dlg 
			PostMessage(LAUNCH_CONDITIONS_UPDATE, 0, 0);
		}
		FUNCTION_EXIT;

	}

	LRESULT CJobRequestMgrDlg::OnLaunchConditionsChange(WPARAM wParam, LPARAM lParam)
	{
		FUNCTION_ENTRY("OnLaunchConditionsChange");
		m_launchConditions = m_pendingLaunchConditions;


		try
		{
			switch(m_launchConditions.launchedFrom)
			{
				case FROM_ALARM:
				{
					initFromAlarm();
					break;
				}
				case FROM_SCHEMATIC:
				{
					initFromSchematic();
					break;
				}
				case INVALID_PARAMS:
				{
					reportFailureAndTerminate("�޷���ȡ��Ӧ������", "�޷���ȡ��Ӧ������");
					break;
				}
				default:
				{
					TA_ASSERT(false, "Launch condition struct is bad.");
				}

			}
		}
		catch(TA_Base_Bus::DependencyFailure& ex)
		{
			std::string disPlayStr = "";
			switch(ex.eType)
			{
			case TA_Base_Bus::DEPENDENCYFAILURE_MMS_SERVER_NO_CONTACTED:
				disPlayStr = "EAM AgentͨѶʧ��";
				break;
			case TA_Base_Bus::DEPENDENCYFAILURE_ALARM_AGENT_NO_CONTACTED:
				disPlayStr = "Alarm AgentͨѶʧ��";
				break;
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_OPERATOR_DETAIL:
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_SPECIFIED_ALARM:
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_SPECIFIED_DATAPOINT:
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_EQUIPMENT_DATA:
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_ALARM_DATA:
			case TA_Base_Bus::DEPENDENCYFAILURE_NO_SESSION:
				disPlayStr = "�޷���ȡ��Ӧ������";
				break;
			default:
				break;
			}
			reportFailureAndTerminate(disPlayStr, disPlayStr);
		}

		if(::IsWindow(m_hWnd))
		{
			//push the data from the members to the GUI
			UpdateData(FALSE);
			OnDateEnableClick(); //set the enable/disable state A/R 
		}

		FUNCTION_EXIT;
		return S_OK;
	}

	unsigned int CJobRequestMgrDlg::getGuiKey()
	{
		FUNCTION_ENTRY("CJobRequestMgrDlg");
		unsigned int key = 0;

		try
		{
			TA_Base_Core::IEntityData* pData = getGenericGUICallbackObject().getGUIEntity();
			key =  pData->getKey();
		}
		catch(...)
		{
			//TODO
		}
		FUNCTION_EXIT;
		return key;

	}

	std::string CJobRequestMgrDlg::getSessionId()
	{
		FUNCTION_ENTRY("getSessionId");
		std::string sessionId;
		try
		{
			sessionId = getGenericGUICallbackObject().getSessionId();
			if( sessionId.empty() ) throw "bugger";
		
		}
		catch(...)
		{
			throw TA_Base_Bus::DependencyFailure( "Could not retrieve the session ID.",TA_Base_Bus::DEPENDENCYFAILURE_NO_SESSION );
		}
		FUNCTION_EXIT;

		return sessionId;
	}


	void CJobRequestMgrDlg::initFromAlarm()
	{
		FUNCTION_ENTRY("initFromAlarm");
		m_jobRequest = 0;
		try
		{
// 			CORBA_CALL_RETURN( m_jobRequest,
// 			                   m_submitter,
// 			                   getDataFromAlarm,
// 			                   ( m_launchConditions.alarmId.c_str(), m_launchConditions.alarmLocationKey, getSessionId().c_str() ) );
			if (true == m_launchConditions.isATSAlarm)
			{

				m_jobRequest= ATSMMSSubmitMgr::getInstance()->getATSSubmitter()->getDataFromAlarmStore(m_launchConditions.alarmId);
			}
			else
			{
				m_jobRequest=TA_Base_Bus::MMSSubmitManager::getInstance()->getSubmitter()->getDataFromAlarm(m_launchConditions.alarmId,
					 m_launchConditions.alarmLocationKey, getSessionId());
			}
			if (m_jobRequest==NULL) throw TA_Base_Bus::DependencyFailure( "The retrieval of the alarm data failed", TA_Base_Bus::DEPENDENCYFAILURE_NO_ALARM_DATA );
		}
		catch(TA_Base_Bus::DependencyFailure& ex)
		{
			throw TA_Base_Bus::DependencyFailure( ex.reason.in(), ex.eType );
		}
		catch(...)
		{
			throw TA_Base_Bus::DependencyFailure( "The retrieval of the alarm data failed",TA_Base_Bus::DEPENDENCYFAILURE_NO_ALARM_DATA );
		}

		initData();
		FUNCTION_EXIT;
	}

	void CJobRequestMgrDlg::initFromSchematic()
	{
		FUNCTION_ENTRY("initFromSchematic");
		m_jobRequest = 0;
		try
		{
// 			CORBA_CALL_RETURN( m_jobRequest,
// 			                   m_submitter,
// 			                   getDataFromSymbol,
// 			                   ( m_launchConditions.equipmentName.c_str(), getSessionId().c_str() ) );
			m_jobRequest=TA_Base_Bus::MMSSubmitManager::getInstance()->getSubmitter()->getDataFromSymbol(m_launchConditions.equipmentName,
				 getSessionId());
			if (m_jobRequest==NULL) throw TA_Base_Bus::DependencyFailure( "The retrieval of the equipment data failed" ,TA_Base_Bus::DEPENDENCYFAILURE_NO_EQUIPMENT_DATA );
		}
		catch(TA_Base_Bus::DependencyFailure& ex)
		{
			throw TA_Base_Bus::DependencyFailure( ex.reason.in(),ex.eType );
		}
		catch(...)
		{
			throw TA_Base_Bus::DependencyFailure( "The retrieval of the equipment data failed" ,TA_Base_Bus::DEPENDENCYFAILURE_NO_EQUIPMENT_DATA );
		}

		initData();
		FUNCTION_EXIT;
	}

	void CJobRequestMgrDlg::initData()
	{
		FUNCTION_ENTRY("initData");
		m_assetId = m_jobRequest->assetId;
		m_requestor = m_jobRequest->requestor;
		m_problem = m_jobRequest->problemReport;
		char alarmDTStr[50] = {0};
		
		//Alarm message should display : <Alarm Date/Time>, <Alarm Fault Code>, <Alarm Description>
		if(m_launchConditions.launchedFrom == FROM_ALARM)
		{
			CTime alarmDT = m_jobRequest->alarmTime;
			sprintf(alarmDTStr,"%02d/%02d/%04d %02d:%02d:%02d", 
				alarmDT.GetDay(), 
				alarmDT.GetMonth(),
				alarmDT.GetYear(),
				alarmDT.GetHour(),
				alarmDT.GetMinute(),
				alarmDT.GetSecond() ); 

			std::stringstream alarmMsg;
			alarmMsg<<alarmDTStr;

			alarmMsg<<","<<m_jobRequest->alarmFaultCode<<","<<m_jobRequest->alarmDescField;

			m_alarmMessage = alarmMsg.str().c_str();
		}
		

		m_compDate = m_jobRequest->targetDate;
		m_compTime = m_jobRequest->targetDate;
		m_dateEnabled = m_jobRequest->targetDateEnabled;		

		//set the min value for the date and time control.
		time_t now;
		time(&now);
		CTime earliest(now);

		m_targetDateCtr.SetRange( &earliest,0 );
		m_targetTimeCtr.SetRange( &earliest,0 );

		//set the max number of chars for the report
// 		int alarmMessageLen = m_alarmMessage.GetLength();
// 		int maxProb = 256;
// 		if(0 != alarmMessageLen)
// 		{
// 			maxProb = 256 - alarmMessageLen - 2;
// 		}

		m_reportCtr.SetLimitText( PROB_REPORT_SIZE);

		if (m_launchConditions.isATSAlarm)
		{
			m_dateEnabled = false;
			m_dateEnableBtn.EnableWindow(FALSE);
			m_reportCtr.EnableWindow(FALSE);
			
			// This will set the control to empty
			m_targetDateCtr.SetFormat(" ");
			m_targetTimeCtr.SetFormat(" ");
		}
		else
		{
			m_dateEnableBtn.EnableWindow(TRUE);
			m_reportCtr.EnableWindow(TRUE);
			
			// This will set the control to its default format
			m_targetDateCtr.SetFormat(NULL);
			m_targetTimeCtr.SetFormat(NULL);
		}

		FUNCTION_EXIT;
	}

	void CJobRequestMgrDlg::reportFailureAndTerminate(const std::string& displayStr,
		const std::string& eventStr)
	{
		FUNCTION_ENTRY("reportFailureAndTerminate");

		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << displayStr;
		userMsg.showMsgBox(IDS_UE_213000, MSG_DLG_TITLE);

		AuditMessageUtility::getInstance()->submitJobRequestFailureMessage(eventStr, m_jobRequest);
		
		//OnCancel();		// if the first init() failed, will crash because it will still execute "m_reportCtr.SetFocus();"
		if (::IsWindow(GetSafeHwnd()))
		{
			PostMessage(WM_CLOSE);
		}

		FUNCTION_EXIT;
	}

	void CJobRequestMgrDlg::reportSubmissionDenial(const std::string& displayStr,
		const std::string& eventStr, bool terminate /*= true*/)
	{
		FUNCTION_ENTRY("reportSubmissionDenial");

		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << displayStr;
		userMsg.showMsgBox(IDS_UW_213001,MSG_DLG_TITLE);

		AuditMessageUtility::getInstance()->submitJobRequestDenialMessage(eventStr, m_jobRequest);

		if(terminate)
		{
            OnCancel();
		}

		FUNCTION_EXIT;

	}

	void CJobRequestMgrDlg::LogSuccessEvent()
	{
		FUNCTION_ENTRY("LogSuccessEvent");
		AuditMessageUtility::getInstance()->submitJobRequestSubmittedMessage(m_jobRequest);
		FUNCTION_EXIT;
	}

	void CJobRequestMgrDlg::OnDateEnableClick() 
	{
		FUNCTION_ENTRY("CJobRequestMgrDlg");
		//The date/time controls should be enabled only if this is checked
		if(1 == m_dateEnableBtn.GetCheck()) //1 == checked
		{
			m_targetTimeCtr.EnableWindow(TRUE);
			m_targetDateCtr.EnableWindow(TRUE);
		}
		else //is unchecked
		{
			m_targetTimeCtr.EnableWindow(FALSE);
			m_targetDateCtr.EnableWindow(FALSE);
		}
		FUNCTION_EXIT;		
	}

	void CJobRequestMgrDlg::OnChangeDate(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY("OnChangeDate");
		// we set the other control to the same value as this one so that 
		//the validation works
		CTime t;
		m_targetDateCtr.GetTime(t);
		m_targetTimeCtr.SetTime(&t);
		
		*pResult = 0;
		FUNCTION_EXIT;
	}

	void CJobRequestMgrDlg::OnChangeTime(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY("OnChangeTime");
		// we set the other control to the same value as this one so that 
		//the validation works
		CTime t;
		m_targetTimeCtr.GetTime(t);
		m_targetDateCtr.SetTime(&t);
		
		*pResult = 0;
		FUNCTION_EXIT;
	}


} //end namespace TA_IRS_App








