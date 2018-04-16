// DBSyncSimulatorDlg.h : header file
//

#pragma once
#include "afxcmn.h"

#include "DbSyncListCtrl.h"
#include "DbSyncTestMonitor.h"
#include "dbSynTestClientManager.h"

// CDBSyncSimulatorDlg dialog
class CDBSyncSimulatorDlg : public CDialog
{
// Construction
public:
	CDBSyncSimulatorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DBSYNCSIMULATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	afx_msg void OnBnClickedBtnStart();

public:
	CDbSyncListCtrl m_dbSyncListCtrl;
	CString m_strPort;
	CString m_strQueueName;
	int m_nClients;
	ULONGLONG m_nStartMsgSeqID;
	ULONGLONG m_nEndMsgSeqID;
	int m_noOfInsert;
	int m_noOfGroup;
	int m_noOfPublic;
	int m_noOfMMS;
	long m_lInsertInterval;
	long m_lGroupInterval;
	long m_lPublicInterval;
	long m_lMMSInterval;
	int m_nPacketSize;
	long m_lTestDuration;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnSetupClients(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStatusUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMgsIDUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClientUpdate(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	void Initialize();
	std::string ulong64ToString(TA_AQ_Lib::ulong64 ulData);

private: 
	TA_DB_Sync::CDbSynTestClientManager* m_pManager;
	CDbSyncTestMonitor* m_pDbSyncMonitor;
	bool m_bIsPaused;
	long m_ulRunningTime;
public:
	afx_msg void OnBnClickedBtnPrintStatus();
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnDestroy();
	std::string u64tostr(TA_AQ_Lib::ulong64 u64Val);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
