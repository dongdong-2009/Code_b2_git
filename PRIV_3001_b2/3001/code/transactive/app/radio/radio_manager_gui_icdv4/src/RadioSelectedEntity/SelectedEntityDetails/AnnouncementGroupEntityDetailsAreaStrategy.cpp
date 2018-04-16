/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/AnnouncementGroupEntityDetailsAreaStrategy.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Constructs the appropriate IEntityDetailsAreaStrategy type.
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/AnnouncementGroupEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"

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

AnnouncementGroupEntityDetailsAreaStrategy::AnnouncementGroupEntityDetailsAreaStrategy()
{
	m_CEntityDetailsAreaDlg = NULL;
}

AnnouncementGroupEntityDetailsAreaStrategy::~AnnouncementGroupEntityDetailsAreaStrategy()
{

}

void AnnouncementGroupEntityDetailsAreaStrategy::initialise(CEntityDetailsAreaDlg* pentityDetailsArea)
{
    FUNCTION_ENTRY("initialise");
    
	TA_ASSERT(pentityDetailsArea != NULL, "pentityDetailsArea is NULL");

	m_CEntityDetailsAreaDlg = pentityDetailsArea;

	// show controls
	//m_CEntityDetailsAreaDlg->m_staticText.SetWindowText("Talkgroups");
	//m_CEntityDetailsAreaDlg->m_listLB.ShowWindow(SW_SHOW);
//TD 18872
	m_CEntityDetailsAreaDlg->m_staticText.SetWindowText("");
	m_CEntityDetailsAreaDlg->m_listLB.ShowWindow(SW_HIDE);
//TD 18872	
	m_CEntityDetailsAreaDlg->m_edit.ShowWindow(SW_HIDE);
	m_CEntityDetailsAreaDlg->m_delete.ShowWindow(SW_HIDE);
	//m_CEntityDetailsAreaDlg->m_deleteMember.ShowWindow(SW_HIDE);
	m_CEntityDetailsAreaDlg->m_save.ShowWindow(SW_HIDE);

	m_CEntityDetailsAreaDlg->m_cancel.ShowWindow(SW_HIDE);

	// display resource data in edit controls
	m_CEntityDetailsAreaDlg->refresh(IDS_RR_ANNGROUP);
	
	m_CEntityDetailsAreaDlg->m_listLB.EnableWindow(FALSE);

	FUNCTION_EXIT;
}


