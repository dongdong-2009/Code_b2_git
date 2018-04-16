/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/AlarmGeneratorDlg.cpp,v $
  * @author:  Andrew Del Carlo
  * @version: $Revision: 1.1.2.12.4.10 $
  * Last modification: $Date: 2006/05/10 09:24:28 $
  * Last modified by:  $Author: derrickl $
  * 
  * implementation if the alarm generator dialog
  */

#include <iostream>

#include "AlarmGeneratorDlg.h"
#include "setupDialog.h"
#include "ModifyMmsDialog.h"
#include "AlarmQtyDlg.h"
#include "AutoAlarmsOptionDlg.h"
#include "AlarmStore.h"

#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\corba\src\CorbaUtil.h"
#include "core\message\src\NameValuePair.h"
#include "core\message\types\MessageTypes.h"
#include "core\message\src\MessageConfig.h"
#include "core\synchronisation\src\ThreadGuard.h"

#include "core\data_access_interface\src\DataTypeEnums.h"
#include "core\data_access_interface\src\Operator.h"
#include "core\data_access_interface\src\OperatorAccessFactory.h"
#include "core\data_access_interface\src\IOperator.h"
#include "core\data_access_interface\src\DatabaseFactory.h"
#include "core\data_access_interface\src\AlarmData.h"
#include "core\alarm\src\AlarmConfig.h"
#include "core\data_access_interface\src\AlarmAccessFactory.h"
#include "core\data_access_interface\src\AlarmSeverityAccessFactory.h"
#include "core\data_access_interface\src\IAlarmSeverityData.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\DataException.h"

//#include "bus/alarm/alarm_store_library/src/AlarmAccessFactory.h"

#include "core\alarm\src\AlarmHelperManager.h"
#include "core\alarm\IDL\src\AlarmMessageCorbaDef.h"
#include <fstream>
#include <boost/regex.hpp>

#include "agDebug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::IAlarmSeverityData;
using TA_Base_Core::RunParams;
using TA_Base_Core::IData;
using TA_Base_Core::IDatabase;
using TA_Base_Core::DatabaseFactory;
using TA_Base_Core::DebugUtil;
//using TA_Base_Core::AlarmAccessFactory;
//using TA_Base_Core::AlarmTypeData;
bool CAlarmGeneratorDlg::m_isSetFilter=false;



// inicate thread running
bool s_bThreadRunning_AutoAlarms = false;		// use a nice way to stop threads, instead of force terminating
#define FILTER_SET_TIME_INTERVAL			"FilterInterval"
#define FILTER_SET_ASSET		        	"FilterAsset"
#define FILTER_SET_DESCRIPTION		    	"FilterDescription"


/////////////////////////////////////////////////////////////////////////////
// CAlarmGeneratorDlg dialog
CAlarmGeneratorDlg::CAlarmGeneratorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlarmGeneratorDlg::IDD, pParent),
		m_hThreadUpdateTime( 0 )
		,m_hSetFilterThread(0)
		,m_bIsSendAlarmStore(FALSE)
{
	//{{AFX_DATA_INIT(CAlarmGeneratorDlg)
	m_nAlarmCount = 0;
	m_nInterval = 1000;
	m_nThreadCount = 0;
	m_nExpiryMins = 0;
	//}}AFX_DATA_INIT
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	mutex = CreateMutex(NULL, FALSE, NULL);
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_alarms.setObserver(this);
}

void CAlarmGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlarmGeneratorDlg)
	DDX_Control(pDX, IDC_ENTITY_LOCATION_KEY, m_CLocationKey);
	DDX_Control(pDX, IDEXIT, m_exitBut);
	DDX_Control(pDX, ID_REFRESH, m_refreshBut);
	DDX_Control(pDX, ID_MULTI_ALARMS, m_multiAlarmBut);
	DDX_Control(pDX, IDMMS, m_mmsButton);
	DDX_Control(pDX, IDC_ALARMTYPE_SELECT, m_alarmTypeSelect);
	DDX_Control(pDX, IDC_ENTITY_KEY_SELECT, m_entityKeySelect);
	DDX_Control(pDX, IDC_SEVERITY_SELECT, m_severitySelect);
	DDX_Control(pDX, IDC_ENTITY_TYPE, m_entityType);
	DDX_Control(pDX, IDC_ACTIVE_ALARMS_LIST, m_activeAlarms);
	DDX_Control(pDX, IDOK, m_okButton);
	DDX_Control(pDX, IDACK, m_ackButton);
	DDX_Control(pDX, IDACKCLOSE, m_ackCloseButton);
	DDX_Control(pDX, IDCLOSE, m_closeButton);
	DDX_Control(pDX, IDC_IS_UNIQUE, m_isUnique);
	DDX_Control(pDX, IDC_NOT_UNIQUE, m_isNotUnique);
	DDX_Control(pDX, IDC_UUID, m_alarmID);
	DDX_Control(pDX, IDC_ALARM_DES, m_alarmDes);
	DDX_Control(pDX, IDC_SUBSYSTEM, m_subsystem);
	DDX_Control(pDX, IDC_ASSETNAME, m_assetName);
	DDX_Control(pDX, IDC_OUTPUT_BOX, m_outputBox);
	DDX_Control(pDX, IDC_CLOSE_VALUE, m_closeValue);
	DDX_Control(pDX, IDC_PARAM1, m_param1);
	DDX_Control(pDX, IDC_PARAM2, m_param2);
	DDX_Control(pDX, IDC_PARAM3, m_param3);
	DDX_Control(pDX, IDC_PARAM4, m_param4);
	DDX_Control(pDX, IDC_PARAM5, m_param5);
	DDX_Control(pDX, IDC_STATIC1, m_static1);
	DDX_Control(pDX, IDC_STATIC2, m_static2);
	DDX_Control(pDX, IDC_STATIC3, m_static3);
	DDX_Control(pDX, IDC_STATIC4, m_static4);
	DDX_Control(pDX, IDC_STATIC5, m_static5);
	DDX_Control(pDX, IDC_ALARM_CLOSE_VALUE_STATIC, m_closeValueStatic);
	DDX_Text(pDX, IDC_EDIT_COUNT, m_nAlarmCount);
	DDV_MinMaxUInt(pDX, m_nAlarmCount, 1, 999999999);
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_nInterval);
	DDV_MinMaxUInt(pDX, m_nInterval, 0, 999999999);
	DDX_Text(pDX, IDC_EDIT_THREAD_COUNT, m_nThreadCount);
	DDV_MinMaxUInt(pDX, m_nThreadCount, 1, 999999999);
	DDX_Check(pDX, IDC_CHECK_ACK_CLOSE, m_bCheckAckClose);
	DDX_Text(pDX, IDC_EDIT_EXPIRY, m_nExpiryMins);
	DDV_MinMaxUInt(pDX, m_nExpiryMins, 1, 999999999);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK1, m_bIsSendAlarmStore);
}

BEGIN_MESSAGE_MAP(CAlarmGeneratorDlg, CDialog)
	//{{AFX_MSG_MAP(CAlarmGeneratorDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDEXIT, OnExit)
	ON_BN_CLICKED(IDACK, OnAck)
	ON_BN_CLICKED(IDCLOSE, OnClose)
	ON_BN_CLICKED(IDACKCLOSE, OnAckClose)
	ON_BN_CLICKED(ID_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_IS_UNIQUE, OnUnique)
	ON_BN_CLICKED(IDC_NOT_UNIQUE, OnNotUnique)
	ON_CBN_SELCHANGE(IDC_ALARMTYPE_SELECT, OnSelchangeAlarmTypeSelect)
	ON_CBN_SELCHANGE(IDC_ENTITY_KEY_SELECT, OnSelchangeEntityKeySelect)
	ON_CBN_SELCHANGE(IDC_ENTITY_TYPE, OnSelchangeEntityTypeSelect)
	ON_LBN_SELCHANGE(IDC_ACTIVE_ALARMS_LIST, OnSelchangeActiveAlarmsList)
	ON_CBN_SELCHANGE(IDC_SEVERITY_SELECT, OnSelchangeSeveritySelect)
	ON_BN_CLICKED(IDMMS, OnModMms)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(ID_MULTI_ALARMS, OnMultiAlarms)
	ON_BN_CLICKED(ID_AUTO, autoSubmitAlarm)
	ON_CBN_SELCHANGE(IDC_ENTITY_LOCATION_KEY, OnSelchangeEntityLocationKey)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmGeneratorDlg message handlers

DWORD WINAPI ThreadSetFilter(LPVOID lpParameter)
{
	reinterpret_cast<CAlarmGeneratorDlg*>(lpParameter)->setExcuteFilter();
	
	return 0;

}


