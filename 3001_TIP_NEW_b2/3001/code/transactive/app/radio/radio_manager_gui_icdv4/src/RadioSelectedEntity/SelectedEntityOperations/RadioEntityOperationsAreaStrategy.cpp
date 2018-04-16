/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/RadioEntityOperationsAreaStrategy.cpp $
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
#include "app/radio/radio_manager_gui_icdv4/src/CRadioManagerDlg.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/RadioEntityOperationsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CTextMessagingDlg.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/CEntityOperationsAreaDlg.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/utilities/src/DebugUtil.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RadioEntityOperationsAreaStrategy::RadioEntityOperationsAreaStrategy() 
{

}

RadioEntityOperationsAreaStrategy::~RadioEntityOperationsAreaStrategy()
{

}


void RadioEntityOperationsAreaStrategy::initialise(CEntityOperationsAreaDlg* pEntityOperationsDlg)
{
	FUNCTION_ENTRY("initialise");

	// Initialize m_CEntityOperationsAreaDlg;
	m_CEntityOperationsAreaDlg = pEntityOperationsDlg;

	// Hide some controls;
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_NEW_PATCH)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_ORIGINAL_PATCH)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_SEND_TEXT)->ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_connectButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.ShowWindow(SW_HIDE);	
	m_CEntityOperationsAreaDlg->m_releaseButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_sendTextButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_ambienceButton.ShowWindow(SW_HIDE);
	m_CEntityOperationsAreaDlg->m_patchButton.ShowWindow(SW_HIDE);

	// Show controls
	m_CEntityOperationsAreaDlg->m_callButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_answerButton.ShowWindow(SW_SHOW);
	m_CEntityOperationsAreaDlg->m_endCallButton.ShowWindow(SW_SHOW);
	//
	checkControlRights();

	////Always retrieve the original positions: m_sendTextButton & m_patchButton;
	//CRect sendTextRect;

	//m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_SEND_TEXT)->GetWindowRect(&sendTextRect);
	//m_CEntityOperationsAreaDlg->ScreenToClient(&sendTextRect);
	//m_CEntityOperationsAreaDlg->m_sendTextButton.SetWindowPos(NULL,sendTextRect.left,sendTextRect.top,sendTextRect.Width(),sendTextRect.Height(),SWP_SHOWWINDOW);

	//CRect patchRect;

	//m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_ORIGINAL_PATCH)->GetWindowRect(&patchRect);
	//m_CEntityOperationsAreaDlg->ScreenToClient(&patchRect);
	//m_CEntityOperationsAreaDlg->m_patchButton.SetWindowPos(NULL,patchRect.left,patchRect.top,patchRect.Width(),patchRect.Height(),SWP_SHOWWINDOW);

	//// Show slider bar & its text;
	//m_CEntityOperationsAreaDlg->m_volumeSlider.ShowWindow(SW_SHOW);	
	//m_CEntityOperationsAreaDlg->m_staticText.ShowWindow(SW_SHOW);
	//m_CEntityOperationsAreaDlg->GetDlgItem(IDC_MIN)->ShowWindow(SW_SHOW);	
	//m_CEntityOperationsAreaDlg->GetDlgItem(IDC_MAX)->ShowWindow(SW_SHOW);	

	//m_CEntityOperationsAreaDlg->m_volumeSlider.SetPos(
	//	The_RadioSystemStatus::instance()->getResourceVolume(m_CEntityOperationsAreaDlg->m_RadioResource));

	UpdateControlState();
	FUNCTION_EXIT;
}



void RadioEntityOperationsAreaStrategy::call()
{
    FUNCTION_ENTRY("call");
    
	The_RadioOperations::instance()->makePrivateCall(m_CEntityOperationsAreaDlg->m_RadioResource);

	FUNCTION_EXIT;
}

void RadioEntityOperationsAreaStrategy::answer()
{
    FUNCTION_ENTRY("answer");

	The_RadioOperations::instance()->answerPrivateCall(m_CEntityOperationsAreaDlg->m_RadioResource);

    FUNCTION_EXIT;
}


void RadioEntityOperationsAreaStrategy::endCall()
{
    FUNCTION_ENTRY("endCall");
	
	The_RadioOperations::instance()->endPrivateCall(m_CEntityOperationsAreaDlg->m_RadioResource);

    FUNCTION_EXIT;
}


void RadioEntityOperationsAreaStrategy::sendText()
{
    FUNCTION_ENTRY("sendText");

	CTextMessagingDlg dlg("Radio Text", m_CEntityOperationsAreaDlg);

	dlg.setRadioResource(m_CEntityOperationsAreaDlg->m_RadioResource);

    dlg.DoModal();	

    FUNCTION_EXIT;
}

