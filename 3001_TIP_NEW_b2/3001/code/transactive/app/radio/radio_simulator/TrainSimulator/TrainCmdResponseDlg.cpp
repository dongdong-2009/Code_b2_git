// TrainCmdResponseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainSimulatorDlg.h"
#include "HeartBeatDlgConfig.h"
#include "TrainCmdResponseDlg.h"


// CTrainCmdResponseDlg dialog

IMPLEMENT_DYNAMIC(CTrainCmdResponseDlg, CDialog)

CTrainCmdResponseDlg::CTrainCmdResponseDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTrainCmdResponseDlg::IDD, pParent)
, m_trainID(0)
, m_timerRspnseType(0)
,m_TrainHeartBeatObserver(NULL)
{

}

CTrainCmdResponseDlg::~CTrainCmdResponseDlg()
{
}

void CTrainCmdResponseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TRAIN_ID_RESPONSE, m_trainIDCmdRspnse);

	DDX_Radio(pDX, IDC_RADIO_TIMER_SEND, m_timerRspnseType);
	DDX_Control(pDX, IDC_EDIT_SET_TIMER, m_timerSet);
}


BEGIN_MESSAGE_MAP(CTrainCmdResponseDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL_TRAIN_RESPONSE, &CTrainCmdResponseDlg::OnBnClickedButtonCancelTrainResponse)
	ON_BN_CLICKED(IDC_BUTTON_OK_TRAIN_RESPONSE, &CTrainCmdResponseDlg::OnBnClickedButtonOkTrainResponse)
	ON_BN_CLICKED(IDC_RADIO_TIMER_TIMEOUT, &CTrainCmdResponseDlg::OnBnClickedRadioTimerTimeout)
	ON_BN_CLICKED(IDC_RADIO_TIMER_SEND, &CTrainCmdResponseDlg::OnBnClickedRadioTimerSend)
END_MESSAGE_MAP()


// CTrainCmdResponseDlg message handlers

void CTrainCmdResponseDlg::setTrainID(int trainID)
{
	m_trainID = trainID;
}

BOOL CTrainCmdResponseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	CString _strTrainID;
	_strTrainID.Format(L"%d",m_trainID);
	m_trainIDCmdRspnse.SetWindowText(_strTrainID);

	return TRUE;
}

void CTrainCmdResponseDlg::OnBnClickedButtonCancelTrainResponse()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CTrainCmdResponseDlg::OnBnClickedButtonOkTrainResponse()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString _timerSet;
	m_timerSet.GetWindowText(_timerSet);
	int timer = _ttoi(_timerSet);

	if ( _timerSet.Compare(L"") == 0 && (TIMER_SEND == getTrainStrategyType()) )
	{
		::MessageBox(this->m_hWnd,L"Invalid, Set the timer value.",L"Invalid Value",MB_ICONERROR | MB_OK);
		return;
	}

	
	TrainStrategyInfo _info;
	_info.StrategyType = getTrainStrategyType();
	_info.TimerSet = timer;
	_info.TrainID = m_trainID;
	//TODO: pass the value to TrainSimulator

	m_TrainHeartBeatObserver->addTrainResponseStrategy(_info);
	OnOK();
}

TrainStrategyType CTrainCmdResponseDlg::getTrainStrategyType() const
{
	TrainStrategyType _strategyType;
	if ( m_timerRspnseType == 0)
	{
		_strategyType = TIMER_SEND;
	}
	else
		_strategyType = TIMER_TIMEOUT;

	return _strategyType;
}

void CTrainCmdResponseDlg::addObserver(TA_IRS_App::ITrainHeartBeat* observer)
{
	m_TrainHeartBeatObserver = observer;
}
void CTrainCmdResponseDlg::OnBnClickedRadioTimerTimeout()
{
	// TODO: Add your control notification handler code here
	m_timerSet.EnableWindow(FALSE);
}

void CTrainCmdResponseDlg::OnBnClickedRadioTimerSend()
{
	// TODO: Add your control notification handler code here
	m_timerSet.EnableWindow(TRUE);
}
