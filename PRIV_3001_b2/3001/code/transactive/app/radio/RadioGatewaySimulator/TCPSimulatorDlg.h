// TCPSimulatorDlg.h : header file
//

#pragma once

#include "DlgWholeText.h"
#include "afxwin.h"
#include "afxcmn.h"

// CTCPSimulatorDlg dialog
#define TIMER_CLIENT_SEND   1
class TCPServer;
class TCPClient;

class CTCPSimulatorDlg : public CDialog
{
// Construction
public:
	CTCPSimulatorDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TCPSIMULATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	void ReadConfig(bool bUpdateConnectIP = true);
	bool StartServer();
	void StopServer();
	void ServerSendMsg();
	void RefreshPointTable();
	bool ConnectServer();
	void SendMsg();
	void DisconnectServer();
	afx_msg LRESULT OnServerEvent(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnClientEvent(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnServerNote(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnClientNote(WPARAM wParam,LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnBnClickedBtStartserver();
	afx_msg void OnBnClickedBtGetpointtable();
	afx_msg void OnBnClickedBtConnectserver();
	afx_msg void OnBnClickedChInfohex();
	afx_msg void OnBnClickedChOutputhex();
	afx_msg void OnBnClickedBtSend();
private:
	TCPServer *m_TCPServer;
	TCPClient *m_TCPClient;
	//SimulatorConfig m_config;
	bool	b_TimerSendRunning;
	CDlgWholeText m_DlgWholeText;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CListCtrl m_LT_ServerInfo;
	CListCtrl m_LT_ClientInfo;
	afx_msg void OnBnClickedBtClearServerinfo();
	afx_msg void OnBnClickedBtClearClientinfo();
	afx_msg void OnBnClickedBtSvrSend();
	afx_msg void OnNMDblclkLtServerInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkLtClientInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedChServerCrc();
	afx_msg void OnBnClickedChClientCrc();
};
