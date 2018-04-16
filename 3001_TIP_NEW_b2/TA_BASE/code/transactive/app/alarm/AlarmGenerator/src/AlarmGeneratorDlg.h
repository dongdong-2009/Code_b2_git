/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/AlarmGeneratorDlg.h,v $
  * @author:  Andrew Del Carlo
  * @version: $Revision: 1.1.2.5.4.5 $
  *
  * Last modification: $Date: 2006/05/10 09:24:28 $
  * Last modified by:  $Author: derrickl $
  * 
  * implementation if the alarm generator dialog
  */


#if !defined(AFX_ALARMGENERATORDLG_H__4F7C6E0F_B2FE_4F0D_B88A_7BBCEAE33BBB__INCLUDED_)
#define AFX_ALARMGENERATORDLG_H__4F7C6E0F_B2FE_4F0D_B88A_7BBCEAE33BBB__INCLUDED_
#include <string>
#include <vector>
#include "core\message\src\NameValuePair.h"
#include "stdafx.h"
#include "Resource.h"
#include <map>
#include "AlarmStore.h"
#include "core\alarm\src\AlarmHelper.h"
#include "core\alarm\src\NonUniqueAlarmHelper.h"
#include "core\alarm\src\AlarmModificationHelper.h"
#include "core\data_access_interface\src\IAlarmData.h"
#include "AutoAlarmsOption.h"	// Added by ClassView
#include "AlarmsCollection.h"	// Added by ClassView
#include "bus/alarm/alarm_list_control/src/Sort.h"
#include "bus/alarm/alarm_list_control/src/Filter.h"
#include "app/alarm/AlarmGenerator/src/IUpdateObserver.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAlarmGeneratorDlg dialog

namespace TA_Core
{
    namespace TA_DataAccessInterface
    {
        class IOperator;
    }
}

class Generator;
static const int submit_num = 256;
static const int ack_num = 256;
class CAlarmGeneratorDlg : public CDialog, public IUpdateObserver
{
// Construction
	typedef std::vector<std::string> param_vec;
	typedef param_vec* param_vec_ptr;
	typedef TA_Base_Core::MessageType mt;
	typedef mt* mt_ptr;
	typedef struct s_msg
	{
		param_vec_ptr p_param;
		mt_ptr	p_mt;
	}* msg_ptr;
	typedef struct s_entity
	{
		std::string name;
		unsigned long subsys;
		unsigned long entity_key;
		unsigned long entity_type;
		unsigned long location;

	}* entity_ptr;
	typedef std::vector<msg_ptr> msg_vec;
	typedef std::vector<entity_ptr> entity_vec;


public:
	AlarmsCollection m_alarms;
	time_t m_tStartAuto;
	HANDLE m_hThreadUpdateTime;
	HANDLE m_hSetFilterThread;
	void setAutoAlarmCount(unsigned long cnt);
	void UpdataActiveAlm();
	bool autos();
	void setExcuteFilter();
	param_vec_ptr getParams(const std::string& paramstr);
	CAlarmGeneratorDlg(CWnd* pParent = NULL);	// standard constructor
	void CAlarmGeneratorDlg::autoa();
	void setAlarmQty(unsigned long qty); // to continually generate alarms
	void UpdateToAlarmStore(TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate);
// Dialog Data
	//{{AFX_DATA(CAlarmGeneratorDlg)
	enum { IDD = IDD_ALARMGENERATOR_DIALOG };
	CComboBox	m_CLocationKey;
	CButton	m_exitBut;
	CButton	m_refreshBut;
	CButton	m_multiAlarmBut;
	CButton	m_mmsButton;
	CComboBox	m_alarmTypeSelect;
	CComboBox	m_entityKeySelect;
	CComboBox	m_severitySelect;
	CComboBox	m_entityType;
	CListBox	m_activeAlarms;
	CButton	m_okButton;
	CButton m_ackButton;
	CButton m_ackCloseButton;
	CButton m_closeButton;
	CButton m_isUnique;
	CButton m_isNotUnique;
	CEdit	m_alarmID;
	CEdit	m_alarmDes;
	CEdit	m_subsystem;
    CEdit   m_assetName;
	CEdit	m_outputBox;
	CEdit	m_closeValue;
	CEdit	m_param1;
	CEdit	m_param2;
	CEdit	m_param3;
	CEdit	m_param4;
	CEdit	m_param5;
	CStatic	m_static1;
	CStatic	m_static2;
	CStatic	m_static3;
	CStatic	m_static4;
	CStatic	m_static5;
	CStatic	m_closeValueStatic;
	UINT	m_nAlarmCount;
	UINT	m_nInterval;
	UINT	m_nThreadCount;
	BOOL	m_bCheckAckClose;
	UINT	m_nExpiryMins;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmGeneratorDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation


protected:
	HICON m_hIcon;
	unsigned long m_alarmTypeKey;
	std::string m_alarmTypeName;
	std::string m_alarmDesc;
	unsigned long m_severity;
	unsigned long m_contextID;

