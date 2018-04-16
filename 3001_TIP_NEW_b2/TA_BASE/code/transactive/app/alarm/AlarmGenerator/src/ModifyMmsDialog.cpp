// ModifyMmsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "alarmgenerator.h"
#include "ModifyMmsDialog.h"

#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmModificationHelper.h"

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\DataException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ModifyMmsDialog dialog


ModifyMmsDialog::ModifyMmsDialog(const std::string alarmId, unsigned long locationKey, TA_Base_Core::MmsStateType curMms, std::string alarmDetail, CWnd* pParent /*=NULL*/)
	: CDialog(ModifyMmsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(ModifyMmsDialog)
	//m_AlarmText = _T("");
	//}}AFX_DATA_INIT

	m_mmsStateType = convertMmsStateTypeToString(curMms);
	m_alarmId = alarmId;
	m_locationKey = locationKey;
	m_alarmDetail = alarmDetail;
}

BOOL ModifyMmsDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	setAllRadioTextExcept(m_mmsStateType);
	disableAllRadioExcept(0);

	m_alarmDetail += "\r\nMms Type: ";
	m_alarmDetail += m_mmsStateType;
	setAlarmDetail(m_alarmDetail);

	return TRUE;
}

void ModifyMmsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ModifyMmsDialog)
	DDX_Control(pDX, IDCOMMIT, m_MmsCommit);
	DDX_Control(pDX, IDCLOSE, m_MmsClose);
	DDX_Control(pDX, IDC_MMS_CUR, m_CurMms);
	DDX_Control(pDX, IDC_MMS1, m_Mms1);
	DDX_Control(pDX, IDC_MMS2, m_Mms2);
	DDX_Control(pDX, IDC_MMS3, m_Mms3);
	DDX_Control(pDX, IDC_MMS4, m_Mms4);
	DDX_Control(pDX, IDC_MMS5, m_Mms5);
	DDX_Control(pDX, IDC_MMS6, m_Mms6);
	DDX_Control(pDX, IDC_ALARM_DETAIL, m_AlarmDetail);
	//DDX_Text(pDX, IDC_ALARM_DETAIL, m_AlarmText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ModifyMmsDialog, CDialog)
	//{{AFX_MSG_MAP(ModifyMmsDialog)
	ON_BN_CLICKED(IDCOMMIT, OnCommit)
	ON_BN_CLICKED(IDC_MMS6, OnMms6)
	ON_BN_CLICKED(IDC_MMS5, OnMms5)
	ON_BN_CLICKED(IDC_MMS4, OnMms4)
	ON_BN_CLICKED(IDC_MMS3, OnMms3)
	ON_BN_CLICKED(IDC_MMS2, OnMms2)
	ON_BN_CLICKED(IDC_MMS1, OnMms1)
	ON_BN_CLICKED(IDC_MMS_CUR, OnMmsCur)
	ON_BN_CLICKED(IDCLOSE, OnClose)
	ON_BN_CLICKED(IDC_REFRESH, OnRefresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ModifyMmsDialog message handlers

BOOL ModifyMmsDialog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DestroyWindow();
}

std::string ModifyMmsDialog::convertMmsStateTypeToString(TA_Base_Core::MmsStateType mmsStateType)
{
	std::string mmsString("");

	switch (mmsStateType)
	{
		case (TA_Base_Core::MMS_NONE):
			mmsString = "NONE";
			break;
		case (TA_Base_Core::MMS_SEMI):
			mmsString = "SEMI";
			break;
		case (TA_Base_Core::MMS_AUTO):
			mmsString = "AUTO";
			break;
		case (TA_Base_Core::MMS_JR_SENT):
			mmsString = "JR_SENT";
			break;
		case (TA_Base_Core::MMS_INHIBIT):
			mmsString = "INHIBIT)";
			break;
		case (TA_Base_Core::MMS_ALM_SENT):
			mmsString = "ALM_SENT";
			break;
		case (TA_Base_Core::MMS_FAILED):
			mmsString = "FAILED";
			break;
		default:
			TA_ASSERT(false, "Unknown MMS State Type specified");
			break;
	}
	return mmsString;
}

TA_Base_Core::MmsStateType ModifyMmsDialog::convertStringToMmsStateType(std::string mmsState)
{
	TA_Base_Core::MmsStateType stateType(TA_Base_Core::MMS_NONE);

	if (mmsState.compare("NONE") == 0) stateType = TA_Base_Core::MMS_NONE;
	else if (mmsState.compare("SEMI") == 0) stateType = TA_Base_Core::MMS_SEMI;
	else if (mmsState.compare("AUTO") == 0) stateType = TA_Base_Core::MMS_AUTO;
	else if (mmsState.compare("JR_SENT") == 0) stateType = TA_Base_Core::MMS_JR_SENT;
	else if (mmsState.compare("ALM_SENT") == 0) stateType = TA_Base_Core::MMS_ALM_SENT;
	else if (mmsState.compare("INHIBIT") == 0) stateType = TA_Base_Core::MMS_INHIBIT;
	else if (mmsState.compare("FAILED") == 0) stateType = TA_Base_Core::MMS_FAILED;
	else TA_ASSERT(false, "Unknown MMS State Type specified");// This is a case of unknown type

	return stateType;
}

