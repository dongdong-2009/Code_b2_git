// AddTrainStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrainSimulatorDlg.h"
#include "CAddTrainStatusDlg.h"
#include <string>
#define MAX_TRAIN 150


// CAddTrainHeartBeatDlg dialog
using namespace TA_IRS_App;

IMPLEMENT_DYNAMIC(CAddTrainStatusDlg, CDialog)

CAddTrainStatusDlg::CAddTrainStatusDlg(ITrainStatusUIUpdate& statusUI, CWnd* pParent /*=NULL*/)
: m_statusUI(statusUI), CDialog(CAddTrainStatusDlg::IDD, pParent)
{

}

CAddTrainStatusDlg::~CAddTrainStatusDlg()
{
}

void CAddTrainStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_TRAINID_ADDSTATUS, m_trainIDlst);
	DDX_Control(pDX, IDC_COMBO_STATUS_ADD, m_statusLst);
}

BOOL CAddTrainStatusDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//Initialize TrainID
	for (int count=0; count < MAX_TRAIN; count++)
	{
		CString trainID;
		trainID.Format(L"%d",count+1);
		m_trainIDlst.AddString(trainID);
	}

	//Initialize Status
	std::vector<std::string>::iterator iter = m_statusData.begin();
	for( iter; iter != m_statusData.end(); iter++)
	{
		CString _status((*iter).c_str());
		m_statusLst.AddString(_status);
	}

	//set default selected value for trainID and status
	m_trainIDlst.SetCurSel(0);
	m_statusLst.SetCurSel(0);

	return TRUE;
}

void CAddTrainStatusDlg::setStatusData(ITrainStatus* statusInfo)
{
	ASSERT( statusInfo != NULL );
   
	m_statusData = statusInfo->getStatusData();
}

BEGIN_MESSAGE_MAP(CAddTrainStatusDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADDTRAIN, &CAddTrainStatusDlg::OnBnClickedButtonAddtrain)
	ON_BN_CLICKED(IDC_BUTTON_CANCELL_STATUS, &CAddTrainStatusDlg::OnBnClickedButtonCancellStatus)
END_MESSAGE_MAP()

void CAddTrainStatusDlg::OnBnClickedButtonAddtrain()
{
	// TODO: Add your control notification handler code here

	//Get train data selected
	short indexTrain;
	indexTrain = m_trainIDlst.GetCurSel();
	CString _trainID;
	m_trainIDlst.GetLBText(indexTrain,_trainID);

	//Get Status Data
	short indexStatus;
	indexStatus = m_statusLst.GetCurSel();
	CString _status;
	m_statusLst.GetLBText(indexStatus,_status);

	//Convert TCHAR to a LPCSTR
	CT2CA conTraindID(_trainID);
	std::string trainID(conTraindID);

	CT2CA conStatus(_status);
	std::string statusID(conStatus);

	//send message
	m_statusUI.UITrainStatusUdpate(ITrainStatusUIUpdate::eTrainStatus::UI_STATUS6,_ttoi(_trainID),statusID);

	OnOK();
}

void CAddTrainStatusDlg::OnBnClickedButtonCancellStatus()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