BOOL CAlarmGeneratorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_autoAlarmsOption.m_nInterval=1000;
	RunParams::getInstance().parseCmdLine(GetCommandLine());
	std::string level = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
	if (!level.empty())
	{
		DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::getDebugLevelFromString(level.c_str()));
	}
	else
	{
		TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::getDebugLevelFromString("INFO"));
	}
	//set sessionId
	std::string StrSessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	if (!StrSessionId.empty())
	{
		m_alarms.setAlarmSessonId(StrSessionId.c_str());
	}
	else
	{
		m_alarms.setAlarmSessonId("TransActive Super Session ID");
	}
    //create set filter thread
	m_hSetFilterThread = CreateThread( NULL, NULL, ThreadSetFilter,this,NULL,NULL);
	
	TA_Base_Core::DebugUtil::getInstance().setFile("c:\\transactive\\logs\\AlarmGenerator.log");
	DebugUtil::getInstance().setMaxSize(10000000);
	//create a new dialog to handle once off setup configuration
    //m_UniqueHelper = TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper();
	//m_nonUniqueHelper = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper();
	//m_modHelper = TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper();
	
	agDebug("Show Setup Dialog to collect undefined info.");
	if(!this->createSetupDialog())
	{
		exit(0);
	}
	agDebug("Close Setup Dialog and continue.");
		
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	agDebug("Start populating active alarms(CAlarmGeneratorDlg::updateActiveAlarms).");
	
	//populate the active alarms list
	this->updateActiveAlarms();

	agDebug("Finished populating active alarms(CAlarmGeneratorDlg::updateActiveAlarms).");

	m_okButton.EnableWindow(false);	
	m_closeButton.EnableWindow(false);
	m_ackButton.EnableWindow(false);
	m_ackCloseButton.EnableWindow(false);
	m_mmsButton.EnableWindow(false);
	m_alarmID.EnableWindow(false);
	m_multiAlarmBut.EnableWindow(false);

	m_alarmQty=0;
	
	//YM1 20091023 : 
	std::ifstream ifs("C:\\transactive\\config\\AlarmGenerator\\locations.txt");

	int i;
	char buf[10];

	agDebug("Start Reading Location:");
	while(ifs >> i)
	{
		itoa(i, buf, 10);
		m_strVecLocations.push_back(buf);
		agDebug("Added location: ",buf);
	}
	agDebug("Done Reading location.");

	agDebug("Initialize Auto Alarm(InitAutoAlarm)");
	InitAutoSubmit(m_strVecLocations);
	agDebug("InitAutoAlarm Done.");
	ifs.close();


	// init worker thread handles array
	ZeroMemory(submit_workers,sizeof(HANDLE)*submit_num);

	// init auto alarms submission parameters
	m_nAlarmCount = 30;
	m_nThreadCount = 5;
	m_nInterval = 1000;
	m_nExpiryMins = 1;
	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAlarmGeneratorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);

		m_activeAlarms.ShowScrollBar(SB_BOTH, TRUE);
	}
	else
	{
		m_activeAlarms.ShowScrollBar(SB_BOTH, TRUE);

		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAlarmGeneratorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void CAlarmGeneratorDlg::updateActiveAlarms() 
{
	unsigned long nRowsCnt = 0;

	this->m_activeAlarms.ResetContent();
	std::vector<std::string> columnNames;

    columnNames.push_back("alarm_id");
	columnNames.push_back("entitykey");
    columnNames.push_back("type");
	columnNames.push_back("context");
    columnNames.push_back("description");
    
	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
  
	
	char tempString[128];
	itoa(TA_Base_Core::AlarmData::OPEN, tempString, 10);
	std::string sqlOpen = "select alarm_id, entitykey, type, context, description from ALARM where STATE = "+std::string(tempString);

	agDebug("Retrieving Alarms-> SQL Exec:", sqlOpen.c_str());

	TA_Base_Core::SQLStatement sql;
	sql.strCommon = sqlOpen;

	IData* dataOpen = databaseConnection->executeQuery(sql,columnNames);
	BOOL hasMore = TRUE;
	int cout = 0;
	while(hasMore && dataOpen->getNumRows() > 0 )
	{
		nRowsCnt += dataOpen->getNumRows();
		agDebug("Retrieving Alarms-> Fetched data: ", nRowsCnt);

		std::string tempAlarm;
		for(unsigned int i=0; i<dataOpen->getNumRows(); i++)
		{
			tempAlarm.erase();
			std::string tempAlarm = dataOpen->getStringData(i,"entitykey") + ":";
			tempAlarm += dataOpen->getStringData(i, "context") + ">";
			tempAlarm += dataOpen->getStringData(i,"type") + ">";
			tempAlarm += dataOpen->getStringData(i,"description") + ">";
			tempAlarm += dataOpen->getStringData(i, "alarm_id");
			this->m_activeAlarms.AddString(tempAlarm.c_str());
			cout++;
		}
		

		//hasMore = databaseConnection->moreData( dataOpen );
		hasMore = FALSE;
	}

	agDebug("Retrieving Alarms-> SQL Done.");

	delete dataOpen;
    dataOpen = NULL;

	std::string sqlAck =  "select alarm_id, entitykey, type, context, description from ALARM where STATE <> "+std::string(tempString);
	sqlAck = sqlAck + " and ACKNOWLEDGED_BY IS NULL";

	agDebug("Retrieving Alarms-> SQL Exec:", sqlAck.c_str());

	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sqlAck;

	IData* dataAck = databaseConnection->executeQuery(sqlCmd, columnNames);
	hasMore = TRUE;
	while ( hasMore && dataAck->getNumRows() > 0 )
	{
		std::string tempAlarm;
		for(unsigned int j=0; j<dataAck->getNumRows(); j++)
		{
				tempAlarm.erase();
			std::string tempAlarm = dataAck->getStringData(j,"entitykey") + ":";
			tempAlarm += dataAck->getStringData(j, "context") + ">";
			tempAlarm += dataAck->getStringData(j,"type") + ">";
			tempAlarm += dataAck->getStringData(j,"description") + ">";
			tempAlarm += dataAck->getStringData(j, "alarm_id");
			this->m_activeAlarms.AddString(tempAlarm.c_str());
				cout ++;
		}
		//hasMore = databaseConnection->moreData( dataOpen );
		hasMore = FALSE;
	}

	agDebug("Retrieving Alarms-> SQL Done.");
	agDebug("Finished Retrieving Alarms.");
	
	delete dataAck;
    dataAck = NULL;
	
	m_activeAlarms.ShowScrollBar(SB_BOTH, TRUE);
}


void CAlarmGeneratorDlg::OnSelchangeActiveAlarmsList() 
{
	// delete the currently selected alarms
	for (unsigned int k=0; k<m_currentSelectedAlarms.size(); ++k)
	{
		TA_Base_Core::IAlarmData* alarmData = m_currentSelectedAlarms[k];
		delete alarmData;
		alarmData = NULL;
	}
	m_currentSelectedAlarms.clear();

	// get the selected alarms
	CString alarm;
	
	std::string entityKey;
	std::string alarmType;
	std::string contextType;
	std::string desc;
	std::string pkey;

	for(int i=0; i< this->m_activeAlarms.GetCount();i++)
	{
		if(this->m_activeAlarms.GetSel(i)>0)
		{
			this->m_activeAlarms.GetText(i, alarm);
			//sscanf(alarm,"%ul:%ul>%s>%s>%s", &entityKey, &alarmType, &contextType, &desc, &id );
			int start = alarm.Find(":", 0);
			entityKey= std::string( alarm.Left( start ) );
			++start;
			int end = alarm.Find(">", start);
			alarmType= std::string( alarm.Mid(start, end-start) );
			start = ++end;
			end = alarm.Find(">", start);
			contextType= std::string( alarm.Mid(start, end-start) );
			start = ++end;
			end = alarm.Find(">", start);
			desc= std::string( alarm.Mid(start, end-start) );
			start = ++end;
			pkey= std::string( alarm.Mid(start, alarm.GetLength()) );

 			this->setDialogOutput(std::string(alarm) + "\r\r\n");

			TA_Base_Core::IAlarmData* alarmData = this->getAlarmFromDatabase(pkey);

			if (alarmData!=NULL) m_currentSelectedAlarms.push_back(alarmData);
		}
	}
	
	if (m_currentSelectedAlarms.size() != 0)
	{
		// only show the first selected alarm if there are multiple selected
		populateAlarmDetails(m_currentSelectedAlarms[0]);
		
		this->m_closeButton.EnableWindow(true);
		m_ackButton.EnableWindow(true);
		m_ackCloseButton.EnableWindow(true);
		m_mmsButton.EnableWindow(true);
		m_alarmID.EnableWindow(true);
		m_okButton.EnableWindow(false);
		m_multiAlarmBut.EnableWindow(false);
		
		// change close value title to close value
		m_closeValueStatic.SetWindowText("Close Value");

	}
}

void CAlarmGeneratorDlg::populateAlarmDetails(TA_Base_Core::IAlarmData* p_alarm)
{
	m_alarmID.SetWindowText(p_alarm->getKey().c_str());

	std::stringstream tempStringStream;
	tempStringStream << p_alarm->getTypeKey();

	int alarmTypeIndex = m_alarmTypeSelect.FindString(-1, tempStringStream.str().c_str() );
	if (alarmTypeIndex>=0)
	{
		m_alarmTypeSelect.SetCurSel(alarmTypeIndex);
		this->OnSelchangeAlarmTypeSelect();
	}

	m_alarmDes.SetWindowText(p_alarm->getParamList().c_str());

	char buffer[32];
	ltoa(p_alarm->getTypeOfAssociatedEntity(), buffer, 10);
	std::string entityTypeString(buffer);
	entityTypeString+=">"; // just indicate end of string to overcome 39 or 390 in list
	int entityTypeIndex = m_entityType.FindString(-1, entityTypeString.c_str());
	if (entityTypeIndex>=0)
	{
		m_entityType.SetCurSel(entityTypeIndex);
		OnSelchangeEntityTypeSelect();
	}

	char buffer2[32];
	ltoa(p_alarm->getEntityKey(), buffer2, 10);
	std::string entityKeyString(buffer2);
	entityKeyString+=">"; // just indicate end of string to overcome 39 or 390 in list
	int entityKeyIndex = m_entityKeySelect.FindString(-1, entityKeyString.c_str());
	if (entityKeyIndex>=0)
	{
		m_entityKeySelect.SetCurSel(entityKeyIndex);
		this->OnSelchangeEntityKeySelect();
	}

	if (p_alarm->getIsUnique())
	{
		OnUnique();
	}
	else
	{
		OnNotUnique();
	}

	m_assetName.SetWindowText(p_alarm->getAssetName().c_str());

	// populate close value if any
	TA_Base_Core::DescriptionParameters descriptionParameters;
	TA_Base_Core::gGetParametersFromString(p_alarm->getParamList(), descriptionParameters);
	for (unsigned int i=0; i<descriptionParameters.size(); ++i)
	{
		TA_Base_Core::NameValuePair* vpair = descriptionParameters[i];
		if (vpair->name == "ClosedValue")
		{
			m_closeValue.SetWindowText(vpair->value.c_str());
			break;
		}
	}
}

void CAlarmGeneratorDlg::populateAlarmTypeSelect()
{
	// set up the column names vector
    std::vector<std::string> columnNames;

    columnNames.push_back("meatyp_id");
    columnNames.push_back("type_name");
	columnNames.push_back("print_alarm");
	columnNames.push_back("persist_alarm");

	try
	{

		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);

		std::string sql = "SELECT meatyp_id, type_name, print_alarm, persist_alarm FROM ME_ALARM_TYPE where ME_ALARM_TYPE.isvisible=1";
		
		try
		{
			TA_Base_Core::SQLStatement sqlCmd;
			sqlCmd.strCommon = sql;

			IData* data = databaseConnection->executeQuery(sqlCmd,columnNames);

			std::string tempString;
			BOOL hasMore = TRUE;
			while ( hasMore && data->getNumRows() > 0 )
			{
			for(int i=0; (unsigned int)i<data->getNumRows(); i++)
			{
				tempString.erase();
				unsigned long key = data->getUnsignedLongData(i, "meatyp_id");
				char buffer[20];
				itoa(key, buffer, 10);
				tempString += buffer;
				tempString += ">";
				tempString += data->getStringData(i,"type_name");;
				tempString += ">To be printed: ";
				data->getBooleanData(i,"print_alarm") ? tempString += "Yes" : tempString += "No";
				tempString += ">To be persisted: ";
				data->getBooleanData(i,"persist_alarm") ?  tempString += "Yes" : tempString += "No";
				m_alarmTypeSelect.AddString( tempString.c_str() );
			}
				hasMore = databaseConnection->moreData(data);
			}
			
			delete data;
			data = NULL;
		}
		catch(TA_Base_Core::DataException& ex)
		{
			AfxMessageBox(ex.what());
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, ex.what());
		}

	}
	catch(TA_Base_Core::DatabaseException& ex)
	{
		AfxMessageBox("Database connection failed\r\nplease check the connection string and try again", MB_ICONSTOP);
		//reshow the settings dialog to re-enter tha database info
		if(!this->createSetupDialog())
		{
			exit(0);
		}
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, ex.what());
	}
	
}


void CAlarmGeneratorDlg::OnSelchangeAlarmTypeSelect() 
{
	int index = m_alarmTypeSelect.GetCurSel();

	if (index < 0) return;

	// clear up all the other boxes
	m_entityType.ResetContent();
	m_CLocationKey.ResetContent();
	m_entityKeySelect.ResetContent();
	m_severitySelect.Clear();
	m_subsystem.ReplaceSel("");
	m_alarmDes.ReplaceSel("");
	m_outputBox.ReplaceSel("");
	m_assetName.ReplaceSel("");
	m_closeValue.ReplaceSel("");
	m_alarmID.ReplaceSel("");
	enableParamWindows(false);

	m_closeValueStatic.SetWindowText("Submit Value");

	CString type;
	m_alarmTypeSelect.GetLBText(index, type);

	std::string key(type.Left( type.Find(">") ));

	m_alarmTypeKey = atol(key.c_str());
	

	std::vector<std::string> columnNames;
	columnNames.push_back("type_name");
    columnNames.push_back("type_description");
	columnNames.push_back("measev_id");
	columnNames.push_back("memcon_id");
	
	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);

	std::string sql = "SELECT type_name, type_description, measev_id, memcon_id FROM ME_ALARM_TYPE WHERE ME_ALARM_TYPE.isvisible=1 and meatyp_id = "+key;

	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sql;

	IData* data = databaseConnection->executeQuery(sqlCmd,columnNames);

	if(data->getNumRows() == 1)
	{
		this->m_alarmTypeName = data->getStringData(0,"type_name");
		this->m_alarmDesc = data->getStringData(0,"type_description");
		this->m_severity = data->getUnsignedLongData(0,"measev_id");
		this->m_contextID = data->getUnsignedLongData(0, "memcon_id");

		populateAlarmDescriptionParam(m_alarmDesc);
		
		char buffer[20];
		ltoa (m_severity, buffer, 10);
		std::string severity(buffer);
		m_severitySelect.SelectString(-1, severity.c_str());
		
		char buffer1[20];
		ltoa (m_alarmTypeKey, buffer1, 10);

		std::string outText("You have chosen an \r\n");
		outText += "Alarm Key: ";
		outText += buffer1;
		outText +=  "\r\n";
		outText += "Alarm Type Name: ";
		outText += m_alarmTypeName;
		outText +=  "\r\n";
		outText += "Alarm Severity: ";
		outText += severity;

		this->setDialogOutput(outText);
	}

    delete data;
    data = NULL;

}

