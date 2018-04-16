/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/SetupDialog.cpp,v $
  * @author:  Andrew Del Carlo
  * @version: $Revision: 1.1.2.3.16.2 $
  *
  * Last modification: $Date: 2005/05/25 03:02:48 $
  * Last modified by:  $Author: derrickl $
  * 
  * implementation of the setup dialog
  */


#include "stdafx.h"
#include "alarmgenerator.h"
#include "SetupDialog.h"
#include "alarmgeneratorDlg.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::RunParams;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog dialog


CSetupDialog::CSetupDialog(CAlarmGeneratorDlg* pParent , std::string* notifyHosts, std::string* dbConnection, std::string* locationName, std::string* profileId, std::string* userId, std::string* mgrPort)
	: CDialog(CSetupDialog::IDD, pParent)
{
	m_notifyHosts = notifyHosts;
	m_dbConnection = dbConnection;
	m_locationName = locationName;
	m_parent = pParent;
	m_profileID = profileId;
	m_userID = userId;
	m_mgrPort = mgrPort;

	//{{AFX_DATA_INIT(CSetupDialog)
	//}}AFX_DATA_INIT
}


void CSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupDialog)
	DDX_Control(pDX, IDC_LOCATION_NAME, m_locationNameEdit);
	DDX_Control(pDX, IDC_NOTIFY, m_notifyHostsEdit);
	DDX_Control(pDX, IDC_DB_CONNECTION, m_dbConnectionEdit);
	DDX_Control(pDX, IDC_PROFILE_ID, m_profileIDEdit);
	DDX_Control(pDX, IDC_USER_ID, m_userIDEdit);
	DDX_Control(pDX, IDC_MGR_PORT, m_mgrPortEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupDialog, CDialog)
	//{{AFX_MSG_MAP(CSetupDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog message handlers

void CSetupDialog::OnOK() 
{
	CString buffer;
	this->m_dbConnectionEdit.GetWindowText(buffer);
	*m_dbConnection = std::string(buffer);
	this->m_notifyHostsEdit.GetWindowText(buffer);
	*m_notifyHosts = std::string(buffer);
	this->m_locationNameEdit.GetWindowText(buffer);
	*m_locationName = std::string(buffer);

	m_profileIDEdit.GetWindowText(buffer);
	*m_profileID = std::string(buffer);

	m_userIDEdit.GetWindowText(buffer);
	*m_userID = std::string(buffer);

	m_mgrPortEdit.GetWindowText(buffer);
	*m_mgrPort = std::string(buffer);

	CDialog::OnOK();
}

void CSetupDialog::OnCancel() 
{
	CDialog::OnCancel();
}

BOOL CSetupDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(RunParams::getInstance().isSet(RPARAM_DBCONNECTION))
	{
		this->m_dbConnectionEdit.SetWindowText(RunParams::getInstance().get(RPARAM_DBCONNECTION).c_str());
	}
	else
	{
		this->m_dbConnectionEdit.SetWindowText("TRANSACT:IT01271301_SITE:IT01271301_SITE");
	}

	if(RunParams::getInstance().isSet(RPARAM_NOTIFYHOSTS))
	{
		this->m_notifyHostsEdit.SetWindowText(RunParams::getInstance().get(RPARAM_NOTIFYHOSTS).c_str());
	}
	else
	{
		this->m_notifyHostsEdit.SetWindowText("occa");
	}

	if(RunParams::getInstance().isSet(RPARAM_MGRPORT))
	{
		this->m_mgrPortEdit.SetWindowText(RunParams::getInstance().get(RPARAM_MGRPORT).c_str());
	}
	else
	{
		this->m_mgrPortEdit.SetWindowText("6666");
	}

	this->m_locationNameEdit.SetWindowText("OCC");
	m_userIDEdit.SetWindowText("1");
	m_profileIDEdit.SetWindowText("0");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
