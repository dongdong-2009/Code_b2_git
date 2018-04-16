/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/TalkGroupEntityDetailsAreaStrategy.cpp $
 * @author:  Alexis Laredo
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioManager.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/TalkGroupEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TalkGroupEntityDetailsAreaStrategy::TalkGroupEntityDetailsAreaStrategy()
{
	m_CEntityDetailsAreaDlg = NULL;
}

TalkGroupEntityDetailsAreaStrategy::~TalkGroupEntityDetailsAreaStrategy()
{

}


void TalkGroupEntityDetailsAreaStrategy::initialise(CEntityDetailsAreaDlg* pentityDetailsArea)
{
    FUNCTION_ENTRY("initialise");
    
	TA_ASSERT(pentityDetailsArea != NULL, "pentityDetailsArea is NULL");
	
	m_CEntityDetailsAreaDlg = pentityDetailsArea;

	// show/hide controls
	//m_CEntityDetailsAreaDlg->m_staticText.SetWindowText("Radio Users");
	m_CEntityDetailsAreaDlg->m_staticText.SetWindowText("");
	//m_CEntityDetailsAreaDlg->m_listLB.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_listLB.ShowWindow(SW_HIDE);
	m_CEntityDetailsAreaDlg->m_edit.ShowWindow(SW_HIDE);
	m_CEntityDetailsAreaDlg->m_delete.ShowWindow(SW_HIDE);
	//m_CEntityDetailsAreaDlg->m_deleteMember.ShowWindow(SW_HIDE);
	m_CEntityDetailsAreaDlg->m_save.ShowWindow(SW_HIDE);
	m_CEntityDetailsAreaDlg->m_cancel.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_staticID.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_staticID.EnableWindow(true);
	m_CEntityDetailsAreaDlg->m_idEL.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_idEL.EnableWindow(true);

	// display resource data in edit controls
	m_CEntityDetailsAreaDlg->refresh(IDS_RR_TALKGROUP);

	m_CEntityDetailsAreaDlg->m_listLB.EnableWindow(FALSE);

	FUNCTION_EXIT;
}