void CAlarmGeneratorDlg::populateAlarmDescriptionParam(std::string description)
{
	enableParamWindows(false);
	m_entityType.EnableWindow(true);
	populateEntityTypeSelect();

	CString des(description.c_str());
	m_alarmDes.SetWindowText(des);

	int start = 0;
	int end = 0;
	int nextstart = 0;
	int count = 0;

	start = des.Find("[", nextstart);
	while (start >= 0)
	{
		end = des.Find("]", nextstart);
		if (end >= 0 && end > start)
		{
			CString param = des.Mid(start+1, end-start-1);
			++count;
			nextstart = end + 1;
			setParamCaption(count, param);
		}
		start = des.Find("[", nextstart);
	}

}

void CAlarmGeneratorDlg::setParamCaption(int count, CString caption)
{
	if (count <= 0) return;

	switch (count)
	{
		case 1:
			m_static1.SetWindowText(caption);
			m_param1.EnableWindow(true);
			m_param1.SetReadOnly(false);
			break;

		case 2:
			m_static2.SetWindowText(caption);
			m_param2.EnableWindow(true);
			m_param2.SetReadOnly(false);
			break;

		case 3:
			m_static3.SetWindowText(caption);
			m_param3.EnableWindow(true);
			m_param3.SetReadOnly(false);
			break;
		
		case 4:
			m_static4.SetWindowText(caption);
			m_param4.EnableWindow(true);
			m_param4.SetReadOnly(false);
			break;
		
		case 5:
			m_static5.SetWindowText(caption);
			m_param5.EnableWindow(true);
			m_param5.SetReadOnly(false);
			break;

		default:
			break;
			// Do nothing;
	}
}

void CAlarmGeneratorDlg::populateEntityTypeSelect()
{
	std::vector<std::string> columnNames;
	
	columnNames.push_back("pkey");
	columnNames.push_back("name");
	columnNames.push_back("ta_subsystemkey");

	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
	std::string sql = "SELECT pkey, name, ta_subsystemkey FROM ENTITYTYPE WHERE pkey > 0";
	
	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sql;

	IData* data =  databaseConnection->executeQuery(sqlCmd,columnNames);
	std::string tempString;
    BOOL hasMore = TRUE;
	while ( hasMore && data->getNumRows() > 0 )
	{
	for(int i=0; (unsigned int)i<data->getNumRows(); i++)
	{
		tempString.erase();
		int key = data->getIntegerData(i,"pkey");
		char buffer[20];
		itoa(key, buffer, 10);
		tempString = buffer;
		tempString += ">";
		tempString += data->getStringData(i,"name");

		int subsystem = data->getIntegerData(i, "ta_subsystemkey");
		char buffer1[20];
		itoa(subsystem, buffer1, 10);
		tempString += "> Subsystem:";
		tempString += buffer1;
		m_entityType.AddString(tempString.c_str());
		}
		hasMore = databaseConnection->moreData(data);
	}
	
	delete data;
	data = NULL;

}


void CAlarmGeneratorDlg::OnSelchangeEntityTypeSelect() 
{
	int index = m_entityType.GetCurSel();
	if (index < 0) return;

	// clear up all the entity related boxes
	m_CLocationKey.ResetContent();
	m_subsystem.ReplaceSel("");
	m_assetName.ReplaceSel("");

	CString type;
	m_entityType.GetLBText(index, type);

	int start =	type.Find(">");
	int next = start + 1;
	int end = type.Find(">", next);
	std::string key( type.Left(start) );
	CString tname = type.Mid(next, end-next);

	m_entityTypeName = tname.GetBuffer(0);
	m_entityTypeKey = atol(key.c_str());
	
	std::vector<std::string> columnNames;

	columnNames.push_back("locationkey");
    
	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
  	
	std::string sql = "SELECT distinct locationkey FROM ENTITY WHERE typekey = "+key+" order by locationkey";
	
	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sql;

	IData* data = databaseConnection->executeQuery(sqlCmd,columnNames);
	BOOL hasMore = TRUE;
	while( hasMore && data->getNumRows()>0)
	{
		m_CLocationKey.EnableWindow(true);
		std::string tempString;

		for(int i=0; (unsigned int)i<data->getNumRows(); i++)
		{
			tempString.erase();
			char buffer[20];
			int key = data->getIntegerData(i,"locationkey");
			itoa(key, buffer, 10);
			tempString += buffer;
			m_CLocationKey.AddString(tempString.c_str());
		}
	
		hasMore = databaseConnection->moreData(data);
	}

    delete data;
    data = NULL;
}


void CAlarmGeneratorDlg::OnSelchangeEntityKeySelect() 
{
	int index = m_entityKeySelect.GetCurSel();
	if (index < 0) return;

	// clear up all the entity related boxes
	m_subsystem.ReplaceSel("");

	CString entity;
	m_entityKeySelect.GetLBText(index, entity);

	int start =	entity.Find(">");
	std::string entityKey = entity.Left(start);

	int next = start + 1;
	start = entity.Find(">", next);
	m_entityName = entity.Mid(next, start-next);
	next = start + 1;
	
	start = entity.Find(":", next);
	next = start + 1;
	std::string subsystemKey( entity.Mid(next, entity.GetLength() - next) );

	m_entityKey = atol(entityKey.c_str());
	m_subSystemKey = atol(subsystemKey.c_str());

	std::vector<std::string> columnNames;

	columnNames.push_back("name");

	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
  	
	std::string sql = "SELECT name FROM SUBSYSTEM WHERE pkey = "+subsystemKey;
	
	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sql;

	IData* data = databaseConnection->executeQuery(sqlCmd,columnNames);

	if(data->getNumRows() == 1)
	{
		m_subSystemName = data->getStringData(0,"name");

		char buffer[20];
		ltoa (m_alarmTypeKey, buffer, 10);
		char buffer1[20];
		ltoa (m_severity, buffer1, 10);
		char buffer2[20];
		ltoa (m_entityTypeKey, buffer2, 10);
		char buffer3[20];
		ltoa (m_entityKey, buffer3, 10);

		m_subsystem.SetWindowText(subsystemKey.c_str());

		std::string outText("");
		outText += "\r\n";
		outText += "Entity Key: ";
		outText += buffer3;
		outText += "\r\n";
		outText += "Entity Name: ";
		outText += m_entityName;
		outText += "\r\n";
		outText += "Subsystem Key: ";
		outText += subsystemKey;
		outText += "\r\n";
		outText += "Subsystem Name: ";
		outText += m_subSystemName;
		this->setDialogOutput(outText);

		this->m_okButton.EnableWindow(true);
	}


    delete data;
    data = NULL;
}


/*
void CAlarmGeneratorDlg::OnSelchangeOperatorSelect() 
{
	// TODO: Add your control notification handler code here
	
}
*/

/*
void CAlarmGeneratorDlg::populateEnityKeySelect()
{
	std::vector<std::string> columnNames;

    columnNames.push_back("pkey");
	columnNames.push_back("parameterKey");
    columnNames.push_back("value");

	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase();
    
	char tempString[128];
	itoa(this->m_entityKey, tempString, 10);

	std::string sql = "(SELECT pkey,parameterKey,value FROM ENTITYPARAMETERVALUE WHERE parameterkey IN (SELECT pkey FROM ENTITYPARAMETER WHERE name = 'TrueAlarmMessage') AND entitykey IN (SELECT entitykey FROM ENTITYPARAMETERVALUE WHERE entitykey = '" + std::string(tempString) + "' AND parameterkey IN (SELECT pkey FROM ENTITYPARAMETER WHERE name = 'TrueAlarmEnable') AND value='1')) UNION (SELECT pkey,parameterKey,value FROM ENTITYPARAMETERVALUE WHERE parameterkey IN (SELECT pkey FROM ENTITYPARAMETER WHERE name = 'FalseAlarmMessage') AND entitykey IN (SELECT entitykey FROM ENTITYPARAMETERVALUE WHERE entitykey = '" + std::string(tempString) + "' AND parameterkey IN (SELECT pkey FROM ENTITYPARAMETER WHERE name = 'FalseAlarmEnable') AND value='1'))";
	
	IData* data = databaseConnection->executeQuery(sql,columnNames);

	for(int i=0; (unsigned int)i<data->getNumRows(); i++)
	{
		this->m_alarmSelect.AddString(data->getStringData(i,"value").c_str());
	}

	if(data->getNumRows()>0)
	{
		this->m_alarmSelect.SetCurSel(0);
		this->m_okButton.EnableWindow(true);
		this->OnSelchangeAlarmSelect();
	}
	else
	{
		this->setDialogOutput("No alarm data found for entity " + this->m_entityName + "\r\r\n");
		this->m_alarmSelect.SetWindowText("enter in an alarm message");
	}

	this->m_alarmSelect.EnableWindow(true);
    delete data;
    data = NULL;
}
*/


void CAlarmGeneratorDlg::setDialogOutput(std::string outText)
{
	this->m_outputBox.ReplaceSel(outText.c_str());
}


void CAlarmGeneratorDlg::populateSeveritySelect()
{
	std::vector<IAlarmSeverityData*> severityData = TA_Base_Core::AlarmSeverityAccessFactory::getInstance().getAllAlarmSeverities();

	for(unsigned int i=0; i<severityData.size(); i++)
	{
		m_severities[severityData[i]->getName()]=severityData[i]->getKey();
		this->m_severitySelect.AddString(severityData[i]->getName().c_str());
	}
	
}


void CAlarmGeneratorDlg::OnSelchangeSeveritySelect() 
{
	CString tempSeverity;

	this->m_severitySelect.GetLBText(m_severitySelect.GetCurSel(), tempSeverity);
	
	this->m_severity=m_severities[tempSeverity.GetBuffer(0)];

}


void CAlarmGeneratorDlg::OnOK() 
{
	// TODO: Add extra validation here

	// Validate input values
	if (validateSubmission()) this->submitAlarm();
		
	//CDialog::OnOK();
}

void CAlarmGeneratorDlg::OnAck()
{
	//acknowledge the selected alarm
	this->acknowledgeAlarm();
	
}


void CAlarmGeneratorDlg::OnClose() 
{
	// TODO: Add your control notification handler code here
	this->closeAlarm();
	
}

void CAlarmGeneratorDlg::OnAckClose()
{
	this->acknowledgeAlarm();
	this->closeAlarm();
}

void CAlarmGeneratorDlg::OnExit() 
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();	
}


void CAlarmGeneratorDlg::OnRefresh() 
{
	// clear all the inputs
	m_alarmID.SetWindowText("");
	m_CLocationKey.SetWindowText("");
	m_alarmTypeSelect.SetWindowText("");
	m_entityType.SetWindowText("");
	m_entityKeySelect.SetWindowText("");
	m_severitySelect.SetWindowText("");
	m_subsystem.SetWindowText("");
	m_alarmDes.SetWindowText("");
	m_assetName.SetWindowText("");
	m_closeValue.SetWindowText("");
	enableParamWindows(false);

	this->m_closeButton.EnableWindow(false);
	m_ackButton.EnableWindow(false);
	m_mmsButton.EnableWindow(false);
	m_ackCloseButton.EnableWindow(false);
	m_alarmID.EnableWindow(false);
	m_okButton.EnableWindow(true);
	m_multiAlarmBut.EnableWindow(true);

	this->updateActiveAlarms();
}


