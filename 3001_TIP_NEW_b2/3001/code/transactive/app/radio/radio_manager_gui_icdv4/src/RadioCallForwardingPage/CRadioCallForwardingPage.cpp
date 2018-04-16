// CRadioCallForwardingPage.cpp : implementation file
//

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioCallForwardingPage/CRadioCallForwardingPage.h"


#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


// CRadioCallForwardingPage dialog

IMPLEMENT_DYNAMIC(CRadioCallForwardingPage, CPropertyPage)

const short CRadioCallForwardingPage::CALL_FORWARDING_UNCONDITIONAL = 1;
const short CRadioCallForwardingPage::CALL_FORWARDING_NOT_REACHABLE = 2;
const short CRadioCallForwardingPage::NEVER_FORWARD = 3;

CRadioCallForwardingPage::CRadioCallForwardingPage(RECT* parentRect)
	: CPropertyPage(CRadioCallForwardingPage::IDD)
{
	FUNCTION_ENTRY("CRadioCallForwardingPage");
	
    if (parentRect != NULL)
    {
        m_parentRect = *parentRect;
    }

	FUNCTION_EXIT;

}

CRadioCallForwardingPage::~CRadioCallForwardingPage()
{
}

void CRadioCallForwardingPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_APPLY, applyBtn);
	DDX_Control(pDX, IDC_COMBO1, radioIdCollection);
	DDX_Control(pDX, IDC_COMBO2, radioAliasCollection);
	DDX_Control(pDX, IDC_RADIO1, immForwd);
	DDX_Control(pDX, IDC_RADIO2, forwdNotReach);
	DDX_Control(pDX, IDC_RADIO3, neverForwd);
}


BEGIN_MESSAGE_MAP(CRadioCallForwardingPage, CPropertyPage)
	ON_BN_CLICKED(IDC_APPLY, onApply)
	ON_BN_CLICKED(IDC_RADIO1, onImmForwd)
	ON_BN_CLICKED(IDC_RADIO2, onForwdNotReach)
	ON_BN_CLICKED(IDC_RADIO3, onNeverForwd)
	ON_MESSAGE(WM_DATABASE_READY,onDatabaseReady)	
	ON_CBN_SELCHANGE(IDC_COMBO1, onSelChangeIdCombo)
	ON_CBN_SELCHANGE(IDC_COMBO2, onSelChangeAliasCombo)
END_MESSAGE_MAP()

BOOL CRadioCallForwardingPage::OnInitDialog()
{
	FUNCTION_ENTRY("OnInitDialog");
	CPropertyPage::OnInitDialog();

	immForwd.SetCheck(1);
	callMode = CALL_FORWARDING_UNCONDITIONAL;
	
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DATABASE_READY);

	operatorISSI = atol(RadioMFTAgentHelper::getRadioSystemISSI().c_str()); // todo, get from database

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Radio System ISSI = %u", operatorISSI);

	queryInitialized = false;

	FUNCTION_EXIT;
	return true;
}

void CRadioCallForwardingPage::onImmForwd()
{
	FUNCTION_ENTRY("onImmForwd");
	immForwd.SetCheck(1);
	forwdNotReach.SetCheck(0);
	neverForwd.SetCheck(0);
	callMode = CALL_FORWARDING_UNCONDITIONAL;

	bool status = The_RadioOperations::instance()->getCallForwardingState();
	if (status) {
		applyBtn.EnableWindow(FALSE);
	} else {
		applyBtn.EnableWindow(TRUE);
	}
	FUNCTION_EXIT;
}

void CRadioCallForwardingPage::onForwdNotReach()
{
	FUNCTION_ENTRY("onForwdNotReach");
	immForwd.SetCheck(0);
	forwdNotReach.SetCheck(1);
	neverForwd.SetCheck(0);
	callMode = CALL_FORWARDING_NOT_REACHABLE;

	bool status = The_RadioOperations::instance()->getCallForwardingState();
	if (status) {
		applyBtn.EnableWindow(FALSE);
	} else {
		applyBtn.EnableWindow(TRUE);
	}
	FUNCTION_EXIT;
}

void CRadioCallForwardingPage::onNeverForwd()
{
	FUNCTION_ENTRY("onNeverForwd");
	immForwd.SetCheck(0);
	forwdNotReach.SetCheck(0);
	neverForwd.SetCheck(1);
	callMode = NEVER_FORWARD;

	radioIdCollection.SetCurSel(-1);
	radioAliasCollection.SetCurSel(-1);

	bool status = The_RadioOperations::instance()->getCallForwardingState();
	if (!status) {
		applyBtn.EnableWindow(FALSE);
	} else {
		applyBtn.EnableWindow(TRUE);
	}
	FUNCTION_EXIT;
}

