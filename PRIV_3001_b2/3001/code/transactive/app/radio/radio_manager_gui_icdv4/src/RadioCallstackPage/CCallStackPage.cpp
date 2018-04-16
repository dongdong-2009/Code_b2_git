/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioCallstackPage/CCallStackPage.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/Resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioHelper.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioCallstackPage/CCallStackPage.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCallStackPage property page
IMPLEMENT_DYNCREATE(CCallStackPage, CPropertyPage)


CCallStackPage::CCallStackPage(RECT* parentRect) : 
        CPropertyPage(CCallStackPage::IDD)
{
    if (parentRect != NULL)
    {
        m_parentRect = *parentRect;
    }
	
	//{{AFX_DATA_INIT(CCallStackPage)
	//}}AFX_DATA_INIT
}

CCallStackPage::~CCallStackPage()
{
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_ASSIGNMENT);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_DE_ASSIGNMENT);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INBOUND_CALL);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INBOUND_CALL_CANCELED);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INBOUND_PTT);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INBOUND_PTT_RELEASED);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_ACTIVITY);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_ACTIVITY_ENDED);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_EMERGENCY);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_EMERGENCY_ACKNOWLEDGED);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_EMERGENCY_ENDED);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_SELECT_SPEAKER_SELECTED);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_SELECT_SPEAKER_UNSELECTED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_OPERATION_MODE);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_AMBIENCE_LISTENING);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_CONNECT_GROUP);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_OUTGOING_CALL);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_ESTABLISHED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_ENDED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_RINGING);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INCOMING_DRIVER_CALL);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ESTABLISHED_DRIVER_CALL);
	// TD18166
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_EMERGENCY);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_EMERGENCY_ACKNOWLEDGED);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_EMERGENCY_ENDED);

}

void CCallStackPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCallStackPage)
	DDX_Control(pDX, IDC_CALLSTACK_LIST, m_RadioCallStackListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCallStackPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCallStackPage)
	//ON_MESSAGE(WM_RESOURCE_ASSIGNMENT,onNotifiedResourceAssignment)
	//ON_MESSAGE(WM_RESOURCE_DE_ASSIGNMENT,onNotifiedResourceDeAssignment)
	//ON_MESSAGE(WM_INBOUND_CALL,onNotifiedInboundCall)
	//ON_MESSAGE(WM_INBOUND_CALL_CANCELED,onNotifiedInboundCallCanceled)
	//ON_MESSAGE(WM_INBOUND_PTT,onNotifiedInboundPTTStatus)
	//ON_MESSAGE(WM_INBOUND_PTT_RELEASED,onNotifiedInboundPTTStatusReleased)
	//ON_MESSAGE(WM_RESOURCE_ACTIVITY,onNotifiedResourceActivity)
	//ON_MESSAGE(WM_RESOURCE_EMERGENCY,onNotifiedResourceEmergency)
	//ON_MESSAGE(WM_RESOURCE_ACTIVITY_ENDED,onNotifiedResourceActivityEnded)
	//ON_MESSAGE(WM_RESOURCE_EMERGENCY_ACKNOWLEDGED,onNotifiedResourceEmergencyAcknowledged)
	//ON_MESSAGE(WM_RESOURCE_EMERGENCY_ENDED,onNotifiedResourceEmergencyEnded)
	ON_MESSAGE(WM_OUTGOING_CALL, onNotifiedMakePrivateCall)
	ON_MESSAGE(WM_PRIVATE_CALL_ESTABLISHED, onNotifiedPrivateCallEstablished)
	ON_MESSAGE(WM_PRIVATE_CALL_ENDED,onNotifiedInboundCallCanceled)
	//ON_MESSAGE(WM_AMBIENCE_LISTENING, onNotifiedListeningCall)
	//ON_MESSAGE(WM_CONNECT_GROUP, onNotifiedConnectGroup)
	//ON_MESSAGE(WM_SELECT_SPEAKER_SELECTED, onNotifiedSelectSpeakerSelected)
	//ON_MESSAGE(WM_SELECT_SPEAKER_UNSELECTED, onNotifiedSelectSpeakerUnselected)
	ON_MESSAGE(WM_ENTITY_EDIT_MODE,onNotifiedEditMode)
	ON_MESSAGE(WM_OPERATION_MODE,onNotifiedOperationMode)
	ON_MESSAGE(WM_DATABASE_READY,onNotifiedDatabaseReady)
	ON_MESSAGE(WM_PRIVATE_CALL_RINGING, onNotifiedPrivateCallRinging)
	ON_MESSAGE(WM_INCOMING_DRIVER_CALL, onNotifiedIncomingDriverCall)
	ON_MESSAGE(WM_ESTABLISHED_DRIVER_CALL, onNotifiedEstablishedDriverCall)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CALLSTACK_LIST, OnItemchanged)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCallStackPage message handlers

