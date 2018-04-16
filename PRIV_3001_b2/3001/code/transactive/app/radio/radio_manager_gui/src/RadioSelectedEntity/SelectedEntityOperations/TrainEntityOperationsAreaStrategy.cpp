/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/TrainEntityOperationsAreaStrategy.cpp $
  * @author:  F. Stuart
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/resource.h"
#include "app/radio/radio_manager_gui/src/CRadioManagerDlg.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/TrainEntityOperationsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"

#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/CEntityOperationsAreaDlg.h"
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

TrainEntityOperationsAreaStrategy::TrainEntityOperationsAreaStrategy() 
{

}

TrainEntityOperationsAreaStrategy::~TrainEntityOperationsAreaStrategy()
{

}


void TrainEntityOperationsAreaStrategy::initialise(CEntityOperationsAreaDlg* pEntityOperationsDlg)
{
	FUNCTION_ENTRY("initialise");

	// Initialize m_CEntityOperationsAreaDlg;
	m_CEntityOperationsAreaDlg = pEntityOperationsDlg;

	if (NULL == m_CEntityOperationsAreaDlg) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "EntityOperationsAreaDlg is NULL");
		return;
	}

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

	UpdateControlState();
	FUNCTION_EXIT;
}



void TrainEntityOperationsAreaStrategy::call()
{
    FUNCTION_ENTRY("call");

	FUNCTION_EXIT;
}

void TrainEntityOperationsAreaStrategy::answer()
{
    FUNCTION_ENTRY("answer");

	if (NULL == m_CEntityOperationsAreaDlg) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "EntityOperationsAreaDlg is NULL");
		return;
	}

	The_RadioOperations::instance()->makeDriverCall(m_CEntityOperationsAreaDlg->getCurrentResource());

    FUNCTION_EXIT;
}


void TrainEntityOperationsAreaStrategy::endCall()
{
    FUNCTION_ENTRY("endCall");
	
	if (NULL == m_CEntityOperationsAreaDlg) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "EntityOperationsAreaDlg is NULL");
		return;
	}

	RadioResource r = m_CEntityOperationsAreaDlg->getCurrentResource();
	r.type = RR_RADIO;
	PrivateCallState ptCallState = 
		The_RadioOperations::instance()->getResource_isInPrivateCall(r);

	switch (ptCallState) {
		case PC_ESTABLISHED:
		case PC_OUTGOING:
			The_RadioOperations::instance()->endPrivateCall(r);
			break;
		case PC_NOT_INVOLVED:
			The_RadioOperations::instance()->endDriverCall(m_CEntityOperationsAreaDlg->getCurrentResource());
			break;
	}

    FUNCTION_EXIT;
}


void TrainEntityOperationsAreaStrategy::sendText()
{
    FUNCTION_ENTRY("sendText");

    FUNCTION_EXIT;
}

void TrainEntityOperationsAreaStrategy::ambienceListening()
{
    FUNCTION_ENTRY("ambienceListening");

    FUNCTION_EXIT;
}


void TrainEntityOperationsAreaStrategy::adjustVolume()
{
	FUNCTION_ENTRY("adjustVolume");

	FUNCTION_EXIT;
}

void TrainEntityOperationsAreaStrategy::patch()
{
	FUNCTION_ENTRY("patch");

	FUNCTION_EXIT;
}


void TrainEntityOperationsAreaStrategy::checkControlRights()
{
	if (NULL == m_CEntityOperationsAreaDlg) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "EntityOperationsAreaDlg is NULL");
		return;
	}

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

void TrainEntityOperationsAreaStrategy::UpdateControlState()
{
	if (NULL == m_CEntityOperationsAreaDlg) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "EntityOperationsAreaDlg is NULL");
		return;
	}

	RadioResource r = m_CEntityOperationsAreaDlg->getCurrentResource();
	r.type = RR_RADIO;
	PrivateCallState ptCallState = 
		The_RadioOperations::instance()->getResource_isInPrivateCall(r);

	if (!The_RadioOperations::instance()->isDriverCall(r.ssi)) {
		ptCallState = PC_INCOMING;
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
			"RadioEntityOperationStrategy %u",ptCallState);
	switch(ptCallState)
	{
		case PC_ESTABLISHED:
			m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_endCallButton.EnableWindow(TRUE);
			break;
		case PC_OUTGOING:
			m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_endCallButton.EnableWindow(TRUE);
			break;
		case PC_NOT_INVOLVED:
			m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(TRUE);
			m_CEntityOperationsAreaDlg->m_endCallButton.EnableWindow(TRUE);
			break;
		default:
			m_CEntityOperationsAreaDlg->m_callButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_answerButton.EnableWindow(FALSE);
			m_CEntityOperationsAreaDlg->m_endCallButton.EnableWindow(FALSE);
			break;
	}

	// TD18119
	//bool isAmbListen = The_RadioOperations::instance()->getResource_isInAmbienceListeningCall(m_CEntityOperationsAreaDlg->m_RadioResource);
	//m_CEntityOperationsAreaDlg->m_ambienceButton.SetCheck(isAmbListen ? 1 : 0);
}
