/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/PatchGroupEntityOperationsAreaStrategy.cpp $
  * @author:  F. Stuart
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */


#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/PatchGroupEntityOperationsAreaStrategy.h"
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

PatchGroupEntityOperationsAreaStrategy::PatchGroupEntityOperationsAreaStrategy()
{

}

PatchGroupEntityOperationsAreaStrategy::~PatchGroupEntityOperationsAreaStrategy()
{

}


void PatchGroupEntityOperationsAreaStrategy::initialise(CEntityOperationsAreaDlg* pentityOperationsDlg)
{
	FUNCTION_ENTRY("initialise");

	m_CEntityOperationsAreaDlg = pentityOperationsDlg; 

	//Hide controls;
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_NEW_PATCH)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_ORIGINAL_PATCH)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_SEND_TEXT)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_callButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_answerButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_endCallButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_sendTextButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_ambienceButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_patchButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_volumeSlider.ShowWindow(SW_HIDE);	
	m_CEntityOperationsAreaDlg->m_staticText.ShowWindow(SW_HIDE);	
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_MIN)->ShowWindow(SW_HIDE);	
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_MAX)->ShowWindow(SW_HIDE);	

	// Show controls
	m_CEntityOperationsAreaDlg->m_connectButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_releaseButton.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(FALSE);
	m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(FALSE);
	m_CEntityOperationsAreaDlg->m_endCallButton.EnableWindow(FALSE);

	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.ShowWindow(SW_HIDE);

	RadioResource& res = m_CEntityOperationsAreaDlg->m_RadioResource;
    GroupConnectionState connectState = The_RadioSystemStatus::instance()->getResource_isGroupConnected(res);

    switch (connectState)
    {
    case CONNECTED:
    	m_CEntityOperationsAreaDlg->m_connectButton.EnableWindow(FALSE);
    	m_CEntityOperationsAreaDlg->m_releaseButton.EnableWindow(FALSE);
		m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.EnableWindow(FALSE);
		//fixed TD14295
		//m_CEntityOperationsAreaDlg->m_volumeSlider.EnableWindow(FALSE);
		
	    if (The_RadioSystemStatus::instance()->getResource_isSelected(res))
	    {
		    m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.SetCheck(1);
		    m_CEntityOperationsAreaDlg->m_staticText.SetWindowText("Selected Volume");
	    }
	    else
	    {
		    m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.SetCheck(0);
		    m_CEntityOperationsAreaDlg->m_staticText.SetWindowText("Unselected Volume");
	    }
        break;

    case CONNECTION_BUSY:
    	m_CEntityOperationsAreaDlg->m_connectButton.EnableWindow(FALSE);
    	m_CEntityOperationsAreaDlg->m_releaseButton.EnableWindow(FALSE);
		m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.EnableWindow(FALSE);
		//fixed TD14295
		//m_CEntityOperationsAreaDlg->m_volumeSlider.EnableWindow(FALSE);
        break;

    default:
    	m_CEntityOperationsAreaDlg->m_connectButton.EnableWindow(FALSE);
    	m_CEntityOperationsAreaDlg->m_releaseButton.EnableWindow(FALSE);
		m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.EnableWindow(FALSE);
		//fixed TD14295
		//m_CEntityOperationsAreaDlg->m_volumeSlider.EnableWindow(FALSE);
    }

	FUNCTION_EXIT;
}


void PatchGroupEntityOperationsAreaStrategy::connect()
{
	FUNCTION_ENTRY("connect");

	if (The_RadioOperations::instance()->connectGroup(m_CEntityOperationsAreaDlg->m_RadioResource))
    {
	    m_CEntityOperationsAreaDlg->m_connectButton.EnableWindow(FALSE);
    }

	FUNCTION_EXIT;
}


void PatchGroupEntityOperationsAreaStrategy::selectSpeaker()
{
	FUNCTION_ENTRY("selectSpeaker");

	The_RadioOperations::instance()->selectSpeaker(m_CEntityOperationsAreaDlg->m_RadioResource);

	FUNCTION_EXIT;
}

void PatchGroupEntityOperationsAreaStrategy::unselectSpeaker()
{
	FUNCTION_ENTRY("unselectSpeaker");

	The_RadioOperations::instance()->unselectSpeaker(m_CEntityOperationsAreaDlg->m_RadioResource);

	FUNCTION_EXIT;
}

void PatchGroupEntityOperationsAreaStrategy::release()
{
	FUNCTION_ENTRY("release");

	if (The_RadioOperations::instance()->releaseGroup(m_CEntityOperationsAreaDlg->m_RadioResource))
    {
    	m_CEntityOperationsAreaDlg->m_releaseButton.EnableWindow(FALSE);
    }

	FUNCTION_EXIT;
}


void PatchGroupEntityOperationsAreaStrategy::adjustVolume()
{
	FUNCTION_ENTRY("adjustVolume");

	int volume = m_CEntityOperationsAreaDlg->m_volumeSlider.GetPos();

	The_RadioOperations::instance()->changeVolume(m_CEntityOperationsAreaDlg->m_RadioResource,volume);

	FUNCTION_EXIT;
}


void PatchGroupEntityOperationsAreaStrategy::onConnectState(GroupConnectionState pState)
{
    switch (pState)
    {
    case CONNECTED:
		m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.EnableWindow(FALSE);
    	m_CEntityOperationsAreaDlg->m_releaseButton.EnableWindow(FALSE);
		//fixed TD14295
		//m_CEntityOperationsAreaDlg->m_volumeSlider.EnableWindow(FALSE);
		
        break;

    case NAG:
    case RELEASED:
    	m_CEntityOperationsAreaDlg->m_connectButton.EnableWindow(FALSE);
    	m_CEntityOperationsAreaDlg->m_releaseButton.EnableWindow(FALSE);
		m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.EnableWindow(FALSE);

		//fixed TD14295
		//m_CEntityOperationsAreaDlg->m_volumeSlider.EnableWindow(FALSE);
		
        break;
    }
}
