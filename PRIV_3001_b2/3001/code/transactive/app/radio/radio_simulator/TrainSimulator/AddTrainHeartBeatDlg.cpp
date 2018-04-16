// AddTrainHeartBeatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainSimulatorDlg.h"
#include "AddTrainHeartBeatDlg.h"
#include <string>


// CAddTrainHeartBeatDlg dialog

IMPLEMENT_DYNAMIC(CAddTrainHeartBeatDlg, CDialog)

CAddTrainHeartBeatDlg::CAddTrainHeartBeatDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAddTrainHeartBeatDlg::IDD, pParent)
 , m_addTrainHeartBeat(NULL)
{

}

CAddTrainHeartBeatDlg::~CAddTrainHeartBeatDlg()
{
}

void CAddTrainHeartBeatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TRAIN_ID_HEARTBEAT, m_addTrain);
	DDX_Control(pDX, IDC_EDIT_DELAY_HEARTBEAT, m_addDelay);
}


void CAddTrainHeartBeatDlg::addObserver(TA_IRS_App::ITrainHeartBeat *tranHeartbeat)
{
	m_addTrainHeartBeat = tranHeartbeat;
}

BEGIN_MESSAGE_MAP(CAddTrainHeartBeatDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAddTrainHeartBeatDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAddTrainHeartBeatDlg message handlers

void CAddTrainHeartBeatDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString _trainID;
	CString _trainDelay;
	m_addTrain.GetWindowText(_trainID);
	m_addDelay.GetWindowText(_trainDelay);

	CT2CA pszTrainID(_trainID);
	std::string sTrainID(pszTrainID);
	int valTrainID = atoi(sTrainID.c_str());

	CT2CA pszTrainDelay(_trainDelay);
	std::string sTrainDelay(pszTrainDelay);
	int valTrainDelay = atoi(sTrainDelay.c_str());

	if ( valTrainID > 0 && valTrainDelay > 0)
	{
		m_addTrainHeartBeat->addTrainHeartbeat(valTrainID,valTrainDelay);
	}
	else
		::MessageBox(this->m_hWnd,L"Invalid value, check the value of Train ID and Delay in Seconds",L"Message",MB_ICONINFORMATION | MB_OK);
	
	
	OnOK();
}