BOOL CCallStackPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	SetWindowPos(NULL, 0, 0, m_parentRect.right, m_parentRect.bottom, 
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

    RECT rc;
    GetClientRect(&rc);

 //   m_RadioCallStackListCtrl.SetWindowPos(NULL, 0, 0, m_parentRect.right+1,m_parentRect.bottom - 35, 
 //       SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	 m_RadioCallStackListCtrl.SetWindowPos(NULL, 0, 0, m_parentRect.right,m_parentRect.bottom - 75, 
        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	m_RadioCallStackListCtrl.SetExtendedStyle( m_RadioCallStackListCtrl.GetExtendedStyle() | 
											   LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_ASSIGNMENT);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_DE_ASSIGNMENT);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INBOUND_CALL);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INBOUND_CALL_CANCELED);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INBOUND_PTT);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INBOUND_PTT_RELEASED);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_ACTIVITY);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_ACTIVITY_ENDED);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_EMERGENCY);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_EMERGENCY_ACKNOWLEDGED);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_EMERGENCY_ENDED);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_SELECT_SPEAKER_SELECTED);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_SELECT_SPEAKER_UNSELECTED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_OPERATION_MODE);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_AMBIENCE_LISTENING);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_CONNECT_GROUP);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_OUTGOING_CALL);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_ENDED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_ESTABLISHED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_RINGING);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_EMERGENCY);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_EMERGENCY_ACKNOWLEDGED);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_EMERGENCY_ENDED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INCOMING_DRIVER_CALL);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ESTABLISHED_DRIVER_CALL);

	setupListControl();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CCallStackPage::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
    FUNCTION_ENTRY("OnItemchanged");
    
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	POSITION pos = m_RadioCallStackListCtrl.GetFirstSelectedItemPosition();

	if ( pos != NULL  )
	{
		int nItem = m_RadioCallStackListCtrl.GetNextSelectedItem(pos);

		std::vector<RadioResource>::iterator iter = m_RadioResource.begin() + nItem;

		// temporary
		if ((*iter).type == RR_RADIO) {
			The_RadioOperations::instance()->entitySelectionChange(*iter, false);		
		}
	}
		
	*pResult = 0;

	FUNCTION_EXIT;
}


afx_msg LRESULT CCallStackPage::onNotifiedResourceAssignment(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceAssignment");
    
	int result = 0;
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		addCall(*resource);
	}
	else
	{
		result = 1;
	}

	FUNCTION_EXIT;
	return result;
}


afx_msg LRESULT CCallStackPage::onNotifiedResourceDeAssignment(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceDeAssignment");
    
	int result = 0;
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		removeCall(*resource);
	}
	else
	{
		result = 1;
	}

	FUNCTION_EXIT;
	return result;
}


afx_msg LRESULT CCallStackPage::onNotifiedInboundCall(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedInboundCall");
    
	int result = 0;
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;

		updateCall(*resource,getStatusTextDescription(*resource,WM_INBOUND_CALL));
	}
	else
	{
		result = 1;
	}

	FUNCTION_EXIT;
	return result;
}

afx_msg LRESULT CCallStackPage::onNotifiedInboundCallCanceled(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedInboundCallCanceled");
    
	int result = 0;
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateCall(*resource,getStatusTextDescription(*resource,WM_INBOUND_CALL_CANCELED));
	}
	else
	{
		result = 1;
	}

	FUNCTION_EXIT;
	return result;
}

