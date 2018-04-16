// AddTrainHeartBeatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainSimulatorDlg.h"
#include "CTrainStatusConfig.h"
#include "CAddTrainStatusDlg.h"
#include <string>

#include "ITrainStatus.h"
#include "TrainStatus6.h"


// CAddTrainHeartBeatDlg dialog

IMPLEMENT_DYNAMIC(CTrainStatusConfig, CDialog)

CTrainStatusConfig::CTrainStatusConfig(CWnd* pParent /*=NULL*/)
: CDialog(CTrainStatusConfig::IDD, pParent)
, m_status6(NULL)
, m_trainStatus6(NULL)
{
	//Initialize objects
	initialize();
}

CTrainStatusConfig::~CTrainStatusConfig()
{
	//Train Status 6
	if ( NULL != m_status6 )
	{
		delete m_status6;
		m_status6 = NULL;
	}

	if ( NULL != m_trainStatus6 )
	{
		delete m_trainStatus6;
		m_trainStatus6 = NULL;
	}
}

void CTrainStatusConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STATUS5, m_listStatus5);
	DDX_Control(pDX, IDC_LIST_STATUS1, m_listStatus1);
	DDX_Control(pDX, IDC_LIST_STATUS6, m_listStatus6);
}

BOOL CTrainStatusConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Initialize Status 1
	m_listStatus1.InsertColumn(0,L"Train ID",LVCFMT_LEFT,50);
	m_listStatus1.InsertColumn(1,L"Status 1",LVCFMT_LEFT,100);

	m_listStatus1.ModifyStyle(0, LVS_REPORT, NULL);
	m_listStatus1.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);


	//Initialize Status 5
	m_listStatus5.InsertColumn(0,L"Train ID",LVCFMT_LEFT,50);
	m_listStatus5.InsertColumn(1,L"Status 5",LVCFMT_LEFT,150);

	m_listStatus5.ModifyStyle(0, LVS_REPORT, NULL);
	m_listStatus5.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);


	//Initialize Status 6
	m_listStatus6.InsertColumn(0,L"Train ID",LVCFMT_LEFT,50);
	m_listStatus6.InsertColumn(1,L"Status 6",LVCFMT_LEFT,150);

	m_listStatus6.ModifyStyle(0, LVS_REPORT, NULL);
	m_listStatus6.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_ONECLICKACTIVATE);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CTrainStatusConfig, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDSTATUS6, &CTrainStatusConfig::OnBnClickedButtonAddstatus6)
END_MESSAGE_MAP()

void CTrainStatusConfig::OnBnClickedButtonAddstatus6()
{
	// TODO: Add your control notification handler code here
	m_trainStatus6->DoModal();
}

void CTrainStatusConfig::initialize()
{
	//Train Status 6
	m_status6 = new TrainStatus6;
	m_trainStatus6 = new CAddTrainStatusDlg(*this);
	m_trainStatus6->setStatusData(m_status6);
}

void CTrainStatusConfig::UITrainStatusUdpate(eTrainStatus trainStatus, int trainID, const std::string& status )
{
	switch(trainStatus)
	{
	case UI_STATUS1:
		//TODO:
		break;

	case UI_STATU5:
		//TODO
		break;

	case UI_STATUS6:
		UIUpdateStatus6(trainID,status);
		break;
	}
}

void CTrainStatusConfig::UIUpdateStatus6(int trainID, const std::string& status)
{
	CString _trainID;
	_trainID.Format(L"%d",trainID);

	CString _status(status.c_str());

	//Search any existing data
	LVFINDINFO _info;
	_info.flags = LVFI_PARTIAL | LVFI_STRING;
	_info.psz = _trainID;
	int item = m_listStatus6.FindItem(&_info);

	int row = m_listStatus6.GetItemCount();

	//if trainID doesn't exist, insert the new train data
	if ( item < 0 )
	{
		m_listStatus6.InsertItem(row,L"");
		m_listStatus6.SetItemText(row,0,_trainID);
		m_listStatus6.SetItemText(row,1,_status);
	}
	else
		::MessageBox(this->m_hWnd,L"TrainID cannot be inserted \nbecause it is already exist in the list",L"TrainID Info",MB_ICONINFORMATION | MB_OK );
	
}