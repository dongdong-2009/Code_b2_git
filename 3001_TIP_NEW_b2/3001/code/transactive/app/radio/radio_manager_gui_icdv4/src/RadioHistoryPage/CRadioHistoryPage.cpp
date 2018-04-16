/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioHistoryPage/CRadioHistoryPage.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioUserDefinedEvents.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioHelper.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioHistoryPage/CRadioHistoryPage.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/types/RadioAudit_MessageTypes.h"

#include <iomanip> 
#include <sstream>


IMPLEMENT_DYNCREATE(CRadioHistoryPage, CPropertyPage)

CRadioHistoryPage::CRadioHistoryPage(RECT* parentRect)
: CPropertyPage(CRadioHistoryPage::IDD),
  m_historyLimit(100)
{
    if (parentRect != NULL)
    {
        m_parentRect = *parentRect;
    }
	
}


CRadioHistoryPage::~CRadioHistoryPage()
{
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_OPERATION_MODE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_AUDIT_EVENT_OCCURRED);
}

void CRadioHistoryPage::DoDataExchange(CDataExchange* pDX)
{
	FUNCTION_ENTRY("CRadioHistoryPage");

	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CRadioHistoryPage)
	DDX_Control(pDX, IDC_HISTORY_LIST, m_HistoryListCtrl);
	//}}AFX_DATA_MAP

	FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CRadioHistoryPage, CPropertyPage)
	//{{AFX_MSG_MAP(CRadioHistoryPage)
	ON_MESSAGE(WM_ENTITY_EDIT_MODE,onNotifiedEditMode)
	ON_MESSAGE(WM_OPERATION_MODE,onNotifiedOperationMode)
	ON_MESSAGE(WM_AUDIT_EVENT_OCCURRED,onNotifiedAuditEventOccurred)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_HISTORY_LIST, OnItemchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CRadioHistoryPage::OnInitDialog() 
{
	FUNCTION_ENTRY("OnInitDialog");

	CPropertyPage::OnInitDialog();

	SetWindowPos(NULL, 0, 0, m_parentRect.right, m_parentRect.bottom, 
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

    RECT rc;
    GetClientRect(&rc);

    m_HistoryListCtrl.SetWindowPos(NULL, 0, 0, m_parentRect.right, m_parentRect.bottom - 75, 
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
    
	setup();

	// Register user-defined messages
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_OPERATION_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_AUDIT_EVENT_OCCURRED);
	
	FUNCTION_EXIT;
	
	return TRUE;  
}


void CRadioHistoryPage::setHistoryLimit(unsigned int number)
{
	FUNCTION_ENTRY("setHistoryLimit");

	if ( number > 0)
	{
		m_historyLimit = number;
	}
	
	FUNCTION_EXIT;
}


afx_msg LRESULT CRadioHistoryPage::onNotifiedAuditEventOccurred(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedAuditEventOccurred");
	 
	// Fixed TD#14297
	// evaluate the Audit Message type
	const char* messageType = (const char*)lParam;

	if (TA_Base_Core::RadioAudit::RadioMakePrivateCall.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Outgoing");
	}
	else if (TA_Base_Core::RadioAudit::RadioEndPrivateCall.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"End Call");
	}
	else if (TA_Base_Core::RadioAudit::RadioAmbienceListeningActivated.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Ambience Listening");
	}
	else if (TA_Base_Core::RadioAudit::RadioAmbienceListeningTerminated.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"End Ambience Listening");
	}
	else if (TA_Base_Core::RadioAudit::RadioGroupSelect.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Selected");
	}
	else if (TA_Base_Core::RadioAudit::RadioGroupUnselect.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Unselected");
	}
	else if ((TA_Base_Core::RadioAudit::RadioPatchGroupConnect.getTypeKey() == messageType) ||
		     (TA_Base_Core::RadioAudit::RadioDynamicRegroupConnect.getTypeKey() == messageType) ||
			 (TA_Base_Core::RadioAudit::RadioMultiselectConnect.getTypeKey() == messageType) ||
			 (TA_Base_Core::RadioAudit::RadioSiteConnect.getTypeKey() == messageType) ||
			 (TA_Base_Core::RadioAudit::RadioMultiSiteConnect.getTypeKey() == messageType))
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Connect");
	}
	else if ((TA_Base_Core::RadioAudit::RadioPatchGroupRelease.getTypeKey() == messageType) ||
		     (TA_Base_Core::RadioAudit::RadioDynamicRegroupRelease.getTypeKey() == messageType))
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Release");
	}
	else if (TA_Base_Core::RadioAudit::RadioTextMessageReceive.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Incoming",true);
	}
	else if (TA_Base_Core::RadioAudit::RadioTextMessageSend.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Outgoing",true);
	}
	else if (TA_Base_Core::RadioAudit::RadioTextMessageRead.getTypeKey() == messageType)
	{
		std::ostringstream desc;
		desc << "Message ID " << wParam;

		updateHistory(desc.str().c_str(),"Read");
	}
	else if (TA_Base_Core::RadioAudit::RadioPredefinedMessageAdd.getTypeKey() == messageType)
	{
		updateHistory("Predefined Message","Add");
	}
	else if (TA_Base_Core::RadioAudit::RadioPredefinedMessageModify.getTypeKey() == messageType)
	{
		std::ostringstream desc;
		desc << "Predefined Message ID " << wParam;
		
		updateHistory(desc.str().c_str(),"Modify");
	}
	else if (TA_Base_Core::RadioAudit::RadioPredefinedMessageDeleted.getTypeKey() == messageType)
	{
		std::ostringstream desc;
		desc << "Predefined Message ID " << wParam;
		
		updateHistory(desc.str().c_str(),"Delete");
	}
	else if (TA_Base_Core::RadioAudit::RadioIncomingPrivateCall.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Incoming Private Call");
	}
	else if (TA_Base_Core::RadioAudit::RadioPrivateCallAnswered.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Answer Private Call");
	}
	//TD17224
	else if (TA_Base_Core::RadioAudit::RadioEmergencyReceived.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Emergency Call Received");
	}
	else if (TA_Base_Core::RadioAudit::RadioEmergencyAcknowledged.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Emergency Call Acknowledged");
	}
	else if (TA_Base_Core::RadioAudit::RadioEmergencyEnded.getTypeKey() == messageType)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Emergency Call Ended");
	}
	//TD17224

	UpdateWindow();

	FUNCTION_EXIT;

    return 0;
}


