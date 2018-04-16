/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusBar/CRadioManagerDlgStatusBar.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"

#include "app/radio/radio_manager_gui_icdv4/src/resource.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusBar/CRadioManagerDlgStatusBar.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"

#include "core/utilities/src/TAAssert.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>

#include <afxcmn.h>


CRadioManagerDlgStatusBar::CRadioManagerDlgStatusBar()
{
	// register user-defined messages
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_SELECT_SPEAKER_SELECTED);
	//The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_SELECT_SPEAKER_UNSELECTED);
//	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INBOUND_CALL);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_ESTABLISHED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_ENDED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INBOUND_PTT);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INBOUND_PTT_RELEASED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_OUTGOING_CALL);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_RINGING);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INBOUND_CALL_CANCELED);

}


CRadioManagerDlgStatusBar::~CRadioManagerDlgStatusBar()
{
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_SELECT_SPEAKER_SELECTED);
	//The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_SELECT_SPEAKER_UNSELECTED);
//	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INBOUND_CALL);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_ESTABLISHED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_ENDED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INBOUND_PTT);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INBOUND_PTT_RELEASED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_OUTGOING_CALL);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_RINGING);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INBOUND_CALL_CANCELED);
}

BEGIN_MESSAGE_MAP(CRadioManagerDlgStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CRadioManagerDlgStatusBar)
	//ON_MESSAGE(WM_SELECT_SPEAKER_SELECTED, onNotifiedSelectSpeakerSelected)
	//ON_MESSAGE(WM_SELECT_SPEAKER_UNSELECTED, onNotifiedSelectSpeakerUnselected)
	//ON_MESSAGE(WM_INBOUND_CALL, onNotifiedInboundCall)
	ON_MESSAGE(WM_PRIVATE_CALL_ESTABLISHED, onNotifiedPrivateCallEstablished)
	ON_MESSAGE(WM_PRIVATE_CALL_ENDED, onNotifiedPrivateCallEnded)
	ON_MESSAGE(WM_INBOUND_PTT, onNotifiedInboundPTTStatus)
	ON_MESSAGE(WM_INBOUND_PTT_RELEASED, onNotifiedInboundPTTStatusReleased)
	ON_MESSAGE(WM_PRIVATE_CALL_RINGING,onNotifiedPrivateCallRinging)
	ON_MESSAGE(WM_OUTGOING_CALL,onNotifiedOutgoingCall)
	ON_MESSAGE(WM_INBOUND_CALL_CANCELED,onNotifiedInboundPTTStatusReleased)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CRadioManagerDlgStatusBar::OnInitDialog()
{
	FUNCTION_ENTRY("OnInitDialog");


	

	FUNCTION_EXIT;
	return true;
}
//
//afx_msg LRESULT CRadioManagerDlgStatusBar::onNotifiedSelectSpeakerSelected(WPARAM wParam, LPARAM lParam)
//{
//	FUNCTION_ENTRY("onNotifiedSelectSpeakerSelected");
//
//	RadioResource* resource = (RadioResource*)wParam;
//
//	CString sPaneText;
//	sPaneText.LoadString(IDS_STATUS_BAR_MSG_3);
//
//	sPaneText = sPaneText + resource->alias.c_str(); 
//
//	SetPaneText( 3, sPaneText );
//
//	FUNCTION_EXIT;
//
//	return 0;
//}
//
//afx_msg LRESULT CRadioManagerDlgStatusBar::onNotifiedSelectSpeakerUnselected(WPARAM wParam, LPARAM lParam)
//{
//	FUNCTION_ENTRY("onNotifiedSelectSpeakerUnselected");
//
//
//	CString sPaneText;
//	sPaneText.LoadString(IDS_STATUS_BAR_MSG_3);
//
//	SetPaneText( 3, sPaneText );
//
//	// clear text in Private Call status & PTT panes
//	CString sPanePTT;
//	sPanePTT.LoadString(IDS_STATUS_BAR_MSG_2);
//
//	SetPaneText( 2, sPanePTT );
//
//	CString sPanePrivateStat;
//	sPanePrivateStat.LoadString(IDS_STATUS_BAR_MSG_1);
//
//	SetPaneText( 1, sPanePrivateStat );
//
//	FUNCTION_EXIT;
//
//	return 0;
//
//}
//
//afx_msg LRESULT CRadioManagerDlgStatusBar::onNotifiedInboundCall(WPARAM wParam, LPARAM lParam)
//{
//    FUNCTION_ENTRY("onNotifiedInboundCall");
//
//	RadioResource* resource = (RadioResource*)wParam;
//
//	CString sPaneText;
//	sPaneText.LoadString(IDS_STATUS_BAR_MSG_1);
//
//	sPaneText = sPaneText + " Incoming " + resource->alias.c_str(); 
//
//	if (resource->type != RR_NIL) 
//	{
//		SetPaneText( 1, sPaneText ); 
//	}
//
//    FUNCTION_EXIT;
//	return 0;
//}