	unsigned long m_entityTypeKey;
	std::string m_entityTypeName;
	unsigned long m_entityKey;
	std::string m_entityName;

	unsigned long m_subSystemKey;
	std::string m_subSystemName;

	std::string m_locationKey;
	std::string m_locationName;
	unsigned long m_region;
	std::string m_mgrPort;

	bool isUniqueAlarm;

	std::string m_contextName;
	std::string m_channelName;

	std::string m_notifyHosts;
	std::string m_dbConnection;
	std::string m_profileId;
	std::string m_userId;
	std::string m_mgrPrt;
	std::string m_dbConnectionFileDirectory;
	std::string m_dbConnectionFileName;
	std::string m_dbConnectionFile;
	unsigned long m_alarmQty;
	
	typedef std::map<std::string, unsigned long> SeverityMap;

	SeverityMap m_severities;

	/**
	* setDialogOutput
	*
	* this method takes a string and adds it to the output window
	* of the dialog
	*
	* @return
	*
	* @exception
	*/
	void setDialogOutput(std::string outText);
	
	
	/**
	* populateAlarmTypeSelect
	*
	* this method populates the m_alarmTypesSelect combo box with
	* all alarm types present in the database for selection
	*
	* @return
	*
	* @exception
	*/
	void populateAlarmTypeSelect();

	/**
	* populateEntityTypeSelect
	*
	* this method populates the m_entityType combo box with
	* all entity type and name present in the database for selection
	*
	* @return
	*
	* @exception
	*/
	void populateEntityTypeSelect();

	/**
	* populateEntityKeySelect
	*
	* this method populates the m_entityKeySelect combo box with
	* all entity key and name present in the database for selection
	*
	* @return
	*
	* @exception
	*/
	//void populateEntityKeySelect();

	/**
	* populateAlarmDescriptionParam
	*
	* this method populates the Alarm Description Param based
	* on the alarm type selected in the m_alarmTypesSelect combo box
	* for the user to key in the description
	*
	* @return
	*
	* @exception
	*/
	void populateAlarmDescriptionParam(std::string description);

	void setParamCaption(int count, CString caption);

	void enableParamWindows(bool isEnable);

	/**
	* populateSeveritySelect
	*
	* this method populates the severitySelect combo box with
	* any boolean alarm message that exists for the selected datapoint
	* from the database
	*
	* @return
	*
	* @exception
	*/
	void populateSeveritySelect();

	/**
	* submitAlarm
	*
	* this method takes the information gathered by all the input boxes to
	* submit an alarm to the alarm agent. Before submitting alarm, there is need to
	* obtain the message context and type based on the alarm type
	*
	* @return
	*
	* @exception
	*/
	void submitAlarm();

	void autoSubmitAlarm();

	bool obtainMsgContextAndType(std::string contextID);