void CAlarmGeneratorDlg::OnUnique() 
{
	m_isUnique.SetCheck(1);
	m_isNotUnique.SetCheck(0);

	isUniqueAlarm = true;
}

void CAlarmGeneratorDlg::OnNotUnique() 
{
	m_isUnique.SetCheck(0);
	m_isNotUnique.SetCheck(1);

	isUniqueAlarm = false;
}

BOOL CAlarmGeneratorDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_isSetFilter=false;
	// Delete all helpers and do a clean up
	TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

	/*
	if (m_UniqueHelper != NULL)
	{
		delete m_UniqueHelper;
		m_UniqueHelper = NULL;
	}
	if (m_nonUniqueHelper != NULL)
	{
		delete m_nonUniqueHelper;
		m_nonUniqueHelper = NULL;
	}
	if (m_modHelper != NULL)
	{
		delete m_modHelper;
		m_modHelper = NULL;
	}
	*/

	unsigned int i = 0;

	
	// stop all threads
	for (i=0;i<submit_num;i++)
	{
		if (submit_workers[i]!=0)
		{
			TerminateThread(submit_workers[i],0);
			CloseHandle(submit_workers[i]);
			submit_workers[i]=0;
		}
	}

	if(m_hThreadUpdateTime!=0)
	{
		TerminateThread(m_hThreadUpdateTime,0);
		CloseHandle(m_hThreadUpdateTime);
	}
	if(m_hSetFilterThread!=0)
	{
		TerminateThread(m_hSetFilterThread,0);
		CloseHandle(m_hSetFilterThread);
	}
	CloseHandle(mutex);

	// clear m_autoalarm
	for(msg_vec::iterator it = m_autoalarm.begin(); it!=m_autoalarm.end(); it++)
	{
		delete ((*it)->p_mt);
		delete ((*it)->p_param);
		delete *it;
	}


	// clear entity collection
	for (entity_vec::iterator it2 = m_allentity.begin(); it2!=m_allentity.end(); it2++)
	{
		delete (*it2);
	}

	for (i=0; i<m_currentSelectedAlarms.size(); ++i)
	{
		TA_Base_Core::IAlarmData* alarmData = m_currentSelectedAlarms[i];
		delete alarmData;
		alarmData = NULL;
	}
	m_currentSelectedAlarms.clear();

	return CDialog::DestroyWindow();
}


void CAlarmGeneratorDlg::submitAlarm()
{
	TA_Base_Core::DescriptionParameters m_descriptionParameters;
	std::map<std::string, std::string> parameters;

	if (m_param1.IsWindowEnabled()) 
	{
		CString stat1;
		CString para1;
		m_static1.GetWindowText(stat1);
		m_param1.GetWindowText(para1);
		if (para1.IsEmpty())
		{
			AfxMessageBox("No parameter entered for "+stat1+". Please enter the parameter", MB_ICONSTOP);
			return;
		}
		std::string paramName(stat1);
		std::string paramDesc(para1);
		parameters [paramName] = paramDesc;
	}
	if (m_param2.IsWindowEnabled()) 
	{
		CString stat2;
		CString para2;
		m_static2.GetWindowText(stat2);
		m_param2.GetWindowText(para2);
		if (para2.IsEmpty())
		{
			AfxMessageBox("No parameter entered for "+stat2+". Please enter the parameter", MB_ICONSTOP);
			return;
		}
		std::string paramName(stat2);
		std::string paramDesc(para2);
		parameters [paramName] = paramDesc;
	}
	if (m_param3.IsWindowEnabled()) 
	{
		CString stat3;
		CString para3;
		m_static3.GetWindowText(stat3);
		m_param3.GetWindowText(para3);
		if (para3.IsEmpty())
		{
			AfxMessageBox("No parameter entered for "+stat3+". Please enter the parameter", MB_ICONSTOP);
			return;
		}
		std::string paramName(stat3);
		std::string paramDesc(para3);
		parameters [paramName] = paramDesc;
	}
	if (m_param4.IsWindowEnabled()) 
	{
		CString stat4;
		CString para4;
		m_static4.GetWindowText(stat4);
		m_param4.GetWindowText(para4);
		if (para4.IsEmpty())
		{
			AfxMessageBox("No parameter entered for "+stat4+". Please enter the parameter", MB_ICONSTOP);
			return;
		}
		std::string paramName(stat4);
		std::string paramDesc(para4);
		parameters [paramName] = paramDesc;
	}
	if (m_param5.IsWindowEnabled()) 
	{
		CString stat5;
		CString para5;
		m_static5.GetWindowText(stat5);
		m_param5.GetWindowText(para5);
		if (para5.IsEmpty())
		{
			AfxMessageBox("No parameter entered for "+stat5+". Please enter the parameter", MB_ICONSTOP);
			return;
		}
		std::string paramName(stat5);
		std::string paramDesc(para5);
		parameters [paramName] = paramDesc;
	}
	
	for (std::map<std::string, std::string>::iterator itr = parameters.begin(); itr != parameters.end(); ++itr)
	{
		TA_Base_Core::NameValuePair* pair = new TA_Base_Core::NameValuePair(itr->first, itr->second);
		m_descriptionParameters.push_back(pair);
	}

	// obtain the datapointvalue or submit value
	/*	if(this->m_alarmType == "TrueAlarmMessage")
		{
			TA_Base_Core::NameValuePair pair3 ("DataPointValue", "TRUE");
			m_descriptionParameters.push_back(&pair3);

			msgType = &TA_Base_Core::DataPointAlarms::DpTrue;
		}
		else if(this->m_alarmType == "FalseAlarmMessage")
		{
			TA_Base_Core::NameValuePair pair3 ("DataPointValue", "FALSE");
			m_descriptionParameters.push_back(&pair3);
    
			msgType = &TA_Base_Core::DataPointAlarms::DpFalse;
		}
	*/
	CString submitValue;
	m_closeValue.GetWindowText(submitValue);
	if (submitValue.IsEmpty()==false)
	{
		TA_Base_Core::NameValuePair* valuePair = new TA_Base_Core::NameValuePair("DataPointValue", std::string(submitValue.GetBuffer(0)) );
		m_descriptionParameters.push_back(valuePair);
	}

    CString assetName;
    m_assetName.GetWindowText(assetName);

	char buffer[20];
	ltoa(m_contextID, buffer, 10);
	std::string context = buffer;

	if (obtainMsgContextAndType(context))
	{
		char buffer1[20];
		ltoa(m_alarmTypeKey, buffer1, 10);
		std::string alarmKey = buffer1;
		m_bIsSendAlarmStore = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
		
		TA_Base_Core::MessageContext msgContext(m_channelName.c_str(), m_contextName.c_str(), TA_Base_Core::AlarmMessage);
		TA_Base_Core::MessageType msgType(msgContext, alarmKey.c_str());

		if(m_bIsSendAlarmStore)
		{
			timeb sourceTime = {0,0,-1,-1};
			ftime( &sourceTime );
			//creat an alarm
			TA_Base_Core::AlarmDetailCorbaDef* p_alarmDetailCorbaDef = m_alarms.createAlarmWithParams(
				msgType,
				this->m_entityKey,
				this->m_entityTypeKey,
				m_descriptionParameters,
				this->m_entityName,
				this->m_region,
				this->m_subSystemKey,
				this->m_severity,
				sourceTime,
				(LPCTSTR)assetName,
		        true);
			//insert into share alarm
			//TA_Base_Bus::SharedAlarmAccessFactory::getInstance().insertAlarm(*p_alarmDetailCorbaDef, true);
			//TA_Base_Bus::AlarmAccessFactory::getInstance().insertAlarm(*p_alarmDetailCorbaDef, true);
			pAlarmStore->newAlarm(*p_alarmDetailCorbaDef);
			delete p_alarmDetailCorbaDef;
		}
		else if (assetName.IsEmpty())
		{
			if (isUniqueAlarm == true) TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithSeverity( 
				msgType,
				this->m_entityKey,
				this->m_entityTypeKey,
				m_descriptionParameters,
				this->m_entityName,
				this->m_region,
				this->m_subSystemKey,
				this->m_severity
			 );
			else if (isUniqueAlarm == false) TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithSeverity( 
				msgType,
				this->m_entityKey,
				this->m_entityTypeKey,
				m_descriptionParameters,
				this->m_entityName,
				this->m_region,
				this->m_subSystemKey,
				this->m_severity
			 );
		}
		else
		{
			if (isUniqueAlarm == true) TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithSeverityAndAsset( 
				msgType,
				this->m_entityKey,
				this->m_entityTypeKey,
				m_descriptionParameters,
				this->m_entityName,
				this->m_region,
				this->m_subSystemKey,
				this->m_severity,
				(LPCTSTR)assetName
			 );
			else if (isUniqueAlarm == false) TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithSeverityAndAsset( 
				msgType,
				this->m_entityKey,
				this->m_entityTypeKey,
				m_descriptionParameters,
				this->m_entityName,
				this->m_region,
				this->m_subSystemKey,
				this->m_severity,
				(LPCTSTR)assetName
			 );
		}

		char tempString[128];
		itoa(this->m_entityKey, tempString, 10);
		setDialogOutput("Submitted alarm with entity key: " + std::string(tempString) +", and message: " + std::string(this->m_alarmDesc) +"\r\r\n"); 
	}
}

CAlarmGeneratorDlg::param_vec_ptr CAlarmGeneratorDlg::getParams(const std::string& paramstr)
{
	boost::regex expr("\\[([a-zA-Z]+)]");
	boost::match_results<std::string::const_iterator> res;
	boost::match_flag_type flags = boost::match_default;
	std::string::const_iterator start = paramstr.begin();
	std::string::const_iterator end = paramstr.end(); 
	param_vec_ptr param_p = NULL;
// 	if(boost::regex_search(start, end, res, expr, flags))
// 	{	
// 		param_p= new param_vec;
// 		param_p->push_back(std::string(res[1].first, res[1].second));
// 		start = res[0].second;      
// 		// update flags:
// 		flags |= boost::match_prev_avail;
// 		flags |= boost::match_not_bob;
// 	}

	param_p= new param_vec;
	while(boost::regex_search(start, end, res, expr, flags))
	{	
		param_p->push_back(std::string(res[1].first, res[1].second));
		start = res[0].second;      
		// update flags:
		flags |= boost::match_prev_avail;
		flags |= boost::match_not_bob;
	}
	return param_p;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	reinterpret_cast<CAlarmGeneratorDlg*>(lpParameter)->autos();
	
	return 0;
}


DWORD WINAPI ThreadUpdateTime(LPVOID pWnd)
{
	time_t t;
	int m = 0;
	int v = 0;
	char str[5];
	DWORD dwRes;


	CAlarmGeneratorDlg* p = (CAlarmGeneratorDlg*)pWnd;
	for (;;)
	{
		t = time(0) - p->m_tStartAuto;
		
		// set sec
		v = t % 60;
		itoa(v,str,10);
		p->SetDlgItemText(IDC_EDIT_SEC,str);

		// set min
		m = t / 60;
		v = m % 60;
		itoa(v,str,10);
		p->SetDlgItemText(IDC_EDIT_MIN,str);

		// set hour
		v = m / 60;
		itoa(v,str,10);
		p->SetDlgItemText(IDC_EDIT_HOUR,str);
		

		// monitoring worker threads
		dwRes = WaitForMultipleObjects(p->m_autoAlarmsOption.m_nThreadCount,p->submit_workers,TRUE,0);
		if( dwRes == WAIT_OBJECT_0 )
		{
			if (p->m_bCheckAckClose)
			{
				if (p->m_alarms.IsStopped())
				{
					p->SetDlgItemText(ID_AUTO,"Auto On");
					break;
				}
			}
			else
			{
				p->SetDlgItemText(ID_AUTO,"Auto On");
				break;
			}
		}
		
		Sleep(1000);
	}

	return 0;
}


