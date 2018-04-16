// BASManagerDlg.h : Hearder
//
#if !defined (TA_3002_App_Bas_BasManager_CBASManagerDlg_H)
#define TA_3002_App_Bas_BasManager_CBASManagerDlg_H
#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "core/data_access_interface/bas/src/Subsystem.h"
#include "core/data_access_interface/bas/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/bas/src/Mode.h"
#include "core/data_access_interface/bas/src/ModeAccessFactory.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/types/ECSAgentComms_MessageTypes.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/bas/src/BasFireAccessFactory.h"
#include "bus/bas/bas_agent/IDL/src/IStationBasAgentCorbaDef.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "app/bas/BASModeManager/src/ReportListCtrl.h"
#include "app/bas/BASModeManager/src/ModesListCtrl.h"
#include "app/bas/BASModeManager/src/ModeStatusListenerManager.h"
#include "app/bas/BASModeManager/src/SubSystemStatus.h"
#include "app/bas/BASModeManager/src/BasManagerSubscriber.h"
#include "app/bas/BASModeManager/src/BasManagerActionThread.h"
#include "app/bas/BASModeManager/src/BasManagerDataHelper.h"


namespace TA_Base_Bus
{
	class RightsLibrary;
};

Namespace_TA_IRS_APP_Begin
// CBASManagerDlg Dialog
class CBASManagerDlg : public TA_Base_Bus::TransActiveDialog,
					   public IModeStatusListenerInterface,
					   public IsubsystemModeListenerInterface,
					   public IActionThreadFunctionInterface,
					   public virtual TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >
{
// Constructor
public:
	CBASManagerDlg(TA_Base_Bus::IGUIAccess& controlClass, CWnd* pParent = NULL);
    void processStationFireFromCommandLine();
// Dialog Data
	enum { IDD = IDD_BASMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV Support

protected:
	void changeToNormalMode();
	void changeToFireMode();
	void subscribeStationFireMessage();
	void unsubscribeStationFireMessage();
	void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);
	void sendStationFireACKEvent(unsigned long locationKey, unsigned long modeValue);
	void receiveStationFireMessage(unsigned long locationKey, unsigned long modeValue, TA_IRS_Core::BasStationFireStatus status);
	void logCurrentModesInfo();
	bool isActionPermitted(unsigned long action);
public:
	// the IModeStatusListenerInterface implement
	void updateModeStatus(unsigned long modeKey, int status);

	// the IsubsystemModeListenerInterface implement
	void updateSubsystemMode(unsigned long locationKey, unsigned long subsystemKey, ModeSet::subsystemModeType modeType);

	// the IActionThreadFunctionInterface implement
	void executeThreadFunction(BasManagerAction action);

// Implement
protected:
	HICON m_hIcon;

	// Message function
	virtual BOOL OnInitDialog();
	virtual void OnClose();
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedBtnApplyFilter();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedBtnDeviceList();
	afx_msg void OnBnClickedBtnExecuteMode();
	afx_msg void OnBnClickedBtnSearchMode();
	afx_msg void OnBnClickedBtnRecover();
	afx_msg void OnBnClickedBtnAutomanuCfg();
    afx_msg void OnBnClickedBtnRevokeMode();
	afx_msg void OnSubsystemChanged();
	afx_msg void OnStationChanged();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	// Fill the dialog data
	afx_msg LRESULT InitializeDialogData(WPARAM wParam, LPARAM lParam);
	// do message update the current mode
	afx_msg LRESULT OnUpdateCurrentMode(WPARAM wParam, LPARAM lParam);
	// do message update the execute mode
	afx_msg LRESULT OnUpdateExpectMode(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnUpdateSubsystemAutoManual(WPARAM wParam, LPARAM lParam);
	// do message update the mode status which executed
	afx_msg LRESULT OnUpdateModeSatus(WPARAM wParam, LPARAM lParam);

	afx_msg void OnHdnItemclickModeList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclickSubsystemList(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
public:
	void InitializeDialogData();
	void prepareClose();
	bool isLaunchInStation(ILocation_SmartPtr locationPtr);

	void changeToLocationStatus(unsigned long locationKey);
	void subscribeSubsystemUpdate();

	int getStationComboxIndexByData(DWORD data);
private:
	// setup the list ctrl header 
	void setupListCtrls();

	// calculate the left fixed area
	void calculateLeftFixedArea();

	// update the current mode
	void UpdateCurrentModeToGUI(unsigned long modeKey, unsigned long subsystemKey);

	// update the execute mode
	void UpdateExpectModeToGUI(unsigned long modeKey, unsigned long subsystemKey);	

	void UpdateSubsystemAutoManualToGUI(unsigned long autoManual, unsigned long subsystemKey);

	// update the mode status which executed
	void UpdateModeStatusToGUI(unsigned long modeKey, int status);

	// fill the filter or search modes
	void fillModeListCtrl(const std::vector<TA_IRS_Core::Mode_SmartPtr> modes);

	// update the mode status which in modes list ctrl
	void updateListCtrlModeStatus();

	std::string getSubsystemNameByKey(unsigned long locationKey, unsigned long subsystemKey);

	bool managerCanQuitNow();

	// add following function, because of hang when change location some times
	void setCurrentSelectLocation(unsigned long locationKey);
	unsigned long getCurrentSelectLocationKey();

    //update Subsystem list
	void updateSubsystemList(unsigned long locationKey);

	//update buttons status of control subsystems
    void updateControlSubsystemButtonsStatus();

	//update buttons status of control modes
	void updateControlModeButtonsStatus();

private:
	CComboBox m_cbStation;
	CComboBox m_cbSubsystem;
	CComboBox m_cbModeType;
	CButton m_btnExecute;
	CButton m_btnDeviceList;
	CButton m_btnAutoManual;
    CButton m_btnRevokeMode;
	CButton m_btnFireRecover;
	ReportListCtrl m_lstSubsytems;
	ModesListCtrl m_lstModes;

	typedef struct _DlgFixSize_
	{
		CRect leftTopFixArea;		// the top filter Rect
		CRect leftBottomFixArea;	// the bottom search Rect
		CRect btnAutoManual;		// the auto/manual button Rect
        CRect btnRevokeMode;        // the revoke mode button Rect
		CRect btnFireMode;			// the fire button rect
		CRect btnExecuteMode;		// the execute mode button rect
		CRect btnDeviceList;		// the device list button rect
		CRect modeCodeStatic;		// the mode code in search frame
		CRect searchText;			// the search text in search frame
		CRect searchButton;			// the search button in search frame
	}DlgFixSize; // this should call once, to make sure the fixed area high only
	DlgFixSize m_dlgFixSizeInfo;	

	BasManagerSubscriber m_basManagerSubscriber;

	BasManagerActionThread* m_initializeThread;
	BasManagerActionThread* m_changeStationThread;
	TA_Base_Core::NonReEntrantThreadLockable m_locationSelectLock;
	unsigned long m_currentSelectLocationKey;

	CImageList m_modeListImageList;
	TA_IRS_Core::BasStationFireStatus m_originalStationStatus;

	// This is used to check operator's rights to perform certain actions
	TA_Base_Bus::RightsLibrary* m_rightsMgr;
	unsigned long m_resourceKey;
};
Namespace_TA_IRS_APP_End
#endif //TA_3002_App_Bas_BasManager_CBASManagerDlg_H
