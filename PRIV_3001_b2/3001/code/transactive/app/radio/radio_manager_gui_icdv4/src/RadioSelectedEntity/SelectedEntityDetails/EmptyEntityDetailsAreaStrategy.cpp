/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/EmptyEntityDetailsAreaStrategy.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */


#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/EmptyEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


EmptyEntityDetailsAreaStrategy::EmptyEntityDetailsAreaStrategy()
{
	m_CEntityDetailsAreaDlg = NULL;
}


EmptyEntityDetailsAreaStrategy::~EmptyEntityDetailsAreaStrategy()
{

}


void EmptyEntityDetailsAreaStrategy::initialise(CEntityDetailsAreaDlg* pEntityDetailsArea)
{
    FUNCTION_ENTRY("initialise");
    
	TA_ASSERT(pEntityDetailsArea != NULL, "pEntityDetailsArea is NULL");

	m_CEntityDetailsAreaDlg = pEntityDetailsArea;

	m_CEntityDetailsAreaDlg->m_staticBlank.SetWindowText("Please select an entity from the directory.");
	m_CEntityDetailsAreaDlg->m_staticBlank.ShowWindow(SW_SHOW);

	// hide all controls
	m_CEntityDetailsAreaDlg->m_staticText.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_staticType.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_staticID.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_staticDesc.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_listLB.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_edit.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_delete.ShowWindow(SW_HIDE);

	//m_CEntityDetailsAreaDlg->m_deleteMember.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_save.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_cancel.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_typeEL.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_idEL.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_descriptionEL.ShowWindow(SW_HIDE);	

	// clear members(if there's any) of previously selected resource
	m_CEntityDetailsAreaDlg->m_members.clear();

	FUNCTION_EXIT;
}

