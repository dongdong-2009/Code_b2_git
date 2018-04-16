#pragma once
#include "resource.h"
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"

#include <map>
#include <vector>
#include <string>

#include "app/signs/pids_manager/src/StationPidUpdater.h"
#include "app/signs/pids_manager/src/AtsTrainDataPoller.h"
#include "app/signs/pids_manager/src/TrainList.h"

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/signs/pids_agent/IDL/src/IPIDSManagerCorbaDef.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/PidsManager/src/IPidSettings.h"
// CPidManagerDialog dialog


namespace TA_IRS_App
{

class CPidManagerDialog : public TA_Base_Bus::TransActiveDialog, public TA_Base_Core::RunParamUser, public ITrainListUpdateObserver
{
	DECLARE_DYNAMIC(CPidManagerDialog)

public:
	//These two function used by PidSelectionManager
	static CPidManagerDialog* getPidDlgInstance();
	bool isSelectMsg();

	CPidManagerDialog(TA_Base_Bus::IGUIAccess* controlClass);
	//CPidManagerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPidManagerDialog();
	
// Dialog Data
	enum { IDD = IDD_PID_MANAGER_DIALOG };
	virtual BOOL OnInitDialog();
	virtual void onRunParamChange(const string& name, const string& value);

	void init();	
//	MessageTime getCyclicMessageStartTime();
	virtual void notifyTrainListUpdate(const std::vector<Train>& trainList);
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckMsgCyclic();
private:
	static CPidManagerDialog* m_pidsDlgInstance;
	//////////////////////////////
	//for cyclic message
	CButton m_pCyclicCheckBox;
	CDateTimeCtrl m_pCyclicStartTime;
	CDateTimeCtrl m_pCyclicEndTime;
	CDateTimeCtrl m_pCyclicPeriod;
	///////////////////////////////
	//message type and showing style
	CButton m_pShowFullScreen;
	CButton m_pShowBottom;
	CButton m_pEmergencyCheckBox;
	///////////////////////////////
	//message related
	CEdit m_pMsgContent;
	CEdit m_pMsgTitle;
	///////////////////////////////
	//all messages
	
	CListCtrl m_pMsgListCtrl;

	vector<TA_IRS_Core::PidMessage>	m_pidMessages;
	///////////////////////////////
	//current selected message or edited message
	wstring				m_strMsgContent;
	wstring				m_strMsgDescription;
	unsigned long		m_iCurSelection;

//	IStationPidUpdater* m_pidUpdater;
	string              m_currentStation;
	HICON m_hIcon;
	UINT				m_timerId;
	bool				m_isChanged;
	//std::map<unsigned long, TiTrain*> m_pkeyTITrainConfigMap;
	CComboBox m_trainList;
	AtsTrainDataPoller m_atsTrainDataPoller;
private:
	void InitPidMsgListCtrl();
	void ShowSelectedMsg();
	void saveMessage();
	//string getCyclicWeekly();
	MessageTime getCyclicTime();
	///
	void UpdatePidMsgListCtrl(const wstring& strDescription, const wstring& strContent, const int id);
	void EditCurSelPidMsg(const wstring& strDescription, const wstring& strContent,const int id);
	void AddNewItemToPidMsgListCtrl(const wstring& strDescription, const wstring& strContent);
	void UpdatePidMsgs(const wstring& strDescription, const wstring& strContent, const int id);
	
	bool isTimeaEqualTimeB(SYSTEMTIME timeA, SYSTEMTIME timeB);
	bool isTimeaAfterTimeB(SYSTEMTIME timeA, SYSTEMTIME timeB);

	//TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList CreateTestZones();
private:
	afx_msg void OnBnClickHelp();
	afx_msg void OnBnClickedButtonEditMsg();
//	afx_msg void OnHdnItemclickListPidMsg(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonMsgCancel();
	afx_msg void OnBnClickedButtonMsgSave();
	afx_msg void OnNMClickListPidMsg(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonCancelAll();
	afx_msg void OnBnClickedButtonSendMsg();
	afx_msg void OnBnClickedButtonCancelTrain();
	afx_msg void OnBnClickedButtonCancelStations();
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();
	//afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnButtonDlgClose();
	afx_msg void OnDestroy();
	virtual afx_msg LRESULT OnSetWindowPosition(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnItemchangedListPidMsg(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditMsgContent();
//	afx_msg void OnEnChangeEditMsgTitle();
public:
	afx_msg void OnCbnSelchangeTrainList();
};

}