void ModifyMmsDialog::setAllRadioTextExcept(std::string curStateType)
{
	CString curType(curStateType.c_str());

	m_CurMms.SetWindowText(curType);

	if (curStateType.compare("NONE") == 0)
	{
		setRadioButtonText("SEMI", 1);
		setRadioButtonText("AUTO", 2);
		setRadioButtonText("JR_SENT", 3);
		setRadioButtonText("ALM_SENT", 4);
		setRadioButtonText("INHIBIT", 5);
		setRadioButtonText("FAILED", 6);
		return;
	}
	else if (curStateType.compare("SEMI") == 0)
	{
		setRadioButtonText("NONE", 1);
		setRadioButtonText("AUTO", 2);
		setRadioButtonText("JR_SENT", 3);
		setRadioButtonText("ALM_SENT", 4);
		setRadioButtonText("INHIBIT", 5);
		setRadioButtonText("FAILED", 6);
		return;
	}
	else if (curStateType.compare("AUTO") == 0)
	{
		setRadioButtonText("NONE", 1);
		setRadioButtonText("SEMI", 2);
		setRadioButtonText("JR_SENT", 3);
		setRadioButtonText("ALM_SENT", 4);
		setRadioButtonText("INHIBIT", 5);
		setRadioButtonText("FAILED", 6);
		return;
	}
	else if (curStateType.compare("JR_SENT") == 0)
	{
		setRadioButtonText("NONE", 1);
		setRadioButtonText("SEMI", 2);
		setRadioButtonText("AUTO", 3);
		setRadioButtonText("ALM_SENT", 4);
		setRadioButtonText("INHIBIT", 5);
		setRadioButtonText("FAILED", 6);
		return;
	}
	else if (curStateType.compare("ALM_SENT") == 0)
	{
		setRadioButtonText("NONE", 1);
		setRadioButtonText("SEMI", 2);
		setRadioButtonText("AUTO", 3);
		setRadioButtonText("JR_SENT", 4);
		setRadioButtonText("INHIBIT", 5);
		setRadioButtonText("FAILED", 6);
		return;
	}
	else if (curStateType.compare("INHIBIT") == 0)
	{
		setRadioButtonText("NONE", 1);
		setRadioButtonText("SEMI", 2);
		setRadioButtonText("AUTO", 3);
		setRadioButtonText("JR_SENT", 4);
		setRadioButtonText("ALM_SENT", 5);
		setRadioButtonText("FAILED", 6);
		return;
	}
	else if (curStateType.compare("FAILED") == 0)
	{
		setRadioButtonText("NONE", 1);
		setRadioButtonText("SEMI", 2);
		setRadioButtonText("AUTO", 3);
		setRadioButtonText("JR_SENT", 4);
		setRadioButtonText("ALM_SENT", 5);
		setRadioButtonText("INHIBIT", 6);
		return;
	}
	else // This is a case of unknown type
	{
		TA_ASSERT(false, "Unknown MMS State Type specified");
	}
}

void ModifyMmsDialog::setRadioButtonText(std::string text, int radioNumber)
{
	CString radioText(text.c_str());

	switch (radioNumber)
	{
		case (1):
			m_Mms1.SetWindowText(radioText);
			break;
		case (2):
			m_Mms2.SetWindowText(radioText);
			break;
		case (3):
			m_Mms3.SetWindowText(radioText);
			break;
		case (4):
			m_Mms4.SetWindowText(radioText);
			break;
		case (5):
			m_Mms5.SetWindowText(radioText);
			break;
		case (6):
			m_Mms6.SetWindowText(radioText);
			break;
		default:
			TA_ASSERT(false, "Radio Button number specified does not exit");
			break;
	}
}

void ModifyMmsDialog::setAlarmDetail(std::string alarmDetail)
{
	m_AlarmDetail.SetSel(0, -1);
	m_AlarmDetail.Clear();
	m_AlarmDetail.ReplaceSel(alarmDetail.c_str());
}

void ModifyMmsDialog::OnCommit() 
{
	// Nothing is done if there is no change
	CString curMms;
	m_CurMms.GetWindowText(curMms);
	if (curMms.CompareNoCase(m_mmsStateType.c_str()) == 0) return;

	//std::auto_ptr<TA_Base_Core::AlarmModificationHelper> alarmModHelper( TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper() );

	TA_Base_Core::MmsStateType mmsType = convertStringToMmsStateType(m_mmsStateType);

	try
	{
		TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().setAlarmMmsState(m_alarmId, m_locationKey, mmsType);
	}
	catch (...)
	{
		// Display message to inform operator that only 1 active alarm can be selected
		int response = AfxMessageBox("Sorry unable to set alarm mms state type. Please check log file.");
		if (response == IDOK) 
		{	
			// set back the mms state to the current one
			this->OnMmsCur();
			return;
		}
	}
	
	// if successful update the detail display
	refreshAlarmDetail();
}

