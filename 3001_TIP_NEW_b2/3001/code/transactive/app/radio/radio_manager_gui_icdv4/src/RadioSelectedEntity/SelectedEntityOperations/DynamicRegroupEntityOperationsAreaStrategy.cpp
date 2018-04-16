/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/DynamicRegroupEntityOperationsAreaStrategy.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/DynamicRegroupEntityOperationsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"


#include "core/utilities/src/DebugUtil.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DynamicRegroupEntityOperationsAreaStrategy::DynamicRegroupEntityOperationsAreaStrategy()
{
}


DynamicRegroupEntityOperationsAreaStrategy::~DynamicRegroupEntityOperationsAreaStrategy()
{

}

void DynamicRegroupEntityOperationsAreaStrategy::initialise(CEntityOperationsAreaDlg* pentityOperationsDlg)
{
    FUNCTION_ENTRY("initialise");
    
	m_CEntityOperationsAreaDlg = pentityOperationsDlg;

	// Hide controls
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_NEW_PATCH)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_ORIGINAL_PATCH)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_SEND_TEXT)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_callButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_answerButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_endCallButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_sendTextButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_ambienceButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_volumeSlider.ShowWindow(SW_HIDE);	
	m_CEntityOperationsAreaDlg->m_staticText.ShowWindow(SW_HIDE);	
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_MIN)->ShowWindow(SW_HIDE);	
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_MAX)->ShowWindow(SW_HIDE);	

	
	m_CEntityOperationsAreaDlg->m_callButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(FALSE);
	m_CEntityOperationsAreaDlg->m_answerButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(FALSE);
	m_CEntityOperationsAreaDlg->m_endCallButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_endCallButton.EnableWindow(FALSE);


	m_CEntityOperationsAreaDlg->m_connectButton.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_releaseButton.ShowWindow(SW_HIDE);

	RadioResource& res = m_CEntityOperationsAreaDlg->m_RadioResource;
	// Selected button
	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.ShowWindow(SW_HIDE);
	//m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.EnableWindow(TRUE);

	//if (The_RadioSystemStatus::instance()->getResource_isSelected(res))
	//{
	//	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.SetCheck(1);
	//	m_CEntityOperationsAreaDlg->m_staticText.SetWindowText("Selected Volume");
	//}
	//else
	//{
	//	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.SetCheck(0);
	//	m_CEntityOperationsAreaDlg->m_staticText.SetWindowText("Unselected Volume");
	//}

	//if (The_RadioSystemStatus::instance()->getResource_isGroupConnected(res) == CONNECTED)
	//{
	//	m_CEntityOperationsAreaDlg->m_connectButton.EnableWindow(FALSE);
	//	m_CEntityOperationsAreaDlg->m_releaseButton.EnableWindow(TRUE);
	//}
	//else
	//{
	//	m_CEntityOperationsAreaDlg->m_connectButton.EnableWindow(TRUE);
	//	m_CEntityOperationsAreaDlg->m_releaseButton.EnableWindow(FALSE);
	//}

	//CRect dummyRect;
	//m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_NEW_PATCH)->GetWindowRect(&dummyRect);
	//m_CEntityOperationsAreaDlg->ScreenToClient(&dummyRect);
	//m_CEntityOperationsAreaDlg->m_patchButton.SetWindowPos(NULL,dummyRect.left,dummyRect.top,dummyRect.Width(),dummyRect.Height(),SWP_SHOWWINDOW);

	FUNCTION_EXIT;
}


void DynamicRegroupEntityOperationsAreaStrategy::selectSpeaker()
{
    FUNCTION_ENTRY("selectSpeaker");
    
	The_RadioOperations::instance()->selectSpeaker(m_CEntityOperationsAreaDlg->m_RadioResource);

	FUNCTION_EXIT;
}

void DynamicRegroupEntityOperationsAreaStrategy::unselectSpeaker()
{
    FUNCTION_ENTRY("unselectSpeaker");
    
	The_RadioOperations::instance()->unselectSpeaker(m_CEntityOperationsAreaDlg->m_RadioResource);

	FUNCTION_EXIT;
}

void DynamicRegroupEntityOperationsAreaStrategy::patch()
{
    FUNCTION_ENTRY("patch");

	//CPatchManagerDlg dlg(CPatchManagerDlg::FROM_RADIO,m_CEntityOperationsAreaDlg); // Fixed TD#13911

	//dlg.setRadioResource(m_CEntityOperationsAreaDlg->m_RadioResource);

	//dlg.DoModal();

    FUNCTION_EXIT;
	
}

void DynamicRegroupEntityOperationsAreaStrategy::connect()
{
    FUNCTION_ENTRY("connect");
    
	if (The_RadioOperations::instance()->connectGroup(m_CEntityOperationsAreaDlg->m_RadioResource))
	{
		m_CEntityOperationsAreaDlg->m_releaseButton.EnableWindow(TRUE);

		m_CEntityOperationsAreaDlg->m_connectButton.EnableWindow(FALSE);
	}
	else
	{
		AfxMessageBox("Failed to connect resource.");
	}


	FUNCTION_EXIT;
}

void DynamicRegroupEntityOperationsAreaStrategy::release()
{
    FUNCTION_ENTRY("release");
    
	if (The_RadioOperations::instance()->releaseGroup(m_CEntityOperationsAreaDlg->m_RadioResource))
	{
		m_CEntityOperationsAreaDlg->m_releaseButton.EnableWindow(FALSE);

		m_CEntityOperationsAreaDlg->m_connectButton.EnableWindow(TRUE);
	}
	else
	{
		AfxMessageBox("Failed to release resource.");
	}

	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.SetCheck(0);

	FUNCTION_EXIT;
}

void DynamicRegroupEntityOperationsAreaStrategy::adjustVolume()
{
	FUNCTION_ENTRY("adjustVolume");

	int volume = m_CEntityOperationsAreaDlg->m_volumeSlider.GetPos();

	The_RadioOperations::instance()->changeVolume(m_CEntityOperationsAreaDlg->m_RadioResource,volume);

	FUNCTION_EXIT;
}