afx_msg LRESULT CRadioManagerDlgStatusBar::onNotifiedPrivateCallEstablished(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedPrivateCallEstablished");

	RadioResource* resource = (RadioResource*)wParam;

	CString sPaneText;
	sPaneText.LoadString(IDS_STATUS_BAR_MSG_1);

	std::ostringstream ssiStr;
	ssiStr << resource->ssi;

	sPaneText = sPaneText + " Ongoing " + ssiStr.str().c_str(); 

	SetPaneText( 1, sPaneText );

	//// Fixed TD#14289
	//CString sPanePTT;
	//sPanePTT.LoadString(IDS_STATUS_BAR_MSG_2);
	//SetPaneText( 2, sPanePTT); 

	//CString sPaneSelect;
	//sPaneSelect.LoadString(IDS_STATUS_BAR_MSG_3);

	//sPaneSelect = sPaneSelect + ssiStr.str().c_str(); 

	//SetPaneText( 3, sPaneSelect); // Select Speaker

	FUNCTION_EXIT

	return 0;

}

afx_msg LRESULT CRadioManagerDlgStatusBar::onNotifiedPrivateCallEnded(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedPrivateCallEnded");


	CString sPaneText;
	sPaneText.LoadString(IDS_STATUS_BAR_MSG_1);

	SetPaneText( 1, sPaneText );

	//CString sPaneSelect;
	//sPaneSelect.LoadString(IDS_STATUS_BAR_MSG_3);
	//SetPaneText( 3, sPaneSelect); // Select Speaker

	FUNCTION_EXIT

	return 0;
}

afx_msg LRESULT CRadioManagerDlgStatusBar::onNotifiedInboundPTTStatus(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedInboundPTTStatus");

	RadioResource* resource = (RadioResource*)lParam;

	CString sPaneText;
	sPaneText.LoadString(IDS_STATUS_BAR_MSG_2);

	std::ostringstream ssiStr;
	ssiStr << resource->ssi;

	sPaneText = sPaneText + " " + ssiStr.str().c_str(); 

	SetPaneText( 2, sPaneText );

	FUNCTION_EXIT

	return 0;
}

afx_msg LRESULT CRadioManagerDlgStatusBar::onNotifiedInboundPTTStatusReleased(WPARAM wParama, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedInboundPTTStatusReleased");

	RadioResource* resource = (RadioResource*)wParama;

	if (resource->type != RR_NIL)
	{
		CString sPaneText;
		sPaneText.LoadString(IDS_STATUS_BAR_MSG_1);

		SetPaneText( 1, sPaneText );

		CString sPanePTT;
		sPanePTT.LoadString(IDS_STATUS_BAR_MSG_2);

		SetPaneText( 2, sPanePTT );
	}
	
	FUNCTION_EXIT

	return 0;
}


LRESULT CRadioManagerDlgStatusBar::onNotifiedPrivateCallRinging(WPARAM wParam, LPARAM lParam)
{
	RadioResource* resource = (RadioResource*)wParam;

	CString sPaneText;
	sPaneText.LoadString(IDS_STATUS_BAR_MSG_1);

	std::ostringstream ssiStr;
	ssiStr << resource->ssi;
	sPaneText = sPaneText + " Incoming, " + resource->alias.c_str();// ssiStr.str().c_str(); 

	SetPaneText( 1, sPaneText );

	//CString sPanePTT;
	//sPanePTT.LoadString(IDS_STATUS_BAR_MSG_2);
	//SetPaneText( 2, sPanePTT); 

	return 0;
}

LRESULT CRadioManagerDlgStatusBar::onNotifiedOutgoingCall(WPARAM wParam, LPARAM lParam)
{
	RadioResource* resource = (RadioResource*)wParam;

	CString sPaneText;
	sPaneText.LoadString(IDS_STATUS_BAR_MSG_1);

	sPaneText = sPaneText + " Outgoing " + resource->alias.c_str(); 

	SetPaneText( 1, sPaneText );

	//CString sPanePTT;
	//sPanePTT.LoadString(IDS_STATUS_BAR_MSG_2);
	//SetPaneText( 2, sPanePTT); 
	
	return 0;
}

//TD14273, Haipeng added this function
int CRadioManagerDlgStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	NONCLIENTMETRICS metrics;
	metrics.cbSize = sizeof(metrics);
	::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, 0);
	
	metrics.lfStatusFont.lfHeight = 20;  //mandatory to set the height as 20
	metrics.lfStatusFont.lfWeight = 700; // TD14957
	_tcscpy(metrics.lfStatusFont.lfFaceName, _T("Arial"));
	m_font.CreateFontIndirect(&metrics.lfStatusFont);
	
	CWnd *pWnd = CWnd::FromHandle(m_hWnd);
	pWnd->SetFont(&m_font);

	return 0;
}