DWORD WINAPI T_UpdateAlarm(LPVOID lpParameter)
{
	while(1)
	{
		Sleep(10000);
 		reinterpret_cast<CAlarmGeneratorDlg*>(lpParameter)->UpdataActiveAlm();
	}
	return 0;
}

DWORD WINAPI T_ackAlarm(LPVOID lpParameter)
{
	while(1)
	{
 		reinterpret_cast<CAlarmGeneratorDlg*>(lpParameter)->autoa();
	}
	return 0;
}
void CAlarmGeneratorDlg::InitAutoSubmit(std::vector<std::string>& vi)
{
	std::vector<std::string> columnNames;
	
	columnNames.push_back("mat.type_description");
	columnNames.push_back("mat.meatyp_id");
	columnNames.push_back("mmc.context_name");
	columnNames.push_back("mec.channel_name");

	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
	std::string sql = "SELECT mat.type_description, mat.meatyp_id,mmc.context_name, mec.channel_name "\
		"FROM ME_ALARM_TYPE mat, ME_MESSAGE_CONTEXT mmc,ME_EVENT_CHANNEL mec "\
		"WHERE mat.isvisible=1 and mat.memcon_id = mmc.memcon_id and mmc.meecha_id = mec.meecha_id";

	agDebug("SQL Exec: ",sql.c_str());
	
	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sql;

	IData* data =  databaseConnection->executeQuery(sqlCmd,columnNames);
	std::string tempString;
	bool hasMore = true;
	while ( hasMore && data->getNumRows() > 0 )
	{
		for(int i=0; (unsigned int)i<data->getNumRows(); i++)
		{
			param_vec_ptr tmp_param = getParams(data->getStringData(i,"mat.type_description"));
			mt_ptr tmp_mt= new mt(*(new TA_Base_Core::MessageContext(data->getStringData(i, "mec.channel_name").c_str(), 
				data->getStringData(i, "mmc.context_name").c_str(), TA_Base_Core::AlarmMessage)), 
				data->getStringData(i, "mat.meatyp_id").c_str());
			msg_ptr tmp_msg = new s_msg;
			tmp_msg->p_mt = tmp_mt;
			tmp_msg->p_param = tmp_param;
			m_autoalarm.push_back(tmp_msg);
		}
		hasMore = databaseConnection->moreData(data);
	}
	
	agDebug("SQL Done.");

	columnNames.clear();
	columnNames.push_back("pkey");
	columnNames.push_back("typekey");
	columnNames.push_back("locationkey");
	columnNames.push_back("subsystemkey");
	columnNames.push_back("name");
	sql = "SELECT pkey, typekey, locationkey, subsystemkey, name FROM ENTITY where locationkey in (";
	std::vector<std::string>::iterator bgn= vi.begin();
	while(bgn != vi.end())
	{
		sql.append(*bgn);
		sql.append(",");
		++ bgn;
	}
	sql.erase(sql.find_last_of(','));
	sql.append(")");


	//YM1 20091023 : 
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "YM LOG: log sql script");
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, sql.c_str());

	agDebug("SQL Exec: ",sql.c_str());

	TA_Base_Core::SQLStatement sqlCmd2;
	sqlCmd2.strCommon = sql;

	data =  databaseConnection->executeQuery(sqlCmd2,columnNames);
	hasMore = true;
	while ( hasMore && data->getNumRows() > 0 )
	{
		for(int i=0; (unsigned int)i<data->getNumRows(); i++)
		{
			entity_ptr tmp_entity = new s_entity;
			tmp_entity->entity_key =data->getIntegerData(i, "pkey");
			tmp_entity->entity_type = data->getIntegerData(i, "typekey");
			tmp_entity->location = data->getIntegerData(i, "locationkey");
			tmp_entity->name = data->getStringData(i, "name");
			tmp_entity->subsys = data->getIntegerData(i, "subsystemkey");
			m_allentity.push_back(tmp_entity);
		}
		hasMore = databaseConnection->moreData(data);
	}

	agDebug("SQL Done.");

	// initialize critical section
	InitializeCriticalSection(&m_csAuto);

}
void CAlarmGeneratorDlg::setExcuteFilter()
{
	FUNCTION_ENTRY("setExcuteFilter");

	//get the parameter
	int iFilterInterval = atoi((TA_Base_Core::RunParams::getInstance().get(FILTER_SET_TIME_INTERVAL)).c_str());
	std::string strFilterAsset = TA_Base_Core::RunParams::getInstance().get(FILTER_SET_ASSET);
	std::string strFilterDescription = TA_Base_Core::RunParams::getInstance().get(FILTER_SET_DESCRIPTION);
	TA_Base_Bus::Filter preFilter;
	pAlarmStore=NULL;
	TA_Base_Bus::Sort sort;
	std::vector<unsigned long> locations;
	int csPos=0;
	
	if(iFilterInterval<50)
	{
		iFilterInterval=1000;
	}
	
	// Clear the filter object of all old filters
	m_filter.removeAll();
	//m_filter = createFilter();
	if(!strFilterAsset.empty())
	{
		m_filter.addFilter(TA_Base_Bus::Filter::FILTER_ASSET_NAME, TA_Base_Bus::Filter::COMPARE_CONTAINS, strFilterAsset);
	}
	if(!strFilterDescription.empty())
	{
		m_filter.addFilter(TA_Base_Bus::Filter::FILTER_DESCRIPTION, TA_Base_Bus::Filter::COMPARE_CONTAINS, strFilterDescription);
	}
	m_filter.setFilterName("FilterForTest");
	
	
	// Create the alarm store so we can get all our alarms to add
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating the alarm store");
	try
	{
		pAlarmStore = new TA_Base_Bus::AlarmStore(preFilter, locations, sort, preFilter, m_alarms.getAlarmSessonId().c_str(), false);

		if (pAlarmStore == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Alarm Store could not be constructed. New failed");
		}
		else
		{
			//pAlarmStore->createIndex(m_filter, sort,0,false,isDefault);
		}


	}
	catch( const TA_Base_Core::DataException& )
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DataException was catched");
	}
	catch( const TA_Base_Core::DatabaseException& )
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "DatabaseException was catched");
	}
	Sleep(2000);


	if(pAlarmStore != NULL && !pAlarmStore->isAlarmStoreValid())
	{
		// This means we have an invalid alarm store
		// Set the number of items to 1 so we can display an error message in the list control
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Indicating the alarm store is not yet initialised");
	}
//	TA_Base_Bus::AlarmAccessFactory::getInstance().addClient("AlarmManagers", "AlarmManagerc", "AlarmManager_remapEvName", "AlarmManager_unmapEvName", csPos, true);
	
// 	while(1)
// 	{
// 		if(m_isSetFilter && pAlarmStore)
// 		{
// 			//get the postion of alarm manager
// //			TA_Base_Bus::AlarmAccessFactory::getInstance().addClient("AlarmManagers", "AlarmManagerc", "AlarmManager_remapEvName", "AlarmManager_unmapEvName", csPos, true);
// 			pAlarmStore->setFilter(m_filter,csPos,false);
// 			pAlarmStore->AlarmStoreDutyChangeTest();
// 			Sleep(iFilterInterval);
// 		}
// 		else
// 		{
// 			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "pAlarmStore is NULL, so filter won't be set");
// 			Sleep(1000);
// 		}
// 	}
	FUNCTION_EXIT
}
bool CAlarmGeneratorDlg::autos()
{
	char strCnt[20];
	std::string strAlarmUUID;

	TA_Base_Core::NameValuePair* ptr[10];
	int alarmsize = m_autoalarm.size();
	int entitysize = m_allentity.size();
	srand( (unsigned)time( NULL ) );
	
	while(s_bThreadRunning_AutoAlarms)
	{	
		// count management
		EnterCriticalSection(&m_csAuto);
		if(m_nAutoAlarmCount)
		{
			m_nAutoAlarmCount--;
			ltoa(m_nAutoAlarmCount,strCnt,10);
			SetDlgItemText(IDC_EDIT_AUTO_ALARM_COUNT,strCnt);
			LeaveCriticalSection(&m_csAuto);
		}	
		else
		{
			LeaveCriticalSection(&m_csAuto);
			return false;
		}


		TA_Base_Core::DescriptionParameters m_descriptionParameters;
		int curalarmindex = rand() % m_autoalarm.size();
		int curentityindex = rand() % m_allentity.size();
		int i = 0;
		if( m_autoalarm[curalarmindex]->p_param != NULL )
		{
			param_vec::iterator pb = m_autoalarm[curalarmindex]->p_param->begin();
			param_vec::iterator pe = m_autoalarm[curalarmindex]->p_param->end();
			
			while(pb != pe)
			{
				ptr[i] = new TA_Base_Core::NameValuePair(*pb, "test");
				m_descriptionParameters.push_back(ptr[i]);
				++ pb;
				++ i;
			}
		}
		mt_ptr tt = m_autoalarm[curalarmindex]->p_mt; 
		m_bIsSendAlarmStore = ((CButton*)GetDlgItem(IDC_CHECK1))->GetCheck();
		std::string AssetName;
		if(m_bIsSendAlarmStore)
		{
			switch(rand()%2)
			{
			case 0:
				AssetName=(m_allentity[curentityindex]->name).c_str();
				break;
			case 1:
				AssetName="tester";
				break;
			default:
				AfxMessageBox("rand error!");	
			}
			//creat an alarm
			//CString assetName;
			//m_assetName.GetWindowText(assetName);
			timeb sourceTime = {0,0,-1,-1};
			ftime( &sourceTime );
			unsigned long alarmSeverityKey( TA_Base_Core::AlarmTypeTableAccessor::getInstance().getSeverityKey(*(m_autoalarm[curalarmindex]->p_mt)) );
			TA_Base_Core::AlarmDetailCorbaDef* p_alarmDetailCorbaDef = m_alarms.createAlarmWithParams(
				*(m_autoalarm[curalarmindex]->p_mt),
				m_allentity[curentityindex]->entity_key,
				m_allentity[curentityindex]->entity_type,
				m_descriptionParameters,
				m_allentity[curentityindex]->name,
				m_allentity[curentityindex]->location,
				m_allentity[curentityindex]->subsys,
				alarmSeverityKey,
				sourceTime,
				AssetName,
				true);
			// store the uuid and return it at the end
			strAlarmUUID = p_alarmDetailCorbaDef->alarmID.in();
			p_alarmDetailCorbaDef->alarmDescription = CORBA::string_dup(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( p_alarmDetailCorbaDef->messageTypeKey,  std::string(p_alarmDetailCorbaDef->alarmParameterList.in())).c_str());
			//insert into share alarm
			//TA_Base_Bus::SharedAlarmAccessFactory::getInstance().insertAlarm(*p_alarmDetailCorbaDef, true);
			//TA_Base_Bus::AlarmAccessFactory::getInstance().insertAlarm(*p_alarmDetailCorbaDef, true);
			pAlarmStore->newAlarm(*p_alarmDetailCorbaDef);
			
			delete p_alarmDetailCorbaDef;
		}
		else{
			switch(rand()%4)
			{
			case 0:
		/*		TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarm( 
					*(m_autoalarm[curalarmindex]->p_mt),
					m_allentity[curentityindex]->entity_key,
					m_allentity[curentityindex]->entity_type,
					m_descriptionParameters,
					m_allentity[curentityindex]->name,
					m_allentity[curentityindex]->location,
					m_allentity[curentityindex]->subsys
					);
				break;*/
			case 1:
				strAlarmUUID = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarm( 
					*(m_autoalarm[curalarmindex]->p_mt),
					m_allentity[curentityindex]->entity_key,
					m_allentity[curentityindex]->entity_type,
					m_descriptionParameters,
					m_allentity[curentityindex]->name,
					m_allentity[curentityindex]->location,
					m_allentity[curentityindex]->subsys
					);
				break;
			case 2:
		/*		TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset( 
					*(m_autoalarm[curalarmindex]->p_mt),
					m_allentity[curentityindex]->entity_key,
					m_allentity[curentityindex]->entity_type,
					m_descriptionParameters,
					m_allentity[curentityindex]->name,
					m_allentity[curentityindex]->location,
					m_allentity[curentityindex]->subsys,
					"tester",
					TA_Base_Core::AlarmConstants::defaultTime
					);
				break;*/
			case 3:
				strAlarmUUID = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset( 
					*(m_autoalarm[curalarmindex]->p_mt),
					m_allentity[curentityindex]->entity_key,
					m_allentity[curentityindex]->entity_type,
					m_descriptionParameters,
					m_allentity[curentityindex]->name,
					m_allentity[curentityindex]->location,
					m_allentity[curentityindex]->subsys,
					"tester"
					);
				break;
			default:
				AfxMessageBox("rand error!");
			}
		}

		// add submitted alarm to alarms collection for further acknowledge and close
		//TA_Base_Core::AlarmData* alarm = new TA_Base_Core::AlarmData(strAlarmUUID,strNull,AlarmConstants.defaultTime,AlarmConstants.defaultTime,t,t));
		agAlarmData* alarm = new agAlarmData();
		alarm->m_strID = strAlarmUUID;
		alarm->m_nLocationKey = m_allentity[curentityindex]->location;
		alarm->m_nSubsystemKey = m_allentity[curentityindex]->subsys;
		alarm->m_nEntityKey = m_allentity[curentityindex]->entity_key;
		alarm->m_nEntityTypeKey = m_allentity[curentityindex]->entity_type;

		alarm->m_tSubmit = time(0);
		
		if (m_bCheckAckClose)
		{
			m_alarms.add( alarm );
			agDebug("Alarm Added: ",strAlarmUUID.c_str());
		}
		else
		{
			m_alarms.addAlarmSize();
			m_alarms.updateGUI();
		}

		while(i != 0)
		{
			free(ptr[--i]);
		}
		Sleep(m_autoAlarmsOption.m_nInterval);
	}
	return false;
}
void CAlarmGeneratorDlg::autoa()
{
	
	std::vector<TA_Base_Core::AlarmDetailCorbaDef*> selectedCorbaAlarms;
	// acknowledge the selected alarms
	WaitForSingleObject(mutex, INFINITE);
	if(active_alm.size() == 0)
	{
		ReleaseMutex(mutex);
		return;
	}
	for(int i = 0; i < rand() % active_alm.size(); ++ i)
	{
		selectedCorbaAlarms.push_back(
			alarmDataToAlarmMessage(getAlarmFromDatabase(active_alm.back())));
		active_alm.pop_back();
	}
	ReleaseMutex(mutex);
	try
	{
		// To simplify implementation and testing
		// Use the super session id instead of requesting one from authentication agent
		if (selectedCorbaAlarms.empty()==false) 
			TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().acknowledgeAlarms(selectedCorbaAlarms, m_alarms.getAlarmSessonId().c_str());

	}
	catch(...)
	{
		std::string reason ("Unable to acknowledge alarms due to no alarm agent. Please try again later." );
		AfxMessageBox(reason.c_str());
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, reason.c_str() );
	}

	// delete the alarm detail and clear the vector
	for (unsigned int j=0; j<selectedCorbaAlarms.size(); ++j)
	{
		delete (selectedCorbaAlarms[j]);
	}
	selectedCorbaAlarms.clear();
}

