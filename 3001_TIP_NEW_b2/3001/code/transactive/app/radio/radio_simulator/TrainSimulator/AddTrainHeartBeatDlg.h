#pragma once
#include "app/radio/radio_simulator/TrainSimulator/ITrainHeartBeat.h"
#include "afxwin.h"


// CAddTrainHeartBeatDlg dialog

class CAddTrainHeartBeatDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddTrainHeartBeatDlg)

public:
	CAddTrainHeartBeatDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddTrainHeartBeatDlg();

	void addObserver(TA_IRS_App::ITrainHeartBeat* tranHeartbeat);

	// Dialog Data
	enum { IDD = IDD_DIALOG_ADD_TRAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	TA_IRS_App::ITrainHeartBeat* m_addTrainHeartBeat;
public:
	afx_msg void OnBnClickedOk();
	CEdit m_addTrain;
	CEdit m_addDelay;
};
