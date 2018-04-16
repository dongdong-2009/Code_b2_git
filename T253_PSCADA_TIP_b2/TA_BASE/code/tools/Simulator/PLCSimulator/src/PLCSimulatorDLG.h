// PLCSimulatorDlg.h
//

#pragma once

#include "afxwin.h"
#include "RTUDialogToolBar.h"
#include "RTUDialogStatusBar.h"
#include "RTUHelpDialog.h"
#include "RTUAboutDlg.h"
#include <AFXPRIV.H>

class CPLCSimulatorDlg : public CDialog
{
public:
	CPLCSimulatorDlg( CWnd* pParent = NULL );
	void Start(void);
	void Pause(void);
	void DisExtSys(void);
	void EnaExtSys(void);
	void EnableDlg(void);
	void ProcessPrimaryCheckState();
	void ProcessDeviceCheckState();
	void SetDeviceCheckState(const char* pDeviceName, int nValue);
	// "SetDataString 123=22;1234=65535;20.0=1;20.1=0;"
	// no more spaces; set word 123 to 22, word 1234 to 3, word 20 bit 0 to 1, word 20 bit 1 to 0;
	// value: 0 -- 65535; while value > 65535, some high bit lost.
	void SetDataString(std::string addressValueString);

	enum { IDD = IDD_PLCSimulator_DIALOG };

	afx_msg void OnBnClickedLoad();
	afx_msg void OnBnClickedLoadStressCfg();
	afx_msg void OnLbnSelchangeDevice();
	afx_msg void OnBnClickedSet();
	afx_msg void OnBnClickedBybit();
	afx_msg void OnBnClickedBybyte();
	afx_msg void OnBnClickedByword();
	afx_msg void OnBnClickedBydword();
	afx_msg void OnBnClickedReloadSaveData();
	afx_msg void OnBnClickedReLoad();
	afx_msg void OnBnClickedStressOn();
	afx_msg void OnBnClickedStressOff();
	afx_msg void OnClose( );
	afx_msg void OnBnClickedLogtofile();
	afx_msg void OnBnClickedOn();
	afx_msg void OnBnClickedOff();
	afx_msg void OnBnClickedPrimary();
	afx_msg void OnBnClickedStandby();
	afx_msg void OnBnClickedSwitch();
	afx_msg void OnBnClickedGet();
	afx_msg void OnBnClickedCheckAddress();
	afx_msg void OnBnClickedDisplayHelp();
	afx_msg void OnBnClickedDisplayAbout();
	afx_msg BOOL OnToolTipText( UINT nID, NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu );
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);

protected:
	HICON m_hIcon;
	virtual void DoDataExchange( CDataExchange* pDX );
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
	afx_msg void OnPaint();
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer( UINT nIDEvent );
	afx_msg void OnSelchangeComboProtocal();
	DECLARE_MESSAGE_MAP()

private:
	void CreateToolbar( void );
	void CreateStatusBar( void );
	void AddTextToButton( UINT buttonId, CString buttonLabel );
	void CalculateToolBarStatusBarHeights( void );
	void SetStatusBarText( int aIndex, const CString &message );
	void ReSetStressCfgFile(std::string cfgFile);
	void ReSetConfigFile(std::string cfgFile);
	void ConfigFileUpdated();
	void CloseMainWindow();

private:
	CCheckListBox m_devices;
	SETTYPE m_esetType;

	UINT m_ToolBarHeight;
	UINT m_StatusBarHeight;

	CRTUDialogToolBar m_WndToolBar;
	CRTUDialogStatusBar m_WndStatusBar;
	CRTUHelpDialog m_HelpDialog;
	CRTUAboutDlg m_AboutDialog;
};
