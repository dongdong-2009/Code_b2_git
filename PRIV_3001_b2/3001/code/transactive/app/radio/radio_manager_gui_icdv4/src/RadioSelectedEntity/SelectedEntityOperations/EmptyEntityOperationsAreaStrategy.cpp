/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/EmptyEntityOperationsAreaStrategy.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"

#include "app/radio/radio_manager_gui_icdv4/src/resource.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityOperations/EmptyEntityOperationsAreaStrategy.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EmptyEntityOperationsAreaStrategy::EmptyEntityOperationsAreaStrategy()
{

}

EmptyEntityOperationsAreaStrategy::~EmptyEntityOperationsAreaStrategy()
{

}

void EmptyEntityOperationsAreaStrategy::initialise(CEntityOperationsAreaDlg* pEntityOperationsDlg)
{
	FUNCTION_ENTRY("initialise");

	m_CEntityOperationsAreaDlg = pEntityOperationsDlg;

	//Hide controls
	
	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_NEW_PATCH)->ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_ORIGINAL_PATCH)->ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->GetDlgItem(IDC_DUMMY_SEND_TEXT)->ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_callButton.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_selectedSpeakerButton.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_answerButton.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_endCallButton.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_sendTextButton.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_patchButton.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_ambienceButton.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_connectButton.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_releaseButton.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_volumeSlider.ShowWindow(SW_HIDE);

	m_CEntityOperationsAreaDlg->m_staticText.ShowWindow(SW_HIDE);	

	FUNCTION_EXIT;
}


void EmptyEntityOperationsAreaStrategy::call()
{

}

void EmptyEntityOperationsAreaStrategy::answer()
{

}

void EmptyEntityOperationsAreaStrategy::endCall()
{

}

void EmptyEntityOperationsAreaStrategy::sendText()
{

}

void EmptyEntityOperationsAreaStrategy::patch()
{

}

void EmptyEntityOperationsAreaStrategy::ambienceListening()
{

}

void EmptyEntityOperationsAreaStrategy::selectSpeaker()
{

}

void EmptyEntityOperationsAreaStrategy::unselectSpeaker()
{

}

void EmptyEntityOperationsAreaStrategy::adjustVolume()
{

}

void EmptyEntityOperationsAreaStrategy::connect()
{

}

void EmptyEntityOperationsAreaStrategy::release()
{

}