void ModifyMmsDialog::OnClose() 
{
	// TODO: Add your control notification handler code here
	this->OnCancel();
}

void ModifyMmsDialog::OnMms6() 
{
	// TODO: Add your control notification handler code here
	disableAllRadioExcept(6);
}

void ModifyMmsDialog::OnMms5() 
{
	// TODO: Add your control notification handler code here
	disableAllRadioExcept(5);
	
}

void ModifyMmsDialog::OnMms4() 
{
	// TODO: Add your control notification handler code here
	disableAllRadioExcept(4);
	
}

void ModifyMmsDialog::OnMms3() 
{
	// TODO: Add your control notification handler code here
	disableAllRadioExcept(3);
	
}

void ModifyMmsDialog::OnMms2() 
{
	// TODO: Add your control notification handler code here
	disableAllRadioExcept(2);
	
}

void ModifyMmsDialog::OnMms1() 
{
	// TODO: Add your control notification handler code here
	disableAllRadioExcept(1);	
}

void ModifyMmsDialog::OnMmsCur() 
{
	// TODO: Add your control notification handler code here
	disableAllRadioExcept(0);	
}

void ModifyMmsDialog::disableAllRadioExcept(int radioNumber) 
{
	// ensure all other radio buttons are not selected except for radio number parameter
	// and set the mms state type to the radio number parameter selected
	m_CurMms.SetCheck(0);
	m_Mms1.SetCheck(0);
	m_Mms2.SetCheck(0);
	m_Mms3.SetCheck(0);
	m_Mms4.SetCheck(0);
	m_Mms5.SetCheck(0);
	m_Mms6.SetCheck(0);
	
	CString winText;

	switch (radioNumber)
	{
		case (0):
			m_CurMms.SetCheck(1);
			m_CurMms.GetWindowText(winText);
			m_mmsStateType = winText.GetBuffer(0);
			break;
		case (1):
			m_Mms1.SetCheck(1);
			m_Mms1.GetWindowText(winText);
			m_mmsStateType = winText.GetBuffer(0);
			break;
		case (2):
			m_Mms2.SetCheck(1);
			m_Mms2.GetWindowText(winText);
			m_mmsStateType = winText.GetBuffer(0);
			break;
		case (3):
			m_Mms3.SetCheck(1);
			m_Mms3.GetWindowText(winText);
			m_mmsStateType = winText.GetBuffer(0);
			break;
		case (4):
			m_Mms4.SetCheck(1);
			m_Mms4.GetWindowText(winText);
			m_mmsStateType = winText.GetBuffer(0);
			break;
		case (5):
			m_Mms5.SetCheck(1);
			m_Mms5.GetWindowText(winText);
			m_mmsStateType = winText.GetBuffer(0);
			break;
		case (6):
			m_Mms6.SetCheck(1);
			m_Mms6.GetWindowText(winText);
			m_mmsStateType = winText.GetBuffer(0);
			break;
		default:
			TA_ASSERT(false, "Radio Button number specified does not exit");
			break;
	}
}

void ModifyMmsDialog::refreshAlarmDetail()
{
	CString oldDetail=m_alarmDetail.c_str();
	CString newDetail;

	// Copy the text left of the last colon
	int start = oldDetail.ReverseFind(':');
	newDetail = oldDetail.Left(start+2);
	
	// Extract the new mms from the data base
	TA_Base_Core::MmsStateType mmsType;
	std::vector<std::string> columnNames;
    columnNames.push_back("mms_state_type");
    
	TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Alarms_Cd, TA_Base_Core::Read);
  
	std::string sql = "select mms_state_type from ALARM where alarm_id = '"+m_alarmId+"'";
	TA_Base_Core::SQLStatement sqlCmd;
	sqlCmd.strCommon = sql;

	TA_Base_Core::IData* data = databaseConnection->executeQuery(sqlCmd,columnNames);

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

	std::string mmsString = convertMmsStateTypeToString(mmsType);
	m_alarmDetail.erase();
	m_alarmDetail = newDetail.GetBuffer(0);
	m_alarmDetail += mmsString.c_str();

	setAlarmDetail(m_alarmDetail);

	setAllRadioTextExcept(m_mmsStateType);
	disableAllRadioExcept(0);
}

void ModifyMmsDialog::OnRefresh() 
{
	// TODO: Add your control notification handler code here
	refreshAlarmDetail();
}