void CAlarmGeneratorDlg::autoSubmitAlarm()
{
	CString str;
	
	GetDlgItemText(ID_AUTO, str);
	if(str.Compare("Auto On") == 0)
	{
		// Setup Auto alarm
		UpdateData();
		setAutoAlarmCount(m_nAlarmCount);
		m_autoAlarmsOption.m_nAlarmCount = m_nAlarmCount;
		m_autoAlarmsOption.m_nThreadCount = m_nThreadCount;
		m_autoAlarmsOption.m_nInterval = m_nInterval;

		// set thread run/stop flag
		s_bThreadRunning_AutoAlarms = true;

		// initialize threads
		ASSERT(m_autoAlarmsOption.m_nThreadCount<=256);
		for(int i = 0; i < m_autoAlarmsOption.m_nThreadCount; i ++)
		{
			if(submit_workers[i])
			{
				TerminateThread(submit_workers[i],0);
				CloseHandle(submit_workers[i]);
			}
			submit_workers[i] = CreateThread(NULL, NULL, ThreadProc, this, CREATE_SUSPENDED, NULL);
			ResumeThread(submit_workers[i]);
		}

		// start auto acknowledge/close
		if (m_bCheckAckClose)
		{
			//m_alarms.start(m_nExpiryMins*60,m_nThreadCount);
			m_alarms.start(m_nExpiryMins,m_nThreadCount);
			m_alarms.setSendAlarmStore(m_bIsSendAlarmStore);
			agDebug("===== INFO ===== : Auto Acknowledge/Close ON");
		}
		else
		{
			agDebug("===== INFO ===== : Auto Acknowledge/Close OFF");
		}

		// start timing and monitoring thread
		m_tStartAuto = time(0);
		m_hThreadUpdateTime = CreateThread( NULL, NULL, ThreadUpdateTime,this,NULL,NULL);

		SetDlgItemText(ID_AUTO, "Auto Off");
	}
	else
	{
		/*
		for(int i = 0; i < m_autoAlarmsOption.m_nThreadCount; i ++)
		{
			if (submit_workers[i])
			{
				TerminateThread(submit_workers[i],0);
				CloseHandle(submit_workers[i]);
				submit_workers[i] = 0;
			}
		}
		*/
		// terminate time thread
		//TerminateThread(m_hThreadUpdateTime,0);
		//CloseHandle(m_hThreadUpdateTime);
		//m_hThreadUpdateTime = 0;
		
		// stop auto submitting
		s_bThreadRunning_AutoAlarms = false;

		// stop auto ack/close
		m_alarms.stop();

		SetDlgItemText(ID_AUTO, "Auto Closing");
	}
}


bool CAlarmGeneratorDlg::obtainMsgContextAndType(std::string contextID)
{
	// Get the context and type based on alarm type selected from database
	// return true if successful otherwise false

	std::string channelID;

	std::vector<std::string> columnNames;

	columnNames.push_back("context_name");
	columnNames.push_back("meecha_id");

	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
  	
	std::string sql = "SELECT context_name, meecha_id FROM ME_MESSAGE_CONTEXT WHERE memcon_id = "+contextID;
	
	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sql;
	
	IData* data = databaseConnection->executeQuery(sqlCmd,columnNames);

	if(data->getNumRows() == 1)
	{
		m_contextName = data->getStringData(0,"context_name");
		channelID = data->getStringData(0, "meecha_id");
	}

    delete data;
    data = NULL;

	if (channelID != "" && m_contextName != "")
	{

		
		columnNames.clear();

		columnNames.push_back("channel_name");

		std::string sql = "SELECT channel_name FROM ME_EVENT_CHANNEL WHERE meecha_id = "+channelID;
		
		TA_Base_Core::SQLStatement sqlCmd;
		sqlCmd.strCommon = sql;

		IData* data = databaseConnection->executeQuery(sqlCmd,columnNames);

		if(data->getNumRows() == 1)
		{
			m_channelName = data->getStringData(0,"channel_name");
		}

		delete data;
		data = NULL;

		if (m_channelName != "")
		{
			return true;
		}
	}

	AfxMessageBox("Cannot locate Context or Channel Name in database", MB_ICONSTOP);

	return false;
}


bool CAlarmGeneratorDlg::validateSubmission()
{
	CString alarmSelect;
	CString entitySelect;
	CString severitySelect;

	m_alarmTypeSelect.GetWindowText(alarmSelect);
	m_entityKeySelect.GetWindowText(entitySelect);
	m_severitySelect.GetWindowText(severitySelect);

	if (alarmSelect.IsEmpty() || entitySelect.IsEmpty() || severitySelect.IsEmpty())
	{
		AfxMessageBox("No alarm type, entity key or severity is selected", MB_ICONSTOP);
		return false;
	}

	if ( m_alarmTypeKey <= 0 ||
		m_alarmTypeName == "" ||
		m_severity <= 0 ||
		m_entityTypeKey <= 0 ||
		m_entityTypeName == "" ||
		m_entityKey <= 0 ||
		m_entityName == "" ||
		m_subSystemKey < 0 ||
		m_region <= 0 )
	{
		AfxMessageBox("No alarm type, entity key or severity is selected", MB_ICONSTOP);
		return false;
	}

	return true;

}

void CAlarmGeneratorDlg::acknowledgeAlarm()
{
	std::vector<TA_Base_Core::AlarmDetailCorbaDef*> selectedCorbaAlarms;
	
	// acknowledge the selected alarms
	for (unsigned int i=0; i<m_currentSelectedAlarms.size(); ++i)
	{
		TA_Base_Core::IAlarmData* alarmData = m_currentSelectedAlarms[i];
		if (alarmData!=NULL)
		{
			selectedCorbaAlarms.push_back(this->alarmDataToAlarmMessage(alarmData));
			
			//sprintf(tempString, "The Alarm id: "+pkey+" of entity type: %ul, is closed\r\r\n", entityKey);
			std::stringstream tempString;
			tempString << "The Alarm id: " << alarmData->getKey();
			tempString << " of entity type: " << alarmData->getEntityKey();
			tempString << " at location key: " << alarmData->getLocationKeyOfAssociatedEntity();
			tempString << " is acknowledged\r\r\n";
			this->setDialogOutput(tempString.str());
		}
	}
	
	try
	{
		// To simplify implementation and testing
		// Use the super session id instead of requesting one from authentication agent
		if (selectedCorbaAlarms.empty()==false) 
			TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().acknowledgeAlarms(selectedCorbaAlarms, m_alarms.getAlarmSessonId().c_str());
	}
	catch(...)
	{
		std::string reason ("Unable to acknowledge alarms due to no alarm agent. Please try again later." );
		AfxMessageBox(reason.c_str());
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, reason.c_str() );
	}

	// delete the alarm detail and clear the vector
	for (unsigned int j=0; j<selectedCorbaAlarms.size(); ++j)
	{
		delete (selectedCorbaAlarms[j]);
	}
	selectedCorbaAlarms.clear();
}

void CAlarmGeneratorDlg::closeAlarm()
{
	CString closeValue;
    m_closeValue.GetWindowText(closeValue);
	std::string cValue(closeValue.GetBuffer(0));

	// close the selected alarms
	for (unsigned int i=0; i<m_currentSelectedAlarms.size(); ++i)
	{
		TA_Base_Core::IAlarmData* alarmData = m_currentSelectedAlarms[i];
		TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(alarmData->getKey(), alarmData->getLocationKeyOfAssociatedEntity(), cValue );
			
		//sprintf(tempString, "The Alarm id: "+pkey+" of entity type: %ul, is closed\r\r\n", entityKey);
		std::stringstream tempString;
		tempString << "The Alarm id: " << alarmData->getKey();
		tempString << " of entity type: ";
		tempString << alarmData->getEntityKey();
		tempString << " of close value: ";
		tempString << cValue;
		tempString << " is closed\r\r\n";
		this->setDialogOutput(tempString.str());
	}
}