afx_msg LRESULT CCallStackPage::onNotifiedInboundPTTStatus(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedInboundPTTStatus");
    	
	int result = 0;
	if ( wParam != 0)
	{
		// update listener status
		RadioResource* listener = (RadioResource*)wParam;

		LV_FINDINFO findinfo;
		findinfo.flags = LVFI_PARAM;
		findinfo.lParam = listener->id;

		int nIndex = m_RadioCallStackListCtrl.FindItem(&findinfo);
		if ( nIndex != -1)
		{
			m_RadioCallStackListCtrl.SetItemText(nIndex,3, getStatusTextDescription(*listener,WM_INBOUND_PTT));
		}

		// update source status
		RadioResource* source = (RadioResource*)lParam;
		if ( lParam != 0)
		{
			findinfo.flags = LVFI_PARAM;
			findinfo.lParam = source->id;

			int index = m_RadioCallStackListCtrl.FindItem(&findinfo);
			if ( index != -1)
			{
				if ((source->type ==RR_TALKGROUP) || (source->type==RR_DYNGROUP))
					updateCall(*listener,"Talking(PTT)");
				else
					updateCall(*listener,"Talking");
			}
			
			std::string sourceStr;
			if (source->type == RR_NIL)
			{
				std::ostringstream str;
				str << source->ssi;

				sourceStr = str.str();
			}
			else
				sourceStr = source->alias;

			//m_RadioCallStackListCtrl.SetItemText(nIndex,4,sourceStr.c_str());
		}
	}
	else
	{
		result = 1;
	}

	FUNCTION_EXIT;
	return result;

}


afx_msg LRESULT CCallStackPage::onNotifiedInboundPTTStatusReleased(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedInboundPTTStatusReleased");
    
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateCall(*resource,getStatusTextDescription(*resource,WM_INBOUND_PTT_RELEASED));
	}

	FUNCTION_EXIT;
	return 0;
}


afx_msg LRESULT CCallStackPage::onNotifiedResourceActivity(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceActivity");
    
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateCall(*resource,getStatusTextDescription(*resource,WM_RESOURCE_ACTIVITY));
	}

	FUNCTION_EXIT;
	return 0;
}

afx_msg LRESULT CCallStackPage::onNotifiedResourceEmergency(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceEmergency");
    
	if ( lParam != 0)
	{
		RadioResource* resource = (RadioResource*)lParam;
		updateCall(*resource,getStatusTextDescription(*resource,WM_RESOURCE_EMERGENCY));
	}

	FUNCTION_EXIT;
	return 0;
}

afx_msg LRESULT CCallStackPage::onNotifiedResourceActivityEnded(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceActivityEnded");
    
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateCall(*resource,getStatusTextDescription(*resource,WM_RESOURCE_ACTIVITY_ENDED));
	}

	FUNCTION_EXIT;
	return 0;
}

afx_msg LRESULT CCallStackPage::onNotifiedResourceEmergencyAcknowledged(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceEmergencyAcknowledged");
    
	if ( lParam != 0)
	{
		RadioResource* resource = (RadioResource*)lParam;
		updateCall(*resource,getStatusTextDescription(*resource,WM_RESOURCE_EMERGENCY_ACKNOWLEDGED));
	}

	FUNCTION_EXIT;
	return 0;
}

afx_msg LRESULT CCallStackPage::onNotifiedResourceEmergencyEnded(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceEmergencyEnded");
    
	if ( lParam != 0)
	{
		RadioResource* resource = (RadioResource*)lParam;
		updateCall(*resource,getStatusTextDescription(*resource,WM_RESOURCE_EMERGENCY_ENDED));
	}

	FUNCTION_EXIT;
	return 0;
}


afx_msg LRESULT CCallStackPage::onNotifiedSelectSpeakerSelected(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceEmergencyEnded");
    
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateCall(*resource, getStatusTextDescription(*resource,WM_SELECT_SPEAKER_SELECTED));
	}

	FUNCTION_EXIT;
	return 0;
}

afx_msg LRESULT CCallStackPage::onNotifiedSelectSpeakerUnselected(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceEmergencyEnded");
    
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateCall(*resource, getStatusTextDescription(*resource,WM_SELECT_SPEAKER_UNSELECTED));
	}

	FUNCTION_EXIT;
	return 0;
}


afx_msg LRESULT CCallStackPage::onNotifiedMakePrivateCall(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceEmergencyEnded");
    
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		//addCall(*resource);
		if (resource->alias.find("Train") == std::string::npos)
			updateCall(*resource, getStatusTextDescription(*resource,WM_OUTGOING_CALL));
	}

	FUNCTION_EXIT;
	return 0;
}

afx_msg LRESULT CCallStackPage::onNotifiedListeningCall(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceEmergencyEnded");
    
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;

		AmbienceListeningCallState status = (AmbienceListeningCallState)lParam;

		if (status==ALC_ESTABLISHED)
		{
			updateCall(*resource, getStatusTextDescription(*resource,WM_AMBIENCE_LISTENING));
		}
		else
		{
			updateCall(*resource,"Idle");
		}
		
	}

	FUNCTION_EXIT;
	return 0;
}


