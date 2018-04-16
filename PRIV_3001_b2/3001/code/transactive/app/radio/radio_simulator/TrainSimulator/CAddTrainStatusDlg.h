#pragma once
#include "app/radio/radio_simulator/TrainSimulator/ITrainHeartBeat.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "ITrainStatus.h"
#include "ITrainStatusUIUpdate.h"

using namespace TA_IRS_App;
// CAddTrainStatusDlg dialog

class CAddTrainStatusDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddTrainStatusDlg)

public:
	CAddTrainStatusDlg(ITrainStatusUIUpdate& statusUI, CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddTrainStatusDlg();

	// Dialog Data
	enum { IDD = IDD_DIALOG_ADD_TRAIN_STATUS };

	void setStatusData(ITrainStatus* statusInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_trainIDlst;
	CComboBox m_statusLst;

private:
	std::vector<std::string> m_statusData;
	ITrainStatusUIUpdate& m_statusUI;
public:
	afx_msg void OnBnClickedButtonAddtrain();
	afx_msg void OnBnClickedButtonCancellStatus();
};