/*
void CAlarmGeneratorDlg::OnEditchangeAlarmSelect() 
{
	this->m_alarmSelect.GetWindowText(m_alarmValue);
	
	this->m_alarmType = "TrueAlarmMessage";
	
	this->m_okButton.EnableWindow(true);
    	
}
*/

bool CAlarmGeneratorDlg::isLocationValid() 
{
	std::vector<std::string> columnNames;
	m_locationKey=-1;
    columnNames.push_back("pkey");
    
	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
  
	std::string sql = "select pkey from location where upper(name) = upper('"+m_locationName+"')";
	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sql;

	IData* data = databaseConnection->executeQuery(sqlCmd,columnNames);
	
	if(data->getNumRows()>0)
	{
		m_locationKey = data->getStringData(0,"pkey");
		m_region = data->getIntegerData(0,"pkey");
	}
	
	
	delete data;
	data = NULL;
	
	
	if(atoi(m_locationKey.c_str())>0)
	{
		return true;
	}
	else
	{
		AfxMessageBox("Location Invalid, please try again", MB_ICONSTOP);
			//reshow the settings dialog to re-enter tha database info
		if(!this->createSetupDialog())
		{
			exit(0);
		}
	}
	return false;
	
}

bool CAlarmGeneratorDlg::createSetupDialog()
{
	bool isInitialised = true;

	RunParams::getInstance().set("EntityName", "AlarmGenerator");

	// Look for the essential 6 initilisation parameters
	// DBConnection, Notify Hosts, Location Key, Mgr Port, User Id, and Profile Id

	if(RunParams::getInstance().isSet(RPARAM_DBCONNECTION))
	{
		m_dbConnection = RunParams::getInstance().get(RPARAM_DBCONNECTION);
	}
	else
	{
		isInitialised = false;
	}

	if(RunParams::getInstance().isSet(RPARAM_NOTIFYHOSTS))
	{
		m_notifyHosts = RunParams::getInstance().get(RPARAM_NOTIFYHOSTS);
	}
	else
	{ 
		isInitialised = false;
	}

	if(RunParams::getInstance().isSet(RPARAM_LOCATIONKEY))
	{
		m_locationKey = RunParams::getInstance().get(RPARAM_LOCATIONKEY);
		m_region = atoi(m_locationKey.c_str());
	}
	else
	{
		isInitialised = false;
	}

	if(RunParams::getInstance().isSet("UserId"))
	{
		m_userId = RunParams::getInstance().get("UserId");
	}
	else
	{
		isInitialised = false;
	}

	if(RunParams::getInstance().isSet("ProfileId"))
	{
		m_profileId = RunParams::getInstance().get("ProfileId");
	}
	else
	{
		isInitialised = false;
	}

	if(RunParams::getInstance().isSet(RPARAM_MGRPORT))
	{
		m_mgrPrt = RunParams::getInstance().get(RPARAM_MGRPORT);
	}
	
	if(!isInitialised)
	{
		CSetupDialog* setupDialog = new CSetupDialog(this, &m_notifyHosts, &m_dbConnection, &m_locationName, &m_userId, &m_profileId, &m_mgrPrt);
		
		int nResponse = setupDialog->DoModal();
		delete setupDialog;
		
		if (nResponse != IDOK)
		{
			return false;
		}
		m_dbConnectionFileDirectory="C:\\transactive\\config\\database\\"; 
		m_dbConnectionFileName = "ConnectionStrings.csv";
		std::stringstream dbConnectionFile;
		dbConnectionFile<<m_dbConnectionFileDirectory;
		dbConnectionFile<<m_locationName;
		dbConnectionFile<<m_dbConnectionFileName;
		m_dbConnectionFile = dbConnectionFile.str();
		RunParams::getInstance().set("DbConnection", m_dbConnection.c_str());
		
		RunParams::getInstance().set("NotifyHosts", m_notifyHosts.c_str());
		
		//RunParams::getInstance().set("LocationKey", m_locationKey.c_str());
		std::string lKey = "1";
		RunParams::getInstance().set("LocationKey", lKey.c_str());
		
		RunParams::getInstance().set("UserId", m_userId.c_str());

		RunParams::getInstance().set("MgrPort", m_mgrPrt.c_str());

		RunParams::getInstance().set("ProfileId", m_profileId.c_str());
		
		RunParams::getInstance().set(RPARAM_DBCONNECTIONFILE, m_dbConnectionFile.c_str());

		RunParams::getInstance().set("DataBase_TRANSACT", RPARAM_DBONLINE);
		
        // DbConnection runparam must be set before calling isLocationValid()
		if(!this->isLocationValid())
		{
			return false;
		}

		//populate the Alarm Types
		this->populateAlarmTypeSelect();
		this->populateSeveritySelect();
	}
	else
	{	
		this->populateAlarmTypeSelect();
		this->populateSeveritySelect();
	}
	
	// initialise using the specified port otherwise use default port 6666
	if (m_mgrPrt != "")
	{
		int port = atoi(m_mgrPrt.c_str());
		TA_Base_Core::CorbaUtil::getInstance().initialise(port);
	}
	else
	{
		TA_Base_Core::CorbaUtil::getInstance().initialise();
	}

	TA_Base_Core::CorbaUtil::getInstance().activate();
	
	this->m_alarmTypeSelect.EnableWindow(true);
	m_entityType.EnableWindow(false);
	m_entityKeySelect.EnableWindow(false);
	enableParamWindows(false);

	m_isUnique.SetCheck(1);
	isUniqueAlarm = true;
	m_isNotUnique.SetCheck(0);

	return true;
}

void CAlarmGeneratorDlg::enableParamWindows(bool isEnable)
{
	m_static1.SetWindowText("");
	m_static2.SetWindowText("");
	m_static3.SetWindowText("");
	m_static4.SetWindowText("");
	m_static5.SetWindowText("");
	
	m_param1.SetWindowText("");
	m_param2.SetWindowText("");
	m_param3.SetWindowText("");
	m_param4.SetWindowText("");
	m_param5.SetWindowText("");

	m_param1.SetReadOnly(!isEnable);
	m_param2.SetReadOnly(!isEnable);
	m_param3.SetReadOnly(!isEnable);
	m_param4.SetReadOnly(!isEnable);
	m_param5.SetReadOnly(!isEnable);

	m_param1.EnableWindow(isEnable);
	m_param2.EnableWindow(isEnable);
	m_param3.EnableWindow(isEnable);
	m_param4.EnableWindow(isEnable);
	m_param5.EnableWindow(isEnable);
}

void CAlarmGeneratorDlg::OnModMms() 
{
	// TODO: Add your control notification handler code here
	if ( m_activeAlarms.GetSelCount() > 1)
	{
		// Display message to inform operator that only 1 active alarm can be selected
		int response = AfxMessageBox("Sorry only ONE alarm can be modified at one time.");
		if (response == IDOK) return;
	}
	CString alarm;
	int index = 0;
	while (index < m_activeAlarms.GetCount())
	{
		if (m_activeAlarms.GetSel(index) > 0) break;
		++index;
	}
	m_activeAlarms.GetText(index, alarm);

	std::string alarmID;
	std::string entityKey;
	std::string alarmType;
	std::string contextType;
	std::string desc;
	TA_Base_Core::MmsStateType mmsType;

	// get the alarm id and details from the edit box
	int start = alarm.Find(":", 0);
	entityKey= std::string( alarm.Left( start ) );

	++start;
	int end = alarm.Find(">", start);
	alarmType= std::string( alarm.Mid(start, end-start) );

	start = ++end;
	end = alarm.Find(">", start);
	contextType= std::string( alarm.Mid(start, end-start) );

	start = ++end;
	end = alarm.Find(">", start);
	desc= std::string( alarm.Mid(start, end-start) );

	start = ++end;
	alarmID= std::string( alarm.Mid(start, alarm.GetLength()) );


	// get the current Mms State Type from database
	std::vector<std::string> columnNames;
    columnNames.push_back("mms_state_type");
    
	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
  
	std::string sql = "SELECT mms_state_type FROM ALARM WHERE alarm_id = '"+alarmID+"'";

	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sql;
	IData* data = databaseConnection->executeQuery(sqlCmd,columnNames);

	if(data->getNumRows()==1) //there should be only one alarm retreived
	{
		mmsType = static_cast<TA_Base_Core::MmsStateType> ( data->getIntegerData(0,"mms_state_type") );
	}
	else
	{
		TA_ASSERT(false, "There is duplication of alarms in the database. Please do house-keeping.");
	}
	
	// free up the database connection and instance
	delete data;
    data = NULL;

	std::string alarmDetail("Selected alarm detail:");
	alarmDetail += "\r\n";
	alarmDetail += "Alarm ID: "+alarmID;
	alarmDetail += "\r\n";
	alarmDetail += "Alarm Type: "+alarmType;
	alarmDetail += "\r\n";
	alarmDetail += "Description: "+desc;
	alarmDetail += "\r\n";
	alarmDetail += "Entity Key: "+entityKey;
	alarmDetail += "\r\n";
	alarmDetail += "Context: "+contextType;
	
	ModifyMmsDialog m_mmsDlg(alarmID, m_region, mmsType, alarmDetail, this);

	if (IDCANCEL == m_mmsDlg.DoModal())
	{
		m_activeAlarms.ResetContent();
		OnRefresh();
		return;
	}
}

void CAlarmGeneratorDlg::UpdateToAlarmStore(TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
{
	if(pAlarmStore != NULL)
	{
		pAlarmStore->updateAlarm(alarmUpdate);
	}
}

void CAlarmGeneratorDlg::setAlarmQty(unsigned long qty)
{
	m_alarmQty = qty;
}

void CAlarmGeneratorDlg::OnMultiAlarms() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_autoAlarmsOption.m_nInterval=m_nInterval;
	AlarmQtyDlg m_qtyDlg(this);

	if (IDOK == m_qtyDlg.DoModal())
	{
		if (m_alarmQty != 0)
		{
			// disable all buttons
			m_exitBut.EnableWindow(false);
			m_refreshBut.EnableWindow(false);
			m_multiAlarmBut.EnableWindow(false);
			m_mmsButton.EnableWindow(false);
			m_okButton.EnableWindow(false);
			m_ackButton.EnableWindow(false);
			m_closeButton.EnableWindow(false);
			m_ackCloseButton.EnableWindow(false);
			
			for (unsigned long i=0; i<m_alarmQty; ++i)
			{
				Sleep(m_autoAlarmsOption.m_nInterval);
				submitAlarm();
			}
		}
		// enable all buttons
		m_exitBut.EnableWindow(true);
		m_refreshBut.EnableWindow(true);
		m_multiAlarmBut.EnableWindow(true);
		m_mmsButton.EnableWindow(true);
		m_okButton.EnableWindow(true);
		m_ackButton.EnableWindow(true);
		m_closeButton.EnableWindow(true);
		m_ackCloseButton.EnableWindow(true);

		// refresh the list
		OnRefresh();
		return;
	}
	
}

TA_Base_Core::IAlarmData* CAlarmGeneratorDlg::getAlarmFromDatabase(const std::string& p_key)
{
	TA_Base_Core::IAlarmData* alarm=NULL;

	try
	{
		alarm = TA_Base_Core::AlarmAccessFactory::getInstance().getAlarm(p_key);
	}
	catch(TA_Base_Core::DataException& ex)
	{
		if (alarm!=NULL)
		{
			delete alarm;
			alarm = NULL;
		}
		AfxMessageBox(ex.what());
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, ex.what());
	}
	catch(TA_Base_Core::DatabaseException& ex)
	{
		if (alarm!=NULL)
		{
			delete alarm;
			alarm = NULL;
		}
		AfxMessageBox("Database connection failed\r\nplease check the connection string and try again", MB_ICONSTOP);
		//reshow the settings dialog to re-enter tha database info
		if(!this->createSetupDialog())
		{
			exit(0);
		}
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, ex.what());
	}

	return alarm;
}