afx_msg LRESULT CCallStackPage::onNotifiedConnectGroup(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedResourceEmergencyEnded");
    
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		updateCall(*resource, getStatusTextDescription(*resource,WM_CONNECT_GROUP, lParam));
	}

	FUNCTION_EXIT;
	return 0;
}


afx_msg LRESULT CCallStackPage::onNotifiedEditMode(WPARAM wParam, LPARAM lParam)
{
	EnableWindow(FALSE);
	return 0;
}

afx_msg LRESULT CCallStackPage::onNotifiedOperationMode(WPARAM wParam, LPARAM lParam)
{
	EnableWindow();
	return 0;
}

afx_msg LRESULT CCallStackPage::onNotifiedDatabaseReady(WPARAM wParam, LPARAM lParam)
{
	std::vector<RadioResource> rlist = The_RadioSystemStatus::instance()->getResourceCollectionAll();

	for(std::vector<RadioResource>::iterator iter=rlist.begin(); iter!= rlist.end();iter++)
	{
		//if ((The_RadioSystemStatus::instance()->getResource_isAssigned(*iter)) ||(iter->type==RR_RADIO))
		if (The_RadioSystemStatus::instance()->getResource_isAssigned(*iter))
		{
			if (RR_RADIO == (*iter).type || RR_TRAIN == (*iter).type)
				addCall(*iter);
		}
	}

	return 0;
}

LRESULT CCallStackPage::onNotifiedPrivateCallEstablished(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		//updateCall(*resource, getStatusTextDescription(*resource,WM_PRIVATE_CALL_ESTABLISHED));
		removeCall(*resource);
	}
	return 0;
}

LRESULT CCallStackPage::onNotifiedPrivateCallRinging(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		addCall(*resource);
		updateCall(*resource, getStatusTextDescription(*resource,WM_PRIVATE_CALL_RINGING));
	}
	return 0;
}

void CCallStackPage::setupListControl()
{
    FUNCTION_ENTRY("setupListControl");
    
	// Create columns in the list control and give each column a header string
	LV_COLUMN lv;  
	lv.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lv.fmt = LVCFMT_LEFT;
	
    UINT columnId[] = { IDS_COL_HEADER_DESC, IDS_COL_HEADER_TYPE, IDS_COL_HEADER_ID,
		IDS_COL_HEADER_STATUS};//	,IDS_COL_HEADER_SOURCE };

    UINT columnWidth[] = {145, 145, 70,105,85};

	 CString str;
	for (int i = 0; i < 4; ++i )
    {
        // load the column title from resource
        str.LoadString(columnId[i]);

		lv.pszText = (LPTSTR) (LPCTSTR) str;
        lv.cx = columnWidth[i];
		
		// insert the column of the list control
		m_RadioCallStackListCtrl.InsertColumn(i, &lv);
	} 

	FUNCTION_EXIT;
}

void CCallStackPage::addCall(RadioResource& resource)
{
    FUNCTION_ENTRY("addCall");
    
	LV_FINDINFO findinfo;
	findinfo.flags = LVFI_PARAM;
	findinfo.lParam = resource.id;

	int nIndex = m_RadioCallStackListCtrl.FindItem(&findinfo);

	if ( nIndex != -1) // already in call stack
	{
		return;
	}

	LV_COLUMN lv;  
	lv.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lv.fmt = LVCFMT_LEFT;

    nIndex = m_RadioCallStackListCtrl.GetItemCount();

	// Description
	m_RadioCallStackListCtrl.InsertItem(nIndex,resource.alias.c_str());

	// Type
	m_RadioCallStackListCtrl.SetItemText(nIndex,1,getTypeText(resource.type));
	
	// id
	std::ostringstream data;
	data << resource.ssi;
	m_RadioCallStackListCtrl.SetItemText(nIndex,2,data.str().c_str());

	if (resource.type==RR_RADIO)
	{
		m_RadioCallStackListCtrl.SetItemText(nIndex,3,"Idle");
	}

	// Associate list control item with RadioResource id
	m_RadioCallStackListCtrl.SetItemData(nIndex,resource.id);

	// add to vector
	m_RadioResource.push_back(resource);

	if (The_RadioSystemStatus::instance()->getResource_isSelected(resource))
	{
		updateCall(resource,getStatusTextDescription(resource,WM_SELECT_SPEAKER_SELECTED));
	}
	else if (The_RadioSystemStatus::instance()->getResource_isEmergencyMode(resource) != RE_NO_EMERGENCY)
	{
		updateCall(resource,getStatusTextDescription(resource,WM_RESOURCE_EMERGENCY));
	}
	else if (The_RadioSystemStatus::instance()->getResource_isActive(resource))
	{
		updateCall(resource,getStatusTextDescription(resource,WM_RESOURCE_ACTIVITY));
	}
	else if (The_RadioSystemStatus::instance()->getResource_isInAmbienceListeningCall(resource))
	{
		updateCall(resource,getStatusTextDescription(resource,WM_AMBIENCE_LISTENING));
	}

	FUNCTION_EXIT;
}


