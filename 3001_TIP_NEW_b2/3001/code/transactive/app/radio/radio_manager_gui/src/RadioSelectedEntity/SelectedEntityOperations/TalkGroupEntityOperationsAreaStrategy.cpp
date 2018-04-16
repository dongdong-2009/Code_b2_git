/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/TalkGroupEntityOperationsAreaStrategy.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/resource.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/TalkGroupEntityOperationsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"



#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TalkGroupEntityOperationsAreaStrategy::TalkGroupEntityOperationsAreaStrategy()
{

}

TalkGroupEntityOperationsAreaStrategy::~TalkGroupEntityOperationsAreaStrategy()
{

}


void TalkGroupEntityOperationsAreaStrategy::initialise(CEntityOperationsAreaDlg* pentityOperationsDlg)
{
	FUNCTION_ENTRY("initialise");

	m_CEntityOperationsAreaDlg = pentityOperationsDlg;

	if (NULL == m_CEntityOperationsAreaDlg) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "EntityOperationsAreaDlg is NULL");
		return;
	}

	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_NEW_PATCH)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_ORIGINAL_PATCH)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_SEND_TEXT)->ShowWindow(SW_HIDE);

	// Hide controls
	m_CEntityOperationsAreaDlg->m_callButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(FALSE);
	m_CEntityOperationsAreaDlg->m_answerButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(FALSE);
	m_CEntityOperationsAreaDlg->m_endCallButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_endCallButton.EnableWindow(FALSE);

	m_CEntityOperationsAreaDlg->m_connectButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_ambienceButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_releaseButton.ShowWindow(SW_HIDE);

	// Show slider & its text;
	m_CEntityOperationsAreaDlg->m_staticText.ShowWindow(SW_HIDE);	
	m_CEntityOperationsAreaDlg->m_volumeSlider.ShowWindow(SW_HIDE);	
	m_CEntityOperationsAreaDlg->m_volumeSlider.EnableWindow(FALSE); // TD18169
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_MIN)->ShowWindow(SW_HIDE);	
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_MAX)->ShowWindow(SW_HIDE);	
	m_CEntityOperationsAreaDlg->m_volumeSlider.SetPos(
		The_RadioSystemStatus::instance()->getResourceVolume(m_CEntityOperationsAreaDlg->m_RadioResource));


	// Show controls
	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.ShowWindow(SW_HIDE);
	//m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.EnableWindow(TRUE);
	//if (The_RadioSystemStatus::instance()->getResource_isSelected(m_CEntityOperationsAreaDlg->m_RadioResource))
	//{
	//	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.SetCheck(1);
	//	m_CEntityOperationsAreaDlg->m_staticText.SetWindowText("Selected Volume");
	//}
	//else
	//{
	//	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.SetCheck(0);
	//	m_CEntityOperationsAreaDlg->m_staticText.SetWindowText("Unselected Volume");
	//}

	////Show m_sendTextButton at the position of m_connectButton;
	//CRect btnConnect;

	//m_CEntityOperationsAreaDlg->m_connectButton.GetWindowRect(&btnConnect);
	//m_CEntityOperationsAreaDlg->ScreenToClient(&btnConnect);
	//m_CEntityOperationsAreaDlg->m_sendTextButton.SetWindowPos(NULL,btnConnect.left,btnConnect.top,btnConnect.Width(),btnConnect.Height(),SWP_SHOWWINDOW);


	////Show m_patchButton at the position of m_releaseButton;
	//CRect btnRelease;

	//m_CEntityOperationsAreaDlg->m_releaseButton.GetWindowRect(&btnRelease);

	//m_CEntityOperationsAreaDlg->ScreenToClient(&btnRelease);

	//m_CEntityOperationsAreaDlg->m_patchButton.SetWindowPos(NULL,btnRelease.left,btnRelease.top,btnRelease.Width(),btnRelease.Height(),SWP_SHOWWINDOW);

	FUNCTION_EXIT;
}


//void TalkGroupEntityOperationsAreaStrategy::sendText()
//{
//	FUNCTION_ENTRY("sendText");
//
//	if (NULL == m_CEntityOperationsAreaDlg) {
//		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "EntityOperationsAreaDlg is NULL");
//		return;
//	}
//
//	CTextMessagingDlg dlg("Radio Text", m_CEntityOperationsAreaDlg);
//
//	dlg.setRadioResource(m_CEntityOperationsAreaDlg->m_RadioResource);
//
//    dlg.DoModal();
//
//	FUNCTION_EXIT;
//}


void TalkGroupEntityOperationsAreaStrategy::selectSpeaker()
{
	FUNCTION_ENTRY("selectSpeaker");

	if (NULL == m_CEntityOperationsAreaDlg) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "EntityOperationsAreaDlg is NULL");
		return;
	}

	The_RadioOperations::instance()->selectSpeaker(m_CEntityOperationsAreaDlg->m_RadioResource);

	FUNCTION_EXIT;
}

void TalkGroupEntityOperationsAreaStrategy::unselectSpeaker()
{
	FUNCTION_ENTRY("unselectSpeaker");

	if (NULL == m_CEntityOperationsAreaDlg) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "EntityOperationsAreaDlg is NULL");
		return;
	}

	The_RadioOperations::instance()->unselectSpeaker(m_CEntityOperationsAreaDlg->m_RadioResource);

	FUNCTION_EXIT;
}

void TalkGroupEntityOperationsAreaStrategy::patch()
{
	FUNCTION_ENTRY("patch");

	//CPatchManagerDlg dlg(CPatchManagerDlg::FROM_RADIO,m_CEntityOperationsAreaDlg); // Fixed TD#13911

	//dlg.setRadioResource(m_CEntityOperationsAreaDlg->m_RadioResource);

	//dlg.DoModal();	

	FUNCTION_EXIT;
}


void TalkGroupEntityOperationsAreaStrategy::adjustVolume()
{
	FUNCTION_ENTRY("adjustVolume");

	if (NULL == m_CEntityOperationsAreaDlg) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "EntityOperationsAreaDlg is NULL");
		return;
	}

	int volume = m_CEntityOperationsAreaDlg->m_volumeSlider.GetPos();

	The_RadioOperations::instance()->changeVolume(m_CEntityOperationsAreaDlg->m_RadioResource,volume);

	FUNCTION_EXIT;
}
