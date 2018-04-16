/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Workfile: CongestionParameterDlg.cpp $
  * @author:  Liu Yu
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Derived from CDialog to allow user to set congestion parameters.
  */


#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/ecs/ecs_manager/src/stdafx.h"
#include <algorithm>

#include "app/ecs/ecs_manager/src/CongestionParameterDlg.h"
#include "app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h"
#include "app/ecs/ecs_manager/src/MappedECSDataCache.h"
#include "app/ecs/ecs_manager/src/RightsMgrSingleton.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/TA_MessageResource/resource.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ECSManager.h"
#include "core/data_access_interface/src/GlobalParameterAccessFactory.h"
#include "core/data_access_interface/src/IConfigGlobalParameter.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"


#define MIN_TRAIN_DEADBAND		1
#define MIN_TRAIN_TEMP			30
#define MAX_TRAIN_TEMP			60
#define MAX_STALL_TIME_SECS		59
#define MAX_STALL_TIME_MINS		10000

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{
	const int TOP_APPLY_OFFSET = 564;
	const int LEFT_APPLY_OFFSET = 236;

    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }

    static bool isTrue(const std::string& value)
    {
        std::string ucValue = toUpper(value);
    
        return (
            ucValue == "T" ||
            ucValue == "TRUE" ||
            ucValue == "1" ||
            ucValue == "Y" ||
            ucValue == "YES" );
    }

	CCongestionParameterDlg::CCongestionParameterDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent /*=NULL*/)
		: TransActiveDialog( genericGUICallback, CCongestionParameterDlg::IDD, pParent ),
          m_thresholdStallTimeInSecs(NULL),
          m_thresholdTemperature(NULL),
          m_temperatureDeadband(NULL),
          m_congestionUsesStallTime(NULL),
          m_congestionUsesTemperature(NULL)
	{
		//{{AFX_DATA_INIT(CCongestionParameterDlg)
		//}}AFX_DATA_INIT

		m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );

		// If necessary this code can go in OnInitDialog before the call to TransActiveDialog::OnInitDialog()
        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise	= true;
        properties.maxHeight	= -1;
        properties.maxWidth		= -1;
        properties.minWidth		= 458;
        properties.minHeight	= 654;
        setResizingProperties(properties);
		
		m_isInitialised = false;
		m_isAllowedToSetParameter = false;
		m_hasChanged = false;

	}

	void CCongestionParameterDlg::DoDataExchange(CDataExchange* pDX)
	{
		TransActiveDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CCongestionParameterDlg)
	    DDX_Control(pDX, IDOK, m_ApplyButton);
		DDX_Control(pDX, IDC_TRAIN_TEMP_CHECK, m_TrainTemp_Check);
		DDX_Control(pDX, IDC_TEMP_SLIDER, m_Temp_Slider);
        DDX_Control(pDX, IDC_DEADBAND_SLIDER, m_Temp_DeadBand_Slider);
		DDX_Control(pDX, IDC_STATUS_EDIT, m_StatusEdit);
		DDX_Control(pDX, IDC_STALL_TIME_CHECK, m_StallTime_Check);
		DDX_Control(pDX, IDC_TEMP_TEXT, m_TempLabel);
        DDX_Control(pDX, IDC_DEADBAND_TEXT, m_DeadBandLabel);
		DDX_Control(pDX, IDC_SPIN2, m_SecondSpin);
		DDX_Control(pDX, IDC_SPIN1, m_MinuteSpin);		
		DDX_Control(pDX, IDC_EDIT2, m_SecondEdit);
		DDX_Control(pDX, IDC_EDIT1, m_MinuteEdit);
	//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CCongestionParameterDlg, TransActiveDialog)
		//{{AFX_MSG_MAP(CCongestionParameterDlg)
		ON_BN_CLICKED(IDCLOSE, OnClose)
		ON_WM_HSCROLL()
		ON_BN_CLICKED(IDC_STALL_TIME_CHECK, OnStallTimeCheck)
		ON_BN_CLICKED(IDC_TRAIN_TEMP_CHECK, OnTrainTempCheck)
		ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		ON_COMMAND(ID_HELP_INDEX, OnAppHelp)
		ON_BN_CLICKED(IDOK, OnApply)
		ON_EN_CHANGE(IDC_MINUTE, OnChangeMinute)
		ON_EN_CHANGE(IDC_SECOND, OnChangeSecond)
		ON_MESSAGE(WM_READ_PARAMETERS, onReadCongestionParameters)
		ON_MESSAGE(WM_CHECK_PERMISSIONS, onCheckPermissions)
		ON_WM_SHOWWINDOW()
		ON_WM_SIZE()
		ON_BN_CLICKED(IDHELP, OnHelpButton) //TD16730
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	void CCongestionParameterDlg::init()
	{//TD18095, jianghp, to fix the performance of showing manager application
		// If necessary this code can go in OnInitDialog before the call to TransActiveDialog::OnInitDialog()
        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise	= true;
        properties.maxHeight	= -1;
        properties.maxWidth		= -1;
        properties.minWidth		= 458;
        properties.minHeight	= 654;
        setResizingProperties(properties);
	}
	
	void CCongestionParameterDlg::OnClose() 
	{	
		// 10070-Start
		// Delete if the RightsMgrSingleton Library has been created
		CRightsMgrSingleton::getInstance()->DeleteInstance();
		// 10070-End
		TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);
        cleanupGlobalParamters();		
		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();
	}

    void CCongestionParameterDlg::cleanupGlobalParamters()
	{
        if (m_thresholdStallTimeInSecs)
        {
            delete m_thresholdStallTimeInSecs;
            m_thresholdStallTimeInSecs = NULL;
        }
        if (m_thresholdTemperature)
        {
            delete m_thresholdTemperature;
            m_thresholdTemperature = NULL;
        }
        if (m_temperatureDeadband)
        {
            delete m_temperatureDeadband;
            m_temperatureDeadband = NULL;
        }
        if (m_congestionUsesStallTime)
        {
            delete m_congestionUsesStallTime;
            m_congestionUsesStallTime = NULL;
        }
        if (m_congestionUsesTemperature)
        {
            delete m_congestionUsesTemperature;
            m_congestionUsesTemperature = NULL;
        }
	}

    void CCongestionParameterDlg::initialiseGlobalParameters()
	{	
        unsigned long ecsSubsystemKey = ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getSubsystem();

        cleanupGlobalParamters();
        
        m_thresholdStallTimeInSecs = dynamic_cast<IConfigGlobalParameter*>
            (GlobalParameterAccessFactory::getInstance().getGlobalParameterByNameAndSubsystem
			(TA_IRS_Bus::ECS_GLOBAL_PARAMETER_THRESHOLD_STALL_TIME, ecsSubsystemKey, true));

        m_thresholdTemperature = dynamic_cast<IConfigGlobalParameter*>
            (GlobalParameterAccessFactory::getInstance().getGlobalParameterByNameAndSubsystem
             (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_THRESHOLD_TEMPERATURE, ecsSubsystemKey, true));

        m_temperatureDeadband = dynamic_cast<IConfigGlobalParameter*>
            (GlobalParameterAccessFactory::getInstance().getGlobalParameterByNameAndSubsystem
             (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_TEMPERATURE_DEADBAND, ecsSubsystemKey, true));

        m_congestionUsesStallTime = dynamic_cast<IConfigGlobalParameter*>
            (GlobalParameterAccessFactory::getInstance().getGlobalParameterByNameAndSubsystem
             (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_CONGESTION_USES_STALL_TIME, ecsSubsystemKey, true));

        m_congestionUsesTemperature = dynamic_cast<IConfigGlobalParameter*>
            (GlobalParameterAccessFactory::getInstance().getGlobalParameterByNameAndSubsystem
             (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_CONGESTION_USES_TEMPERATURE, ecsSubsystemKey, true));
	}

	BOOL CCongestionParameterDlg::OnInitDialog() 
	{
		TransActiveDialog::OnInitDialog();

		// register to receive updates if SessionID changes
		TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

		SetIcon( m_hIcon, TRUE );  

		// initialise some GUI components
		m_MinuteEdit.SetWindowText("0");
		m_SecondEdit.SetWindowText("0");		
		m_MinuteSpin.SetBuddy(&m_MinuteEdit);
		m_MinuteSpin.SetRange(0, MAX_STALL_TIME_MINS);
		m_SecondSpin.SetBuddy(&m_SecondEdit);
		m_SecondSpin.SetRange(0, MAX_STALL_TIME_SECS);

		// label
		CWnd *wnd = GetDlgItem(IDC_TEMP_LOW);
		if (wnd)
		{
			CString s;
			s.Format("%d", MIN_TRAIN_TEMP);
			wnd->SetWindowText(s);
		}

		// label
		wnd = GetDlgItem(IDC_TEMP_HIGH);
		if (wnd)
		{
			CString s;
			s.Format("%d", MAX_TRAIN_TEMP);
			wnd->SetWindowText(s);
		}

		// slider
		m_Temp_Slider.SetPos(0);
		m_Temp_Slider.SetRange(MIN_TRAIN_TEMP, MAX_TRAIN_TEMP, TRUE);							

        m_Temp_DeadBand_Slider.SetPos(0);
        m_Temp_DeadBand_Slider.SetRange(MIN_TRAIN_DEADBAND, MIN_TRAIN_TEMP,TRUE);
		return TRUE;  
	}

	void CCongestionParameterDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
		if (m_isInitialised)
		{
            if(m_Temp_Slider.GetSafeHwnd() == pScrollBar->GetSafeHwnd())
            {
			    int pos = m_Temp_Slider.GetPos();
			    CString tempLabel;

			    tempLabel.Format("%d",pos);
			    m_TempLabel.SetWindowText(tempLabel);

            }
            else if(m_Temp_DeadBand_Slider.GetSafeHwnd() == pScrollBar->GetSafeHwnd())
            {
			    int pos = m_Temp_DeadBand_Slider.GetPos();
			    CString tempLabel;

			    tempLabel.Format("%d",pos);
			    m_DeadBandLabel.SetWindowText(tempLabel);

            }
			m_hasChanged = true;
			m_ApplyButton.EnableWindow();
		}

		TransActiveDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}

	LRESULT CCongestionParameterDlg::onCheckPermissions(WPARAM wParam, LPARAM lParam)
	{		
		CheckPermissions();
		
		return 1;
	}

	LRESULT CCongestionParameterDlg::onReadCongestionParameters(WPARAM wParam, LPARAM lParam)
	{
        try
        {
            initialiseGlobalParameters();

			bool bUsesStallTime = true;
            bool bUsesTemperature = true;
			int	nTemperature = 0;
            int nTempDeadBand = 0;
            int nStallTime = 0;
			std::string value;
			
			// Is congestion using stall time
			value = m_congestionUsesStallTime->getValue();
            bUsesStallTime = isTrue(value);

            m_inParams.bUsesStallTime = bUsesStallTime;
			
			// Get the Threshold stall time (in seconds !!!!)
			//if (bUsesStallTime)
			{
                value = m_thresholdStallTimeInSecs->getValue();				
				nStallTime = (int)atoi(value.c_str());
			}

			m_inParams.nStallTime = nStallTime;
			
			// Is congestion using train temperature
            value = m_congestionUsesTemperature->getValue();
            bUsesTemperature = isTrue(value);


			m_inParams.bUsesTrainTemp = bUsesTemperature;

			// Get train temperature
			//if (bUsesTemperature)
			{
                value = m_thresholdTemperature->getValue();
				nTemperature = (int)atoi(value.c_str());

                value = m_temperatureDeadband->getValue();
                nTempDeadBand = (int)atoi(value.c_str());
			}
			m_inParams.nTrainTemp = nTemperature;
            m_inParams.nTempDeadBand = nTempDeadBand;


            // Adjust GUI()
			m_StallTime_Check.SetCheck(bUsesStallTime ? BST_CHECKED : BST_UNCHECKED);

			int mins = nStallTime / 60;
			int secs = nStallTime % 60;
			CString s;

			s.Format("%d", mins);
			m_MinuteEdit.SetWindowText(s);
			m_MinuteSpin.SetPos(mins);			

			s.Format("%d", secs);
			m_SecondEdit.SetWindowText(s);
			m_SecondSpin.SetPos(secs);			

            EnableStallTimeRelatedGUI(bUsesStallTime);

			m_TrainTemp_Check.SetCheck(bUsesTemperature ? BST_CHECKED : BST_UNCHECKED);

            m_Temp_Slider.SetPos(nTemperature);			

            m_Temp_DeadBand_Slider.SetPos(nTempDeadBand);			

            CString st;
            st.Format("%d", nTemperature);
            m_TempLabel.SetWindowText(st);

            CString sd;
            sd.Format("%d", nTempDeadBand);
            m_DeadBandLabel.SetWindowText(sd);

            EnableTempRelatedGUI(bUsesTemperature);

            m_StallTime_Check.EnableWindow(TRUE);
		    m_TrainTemp_Check.EnableWindow(TRUE);		

			WriteStatusLine("Loaded global Congestion parameters.");
			m_isInitialised = true;
        } // try
        catch(...)
		{
			m_StallTime_Check.EnableWindow(FALSE);			
			m_TrainTemp_Check.EnableWindow(FALSE);	

			EnableTempRelatedGUI(false);
			EnableStallTimeRelatedGUI(false);
		
			WriteStatusLine("Unable to load global Congestion parameters.");
		}

		return 1;
	}


	void CCongestionParameterDlg::OnStallTimeCheck() 
	{
		if (m_isInitialised)
		{
			if (!m_hasChanged)
				m_hasChanged = true;

			m_ApplyButton.EnableWindow();
		
			if (m_StallTime_Check.GetCheck() == BST_CHECKED)		
				EnableStallTimeRelatedGUI(true);
			else
				EnableStallTimeRelatedGUI(false);
		}
	}


	void CCongestionParameterDlg::OnTrainTempCheck() 
	{
		if (m_isInitialised)
		{
			if (!m_hasChanged)
				m_hasChanged = true;

			m_ApplyButton.EnableWindow();
		
			if (m_TrainTemp_Check.GetCheck() == BST_CHECKED)
				EnableTempRelatedGUI(true);
			else
				EnableTempRelatedGUI(false);		
		}
	}


	void CCongestionParameterDlg::EnableStallTimeRelatedGUI(bool enable)
	{
		BOOL flag = enable ? TRUE : FALSE;
		
		m_MinuteEdit.EnableWindow(flag);
		m_MinuteSpin.EnableWindow(flag);
		m_SecondEdit.EnableWindow(flag);
		m_SecondSpin.EnableWindow(flag);

		CWnd *wnd = GetDlgItem(IDC_MIN_TEXT);
		if (wnd)
			wnd->EnableWindow(flag);
		
		wnd = GetDlgItem(IDC_SEC_TEXT);
		if (wnd)
			wnd->EnableWindow(flag);
	}

	void CCongestionParameterDlg::EnableTempRelatedGUI(bool enable)
	{
		BOOL flag = enable ? TRUE : FALSE;

		m_Temp_Slider.EnableWindow(flag);

		CWnd *wnd = GetDlgItem(IDC_TEMP_TEXT);
		if (wnd)
			wnd->EnableWindow(flag);

		wnd = GetDlgItem(IDC_TEMP_LOW);
		if (wnd)
			wnd->EnableWindow(flag);

		wnd = GetDlgItem(IDC_TEMP_HIGH);
		if (wnd)
			wnd->EnableWindow(flag);

        m_Temp_DeadBand_Slider.EnableWindow(flag);
        wnd = GetDlgItem(IDC_DEADBAND_TEXT);
		if (wnd)
			wnd->EnableWindow(flag);

		wnd = GetDlgItem(IDC_DEADBAND_LOW);
		if (wnd)
			wnd->EnableWindow(flag);

		wnd = GetDlgItem(IDC_DEADBAND_HIGH);
		if (wnd)
			wnd->EnableWindow(flag);
	}



	void CCongestionParameterDlg::WriteStatusLine(const CString &lineOfText)
	{
		CString new_s, old_s, this_s;
		time_t current_time;
		time(&current_time);

		CTime now(current_time);
		CString timeStr = now.Format("%d %b - %I:%M%p");				

		// GET WHATS IN THE BOX NOW AND JUST ADD IT TO THE NEW
		this_s.Format("%s\t%s", timeStr, lineOfText);

		CWnd *wnd = GetDlgItem(IDC_STATUS_EDIT);
		if (wnd)
		{
			wnd->GetWindowText(old_s);
			if (old_s.IsEmpty())
				new_s.Format("%s", this_s);
			else			
				new_s.Format("%s\r\n%s", this_s, old_s);							
			
			wnd->SetWindowText(new_s.GetBuffer(new_s.GetLength()));							
			wnd->UpdateWindow();			
		}		
	}


	void CCongestionParameterDlg::OnAppAbout() 
	{
		using TA_Base_Bus::HelpLauncher;		
		HelpLauncher::getInstance().displayAboutBox();
	}


	void CCongestionParameterDlg::OnApply() 
	{		
		if (true == m_hasChanged)
		{
			SaveCongestionParameters();	
		}
	}


	// this will only be run if a parameter has changed
	void CCongestionParameterDlg::SaveCongestionParameters()
	{
		// Retrieve the parameters settings
		CString str;
		std::ostringstream parameterList;

		m_MinuteEdit.GetWindowText(str);				
		str.Remove(',');
		int mins = (int)atoi(str.GetBuffer(str.GetLength()));
		m_SecondEdit.GetWindowText(str);
		int secs = (int)atoi(str.GetBuffer(str.GetLength()));
		int totalSecs = (60 * mins) + secs;

		CString maxTrainStallTime;

        maxTrainStallTime.Format("%d", totalSecs);
        m_outParams.nStallTime = totalSecs;
        m_outParams.bUsesStallTime = (m_StallTime_Check.GetCheck() == BST_CHECKED);
		if (m_outParams.bUsesStallTime)
		{
			parameterList << "Train stall time checked\n";
			parameterList << "Maximum train stall time to " << mins << " minutes " << secs << " seconds\n";
		} 
        else
		{
			parameterList << "Train stall time unchecked\n";
		}

		std::string maxTemperatureStr;
		std::string temperatureDeadbandStr;

        m_TempLabel.GetWindowText(str);
        maxTemperatureStr = str.GetBuffer(str.GetLength());
        int maxTemperature = (int) atoi(str.GetBuffer(str.GetLength()));

        m_DeadBandLabel.GetWindowText(str);
        temperatureDeadbandStr = str.GetBuffer(str.GetLength());
        int temperatureDeadband = (int) atoi(str.GetBuffer(str.GetLength()));

        m_outParams.nTrainTemp = maxTemperature;
        m_outParams.nTempDeadBand = temperatureDeadband;
        m_outParams.bUsesTrainTemp = (m_TrainTemp_Check.GetCheck() == BST_CHECKED);

		if ( m_outParams.bUsesTrainTemp )
		{
			parameterList << "Temperature of stalled train checked\n";
			parameterList << "Maximum temperature of stalled train to " <<  maxTemperature << "\n";
			parameterList << "Temperature deadband to " << temperatureDeadband;
		} 
		else
		{
			parameterList << "Temperature of stalled train unchecked";
		}

		// Get user confirmation
		TA_Base_Bus::TransActiveMessage userMsg;
		CString message = parameterList.str().c_str();
		userMsg << message;
		UINT selectionButton = userMsg.showMsgBox(IDS_UW_650010);

		if (selectionButton == IDCANCEL)
		{
			return;
		}

		WriteStatusLine("Saving global Congestion parameters.");

		// Set parameters:

		// Congestion Trigger: Train stall time	
		try
		{
			std::string useStallTime = m_outParams.bUsesStallTime?"Yes":"No";
            m_congestionUsesStallTime->setValue(useStallTime);
            m_congestionUsesStallTime->applyChanges();
		}
		catch (...)
		{
		
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520001);
            WriteStatusLine("Failed.");
            return;
		}

		// Trigger Settings: Max train stall time
		try
        {
            m_thresholdStallTimeInSecs->setValue(maxTrainStallTime.GetBuffer(maxTrainStallTime.GetLength()));
            m_thresholdStallTimeInSecs->applyChanges();
		}
		catch (...)
		{			
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520002);
            WriteStatusLine("Failed.");
            return;
		}

		// Congestion Trigger: Temperature of stalled train
		try
		{
			std::string useTrainTemperature = m_outParams.bUsesTrainTemp?"Yes":"No";
            m_congestionUsesTemperature->setValue(useTrainTemperature);
            m_congestionUsesTemperature->applyChanges();
		}
		catch (...)
		{
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520003);
            WriteStatusLine("Failed.");
            return;
		}

		// Trigger Settings: Maximum temperature of stalled train
		try
		{
			m_thresholdTemperature->setValue(maxTemperatureStr);
            m_thresholdTemperature->applyChanges();
		}
		catch (...)
		{		
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520002);
            WriteStatusLine("Failed.");
            return;
		}

		// Trigger Settings: Temperature Deadband
		try
		{
            m_temperatureDeadband->setValue(temperatureDeadbandStr);
            m_temperatureDeadband->applyChanges(); 
		}
		catch (...)
		{		
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520007);
            WriteStatusLine("Failed.");
            return;
		}
		
		
		WriteStatusLine("Saved.");

		WriteStatusLine("Sending Online Updates...");
		
		// compare changed parameters
		std::vector<std::string> changedParams;
		if (m_inParams.bUsesStallTime != m_outParams.bUsesStallTime)
		{
			changedParams.push_back(TA_IRS_Bus::ECS_GLOBAL_PARAMETER_CONGESTION_USES_STALL_TIME);
			
			// send audit event
			CAuditMessageUtility auditLogWriter;
			std::string parameterName(TA_IRS_Bus::ECS_GLOBAL_PARAMETER_CONGESTION_USES_STALL_TIME);				
			std::string parameterNewValue;

			if (m_outParams.bUsesStallTime)
				parameterNewValue = "Yes";
			else
				parameterNewValue = "No";
			auditLogWriter.LogParameterChangeAuditMessage(parameterName, parameterNewValue, ECSAudit::ECSCongestionParameterChange);		
		}

		if ((m_inParams.nStallTime != m_outParams.nStallTime) && (m_outParams.bUsesStallTime))
		{
			changedParams.push_back(TA_IRS_Bus::ECS_GLOBAL_PARAMETER_THRESHOLD_STALL_TIME);
			
			// send audit event
			CAuditMessageUtility auditLogWriter;
			std::string parameterName(TA_IRS_Bus::ECS_GLOBAL_PARAMETER_THRESHOLD_STALL_TIME);												
			CString s;
			s.Format("%d", m_outParams.nStallTime);
			std::string parameterNewValue(s.GetBuffer(s.GetLength()));
			
			auditLogWriter.LogParameterChangeAuditMessage(parameterName, parameterNewValue, ECSAudit::ECSCongestionParameterChange);		
		}

		if (m_inParams.bUsesTrainTemp != m_outParams.bUsesTrainTemp)
		{
			changedParams.push_back(TA_IRS_Bus::ECS_GLOBAL_PARAMETER_CONGESTION_USES_TEMPERATURE);
			
			// send audit event
			CAuditMessageUtility auditLogWriter;
			std::string parameterName(TA_IRS_Bus::ECS_GLOBAL_PARAMETER_CONGESTION_USES_TEMPERATURE);				
			std::string parameterNewValue;

			if (m_outParams.bUsesTrainTemp)
				parameterNewValue = "Yes";
			else
				parameterNewValue = "No";
			auditLogWriter.LogParameterChangeAuditMessage(parameterName, parameterNewValue, ECSAudit::ECSCongestionParameterChange);		
		}

		if ((m_inParams.nTrainTemp != m_outParams.nTrainTemp) && (m_outParams.bUsesTrainTemp))
		{
			changedParams.push_back(TA_IRS_Bus::ECS_GLOBAL_PARAMETER_THRESHOLD_TEMPERATURE);
			
			// send audit event
			CAuditMessageUtility auditLogWriter;
			std::string parameterName(TA_IRS_Bus::ECS_GLOBAL_PARAMETER_THRESHOLD_TEMPERATURE);												
			CString s;
			s.Format("%d", m_outParams.nTrainTemp);
			std::string parameterNewValue(s.GetBuffer(s.GetLength()));
			
			auditLogWriter.LogParameterChangeAuditMessage(parameterName, parameterNewValue, ECSAudit::ECSCongestionParameterChange);		
		}

		if ((m_inParams.nTempDeadBand != m_outParams.nTempDeadBand) && (m_outParams.bUsesTrainTemp))
		{
			changedParams.push_back(TA_IRS_Bus::ECS_GLOBAL_PARAMETER_TEMPERATURE_DEADBAND);
			
			// send audit event
			CAuditMessageUtility auditLogWriter;
			std::string parameterName(TA_IRS_Bus::ECS_GLOBAL_PARAMETER_TEMPERATURE_DEADBAND);												
			CString s;
			s.Format("%d", m_outParams.nTempDeadBand);
			std::string parameterNewValue(s.GetBuffer(s.GetLength()));
			
			auditLogWriter.LogParameterChangeAuditMessage(parameterName, parameterNewValue, ECSAudit::ECSCongestionParameterChange);		
		}

		// Need to send a specialised message to all ECS Station Agents to 
		// get them to update their cached config of these parameters
		 ConfigUpdateMessageSender *configUpdateSender = 
			 MessagePublicationManager::getInstance().getConfigUpdateMessageSender(TA_Base_Core::ConfigUpdate::Context);
		 
         unsigned long ecsSubsystemKey = ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getSubsystem();

		 try
		 {
			 configUpdateSender->sendConfigUpdateMessage(
				TA_Base_Core::ConfigUpdate::ConfigGlobalParameter,	// Message Type
				ecsSubsystemKey,			                    // Key of the changed entity
				TA_Base_Core::Update,							    // Update Type
				changedParams,								    // Parameter changed
				0);  
			
			WriteStatusLine("Success.");
			
			// update sent so unset the m_hasChanged flag
			m_hasChanged = false;
			m_ApplyButton.EnableWindow(FALSE);

			// reset m_inParams to new values
			m_inParams.bUsesStallTime = m_outParams.bUsesStallTime;
			m_inParams.bUsesTrainTemp = m_outParams.bUsesTrainTemp;
			m_inParams.nStallTime = m_outParams.nStallTime;
			m_inParams.nTrainTemp = m_outParams.nTrainTemp;				
            m_inParams.nTempDeadBand = m_outParams.nTempDeadBand;				
		 }
		 catch (...)
		 {
			 if(configUpdateSender)
			 {
				 delete configUpdateSender;			
				 configUpdateSender = 0;
			 }
			WriteStatusLine("Failed.");
			// something went wrong so keep updates until it is successful
		 }

		 if (configUpdateSender)
		 {
			 delete configUpdateSender;	
			 configUpdateSender = 0;
		 }
	}


	void CCongestionParameterDlg::OnChangeMinute() 
	{
		if (m_isInitialised)
		{
			CString s;
			m_MinuteEdit.GetWindowText(s);

			if ((int)atoi(s.GetBuffer(s.GetLength())) > MAX_STALL_TIME_MINS)
			{
				CString s;
				s.Format("%d", MAX_STALL_TIME_MINS);

				m_MinuteEdit.SetWindowText(s);
			}

			if (!m_hasChanged)
				m_hasChanged = true;
			
			m_ApplyButton.EnableWindow();
		}

		// wen ching++
		// bug 701 (TD10591)
		//TransActiveDialog::OnInitDialog();
		// ++wen ching
		// bug 701 (TD10591)
	
	}



	void CCongestionParameterDlg::OnChangeSecond() 
	{
		if (m_isInitialised)
		{
			CString s;
			m_SecondEdit.GetWindowText(s);

			if ((int)atoi(s.GetBuffer(s.GetLength())) > MAX_STALL_TIME_SECS)
			{
				CString s;

				s.Format("%d", MAX_STALL_TIME_SECS);
				m_SecondEdit.SetWindowText(s);
			}

			if (!m_hasChanged)
				m_hasChanged = true;

			m_ApplyButton.EnableWindow();
		}

		// wen ching++
		// bug 701 (TD10591)
		//TransActiveDialog::OnInitDialog();
		// ++wen ching
		// bug 701 (TD10591)

	}


	void CCongestionParameterDlg::onRunParamChange(const std::string& name, const std::string& value)
	{
		if (name.compare(RPARAM_SESSIONID) == 0)
			CheckPermissions();
	}

	void CCongestionParameterDlg::CheckPermissions()
	{
		WriteStatusLine("Verifying operator permissions...");
		
		m_isAllowedToSetParameter = false;
		
        
        TA_Base_Bus::RightsLibraryFactory rightsFactory;
		
		// 10070-Start
		// Commented out for fixing bug related to avoid building rights library 
		// Now the rights library is built inside the CRightsMgrSingleton
         //TA_Base_Bus::RightsLibrary *rightsMgr = rightsFactory.buildRightsLibrary();
		// Added to call the Rights Mgr
			
		TA_Base_Bus::RightsLibrary *rightsMgr = CRightsMgrSingleton::getInstance()->getRightsMgr();
		
		// 10070-End

		std::auto_ptr<TA_Base_Core::IEntityData> p_ecsManager(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str()));
        TA_Base_Core::IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(p_ecsManager->getKey());

      
        unsigned long resourceId = resource->getKey();
        delete resource;

		TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		std::string reason;

		// get current session id for the operator
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);		

		try
		{
			m_isAllowedToSetParameter = rightsMgr->isActionPermittedOnResource(
										sessionId, 
										resourceId, 
										TA_Base_Bus::aca_SET_PARAMETER_ECS, 
										reason, 
										decisionModule);													
		}
		catch (...)
		{
			m_isAllowedToSetParameter = false;
		}
		
		// 10070-Start
		// Commented out for fixing bug related to avoid building rights library 
		// delete rightsMgr;
		// 10070-End

		if (m_isAllowedToSetParameter == false)
		{
			WriteStatusLine("Failed !");
			// TD14164 ++
			/*MessageBox(MAKEINTRESOURCE(IDS_UE_520005));*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_520005);
			// TD14164 ++
			
			// 10070-Start
			// Delete if the RightsMgrSingleton Library has been created
			CRightsMgrSingleton::getInstance()->DeleteInstance();
			// 10070-End
			//TD18095, jianghp, to fix the performance of showing manager application
			TransActiveDialog::DestroyWindow();
		} else
		{
			WriteStatusLine("Success !");
		}
	}

	void CCongestionParameterDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		TransActiveDialog::OnShowWindow(bShow, nStatus);
		
		// check operator permissions
		PostMessage(WM_READ_PARAMETERS);		

        PostMessage(WM_CHECK_PERMISSIONS);		
	}

	void CCongestionParameterDlg::OnSize(UINT nType, int cx, int cy) 
	{
		TransActiveDialog::OnSize(nType, cx, cy);
		
		// TODO: Add your message handler code here

		CRect thisRect;
        GetWindowRect( &thisRect );
        ScreenToClient( &thisRect );
        // Start by resizing the menu line
        CWnd* menuLine = GetDlgItem( IDC_MENU_SEPERATOR );
        if ( NULL != menuLine )
        {
            // Get the rectangle for the menu line
            CRect lineRect;
            menuLine->GetWindowRect( &lineRect );
            ScreenToClient( &lineRect );

            // Don't care about it's height - it's just a line. Set the line width bounds to 
            // the same as the main application window
            lineRect.right = ( lineRect.left + cx + 2 );
            menuLine->MoveWindow( &lineRect );
        }

		CRect resizeApplyRect, resizeHelpRect, resizeCloseRect, resizeStaticRect, resizeStatusRect;
		CWnd* p_resizeApplyWnd;
		CWnd* p_resizeHelpWnd;
		CWnd* p_resizeCloseWnd;
		CWnd* p_resizeStaticWnd;
		CWnd* p_resizeStatusWnd;

		p_resizeApplyWnd = GetDlgItem(IDOK);
		p_resizeHelpWnd = GetDlgItem(IDHELP);
		p_resizeCloseWnd = GetDlgItem(IDCLOSE);
		p_resizeStaticWnd = GetDlgItem(IDC_CON_STATIC2);
		p_resizeStatusWnd = GetDlgItem(IDC_STATUS_EDIT);

		if(p_resizeApplyWnd)
		{
			p_resizeApplyWnd->GetWindowRect(&resizeApplyRect);
			ScreenToClient(&resizeApplyRect);
			
			p_resizeApplyWnd->MoveWindow(LEFT_APPLY_OFFSET + (cx-450), TOP_APPLY_OFFSET + (cy-606), resizeApplyRect.Width(), resizeApplyRect.Height());
			
		}

		if(p_resizeHelpWnd)
		{
			p_resizeHelpWnd->GetWindowRect(&resizeHelpRect);
			ScreenToClient(&resizeHelpRect);
			
			p_resizeHelpWnd->MoveWindow(6, TOP_APPLY_OFFSET + (cy-606), resizeHelpRect.Width(), resizeHelpRect.Height());
			
		}

		if(p_resizeCloseWnd)
		{
			p_resizeCloseWnd->GetWindowRect(&resizeCloseRect);
			ScreenToClient(&resizeCloseRect);
			
			p_resizeCloseWnd->MoveWindow(344 + (cx-450), TOP_APPLY_OFFSET + (cy-606), resizeCloseRect.Width(), resizeCloseRect.Height());
			
		}

		if(p_resizeStaticWnd)
		{
			p_resizeStaticWnd->GetWindowRect(&resizeStaticRect);
			ScreenToClient(&resizeStaticRect);
			
			int width = resizeStaticRect.Width();
			int height = resizeStaticRect.Height();

			p_resizeStaticWnd->MoveWindow(6, 452, 438 + (cx - 450), 102 + (cy -606));
			
		}

		if(p_resizeStatusWnd)
		{
			p_resizeStatusWnd->GetWindowRect(&resizeStatusRect);
			ScreenToClient(&resizeStatusRect);
			p_resizeStatusWnd->MoveWindow(16, 474, 418 + (cx - 450), 68 + (cy -606));
			
		}
		
		// Redraw all the buttons at once to prevent artifacts 
        RedrawWindow( thisRect );
		
	}

	//TD16730
	void CCongestionParameterDlg::OnHelpButton() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
	}

	void CCongestionParameterDlg::OnAppHelp() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
	}
	
} // namespace
