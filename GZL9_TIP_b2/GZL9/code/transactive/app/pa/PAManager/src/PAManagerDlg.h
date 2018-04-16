/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#if !defined(AFX_PAMANAGERDLG_H__D55DB830_1342_4D9E_BA4C_C554E25FC74B__INCLUDED_)
#define AFX_PAMANAGERDLG_H__D55DB830_1342_4D9E_BA4C_C554E25FC74B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "app/pa/PAManager/src/PaDvaTabCtrl.h"
#include "app/pa/PAManager/src/ICurSelDVAMsgObserver.h"
#include "app/pa/PAManager/src/IDVAMsgShortcutConfigObserver.h"
#include "app/pa/PAManager/src/NumbericEdit.h"
#include "app/pa/PAManager/src/PaRuntimeModel.h"

class TAPropertySheet;
class EmergencyDVAMsgListPage;
class NormalDVAMsgListPage;
class OtherDVAMsgListPage;
class BackupKeyMonitor;
class PACorbaMessageSubscriber;
class PAScheduleInDayDlg;

/////////////////////////////////////////////////////////////////////////////
// PAManagerDlg dialog

class PAManagerDlg : public TA_Base_Bus::TransActiveDialog, 
                     public ICurSelDVAMsgObserver,
                     public IDVAMsgShortcutConfigObserver,
					 public IAddDVAMsgListObserver
{
// Construction
public:
	//PAManagerDlg(CWnd* pParent = NULL);   // standard constructor
	PAManagerDlg(TA_Base_Bus::IGUIAccess& controlClass);	// standard constructor
    virtual ~PAManagerDlg();
	virtual BOOL DestroyWindow();

    virtual void init();

    virtual void processDVAMsgSelChanged( int nType );
    virtual void processDVAMsgShortcutChanged( unsigned long ulShortcutIndex );
    virtual void processDVAMsgListChanged( unsigned long ulDVAMsgKey );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PAManagerDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
private:
    void updateBackupStatusImage( int nBackupKey );
    void selectQuickBroadcastDVAMsg( unsigned int unIndex );
    void updateOpScopeGUI();
    void updateBroadcastBtnsStatus();
	void updateZoneMonitoringBtnsStatus();//added by lixiaoxia
    void updateLiveBroadcastCheckBtnStatus();
	void updateBackgroundMusicCheckBtnStatus();
	void startLiveBroadcast();
	void stopLiveBroadcast();
	void startBackgroundMusicBroadcast();
	void stopBackgroundMusicBroadcast();

protected:
	// Generated message map functions
	//{{AFX_MSG(PAManagerDlg)
    virtual BOOL OnInitDialog();
	virtual afx_msg void OnPaint();
	virtual afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual afx_msg void OnMove(int x, int y);
    virtual afx_msg void OnDestroy();
    virtual afx_msg void OnBnClickedBtnBroadcastStart();
    virtual afx_msg void OnBnClickedBtnBroadcastStop();
    virtual afx_msg void OnBnClickedBtnHelp();
    virtual afx_msg void OnBnClickedBtnGroupSave();
    virtual afx_msg void OnBnClickedBtnGroupDelete();
    virtual afx_msg void OnBnClickedBtnSchedule();
    virtual afx_msg void OnBnClickedCheckCycle();
    virtual afx_msg void OnBnClickedCheckScheduleInDay();
    virtual afx_msg void OnBnClickedQuickBroadcastA();
    virtual afx_msg void OnBnClickedQuickBroadcastB();
    virtual afx_msg void OnBnClickedQuickBroadcastC();
    virtual afx_msg void OnBnClickedQuickBroadcastD();
    virtual afx_msg void OnBnClickedQuickBroadcastE();
    virtual afx_msg void OnBnClickedQuickBroadcastF();
    virtual afx_msg void OnBnClickedQuickBroadcastG();
    virtual afx_msg void OnBnClickedQuickBroadcastH();
    virtual afx_msg void OnBnClickedQuickBroadcastI();
    virtual afx_msg void OnBnClickedQuickBroadcastJ();
    virtual afx_msg void OnBnClickedQuickBroadcastK();
	virtual afx_msg void OnBnClickedQuickBroadcastL();
	virtual afx_msg void OnBnClickedQuickBroadcastM();
	virtual afx_msg void OnBnClickedQuickBroadcastN();
	virtual afx_msg void OnBnClickedQuickBroadcastO();
	virtual afx_msg void OnBnClickedQuickBroadcastP();
	virtual afx_msg void OnBnClickedQuickBroadcastQ();
	virtual afx_msg void OnBnClickedQuickBroadcastR();
	virtual afx_msg void OnBnClickedQuickBroadcastS();
	virtual afx_msg void OnBnClickedQuickBroadcastT();
	virtual afx_msg void OnBnClickedQuickBroadcastU();
	virtual afx_msg void OnBnClickedQuickBroadcastV();
	virtual afx_msg void OnBnClickedQuickBroadcastW();
	virtual afx_msg void OnBnClickedQuickBroadcastX();
	virtual afx_msg void OnBnClickedQuickBroadcastY();
	virtual afx_msg void OnBnClickedQuickBroadcastZ();
    virtual afx_msg void OnCbnSelchangeComboGroup();
    virtual afx_msg void OnBnClickedCheckDvaBroadcast();
    virtual afx_msg void OnBnClickedCheckLiveBroadcast();
	virtual afx_msg void OnBnClickedCheckBackgroundMusic();
    virtual afx_msg void OnClickedCheckScheduleManager();
	virtual afx_msg void OnBnClickedBtnMonitorStart();//added by lixiaoxia
    virtual afx_msg void OnBnClickedBtnMonitorStop();//added by lixiaoxia
    virtual afx_msg void OnBnClickedBtnTasteDva();
    virtual afx_msg void OnBnClickedBtnExit();
    virtual afx_msg void OnBnClickedBtnPlayMsgUp();
    virtual afx_msg void OnBnClickedBtnPlayMsgDown();
    virtual afx_msg void OnBnClickedBtnPlayMsgDelete();	
	virtual afx_msg void OnLvnItemchangedPlayMsgList(NMHDR *pNMHDR, LRESULT *pResult);
    virtual afx_msg LRESULT OnUpdateDVAMsgList( WPARAM wParam, LPARAM lParam );
    virtual afx_msg LRESULT OnUpdateZoneGroupList( WPARAM wParam, LPARAM lParam );
    virtual afx_msg LRESULT OnPopupMessageBox( WPARAM wParam, LPARAM lParam );
    virtual afx_msg LRESULT OnUpdateZoneSelection( WPARAM wParam, LPARAM lParam );
    virtual afx_msg LRESULT OnUpdateDVAMsgShortcutBtnText( WPARAM wParam, LPARAM lParam );
    virtual afx_msg LRESULT OnUpdateOpScopeGUI( WPARAM wParam, LPARAM lParam );
    virtual afx_msg LRESULT OnUpdateBackupKeyStatus( WPARAM wParam, LPARAM lParam );
    virtual afx_msg LRESULT OnScheduleBroadcastUpdate( WPARAM wParam, LPARAM lParam );
    virtual afx_msg LRESULT OnScheduleInDayDlgHide( WPARAM wParam, LPARAM lParam );
    //}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    /**
     * Intercept all messages to allow provision of top level exception handler
     *
     */
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
    // Model for this dialog view
    PaRuntimeModel 				m_oBroadcastModel;

    EmergencyDVAMsgListPage* 	m_pEmergencyListPage;
    NormalDVAMsgListPage* 		m_pNormalListPage;
    OtherDVAMsgListPage* 		m_pOtherListPage;
    PAScheduleInDayDlg* 		m_pScheduleBroadcastInDayDlg;
    PACorbaMessageSubscriber* 	m_pPACorbaMessageSubscriber;
    BackupKeyMonitor* 			m_pBackupKeyMonitor;

    HICON m_hLockMode;
    HICON m_hUnlockMode;
    HICON m_hUnknownlockMode;

    int m_nBroadcastMode;
    CFont m_fntList;
	
	int m_indexInplayMsgListCtrl;

// Dialog Data
	//{{AFX_DATA(PAManagerDlg)
    enum { IDD = IDD_DLG_PA_MAIN_WND };
    CButton m_ckCycleBroadcast;
    CButton m_ckScheduleBroadcast;
    CButton m_btnSchedule;
    CButton m_btnAddToSchedule;
    CButton m_btnDVABroadcastStart;
    CButton m_btnDVABroadcastStop;
    CButton m_btnDVAMsgPlayback;
    CButton m_btnMonitorStart;//added by lixiaoxia
    CButton m_btnMonitorStop;//added by lixiaoxia
    CButton m_btnSaveZoneGroup;
    CButton m_btnDelZoneGroup;
    CButton m_ckDVABroadcastMode;
    CButton m_ckLiveBroadcastMode;
	CButton m_ckBackgroundMusicMode;
    CButton m_ckScheduleInDay;
    CButton m_ckSchedulemanager;
    CDateTimeCtrl 	m_dtStartTimeInDay;
    CComboBox 		m_cbGroupNameList;
    CEdit 			m_edBroadcastRecordName;
    NumbericEdit 	m_edCycleTimes;
    NumbericEdit 	m_edDwellTime;
    CListBox 		m_lbSeledZones;
    CStatic 		m_picBackupStatus;
    PaDvaTabCtrl 	m_tabDvaMsg;
	CListCtrl 		m_playMsgListCtrl;
    //}}AFX_DATA
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PAMANAGERDLG_H__D55DB830_1342_4D9E_BA4C_C554E25FC74B__INCLUDED_)