void RadioEntityOperationsAreaStrategy::ambienceListening()
{
    FUNCTION_ENTRY("ambienceListening");

	The_RadioOperations::instance()->makeAmbienceListeningCall(m_CEntityOperationsAreaDlg->m_RadioResource);

    FUNCTION_EXIT;
}


void RadioEntityOperationsAreaStrategy::adjustVolume()
{
	FUNCTION_ENTRY("adjustVolume");

	int volume = m_CEntityOperationsAreaDlg->m_volumeSlider.GetPos();

	The_RadioOperations::instance()->changeVolume(m_CEntityOperationsAreaDlg->m_RadioResource,volume);

	FUNCTION_EXIT;
}

void RadioEntityOperationsAreaStrategy::patch()
{
	FUNCTION_ENTRY("patch");

	//CPatchManagerDlg dlg(CPatchManagerDlg::FROM_RADIO,m_CEntityOperationsAreaDlg); // Fixed TD#13911

	//dlg.setRadioResource(m_CEntityOperationsAreaDlg->m_RadioResource);
	
	//dlg.DoModal();

	FUNCTION_EXIT;
}


void RadioEntityOperationsAreaStrategy::checkControlRights()
{
	CRadioManagerDlg* pMainWnd = dynamic_cast<CRadioManagerDlg*>(AfxGetMainWnd());    
	ASSERT_VALID(pMainWnd);

	TA_Base_Bus::AccessControlledAction callAction;
	TA_Base_Bus::AccessControlledAction answerAction;

/* AZ: TD14314 : this is not the correct way
	bool bIsEmergencyMode = The_RadioSystemStatus::instance()->getIsEmergecyMode();

	if (bIsEmergencyMode)
	{
		// update Rights for emergency mode
		callAction = TA_Base_Bus::aca_RADIO_OUTGOING_EMERGENCY_CALL;
		answerAction = TA_Base_Bus::aca_RADIO_INCOMING_EMERGENCY_CALL;
	}
	else
    */
	{
		callAction = TA_Base_Bus::aca_RADIO_OUTGOING_VOICE_CALL;
		answerAction = TA_Base_Bus::aca_RADIO_INCOMING_VOICE_CALL;
	}

	if (!pMainWnd->checkRights(callAction))
	{
		m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(FALSE);
	}

	if (!pMainWnd->checkRights(answerAction))
	{
		m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(FALSE);
	}

}

void RadioEntityOperationsAreaStrategy::UpdateControlState()
{
	PrivateCallState ptCallState = 
		The_RadioOperations::instance()->getResource_isInPrivateCall(m_CEntityOperationsAreaDlg->m_RadioResource);
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			"RadioEntityOperationStrategy %u",ptCallState);
	switch(ptCallState)
	{
		case  PC_INCOMING:
			m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(TRUE);
			m_CEntityOperationsAreaDlg->m_endCallButton.EnableWindow(TRUE);
			//m_CEntityOperationsAreaDlg->m_ambienceButton.EnableWindow(TRUE);
			//m_CEntityOperationsAreaDlg->m_patchButton.EnableWindow(FALSE);
			break;
		case PC_ESTABLISHED:
			m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_endCallButton.EnableWindow(TRUE);
			//m_CEntityOperationsAreaDlg->m_ambienceButton.EnableWindow(FALSE);
			//m_CEntityOperationsAreaDlg->m_patchButton.EnableWindow(TRUE);
			break;
		case PC_OUTGOING:
			m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_endCallButton.EnableWindow(TRUE);
			//m_CEntityOperationsAreaDlg->m_ambienceButton.EnableWindow(FALSE);
			//m_CEntityOperationsAreaDlg->m_patchButton.EnableWindow(TRUE);
			break;
		case PC_NOT_INVOLVED:
			m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(TRUE);
			m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_endCallButton.EnableWindow(FALSE);
			//m_CEntityOperationsAreaDlg->m_ambienceButton.EnableWindow(TRUE);
			//m_CEntityOperationsAreaDlg->m_patchButton.EnableWindow(FALSE);
			break;
	}

	// TD18119
	//bool isAmbListen = The_RadioOperations::instance()->getResource_isInAmbienceListeningCall(m_CEntityOperationsAreaDlg->m_RadioResource);
	//m_CEntityOperationsAreaDlg->m_ambienceButton.SetCheck(isAmbListen ? 1 : 0);
}
