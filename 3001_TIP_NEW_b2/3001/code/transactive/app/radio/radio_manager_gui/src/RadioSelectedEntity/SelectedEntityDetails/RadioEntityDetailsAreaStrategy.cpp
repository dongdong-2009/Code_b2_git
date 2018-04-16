/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/RadioEntityDetailsAreaStrategy.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioManager.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/RadioEntityDetailsAreaStrategy.h"

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

RadioEntityDetailsAreaStrategy::RadioEntityDetailsAreaStrategy()
{
	m_CEntityDetailsAreaDlg = NULL;
}

RadioEntityDetailsAreaStrategy::~RadioEntityDetailsAreaStrategy()
{

}


void RadioEntityDetailsAreaStrategy::initialise(CEntityDetailsAreaDlg* pentityDetailsArea)
{
    FUNCTION_ENTRY("initialise");
    
	TA_ASSERT(pentityDetailsArea != NULL, "pentityDetailsArea is NULL");
	
	m_CEntityDetailsAreaDlg = pentityDetailsArea;
	
	// hide controls
	m_CEntityDetailsAreaDlg->m_staticText.SetWindowText("");
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
	m_CEntityDetailsAreaDlg->refresh(IDS_RR_RADIO);

	// clear previously selected resource's members 
	m_CEntityDetailsAreaDlg->m_members.clear();

	FUNCTION_EXIT;

}


