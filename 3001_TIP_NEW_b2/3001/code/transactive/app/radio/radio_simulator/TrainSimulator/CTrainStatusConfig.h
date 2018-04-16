#pragma once
#include "app/radio/radio_simulator/TrainSimulator/ITrainHeartBeat.h"
#include "afxwin.h"
#include "afxcmn.h"

#include "ITrainStatus.h"
#include "TrainStatus6.h"
#include "CAddTrainStatusDlg.h"
#include "ITrainStatusUIUpdate.h"

// CTrainStatusConfig dialog
using namespace TA_IRS_App;

class CTrainStatusConfig : public CDialog, public ITrainStatusUIUpdate
{
	DECLARE_DYNAMIC(CTrainStatusConfig)

public:
	CTrainStatusConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTrainStatusConfig();

	// Dialog Data
	enum { IDD = IDD_DIALOG_TRAIN_STATUS_CONFIG };

	//ITrainStatusUIUpdate interface
	void UITrainStatusUdpate(eTrainStatus trainStatus, 
							int trainID, 
							const std::string& status);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_listStatus5;
	CListCtrl m_listStatus1;
	
	CListCtrl m_listStatus6;
	afx_msg void OnBnClickedButtonAddstatus6();

private:

	ITrainStatus* m_status6;
	CAddTrainStatusDlg* m_trainStatus6;

	void initialize();
	void UIUpdateStatus6(int trainID, const std::string& status);
};