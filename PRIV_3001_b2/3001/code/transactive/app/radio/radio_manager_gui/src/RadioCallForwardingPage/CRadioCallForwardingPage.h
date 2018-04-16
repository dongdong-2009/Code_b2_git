#pragma once
#include "app/radio/radio_manager_gui/src/resource.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "bus\radio\radio_bus\src\radiotypes.h"
#include "afxwin.h"

#include "app\radio\radio_mft_agent\src\RadioMFTAgentHelper.h"
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

	static const short CALL_FORWARDING_UNCONDITIONAL;
	static const short CALL_FORWARDING_NOT_REACHABLE;
	static const short NEVER_FORWARD;
	static const short NEVER_FORWARD_RADIO_CODE;
protected:
	virtual BOOL OnInitDialog();
	afx_msg void onImmForwd();
	afx_msg void onForwdNotReach();
	afx_msg void onNeverForwd();
	afx_msg LRESULT onDatabaseReady(WPARAM wParam, LPARAM lParam);
	afx_msg void onSelChangeIdCombo();
	afx_msg void onSelChangeAliasCombo();
	afx_msg void onApply();
	virtual void OnPaint();
	afx_msg BOOL OnEraseBkgnd( CDC* pDC );

	afx_msg LRESULT onNotifiedDbUpdate(WPARAM pOldResource = 0, LPARAM pNewResource = 0);
	afx_msg LRESULT onNotifiedDbDelete(WPARAM pOldResource = 0, LPARAM lParam = 0);
	afx_msg LRESULT onNotifiedDbNew(WPARAM pNewResource = 0, LPARAM lParam = 0);
	afx_msg LRESULT onNotifiedDbFinish(WPARAM wParam = 0, LPARAM lParam = 0);

	void refreshList();

	CComboBox radioIdCollection;
	CComboBox radioAliasCollection;
	CButton immForwd;
	CButton forwdNotReach;
	CButton neverForwd;

	// 1 -> unconditional
	// 2 -> Not reachable
	// 3 -> Never Forward
	unsigned short callMode;

	std::vector<RadioResource> radioSubscribers;

	unsigned long operatorISSI;

	bool queryInitialized;
};
