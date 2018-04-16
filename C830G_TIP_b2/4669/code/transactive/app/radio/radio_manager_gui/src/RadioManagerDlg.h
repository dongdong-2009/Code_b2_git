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
// TestDialogsDlg.h : header file
//
//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#if !defined(AFX_RADIOMANAGERDIALOG_H__AB68D997_4E17_11D7_B14B_0050BAB1D931__INCLUDED_)
#define AFX_RADIOMANAGERDIALOG_H__AB68D997_4E17_11D7_B14B_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "REBProgressManager.h"
#include "CurrentCall.h"
#include "RadioCallStack.h"
#include "RadioFacilities.h"
#include "ReadSDSMessageDialog.h"
#include "ToolTipComboBox.h"
#include "dlgbars.h"

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/radio/PTTMonitorLibrary/src/PTTButton.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioCorbaDef.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioDirectoryCorbaDef.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioSessionCorbaDef.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"

#include "core/data_access_interface/radio/src/RASelectedCallAccessFactory.h"
#include "core/naming/src/NamedObject.h"

// Define our own messages
const unsigned long REFRESH_DIALOG_MSG       = WM_USER + 1;
const unsigned long PTT_NOTIFY_MSG     = WM_USER + 2;
const unsigned long CURRENT_CALL_UPDATED_MSG = WM_USER + 3;
const unsigned long WM_CALLID_RPARAM_CHANGE  = WM_USER + 4;
const unsigned long WM_DISPLAY_SDS_MESSAGE   = WM_USER + 5;
const unsigned long WM_DISPLAY_FALLBACK_MESSAGE   = WM_USER + 6;

const unsigned long TRAIN_INBOUND_MSG = WM_USER + 0x100;
const unsigned long TRAIN_OUTBOUND_MSG = WM_USER + 0x101;
const unsigned long TRAIN_UPDATE_SELECTION_MSG = WM_USER + 0x102;
const unsigned long ADD_DYNAMIC_GROUP_MEMBERS = WM_USER + 0x103;
const unsigned long ADD_CALL_TO_HISTORY = WM_USER + 0x104;		//TD18283 add by hongzhi

namespace TA_Base_Bus // Forward declarations
{
    class RightsLibrary; 
}

namespace TA_IRS_App
{
	class PTTMessagePoster;   // Forward declaration
}

class UserInitThread;//liqiang++

/////////////////////////////////////////////////////////////////////////////
// CTestDialogsDlg dialog