void CRadioHistoryPage::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	FUNCTION_ENTRY("OnItemchanged");

	POSITION pos = m_HistoryListCtrl.GetFirstSelectedItemPosition();

	if ( pos != NULL  )
	{
		int nItem = m_HistoryListCtrl.GetNextSelectedItem(pos);

		The_RadioOperations::instance()->entitySelectionChange(m_resourceList[nItem], false);
	}
	
	FUNCTION_EXIT;
	//return TRUE;
}


afx_msg LRESULT CRadioHistoryPage::onNotifiedEditMode(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedEditMode");

	// disable ListControl
	EnableWindow(FALSE);

	FUNCTION_EXIT;

	return 1;
}

afx_msg LRESULT CRadioHistoryPage::onNotifiedOperationMode(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedEditMode");

	// enable ListControl
	EnableWindow(TRUE);

	FUNCTION_EXIT;

	return 1;
}

afx_msg LRESULT CRadioHistoryPage::onNotifiedResourceAssignment(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Assigned");
	}
	return 1;
}

afx_msg LRESULT CRadioHistoryPage::onNotifiedResourceDeAssignment(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Deassigned");
	}
	return 1;
}

afx_msg LRESULT CRadioHistoryPage::onNotifiedInboundCall(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Incoming");
	}
	return 1;
}


afx_msg LRESULT CRadioHistoryPage::onNotifiedSelectSpeakerSelected(WPARAM wParam, LPARAM unused)
{
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Selected");
	}
	return 1;
}

afx_msg LRESULT CRadioHistoryPage::onNotifiedSelectSpeakerUnselected(WPARAM wParam, LPARAM unused)
{
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Unselected");
	}
	return 1;
}

afx_msg LRESULT CRadioHistoryPage::onNotifiedMakePrivateCall(WPARAM wParam, LPARAM unused)
{
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Outgoing");
	}
	return 1;
}

afx_msg LRESULT CRadioHistoryPage::onNotifiedListeningCall(WPARAM wParam, LPARAM unused)
{
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateHistory(*resource,"Ambience Listening");
	}
	return 1;
}

