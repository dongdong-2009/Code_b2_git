#pragma once
#include "resource.h"
#include "afxwin.h"
#include "afxdtctl.h"
#include "afxcmn.h"

#include <map>
#include <vector>
#include <string>

#include "core\utilities\src\RunParams.h"
#include "core\data_access_interface\PidsManager\src\IPidSettings.h"
#include "core\data_access_interface\PidsManager\src\TiTrain.h"
#include "bus\mfc_extensions\src\auto_sort_list_ctrl\autosortlistctrl.h"
#include "bus\mfc_extensions\src\coloured_controls\ColorCheck.h"
#include "bus\generic_gui\src\TransActiveDialog.h"
#include "bus\signs\pids_agent\IDL\src\IPIDSManagerCorbaDef.h"
#include "app\signs\pids_manager\src\StationPidUpdater.h"
// CPidManagerDialog dialog

using namespace std;
using TA_Base_Bus::AutoSortListCtrl;
typedef TA_Base_Bus::AutoSortListCtrl::SortInfo SortInfo;

class PidMessage;

namespace TA_Base_Core
{
	class AuditMessageSender;
}

class CPidManagerDialog : public TA_Base_Bus::TransActiveDialog, public TA_Base_Core::RunParamUser //public CDialog
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
	CColorCheck m_pEmergencyCheckBox;
	CStatic m_pEmergencyLabel;
	///////////////////////////////
	//message related
	CEdit m_pMsgContent;
	CEdit m_pMsgTitle;
	///////////////////////////////
	//all messages
	
	TA_Base_Bus::AutoSortListCtrl m_pMsgListCtrl;
	// current sort information
	SortInfo m_sortInfo;

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
	//train display
	std::map<unsigned long, TiTrain*> m_pkeyTITrainConfigMap;
	CComboBox m_trainList;

	unsigned long m_entityKey;
	TA_Base_Core::AuditMessageSender*     m_auditMessageSender;

private:
	void InitPidMsgListCtrl();
	void ShowSelectedMsg();
	void saveMessage();
	void StartTimer();
	void EndTimer();
	//string getCyclicWeekly();
	MessageTime getCyclicTime();
	///
	void UpdatePidMsgListCtrl(const wstring& strDescription, const wstring& strContent, const int id);
	void EditCurSelPidMsg(const wstring& strDescription, const wstring& strContent,const int id);
	void AddNewItemToPidMsgListCtrl(const wstring& strDescription, const wstring& strContent);
	void UpdatePidMsgs(const wstring& strDescription, const wstring& strContent, const int id);
	
	bool isTimeaEqualTimeB(SYSTEMTIME timeA, SYSTEMTIME timeB);
	bool isTimeaAfterTimeB(SYSTEMTIME timeA, SYSTEMTIME timeB);

	std::string WstringToString(const std::wstring str);
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
	afx_msg void OnBnClickedButtonCancelStations();
	afx_msg void OnBnClickedButtonCancelTrain();
	afx_msg void OnTimer(UINT nIDEvent); 
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnMove(int x, int y);
	afx_msg void OnPaint();
	//afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnButtonDlgClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimeStartChange(NMHDR* pNMHDR, LRESULT* pResult) ;
	afx_msg void OnTimeEndChange(NMHDR* pNMHDR, LRESULT* pResult) ;
	virtual afx_msg LRESULT OnSetWindowPosition(WPARAM wParam, LPARAM lParam);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnLvnItemchangedListPidMsg(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEditMsgContent();
//	afx_msg void OnEnChangeEditMsgTitle();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnCbnSelchangeTrainList();
};