	bool validateSubmission();
	

	/**
	* closeAlarm
	*
	* this method is activated by the 'close' button
	* it cycles through the list of open alarms and
	* send a message to the alarm agent to close the selected ones
	*
	* @return
	*
	* @exception
	*/	
	void closeAlarm();

	/**
	* acknowledgeAlarm
	*
	* this method is activated by the 'ack' button
	* it cycles through the list of open alarms and
	* send update message to the alarm agent to ack the selected ones
	*
	* @return
	*
	* @exception
	*/	
	void acknowledgeAlarm();

	/**
	* updateActiveAlarms
	*
	* gets a list of open alarms from the database and populates the
	* active alarm list with [entityKey],[alarmType]=>[message] format
	*
	* @return
	*
	* @exception
	*/
	void updateActiveAlarms();

	/**
	* createSetupDialog
	*
	* creates a setup dialog that prompts for once off configuration
	* information
	*
	* @return
	*
	* @exception
	*/
	bool createSetupDialog();

	/**
	* isLocationValid()
	*
	* determines if location information is valid
	*
	* @return true if valid
	*
	* @exception
	*/
	bool isLocationValid();
	void OnBnClickedButton1();


	// Generated message map functions
	//{{AFX_MSG(CAlarmGeneratorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	afx_msg void OnExit();
	afx_msg void OnAck();
	afx_msg void OnClose();
	afx_msg void OnAckClose();
	afx_msg void OnRefresh();
	afx_msg void OnUnique();
	afx_msg void OnNotUnique();
	afx_msg void OnChangeSetDataBaseConnection();
	afx_msg void OnChangeSetNotifyHosts();
	afx_msg void OnChangeSetSubmitterCount();
	afx_msg void OnChangeSetAlarmCount();
	afx_msg void OnChangeSetReceiverCount();
	afx_msg void OnChangeSetCount();
	afx_msg void OnSelchangeAlarmTypeSelect();
	afx_msg void OnSelchangeEntityKeySelect();
	afx_msg void OnSelchangeEntityTypeSelect();
	afx_msg void OnSelchangeActiveAlarmsList();
	afx_msg void OnSelchangeSeveritySelect();
	afx_msg void OnModMms();
	afx_msg void OnMultiAlarms();
	afx_msg void OnSelchangeEntityLocationKey();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	HANDLE submit_workers[submit_num];
	AutoAlarmsOption m_autoAlarmsOption;

private:
	CRITICAL_SECTION m_csAuto;
	unsigned long m_nAutoAlarmCount;
	HANDLE mutex;
	TA_Base_Bus::Filter m_filter;
	std::vector<std::string> active_alm;
	void InitAutoSubmit(std::vector<std::string>& vi);
	msg_vec m_autoalarm;
	entity_vec m_allentity;
	static bool m_isSetFilter;
	std::vector<TA_Base_Core::IAlarmData*>  m_currentSelectedAlarms;

	TA_Base_Bus::AlarmStore* pAlarmStore;

	//TA_Base_Core::AlarmHelper* m_UniqueHelper;
	//TA_Base_Core::NonUniqueAlarmHelper* m_nonUniqueHelper;
	//TA_Base_Core::AlarmModificationHelper* m_modHelper;

	TA_Base_Core::AlarmDetailCorbaDef* alarmDataToAlarmMessage( TA_Base_Core::IAlarmData* p_alarmData );
	TA_Base_Core::DecisionSupportStateType convertToDecisionSupportStateType(const std::string& stateTypeString);
	TA_Base_Core::IAlarmData* getAlarmFromDatabase(const std::string& p_key);
	void populateAlarmDetails(TA_Base_Core::IAlarmData* p_alarmData);
private:
	BOOL m_bIsSendAlarmStore;
	std::vector<std::string> m_strVecLocations;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMGENERATORDLG_H__4F7C6E0F_B2FE_4F0D_B88A_7BBCEAE33BBB__INCLUDED_)
