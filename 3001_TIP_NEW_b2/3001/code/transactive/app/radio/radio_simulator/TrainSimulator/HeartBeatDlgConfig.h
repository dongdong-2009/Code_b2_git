#pragma once
#include "afxcmn.h"
#include "GlobalDataTypes.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "app/radio/radio_simulator/TrainSimulator/ITrainHeartBeat.h"
#include "core/threads/src/Thread.h"
#include "afxwin.h"
#include <map>
#include "app/radio/radio_simulator/CommandProcessorLib/src/HeartBeatCommandProcessor.h"

// CHeartBeatDlgConfig dialog

class CHeartBeatDlgConfig : public CDialog, public TA_IRS_App::ITrainHeartBeat
{
	DECLARE_DYNAMIC(CHeartBeatDlgConfig)

public:
	CHeartBeatDlgConfig(CWnd* pParent = NULL);   // standard constructor
	virtual ~CHeartBeatDlgConfig();

	// Dialog Data
	enum { IDD = IDD_DIALOG_TRAIN_HEART_BEAT };

	void upDateUIHeartBeatInfo();

	void setHeartBeatInfo(const HeartBeatInfo& info);

	void setHeatBeatProcessor(const std::vector<TA_IRS_App::HeartBeatCommandProcessor*> heartBeatProcessor);
	void updateHeartBeatInfo();

	virtual void addTrainHeartbeat(int trainID, int delay);

	void addObserver(TA_IRS_App::ITrainHeartBeat* trainHeartBeat);

	void setCurrentState(TA_Base_Core::Thread::ThreadState_t state);
	void setCommsObserverStatus(int stat);

	afx_msg LRESULT onUIUPdate(WPARAM wParm, LPARAM lParam);
	void sendStatusTrainHeartBeat(TrainStatus stat, int trainID){}

	void addTrainResponseStrategy(const TrainStrategyInfo& info);
	void setTrainStrategyMap(const std::map<int, TrainStrategyInfo>& trainStrategy);

	void removeTrainResponseStrategy(int trainID){}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lstHeartBeatInfo;

	HeartBeatInfo m_HeartBeatInfo;
	
	afx_msg void OnBnClickedButtonAddTrain();

private:
	TA_IRS_App::ITrainHeartBeat* m_addTrainHeartBeatDlg;
	TA_Base_Core::Thread::ThreadState_t m_state;
	int m_commsObserverStat;

	bool m_isUiUpdated;
	std::map<CString,CString> TrainStatusMap;
	std::vector<TA_IRS_App::HeartBeatCommandProcessor*> m_heartBeatProcessor;

	void updateTrainsListInfo();
	void compareTrainStatusUpdate(const HeartBeatInfo& info);
	void updateTrainStatus(CString _trainID, CString _status);
	void removeTrainIDList(int trainID);

	void addTrainList(int trainID, int delay, std::string status);
	void addTrainStrategeyList(const TrainStrategyInfo& info);

	CString getTrainStrategyType(const TrainStrategyType& type);

	std::map<int, TrainStrategyInfo> TrainStrategyMapInfo;

	BOOL canAddTrainStrategyToMap(const TrainStrategyInfo& info);

	void updateTrainStrategyUI();

	void removeTrainIDStrategyMap(int trainID);
	

	
	

public:
	CButton m_trainStatus;
	afx_msg void OnNMClickListHeartbeatInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonTrainStatus();
	afx_msg void OnBnClickedButtonKillTrain();
	CListCtrl m_lstResponseStrategy;
	afx_msg void OnBnClickedButtonAddStrategy();
	afx_msg void OnBnClickedButtonRemoveTrainStrategy();
};
