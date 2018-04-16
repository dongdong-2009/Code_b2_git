#pragma once
#include "afxwin.h"
#include "app/radio/radio_simulator/TrainSimulator/GlobalDataTypes.h"
#include "app/radio/radio_simulator/TrainSimulator/ITrainHeartBeat.h"


// CTrainCmdResponseDlg dialog

class CTrainCmdResponseDlg : public CDialog
{
	DECLARE_DYNAMIC(CTrainCmdResponseDlg)

public:
	CTrainCmdResponseDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CTrainCmdResponseDlg();

	// Dialog Data
	enum { IDD = IDD_DIALOG_COMMAND_RESPONSE };

	void addObserver(TA_IRS_App::ITrainHeartBeat* observer);

	void setTrainID(int trainID);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	TrainStrategyType getTrainStrategyType() const;

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_trainIDCmdRspnse;

private:
	int m_trainID;
	TA_IRS_App::ITrainHeartBeat* m_TrainHeartBeatObserver;
public:
	afx_msg void OnBnClickedButtonCancelTrainResponse();
	
	int m_timerRspnseType;
	afx_msg void OnBnClickedButtonOkTrainResponse();
	CEdit m_timerSet;
	afx_msg void OnBnClickedRadioTimerTimeout();
	afx_msg void OnBnClickedRadioTimerSend();
};
