// TrainSimulatorDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "IView.h"
#include "IController.h"
#include "resource.h"
#include "afxwin.h"
#include "GlobalDataTypes.h"
#include <fstream>
#include <afx.h>

#include "HeartBeatDlgConfig.h"
#include "app/radio/radio_simulator/TrainSimulator/ITrainHeartBeat.h"
#include "CTrainStatusConfig.h"
#include "SendingAutoRespConf.h"

#include "CTrainStatusConfig.h"
#include "AutoSendDataModel.h"


// CTrainSimulatorDlg dialog
using namespace TA_IRS_App;
class CTrainSimulatorDlg : public CDialog, public IView, public ITrainHeartBeat
{
// Construction
public:
	CTrainSimulatorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TRAINSIMULATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	
	void addListener(IController* controller);
	void updateUI(MessageInfo msgInfo);

	virtual void addTrainHeartbeat(int trainID, int delay);
	void sendStatusTrainHeartBeat(TrainStatus stat, int trainID);

	void addTrainResponseStrategy(const TrainStrategyInfo& info);

	void removeTrainResponseStrategy(int trainID);

	int getTrainIDSelected() const;
	int getTrainIDMultipleSelected() const;
	int trainIDSelected(int nID) const;
	int getStatusSelected();
	CommandType getCommandSelected() const;

	std::vector<CString> getTTISCmdStatus() const;
	std::vector<CString> getChangeoverCmdStatus() const;
	void loadStatusData(CommandType command);
	void loadTTISStatusData();
	void loadChangeoverStatusData();
	CString getStatusTextSelected();
	

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT onUpdateLogView(WPARAM wparm, LPARAM lparam);

	afx_msg LRESULT OnUIUpdateHeartBeatInfo(WPARAM wParm,LPARAM lParm);

	DECLARE_MESSAGE_MAP()

	void setBtnCaption(const std::string& caption);
	void updateListInfo(const std::string& title, const std::string& msge);
	void readConfig();

	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

	BOOL checkServerStatus();
	BOOL sendStatusMessage(int status);

public:
	afx_msg void OnBnClickedButtonStartserver();
	CListCtrl m_listInfo;
	CComboBox m_comboTsiSetup;
	IController* m_controller;
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnRadioIdSelectionChange();
	//CEdit m_logView;
	CEdit m_logView;

	FILE *						m_fileHandler;
	CString						m_strFileName;
	CFile*                      m_FileLogs;


private:
	void updateLogFile(const std::string& log);
	void initializeData();
	int getCommandID(CommandType command) const;
	BOOL ttisStatusCheck();

	int trainSelectedType;

	CHeartBeatDlgConfig m_dlg;
	void clearData(int nID);
	void populateTrains(int nID);
	std::vector<int> getTrainsSelected() const;
	CTrainStatusConfig m_dlgTrainStatus;
	int m_logFlag;
	
public:

	afx_msg void OnBnClickedButtonSendStatus();
	CComboBox m_comboStatus;
	CComboBox m_combo_trainID;
	CComboBox m_combo_trainIDMultiple;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonTrainHeartbetat();

	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonChangeoverStatus();
	CComboBox m_TimsCommand;
	afx_msg void OnCbnSelchangeComboTimsCommand();
	afx_msg void OnBnClickedRadioSingleTrain();
	afx_msg void OnBnClickedRadioMultipleTrains();
	CStatic m_toTrainID;
	afx_msg void OnCbnSelchangeComboTrainId();
	afx_msg void OnBnClickedButtonStatusConfig();

	private:
		CSendingAutoRespConf m_autoSendingConfDlg;
public:
	afx_msg void OnBnClickedButton2();
};