//
// alarmDataToAlarmMessage
//
TA_Base_Core::AlarmDetailCorbaDef* CAlarmGeneratorDlg::alarmDataToAlarmMessage( TA_Base_Core::IAlarmData* p_alarmData )
{
    FUNCTION_ENTRY( "alarmDataToAlarmMessage" );

    TA_Base_Core::AlarmDetailCorbaDef* alarmMsg = new TA_Base_Core::AlarmDetailCorbaDef();

    alarmMsg->subsystemKey = p_alarmData->getSubsystemKeyOfAssociatedEntity();
    alarmMsg->locationKey = p_alarmData->getLocationKeyOfAssociatedEntity();
    //alarmMsg->entityName = CORBA::string_dup(p_alarmData->getNameOfAssociatedEntity().c_str());
	alarmMsg->entityName = CORBA::string_dup("");
    alarmMsg->assocEntityTypeKey = p_alarmData->getTypeOfAssociatedEntity();

    alarmMsg->sourceTime.time = p_alarmData->getSourceTime().time;
    alarmMsg->sourceTime.millitm = p_alarmData->getSourceTime().millitm;
    alarmMsg->transactiveTime.time = p_alarmData->getTransactiveTime().time;
    alarmMsg->transactiveTime.millitm = p_alarmData->getTransactiveTime().millitm;
    alarmMsg->ackTime = p_alarmData->getAckTime();
    alarmMsg->closeTime = p_alarmData->getCloseTime();
    alarmMsg->assocEntityKey = p_alarmData->getEntityKey();
    alarmMsg->alarmSeverity = (unsigned short)p_alarmData->getSeverity();
    alarmMsg->messageTypeKey = p_alarmData->getTypeKey();
    alarmMsg->messageContext = CORBA::string_dup(p_alarmData->getContext().c_str());
    alarmMsg->alarmID = CORBA::string_dup(p_alarmData->getKey().c_str());
    alarmMsg->alarmComments = CORBA::string_dup(p_alarmData->getComments().c_str());
    alarmMsg->assetName = CORBA::string_dup(p_alarmData->getAssetName().c_str());
	alarmMsg->avalancheHeadID = CORBA::string_dup(p_alarmData->getAVLHeadId().c_str());
    //alarmMsg->alarmDescription = CORBA::string_dup(p_alarmData->getDescription().c_str());
	alarmMsg->alarmDescription = CORBA::string_dup("");
    alarmMsg->alarmParameterList = CORBA::string_dup(p_alarmData->getParamList().c_str());
    alarmMsg->parentAlarmID = CORBA::string_dup("");
    alarmMsg->messageSequenceNumber = 0;
    switch ( p_alarmData->getState() )
    {
		case TA_Base_Core::IAlarmData::CLOSED:
			alarmMsg->state = TA_Base_Core::AlarmClosed;
			break;
		case TA_Base_Core::IAlarmData::OPEN:
			alarmMsg->state = TA_Base_Core::AlarmOpen;
			break;
		case TA_Base_Core::IAlarmData::SUPPRESSED:
			alarmMsg->state = TA_Base_Core::AlarmSuppressed;
			break;
		default:
			TA_ASSERT( false, "Unknown alarm state" );
    }

    switch ( p_alarmData->getMmsStateType() )
    {
		case TA_Base_Core::IAlarmData::TYPE_NONE:
			alarmMsg->mmsState = TA_Base_Core::MMS_NONE;
			break;
		case TA_Base_Core::IAlarmData::TYPE_SEMI:
			alarmMsg->mmsState = TA_Base_Core::MMS_SEMI;
			break;
		case TA_Base_Core::IAlarmData::TYPE_AUTO:
			alarmMsg->mmsState = TA_Base_Core::MMS_AUTO;
			break;
		case TA_Base_Core::IAlarmData::TYPE_JR_SENT:
			alarmMsg->mmsState = TA_Base_Core::MMS_JR_SENT;
			break;
		case TA_Base_Core::IAlarmData::TYPE_ALM_SENT:
			alarmMsg->mmsState = TA_Base_Core::MMS_ALM_SENT;
			break;
		case TA_Base_Core::IAlarmData::TYPE_INHIBIT:
			alarmMsg->mmsState = TA_Base_Core::MMS_INHIBIT;
			break;
		case TA_Base_Core::IAlarmData::TYPE_FAILED:
			alarmMsg->mmsState = TA_Base_Core::MMS_FAILED;
			break;
		default:
			TA_ASSERT( false, "Unknown alarm mms state type" );
	}

	switch ( p_alarmData->getAVLStatus() )
	{
		case TA_Base_Core::IAlarmData::HEAD_AVL:
			alarmMsg->isHeadOfAvalanche = true;
			alarmMsg->isChildOfAvalanche = false;
			break;
		case TA_Base_Core::IAlarmData::CHILD_AVL:
			alarmMsg->isHeadOfAvalanche = false;
			alarmMsg->isChildOfAvalanche = true;
			break;
		default:
			alarmMsg->isHeadOfAvalanche = false;
			alarmMsg->isChildOfAvalanche = false;
	}

	alarmMsg->isUniqueAlarm = p_alarmData->getIsUnique();

	// TD 10687 - getOperatorDelimList uses a monster query on the event table...
    // Until we have a better (and faster!) way to retrieve session override details
    // it should remain disabled.
    std::string tempAckedBy;
	tempAckedBy = p_alarmData->getOperatorNameForAcknowledgedBySessionKey();
	alarmMsg->alarmAcknowledgedBy = CORBA::string_dup(tempAckedBy.c_str());
	alarmMsg->alarmAcknowledgedSession = CORBA::string_dup(p_alarmData->getAcknowledgedBy().c_str());
	alarmMsg->decisionSupportState = convertToDecisionSupportStateType(p_alarmData->getPlanStatus());

    // alarmMsg->mmsState = TA_Base_Core::MMS_NONE;

	// initialise display time based on alarm state
	if (p_alarmData->getState() == TA_Base_Core::IAlarmData::CLOSED)
	{
		// display time is closed time
		alarmMsg->displayTime.time = alarmMsg->closeTime;
		alarmMsg->displayTime.millitm = 0;
	}
	else
	{
		// display is the source time
		alarmMsg->displayTime = alarmMsg->sourceTime;
	}
	alarmMsg->alarmValue = CORBA::string_dup(""); // leave it empty

    FUNCTION_EXIT;

    return alarmMsg;
}

//
// convertToDecisionSupportStateType
//
TA_Base_Core::DecisionSupportStateType CAlarmGeneratorDlg::convertToDecisionSupportStateType(const std::string& stateTypeString)
{
    FUNCTION_ENTRY( "convertToDecisionSupportStateType" );

    TA_Base_Core::DecisionSupportStateType decisionStateType; 

//    if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::AUTO_PLAN_NOT_RUN) )
//    {
//        decisionStateType = TA_Base_Core::RelatedAutomaticPlanNotRun;
//    }
	if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::SEMIAUTO_PLAN_NOT_RUN) )
	{
        decisionStateType = TA_Base_Core::RelatedSemiAutomaticPlanNotRun;
	}
//    else if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::PLAN_RUN) )
//    {
//        decisionStateType = TA_Base_Core::RelatedPlanHasBeenRun;
//    }
//    else if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::PLAN_POSTPONED) )
//    {
//        decisionStateType = TA_Base_Core::RelatedPlanHasBeenPostponed;
//    }
    else if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::NO_ASSOC_PLAN) )
    {
        decisionStateType = TA_Base_Core::NoRelatedPlan;
    }
	else
	{
		TA_ASSERT(false, "Unknown DSS state type string submitted.");
	}

    FUNCTION_EXIT;
    return decisionStateType;
}
void CAlarmGeneratorDlg::OnSelchangeEntityLocationKey() 
{
	// TODO: Add your control notification handler code here
	int index = m_entityType.GetCurSel();
	if (index < 0) return;
	int locationKeyIndex = m_CLocationKey.GetCurSel();
	if (index < 0) return;
    CString locationKey;
	m_CLocationKey.GetLBText(locationKeyIndex,locationKey);
	// clear up all the entity related boxes
	m_entityKeySelect.ResetContent();
	m_subsystem.ReplaceSel("");
	m_assetName.ReplaceSel("");

	CString type;
	m_entityType.GetLBText(index, type);

	int start =	type.Find(">");
	int next = start + 1;
	int end = type.Find(">", next);
	std::string key( type.Left(start) );
	CString tname = type.Mid(next, end-next);

	m_entityTypeName = tname.GetBuffer(0);
	m_entityTypeKey = atol(key.c_str());
	
	std::vector<std::string> columnNames;

    columnNames.push_back("pkey");
	columnNames.push_back("name");
	columnNames.push_back("subsystemkey");

	
    
	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
  	
	std::string sql = "SELECT pkey, name, subsystemkey FROM ENTITY WHERE typekey = "+key;
				sql += " and locationkey = "+locationKey;
	
	
	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sql;
	IData* data = databaseConnection->executeQuery(sqlCmd,columnNames);
	int count = 0;
	BOOL hasMore = true;
	while(hasMore && data->getNumRows()>0)
	{
		m_entityKeySelect.EnableWindow(true);
		std::string tempString;

		for(int i=0; (unsigned int)i<data->getNumRows(); i++)
		{
			tempString.erase();
			char buffer[20];
			int key = data->getIntegerData(i,"pkey");
			itoa(key, buffer, 10);
			tempString += buffer;
			tempString += ">";
			tempString += data->getStringData(i,"name");
			tempString += "> Subsystem:";
			char buffer1[20];
			int subsystem = data->getIntegerData(i, "subsystemkey");
			itoa(subsystem, buffer1, 10);
			tempString += buffer1;
			m_entityKeySelect.AddString(tempString.c_str());
			count++;
		}
	

		std::string outText("");
		outText += "\r\n";
		outText += "Entity Type Key: ";
		outText += key;
		outText += "\r\n";
		outText += "Entity Type Name: ";
		outText += m_entityTypeName;
		this->setDialogOutput(outText);
		hasMore = databaseConnection->moreData( data );
	}

    delete data;
    data = NULL;
}




void CAlarmGeneratorDlg::UpdataActiveAlm()
{
	active_alm.clear();
	std::vector<std::string> columnNames;
    columnNames.push_back("alarm_id");
    
	IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
  
	
	char tempString[128];
	itoa(TA_Base_Core::AlarmData::OPEN, tempString, 10);
	std::string sqlOpen = "select alarm_idfrom ALARM where STATE = "+std::string(tempString);

	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sqlOpen;

	IData* dataOpen = databaseConnection->executeQuery(sqlCmd,columnNames);
	BOOL hasMore = true;
	while(hasMore && dataOpen->getNumRows() > 0)
	{
		for(int i=0; (unsigned int)i<dataOpen->getNumRows(); i++)
		{
			active_alm.push_back(dataOpen->getStringData(i, "alarm_id"));
		}
		hasMore = databaseConnection->moreData( dataOpen );
	}
	delete dataOpen;
    dataOpen = NULL;
}

void CAlarmGeneratorDlg::setAutoAlarmCount(unsigned long cnt)
{
	EnterCriticalSection(&m_csAuto);
	m_nAutoAlarmCount = cnt;
	LeaveCriticalSection(&m_csAuto);
}







void CAlarmGeneratorDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
    if(m_isSetFilter == false)
	{
		m_isSetFilter = true;
		SetDlgItemText(IDC_BUTTON1,"StopSetFilter");
	}
	else
	{
		m_isSetFilter = false;
		SetDlgItemText(IDC_BUTTON1,"StartSetFilter");
	}
	
}
