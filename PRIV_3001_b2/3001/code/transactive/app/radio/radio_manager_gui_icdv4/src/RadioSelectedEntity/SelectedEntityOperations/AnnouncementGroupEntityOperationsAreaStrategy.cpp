/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/AnnouncementGroupEntityOperationsAreaStrategy.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */


#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/AnnouncementGroupEntityOperationsAreaStrategy.h"
#include "core/utilities/src/DebugUtil.h"


AnnouncementGroupEntityOperationsAreaStrategy::AnnouncementGroupEntityOperationsAreaStrategy()
{

}


AnnouncementGroupEntityOperationsAreaStrategy::~AnnouncementGroupEntityOperationsAreaStrategy()
{

}


void AnnouncementGroupEntityOperationsAreaStrategy::initialise(CEntityOperationsAreaDlg* pEntityOperationsDlg)
{
	FUNCTION_ENTRY("initialise");

	m_CEntityOperationsAreaDlg = pEntityOperationsDlg;

	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_NEW_PATCH)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_ORIGINAL_PATCH)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_SEND_TEXT)->ShowWindow(SW_HIDE);

	// Hide controls
	m_CEntityOperationsAreaDlg->m_callButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_answerButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_endCallButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_sendTextButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_patchButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_ambienceButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_connectButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_releaseButton.ShowWindow(SW_HIDE);

	// Show controls
	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.EnableWindow(TRUE);
	if (The_RadioSystemStatus::instance()->getResource_isSelected(m_CEntityOperationsAreaDlg->m_RadioResource))
	{
		m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.SetCheck(1);
		m_CEntityOperationsAreaDlg->m_staticText.SetWindowText("Selected Volume");
	}
	else
	{
		m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.SetCheck(0);
		m_CEntityOperationsAreaDlg->m_staticText.SetWindowText("Unselected Volume");
	}

	//Slider bar & its text;
	m_CEntityOperationsAreaDlg->m_volumeSlider.ShowWindow(SW_SHOW);	
	m_CEntityOperationsAreaDlg->m_staticText.ShowWindow(SW_SHOW);	
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_MIN)->ShowWindow(SW_SHOW);	
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_MAX)->ShowWindow(SW_SHOW);	

 	m_CEntityOperationsAreaDlg->m_volumeSlider.SetPos(
		The_RadioSystemStatus::instance()->getResourceVolume(m_CEntityOperationsAreaDlg->m_RadioResource));

	FUNCTION_EXIT;
}

void AnnouncementGroupEntityOperationsAreaStrategy::selectSpeaker()
{
    FUNCTION_ENTRY("selectSpeaker");
    
	The_RadioOperations::instance()->selectSpeaker(m_CEntityOperationsAreaDlg->m_RadioResource);

	FUNCTION_EXIT;
}

void AnnouncementGroupEntityOperationsAreaStrategy::unselectSpeaker()
{
    FUNCTION_ENTRY("unselectSpeaker");

    The_RadioOperations::instance()->unselectSpeaker(m_CEntityOperationsAreaDlg->m_RadioResource);

    FUNCTION_EXIT;
}

void AnnouncementGroupEntityOperationsAreaStrategy::adjustVolume()
{
	FUNCTION_ENTRY("adjustVolume");

	int volume = m_CEntityOperationsAreaDlg->m_volumeSlider.GetPos();

	The_RadioOperations::instance()->changeVolume(m_CEntityOperationsAreaDlg->m_RadioResource,volume);

	FUNCTION_EXIT;
}