void CCallStackPage::removeCall(RadioResource& resource)
{
    FUNCTION_ENTRY("removeCall");
    
	LV_FINDINFO findinfo;
	findinfo.flags = LVFI_PARAM;
	findinfo.lParam = resource.id;

	int nIndex = m_RadioCallStackListCtrl.FindItem(&findinfo);

	if ( nIndex != -1)
	{
		// remove from vector
		std::vector<RadioResource>::iterator iter = m_RadioResource.begin() + nIndex;
		m_RadioResource.erase(iter);

		// remove from List Control			
		m_RadioCallStackListCtrl.DeleteItem(nIndex);
	}

	FUNCTION_EXIT;
}


void CCallStackPage::updateCall(RadioResource& resource, CString statusText)
{
    FUNCTION_ENTRY("updateCall");
    
	LV_FINDINFO findinfo;
	findinfo.flags = LVFI_PARAM;
	findinfo.lParam = resource.id;

	int nIndex = m_RadioCallStackListCtrl.FindItem(&findinfo);

	if ( nIndex != -1)
	{
		m_RadioCallStackListCtrl.SetItemText(nIndex,3, statusText);
		//m_RadioCallStackListCtrl.SetItemText(nIndex,4, "");
	}


	FUNCTION_EXIT;
}


CString CCallStackPage::getStatusTextDescription(RadioResource& resource,unsigned long message,
                                                 LPARAM pData)
{
	CString status;
	
	switch(message)
	{
		case WM_INBOUND_CALL:
			status = "Incoming";
			break;
		case WM_INBOUND_PTT:
			if ((resource.type ==RR_TALKGROUP) || (resource.type==RR_DYNGROUP))
				status = "Listening(PTT)";
			else
				status = "Listening";
			break;
		case WM_SELECT_SPEAKER_SELECTED:
			if ( resource.type == RR_RADIO )
				status = "Established (Selected)";
			else 
				status = "Selected";
			break;
		case WM_SELECT_SPEAKER_UNSELECTED:
			if ((resource.type == RR_RADIO) || (resource.type == RR_TALKGROUP))
				status = "Monitoring (Unselected)";
			else 
				status = "UnSelected";
			break;
		case WM_RESOURCE_ACTIVITY:
			status = "Listening";
			break;
		case WM_RESOURCE_EMERGENCY:
			status = "Emergency";
			break;
		case WM_RESOURCE_EMERGENCY_ACKNOWLEDGED:
			status = "Emergency(Ack)";
			break;

		case WM_CONNECT_GROUP:
            {
                GroupConnectionState state = (GroupConnectionState) pData;
                if (pData == CONNECTED)
                {
				    status = "Connected";
			    }
			    else
			    {
				    status = "Not Connected";
			    }
            }
			break;
		case WM_OUTGOING_CALL:
			status = "Outgoing";
			break;
		case WM_AMBIENCE_LISTENING:
			status = "Ambience Listening";
			break;
		case WM_PRIVATE_CALL_ESTABLISHED: // Fixed TD#14289
			status = "Established";
			break;
		case WM_PRIVATE_CALL_RINGING:
			status = "Ringing";
			break;
		case WM_INCOMING_DRIVER_CALL:
			status = "Callback";
			break;

		default:
			if (resource.type == RR_RADIO)
				status = "Idle";
	}

	return status;
}


CString CCallStackPage::getTypeText(RadioResourceType type)
{
    return CRadioHelper::getTypeText(type);
}


LRESULT CCallStackPage::onNotifiedIncomingDriverCall(WPARAM wParam, LPARAM lParam)
{
		if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		addCall(*resource);
		updateCall(*resource, getStatusTextDescription(*resource,WM_INCOMING_DRIVER_CALL));
	}
	return 0;
}

LRESULT CCallStackPage::onNotifiedEstablishedDriverCall(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;
		//updateCall(*resource, getStatusTextDescription(*resource,WM_PRIVATE_CALL_ESTABLISHED));
		removeCall(*resource);
	}
	return 0;
}
		
		
		
		
	


	