class CRadioManagerDialog : public TA_Base_Bus::TransActiveDialog,
							public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
{
// Construction
public:

	CRadioManagerDialog(TA_Base_Bus::IGUIAccess& controlClass);	
	virtual ~CRadioManagerDialog();	

    // Dialog Data
	//{{AFX_DATA(CRadioManagerDialog)
	enum { IDD = IDD_RADIO_MANAGER_DIALOG };
    CComboBox    m_tetraidCombo;
	CCurrentCall       m_CurrentCallList;
	CRadioCallStack    m_CallStack;
	CRadioFacilities   m_RadioFacilities;
	CComboBox          m_CallTypeCombo;
	ToolTipComboBox    m_SDSMessageCombo;
	CButton			   m_holdButton;
	CButton			   m_releaseButton;
	CButton			   m_volumeUpButton;
	CButton			   m_volumeDownButton;
	CButton            m_includeButton;
	//}}AFX_DATA

	void init();
	bool isFunctionPermitted(TA_Base_Bus::AccessControlledAction action);
	void addCallToHistory(unsigned long callID);

	/**
	 * refreshes the enabled state of the answer button on the toolbar
	 */
	void refreshAnswerToolbarButton();

    virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

	void UserInit();

	void setRadioSession( REBProgressManager& mgr );//liqiang++

	unsigned long getConsoleKey();
    CString createDynamicGroupTSI();
protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRadioManagerDialog)
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:

	void CreateToolbar();
	void AddTextToButton(UINT buttonId, CString buttonLabel );
	void setupPreDefinedSDS();
	void setupCallTypes();
	void EnableToolBar(bool bEnable);
	void EnableStatusBar(bool bEnable);

    /** 
      * getCurrentTime
      *
      * Gets the current time and formats it into a string.
      *
      * @return std::string   The current time.
      *
      */
	static std::string getCurrentTime();

	// Generated message map functions
	//{{AFX_MSG(CRadioManagerDialog)
	virtual BOOL OnInitDialog();
	afx_msg void onUpdateButtons();
	afx_msg void OnLineforward();
	afx_msg void OnReleaseCall();
	afx_msg void OnAcceptCall();
	afx_msg void OnInclude();
	afx_msg void OnHoldCall();
	afx_msg void OnCallButton();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void onViewToolbar();
	afx_msg void onViewStatusbar();
	afx_msg void onHelpAboutRadioController();
    afx_msg void onHelpRadioController();
	afx_msg void onChangeSelectedAreaTetraID();
	afx_msg void onEditTextMessage();
	afx_msg void onSelectTextMessage();
	afx_msg void OnDestroy();
	afx_msg void OnClose();
	afx_msg void onSendText();
	afx_msg void onVolumeUp();
	afx_msg void onVolumeDown();
    afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnUserInit( WPARAM wParam, LPARAM lParam);

    afx_msg void OnSelchangeSaTetraid();
    //}}AFX_MSG
	afx_msg LRESULT onPTTNotification(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshDialog(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onCurrentCallUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT onDisplaySDSMessage(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT onDisplayFallbackMessage(WPARAM wParam, LPARAM lParam);

	//TD18283 add by hongzhi
	afx_msg LRESULT OnAddCallToHistory(WPARAM wParam, LPARAM lParam);
	//TD18283

	afx_msg LRESULT OnKillInitThread(WPARAM wParam, LPARAM lParam);//liqiang++

	BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()


private:

	unsigned long getLocationKey(unsigned long consoleKey);
	std::string   getRadioEntityName(unsigned long locationKey);
	std::string   getRadioSessionEntityName(unsigned long locationKey,unsigned long consoleKey);
    std::string   getRadioDirectoryEntityName(unsigned long locationKey);
    std::string   getRadioSessionConsoleComPort(unsigned long locationKey,unsigned long consoleKey);

	bool isCurrentCallSelected();
	void showPTTControls(bool areShown);

    void processSessionUpdate(const TA_Base_Bus::IRadioCorbaDef::SessionUpdateNotification& sessionUpdate);


private:
	HICON					m_hIcon;

	// Status bar
	CDlgStatusBar m_wndStatusBar;

	//UINT m_nIDTracking;
	//UINT m_nIDLastMessage;

    // Toolbar
	CDlgToolBar				m_toolBar;
	UINT					m_toolBarHeight;
	UINT                    m_statusBarHeight;
    TA_Base_Core::RASelectedCallAccessFactory::SelectedCallHistoryList m_history;

public:  
    void updateSelectedArea();

    typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioCorbaDef,
				TA_Base_Bus::IRadioCorbaDef_ptr,
				TA_Base_Bus::IRadioCorbaDef_var> IRadio;

	typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioSessionCorbaDef,
				TA_Base_Bus::IRadioSessionCorbaDef_ptr,
				TA_Base_Bus::IRadioSessionCorbaDef_var> IRadioSession;

	typedef TA_Base_Core::NamedObject<TA_Base_Bus::IRadioDirectoryCorbaDef,
				TA_Base_Bus::IRadioDirectoryCorbaDef_ptr,
				TA_Base_Bus::IRadioDirectoryCorbaDef_var> IRadioDirectory;
                   
    IRadio& getRadioCorbaDef() {return m_radio;};
    IRadioSession& getRadioSessionCorbaDef() {return m_radioSession;};
    IRadioDirectory& getRadioDirectoryCorbaDef() {return m_radioDirectory;};

    TA_IRS_Bus::MFCTrainSelectorGUI& getTrainSelectorGUI();
    TA_IRS_Bus::TrainSelector& getTrainSelector();
    void resetStationMode();
	void setDepotMode();
    long getLocationKey() { return m_locationKey; }

private:
    TA_IRS_App::ReadSDSMessageDialog    m_displaySDSMessage;

    IRadio m_radio;
    IRadioSession m_radioSession;
    IRadioDirectory m_radioDirectory;

	unsigned long      m_radioSessionEntityKey;

	TA_Base_Bus::RightsLibrary*          m_rightsLibrary;	

    TA_IRS_Bus::MFCTrainSelectorGUI* m_trainSelectorGUI;
    TA_IRS_Bus::TrainSelector*       m_trainSelector;
    unsigned long   m_locationKey;
	unsigned long   m_radioEntityKey;
	unsigned long	m_radioSubsystemKey;

	/*
	 * Current text that is defined for an SDS message
	 */
	std::string   m_currentSDSMessageText;

	/*
	 * Last selected call - use to deselect when new selection made.
	 */
	CCurrentCall::SelectedCalls m_lastSelectedCallSet;

	/*
	 * PTT Button monitor
	 */
	TA_IRS_App::PTTMessagePoster*      m_pttMessagePoster;
	TA_IRS_Bus::PTTButton*             m_pttButton;


    TA_IRS_Bus::LocationCache::EStationType m_stationType;
    std::string m_ConsoleComPort;
    bool                                m_radioInFallback;
	bool								m_initReady;
	
	/*
	 disable close menu to prevent
	 premature termination during 
	 dialog initialization 
	 this will be enable after threads 
	 dialog setup is done	
	*/
	void EnableCloseMenu (const bool & bSetFlag);
	/*
	  container buffer for all dialog
	  initializer thread
	*/
	std::vector <long> m_vctrThrdIDBuffer; 
	/*
	  thread lock to avoid race 
	  condtion on shared resource 
	  m_vctrThrdIDBuffer
	*/
	TA_Base_Core::NonReEntrantThreadLockable m_threadBufLock;
	/*
	  Set thread id of thread that will
	  be use to setup dialog
	*/
	void AddThreadToBuffer (const long & lThreadID);
	void RemoveThreadToBuff (const long & lThreadID);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RADIOMANAGERDIALOG_H__AB68D997_4E17_11D7_B14B_0050BAB1D931__INCLUDED_)