LRESULT CRadioCallForwardingPage::onDatabaseReady(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onDatabaseReady");
	radioSubscribers = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_RADIO);
	std::vector<RadioResource>::iterator iter = radioSubscribers.begin();

	for (; iter!= radioSubscribers.end(); iter++) {
		if (0 != (*iter).ssi) {
			std::stringstream ss;
			std::string s;
			ss << (*iter).ssi;
			ss >> s;
			if (CB_ERR == radioIdCollection.FindString(-1, s.c_str())) {
				radioIdCollection.AddString(s.c_str());
				radioAliasCollection.AddString((*iter).alias.c_str());
			}		
		}
	}

	if (!queryInitialized) {
		queryInitialized = true;
		CallForwardingStatus cfStat;

		try {
		// initialize current callforwarding status
		cfStat = The_RadioSystemStatus::instance()->queryCallForwarding(operatorISSI);
		}	
		catch(TA_Base_Core::TransactiveException& e)
		{
			::MessageBox(NULL, "Agent cannot be contacted", "Error connecting", MB_OK|MB_ICONERROR);
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			cfStat.condition = NEVER_FORWARD;
			cfStat.forwardedISSI = 0;
			cfStat.forwardingISSI = 0;
		}

		if (cfStat.condition == CALL_FORWARDING_UNCONDITIONAL) {
			The_RadioOperations::instance()->setCallForwardingState(true);
			onImmForwd();			
		} else if (cfStat.condition == CALL_FORWARDING_NOT_REACHABLE) {
			The_RadioOperations::instance()->setCallForwardingState(true);
			onForwdNotReach();
		} else {
			The_RadioOperations::instance()->setCallForwardingState(false);
			onNeverForwd();
		}

		std::stringstream ss;
		std::string s;
		ss << cfStat.forwardedISSI;
		ss >> s;
		int tempId = radioIdCollection.FindString(0, s.c_str());
		if (CB_ERR != tempId) {
			radioIdCollection.SetCurSel(tempId);
			onSelChangeIdCombo();
		}
		else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error on query call forwarding status, radio id %u not found on the list of radio", cfStat.forwardedISSI);
		}
	}
	FUNCTION_EXIT;
	return true;
}

void CRadioCallForwardingPage::onSelChangeIdCombo() 
{
	FUNCTION_ENTRY("onSelChangeIdCombo");
	char tempId[100];
	radioIdCollection.GetLBText(radioIdCollection.GetCurSel(),tempId);

	std::vector<RadioResource>::iterator iter = radioSubscribers.begin();
	for (; iter!= radioSubscribers.end(); iter++) {
		std::stringstream ss;
		std::string s;
		ss << (*iter).ssi;
		ss >> s;
		
		if (strcmp(s.c_str(), tempId) == 0) {
			radioAliasCollection.SelectString(0, (*iter).alias.c_str());
		}
	}
	FUNCTION_EXIT;
}

void CRadioCallForwardingPage::onSelChangeAliasCombo()
{
	FUNCTION_ENTRY("onSelChangeAliasCombo");
	char tempAlias[100];
	radioAliasCollection.GetLBText(radioAliasCollection.GetCurSel(),tempAlias);

	std::vector<RadioResource>::iterator iter = radioSubscribers.begin();
	for (; iter!= radioSubscribers.end(); iter++) {
		
		if (strcmp((*iter).alias.c_str(), tempAlias) == 0) {
			std::stringstream ss;
			std::string s;
			ss << (*iter).ssi;
			ss >> s;
			radioIdCollection.SelectString(0, s.c_str());
		}
	}
	FUNCTION_EXIT;
}

void CRadioCallForwardingPage::onApply()
{
	FUNCTION_ENTRY("onApply");

	char tempAlias[100];
	radioAliasCollection.GetLBText(radioAliasCollection.GetCurSel(),tempAlias);
	CString checkEmpty;
	radioAliasCollection.GetWindowTextA(checkEmpty);
	if (NEVER_FORWARD == callMode) {
		bool status;
		try {
			status = The_RadioOperations::instance()->cancelCallForwarding(operatorISSI);
			if (status) {
				radioIdCollection.SetCurSel(-1);
				radioAliasCollection.SetCurSel(-1);
			}
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			::MessageBox(NULL, "Agent cannot be contacted", "Error connecting", MB_OK|MB_ICONERROR);
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			FUNCTION_EXIT;
			return;
		}
		if (status) {
			AfxMessageBox("CallForwarding Command Successful");		
			applyBtn.EnableWindow(FALSE);
		}
		else {
			AfxMessageBox("CallForwarding Command Unsuccessful");
			applyBtn.EnableWindow(TRUE);
		}
	} else {
		if (checkEmpty.IsEmpty()) {
			AfxMessageBox("No radio assigned");
			return;
		}
		unsigned long forwardedISSI;
		bool found = false;
		bool status;

		std::vector<RadioResource>::iterator iter = radioSubscribers.begin();
		for (; iter!= radioSubscribers.end(); iter++) {

			if (strcmp((*iter).alias.c_str(), tempAlias) == 0) {
				forwardedISSI = (*iter).ssi;
				found = true;
			}
		}
		if (!found) return;

		try {
			if (CALL_FORWARDING_UNCONDITIONAL == callMode) {
				status = The_RadioOperations::instance()->callForwarding(1,operatorISSI,forwardedISSI);
			} else if (CALL_FORWARDING_NOT_REACHABLE == callMode) {
				status = The_RadioOperations::instance()->callForwarding(0,operatorISSI,forwardedISSI);
			} else if (NEVER_FORWARD == callMode) {
				status = The_RadioOperations::instance()->cancelCallForwarding(operatorISSI);
				if (status) {
					radioIdCollection.SetCurSel(-1);
					radioAliasCollection.SetCurSel(-1);
				}
			}
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			::MessageBox(NULL, "Agent cannot be contacted", "Error connecting", MB_OK|MB_ICONERROR);
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			FUNCTION_EXIT;
			return;
		}
		if (status) {
			AfxMessageBox("CallForwarding Command Successful");		
			applyBtn.EnableWindow(FALSE);
		}
		else {
			AfxMessageBox("CallForwarding Command Unsuccessful");
			applyBtn.EnableWindow(TRUE);
		}
	}
	FUNCTION_EXIT;
}
