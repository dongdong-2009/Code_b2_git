/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/system_control/control_station/stub/src/Dummy_Process_MonitorDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// Dummy_Process_MonitorDlg.h : header file
//

#if !defined(AFX_DUMMY_PROCESS_MONITORDLG_H__2468F525_0FAE_4672_B496_1A02E85ECF9B__INCLUDED_)
#define AFX_DUMMY_PROCESS_MONITORDLG_H__2468F525_0FAE_4672_B496_1A02E85ECF9B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include <vector>
#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"
#include "core/process_management/IDL/src/IManagedApplicationCorbaDef.h"
#include "core/process_management/IDL/src/IManagedGUICorbaDef.h"
#include "core/corba/src/ServantBase.h"

/////////////////////////////////////////////////////////////////////////////
// DummyProcessMonitorDlg dialog

class DummyProcessMonitorDlg : public CDialog,
                               public virtual POA_TA_Base_Core::IProcessManagerCorbaDef,
                               public virtual TA_Base_Core::ServantBase
{
// Construction
public:
	DummyProcessMonitorDlg(CWnd* pParent = NULL);	// standard constructor
    virtual ~DummyProcessMonitorDlg();

// Dialog Data
	//{{AFX_DATA(DummyProcessMonitorDlg)
	enum { IDD = IDD_DUMMY_PROCESS_MONITOR_DIALOG };
	CListCtrl	m_listParameters;
	CListCtrl	m_listApps;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DummyProcessMonitorDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


public:

    // Methods to meet the ProcessManagerCorbaDef interface requirements.
    void terminating( TA_Base_Core::ProcessId p_processID, 
                      TA_Base_Core::EManagedProcessTerminateCode p_code);

    void registerManagedProcess( TA_Base_Core::IManagedProcessCorbaDef_ptr p_managedProcess,
                                 TA_Base_Core::ProcessId p_processID, 
                                 CORBA::ULong applicationType,
                                 const char* entity);

    void registerManagedApplication(TA_Base_Core::IManagedApplicationCorbaDef_ptr p_managedApp,
                                    TA_Base_Core::ProcessId p_processId);

    TA_Base_Core::RunParamSeq* getParams(TA_Base_Core::ProcessId id, const char* hostname);

    void poll();

    void statusChanged(TA_Base_Core::EProcessStatus status, TA_Base_Core::ProcessId p_processId);


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(DummyProcessMonitorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonTerminate();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonEdit();
	afx_msg void OnDblclkParameterList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMaximise();
	afx_msg void OnMinimise();
	afx_msg void OnRestore();
	afx_msg void OnServerDown();
	afx_msg void OnServerUp();
	afx_msg void OnButtonClear();
	afx_msg void OnButtonLoad();
	afx_msg void OnButtonSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

    TA_Base_Core::IManagedGUICorbaDef_ptr getGUI();

    void updateSelectedApp(std::string runParamName, std::string runParamValue);

    bool loadParamsFromFile(std::string paramFile);


    typedef std::map<int,TA_Base_Core::IManagedProcessCorbaDef_ptr> ProcessMap;
    typedef std::map<int,TA_Base_Core::IManagedApplicationCorbaDef_ptr> AppMap;

    ProcessMap m_currentRunningApps;
    AppMap     m_currentRunningGUIAgentApps;
    TA_Base_Core::RunParamSeq_var m_runParams;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DUMMY_PROCESS_MONITORDLG_H__2468F525_0FAE_4672_B496_1A02E85ECF9B__INCLUDED_)
