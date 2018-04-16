// CRadioCallForwardingPage.cpp : implementation file
//

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioCallForwardingPage/CRadioCallForwardingPage.h"


#include "app/radio/radio_manager_gui/src/RadioOperations/TcpMethodTimeoutException.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/TcpException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "app/radio/radio_manager_gui/src/RadioErrorMsg.h"


// CRadioCallForwardingPage dialog

IMPLEMENT_DYNAMIC(CRadioCallForwardingPage, CPropertyPage)

const short CRadioCallForwardingPage::CALL_FORWARDING_UNCONDITIONAL = 1;
const short CRadioCallForwardingPage::CALL_FORWARDING_NOT_REACHABLE = 2;
const short CRadioCallForwardingPage::NEVER_FORWARD = 0;
const short CRadioCallForwardingPage::NEVER_FORWARD_RADIO_CODE = 9;

CRadioCallForwardingPage::CRadioCallForwardingPage(RECT* parentRect)
	: CPropertyPage(CRadioCallForwardingPage::IDD),
	callMode(0),
	queryInitialized(false),
	operatorISSI(0)
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
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this, WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this, WM_DB_UPDATE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this, WM_DB_NEW);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this, WM_DB_DELETE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this, WM_DB_FINISH);
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
	ON_MESSAGE(WM_DB_UPDATE, onNotifiedDbUpdate)
	ON_MESSAGE(WM_DB_NEW, onNotifiedDbNew)
	ON_MESSAGE(WM_DB_DELETE, onNotifiedDbDelete)
	ON_MESSAGE(WM_DB_FINISH, onNotifiedDbFinish)
	ON_CBN_SELCHANGE(IDC_COMBO1, onSelChangeIdCombo)
	ON_CBN_SELCHANGE(IDC_COMBO2, onSelChangeAliasCombo)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BOOL CRadioCallForwardingPage::OnInitDialog()
{
	FUNCTION_ENTRY("OnInitDialog");
	CPropertyPage::OnInitDialog();

	immForwd.SetCheck(1);
	callMode = CALL_FORWARDING_UNCONDITIONAL;
	
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DB_UPDATE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DB_NEW);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DB_DELETE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DB_FINISH);

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

	short status = The_RadioOperations::instance()->getCallForwardingState();
	if (status == CALL_FORWARDING_UNCONDITIONAL) {
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

	short status = The_RadioOperations::instance()->getCallForwardingState();
	if (status == CALL_FORWARDING_NOT_REACHABLE) {
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

	short status = The_RadioOperations::instance()->getCallForwardingState();
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
		catch(TA_IRS_App::TcpException& e)
		{
			AddErrorMessage("Unable to establish Connection with the Radio System");
			LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
			cfStat.condition = NEVER_FORWARD_RADIO_CODE;
			cfStat.forwardedISSI = 0;
			cfStat.forwardingISSI = 0;
		}
		catch(const TA_Base_Core::ObjectResolutionException& e)	
		{			
			AddErrorMessage("Agent cannot be contacted");
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
			cfStat.condition = NEVER_FORWARD_RADIO_CODE;
			cfStat.forwardedISSI = 0;
			cfStat.forwardingISSI = 0;
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			AddErrorMessage("Unable to perform requested operation");
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			cfStat.condition = NEVER_FORWARD_RADIO_CODE;
			cfStat.forwardedISSI = 0;
			cfStat.forwardingISSI = 0;
		}

		if (cfStat.condition == 1) {
			The_RadioOperations::instance()->setCallForwardingState(CALL_FORWARDING_UNCONDITIONAL);
			The_RadioSystemStatus::instance()->updateGX("Type[RadioCallForwarding] ForwardAlways");
			onImmForwd();			
		} else if (cfStat.condition == 0) {
			The_RadioOperations::instance()->setCallForwardingState(CALL_FORWARDING_NOT_REACHABLE);
			The_RadioSystemStatus::instance()->updateGX("Type[RadioCallForwarding] ForwardNotReachable");
			onForwdNotReach();
		} else {
			The_RadioOperations::instance()->setCallForwardingState(NEVER_FORWARD);
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
		catch(TA_IRS_App::TcpException& e)
		{
			AddErrorMessage("Unable to establish Connection with the Radio System");
			LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
			FUNCTION_EXIT;
			return;
		}
		catch(TA_Base_Core::ObjectResolutionException& e)
		{
			AddErrorMessage("Agent cannot be contacted");
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
			FUNCTION_EXIT;
			return;
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			AddErrorMessage("Unable to perform requested operation");
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			FUNCTION_EXIT;
			return;
		}
		if (status) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Canceling CallForwarding Command Successful");		
			The_RadioOperations::instance()->setCallForwardingState(NEVER_FORWARD);
			The_RadioSystemStatus::instance()->updateGX("Type[RadioCallForwarding] NeverForward");
			applyBtn.EnableWindow(FALSE);
		}
		else {
			AddErrorMessage("Unable to perform requested operation");
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
			}
			//following code cannot be passed through, hence commented
			/*else if (NEVER_FORWARD == callMode) {
				status = The_RadioOperations::instance()->cancelCallForwarding(operatorISSI);
				if (status) {
					radioIdCollection.SetCurSel(-1);
					radioAliasCollection.SetCurSel(-1);
				}
			}*/
		}
		catch(TA_IRS_App::TcpException& e)
		{
			AddErrorMessage("Unable to establish Connection with the Radio System");
			LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
			FUNCTION_EXIT;
			return;
		}
		catch(TA_Base_Core::ObjectResolutionException& e)
		{
			AddErrorMessage("Agent cannot be contacted");
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
			FUNCTION_EXIT;
			return;
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			AddErrorMessage("Unable to perform requested operation");
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			FUNCTION_EXIT;
			return;
		}
		if (status) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "CallForwarding Command to radio %u Successful", forwardedISSI);		
			The_RadioOperations::instance()->setCallForwardingState(callMode);
			if (CALL_FORWARDING_UNCONDITIONAL == callMode) {
				The_RadioSystemStatus::instance()->updateGX("Type[RadioCallForwarding] ForwardAlways");
			} else if (CALL_FORWARDING_NOT_REACHABLE == callMode) {
				The_RadioSystemStatus::instance()->updateGX("Type[RadioCallForwarding] ForwardNotReachable");
			}
			applyBtn.EnableWindow(FALSE);
		}
		else {
			AddErrorMessage("Unable to perform requested operation");
			applyBtn.EnableWindow(TRUE);
		}
	}
	FUNCTION_EXIT;
}

