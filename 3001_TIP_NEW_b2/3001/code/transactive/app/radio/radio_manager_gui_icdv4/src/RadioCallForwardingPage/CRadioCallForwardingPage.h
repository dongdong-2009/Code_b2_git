#pragma once
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "bus\radio\radio_bus\src\radiotypes.h"
#include "afxwin.h"

#include "app\radio\radio_mft_agent_icdv4\src\RadioMFTAgentHelper.h"
// CRadioCallForwardingPage dialog

class CRadioCallForwardingPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CRadioCallForwardingPage)

public:
	CRadioCallForwardingPage(RECT* parentRect = NULL);
	virtual ~CRadioCallForwardingPage();

// Dialog Data
	enum { IDD = IDD_CALLFORWARDING_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	RECT m_parentRect;

	DECLARE_MESSAGE_MAP()
public:
	CButton applyBtn;
protected:
	virtual BOOL OnInitDialog();
	afx_msg void onImmForwd();
	afx_msg void onForwdNotReach();
	afx_msg void onNeverForwd();
	afx_msg LRESULT onDatabaseReady(WPARAM wParam, LPARAM lParam);
	afx_msg void onSelChangeIdCombo();
	afx_msg void onSelChangeAliasCombo();
	afx_msg void onApply();

	CComboBox radioIdCollection;
	CComboBox radioAliasCollection;
	CButton immForwd;
	CButton forwdNotReach;
	CButton neverForwd;

	// 1 -> unconditional
	// 2 -> Not reachable
	// 3 -> Never Forward
	unsigned short callMode;

	static const short CALL_FORWARDING_UNCONDITIONAL;
	static const short CALL_FORWARDING_NOT_REACHABLE;
	static const short NEVER_FORWARD;

	std::vector<RadioResource> radioSubscribers;

	unsigned long operatorISSI;

	bool queryInitialized;
};