void CRadioHistoryPage::updateHistory(RadioResource& resource, const char* strAction, bool isText/* =false */)
{
    FUNCTION_ENTRY("addCall");
    
	LV_COLUMN lv;  
	lv.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lv.fmt = LVCFMT_LEFT;

	// Description
	time_t now;
	time(&now);

	m_HistoryListCtrl.InsertItem(0,timeToString(now).c_str());

	// Action
	m_HistoryListCtrl.SetItemText(0,1,strAction);
	
	// Description
	m_HistoryListCtrl.SetItemText(0,2,resource.alias.c_str());

	// Type
	if (resource.type == RR_RADIO)
	{
		/* added checking to distinguish text messages & calls */
		CString cType = getTypeText(resource.type);

		if (isText)
			cType += " (Text)";
		else
			cType += " (Voice)";

		m_HistoryListCtrl.SetItemText(0,3,cType);
	}
	else
	{
		m_HistoryListCtrl.SetItemText(0,3,getTypeText(resource.type));
	}

	std::ostringstream data;
	//data << resource.ssi;
	data << resource.id;
	m_HistoryListCtrl.SetItemText(0,4,data.str().c_str());


	// add to vector
	m_resourceList.insert(m_resourceList.begin(), resource);

    int nCount = m_resourceList.size();
    if (nCount >= m_historyLimit)
    {
		--nCount;
        m_HistoryListCtrl.DeleteItem(nCount);
        m_resourceList.erase(m_resourceList.begin() + nCount);
    }

	FUNCTION_EXIT;
}

void CRadioHistoryPage::updateHistory(const char* description, const char* strAction)
{
    FUNCTION_ENTRY("addCall");
    
	LV_COLUMN lv;  
	lv.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lv.fmt = LVCFMT_LEFT;

	// Description
	time_t now;
	time(&now);

	m_HistoryListCtrl.InsertItem(0,timeToString(now).c_str());

	// Action
	m_HistoryListCtrl.SetItemText(0,1,strAction);
	
	// Description
	m_HistoryListCtrl.SetItemText(0,2,description);

	// Type
	CString cType = getTypeText(RR_RADIO) + " (Text)";

	m_HistoryListCtrl.SetItemText(0,3,cType);

	m_HistoryListCtrl.SetItemText(0,4,"");

	// set dummy resource
	RadioResource resource;
	resource.type = RR_RADIO;
	resource.alias = description;

	// add to vector
	m_resourceList.insert(m_resourceList.begin(), resource);

    int nCount = m_resourceList.size();
    if (nCount >= m_historyLimit)
    {
		--nCount;
        m_HistoryListCtrl.DeleteItem(nCount);
        m_resourceList.erase(m_resourceList.begin() + nCount);
    }

	FUNCTION_EXIT;
}

void CRadioHistoryPage::setup()
{
    FUNCTION_ENTRY("setupListControl");
    
	/*
	 * Initialize HistoryListControl: create columns in the list control and give each column a header string
	 */
	m_HistoryListCtrl.SetExtendedStyle( m_HistoryListCtrl.GetExtendedStyle() 
										| LVS_EX_FULLROWSELECT 
										| LVS_EX_GRIDLINES );


	//Step 1: set header controls in our HistoryListCtrl;
	//Step1 A: Load resource strings to set the headers;
    UINT columnId[5] = {	IDS_COL_HEADER_TIME,
									IDS_COL_HEADER_ACTION,
									IDS_COL_HEADER_DESC,
									IDS_COL_HEADER_TYPE,
									IDS_COL_HEADER_ID };	

	//fixed TD14278, resized the column
    UINT columnWidth[5] = {120, 120, 110, 60, 75};	

	// Create columns in the list control and give each column a header string
	LV_COLUMN lv;  
	lv.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lv.fmt = LVCFMT_LEFT;

    CString str;
	for (int i = 0; i < 5; ++i )
    {
        // load the column title from resource
        str.LoadString(columnId[i]);

		lv.pszText = (LPTSTR) (LPCTSTR) str;
        lv.cx = columnWidth[i];
		
		// insert the column of the list control
		m_HistoryListCtrl.InsertColumn(i, &lv);
	} 


	FUNCTION_EXIT;
}


std::string CRadioHistoryPage::timeToString(const time_t t)
{
    return CRadioHelper::timeToString(t);
}

CString CRadioHistoryPage::getTypeText(RadioResourceType type)
{
    return CRadioHelper::getTypeText(type);
}