void CRadioCallForwardingPage::OnPaint() 
{
	CPaintDC* dc = NULL;
	try {
		dc = new CPaintDC(this);
	}
	catch (...) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurs");
		return;
	}
	CBrush b;
	b.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	CBrush* pOldBrush = NULL;
	pOldBrush = dc->SelectObject(&b);
	CRect r(0,0,0,0);
	GetClientRect(&r);
	dc->FillRect(&r, &b);
	
	CPaintDC dcId(GetDlgItem(IDC_STATIC_ID));
	CBrush bId;
	bId.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	CRect rId(0,0,0,0);
	GetDlgItem(IDC_STATIC_ID)->GetClientRect(&rId);
	dcId.SetBkMode(TRANSPARENT);
	dcId.FillRect(&rId,&bId);
	dcId.DrawText("ID", &rId, DT_BOTTOM);

	CPaintDC dcDe(GetDlgItem(IDC_STATIC_DESC));
	CBrush bDe;
	bDe.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	CRect rDe(0,0,0,0);
	GetDlgItem(IDC_STATIC_DESC)->GetClientRect(&rDe);
	dcDe.SetBkMode(TRANSPARENT);
	dcDe.FillRect(&rDe,&bDe);
	dcDe.DrawText("Description", &rDe, DT_BOTTOM);

	delete dc;
	dc = NULL;
}

BOOL CRadioCallForwardingPage::OnEraseBkgnd( CDC* pDC )
{
    return TRUE;
}

LRESULT CRadioCallForwardingPage::onNotifiedDbUpdate(WPARAM pOldResource, LPARAM pNewResource)
{
	FUNCTION_ENTRY("onNotifiedDbUpdate");
	RadioResource* oldRes = (RadioResource*) pOldResource;
	RadioResource* newRes = (RadioResource*) pNewResource;
	if (RR_RADIO != oldRes->type) {
		FUNCTION_EXIT;
		return 0;
	}		
	std::vector<RadioResource>::iterator iter = radioSubscribers.begin();
	for (; iter!= radioSubscribers.end(); iter++) {
		if ((*iter).id == oldRes->id) {
			(*iter) = *newRes;
			refreshList();
			break;
		}
	}
	FUNCTION_EXIT;
	return 0;
}

LRESULT CRadioCallForwardingPage::onNotifiedDbDelete(WPARAM pOldResource, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedDbDelete");	
	RadioResource* res = (RadioResource*) pOldResource;
	if (RR_RADIO != res->type) {
		FUNCTION_EXIT;
		return 0;
	}
	std::vector<RadioResource>::iterator iter = radioSubscribers.begin();
	for (; iter!= radioSubscribers.end(); iter++) {
		if ((*iter).id == res->id) {
			radioSubscribers.erase(iter);
			refreshList();
			break;
		}
	}
	FUNCTION_EXIT;
	return 0;
}

LRESULT CRadioCallForwardingPage::onNotifiedDbNew(WPARAM pNewResource, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedDbNew");
	RadioResource* res = (RadioResource*) pNewResource;
	if (RR_RADIO != res->type) {
		FUNCTION_EXIT;
		return 0;
	}
	radioSubscribers.push_back(*res);
	std::sort(radioSubscribers.begin(), radioSubscribers.end(), SortIdAsc());
	refreshList();
	FUNCTION_EXIT;
	return 0;
}

LRESULT CRadioCallForwardingPage::onNotifiedDbFinish(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedDbNew");
	std::sort(radioSubscribers.begin(), radioSubscribers.end(), SortIdAsc());
	refreshList();
	FUNCTION_EXIT;
	return 0;
}

void CRadioCallForwardingPage::refreshList()
{
	FUNCTION_ENTRY("refreshList");	
	std::vector<RadioResource>::iterator iter = radioSubscribers.begin();
	radioIdCollection.ResetContent();
	radioAliasCollection.ResetContent();

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
	FUNCTION_EXIT;
}