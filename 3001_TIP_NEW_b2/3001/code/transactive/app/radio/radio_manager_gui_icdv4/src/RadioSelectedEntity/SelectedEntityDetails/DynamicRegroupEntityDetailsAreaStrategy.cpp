/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/DynamicRegroupEntityDetailsAreaStrategy.cpp $
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
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/DynamicRegroupEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "app/radio/radio_manager_gui_icdv4/src/CRadioManagerDlg.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <list>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DynamicRegroupEntityDetailsAreaStrategy::DynamicRegroupEntityDetailsAreaStrategy()
{
	m_CEntityDetailsAreaDlg = NULL;
}

DynamicRegroupEntityDetailsAreaStrategy::~DynamicRegroupEntityDetailsAreaStrategy()
{

}


void DynamicRegroupEntityDetailsAreaStrategy::initialise(CEntityDetailsAreaDlg* pEntityDetailsArea)
{
    FUNCTION_ENTRY("initialise");
    
	TA_ASSERT(pEntityDetailsArea != NULL, "pEntityDetailsArea is NULL");

	m_CEntityDetailsAreaDlg = pEntityDetailsArea;

	// show controls
	m_CEntityDetailsAreaDlg->m_staticText.SetWindowText("DG Talkgroup");
	m_CEntityDetailsAreaDlg->m_listLB.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_listLB.setReadOnly(true);
	m_CEntityDetailsAreaDlg->m_edit.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_delete.ShowWindow(SW_SHOW);
	//m_CEntityDetailsAreaDlg->m_deleteMember.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_save.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_cancel.ShowWindow(SW_SHOW);
	
	//++Fix TD#14320
	m_CEntityDetailsAreaDlg->m_dgText.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_dgTalkgroupEL.ShowWindow(SW_SHOW);

	int xPos = m_CEntityDetailsAreaDlg->m_listRect.left;
	int yPos = m_CEntityDetailsAreaDlg->m_listRect.top;
	
	CRect dummyRect = m_CEntityDetailsAreaDlg->m_editRect;

	m_CEntityDetailsAreaDlg->m_listLB.MoveWindow(xPos,yPos+dummyRect.Height()+1,m_CEntityDetailsAreaDlg->m_listRect.Width(),m_CEntityDetailsAreaDlg->m_listRect.Height());

	m_CEntityDetailsAreaDlg->m_dgTalkgroupEL.MoveWindow(xPos,yPos,dummyRect.Width(),dummyRect.Height());

	if (The_RadioOperations::instance()->getResource_isGroupConnected(m_CEntityDetailsAreaDlg->m_RadioResource) == CONNECTED)
	{
		RadioResource dgTalkgroup = The_RadioSystemStatus::instance()->getDGTalkgroup(m_CEntityDetailsAreaDlg->m_RadioResource);

		m_CEntityDetailsAreaDlg->m_dgTalkgroupEL.SetWindowText(dgTalkgroup.alias.c_str());
	}
	else
	{
		m_CEntityDetailsAreaDlg->m_dgTalkgroupEL.SetWindowText("");
	}

	xPos = m_CEntityDetailsAreaDlg->m_EditButtonRect.left;
	yPos = m_CEntityDetailsAreaDlg->m_EditButtonRect.top;
	m_CEntityDetailsAreaDlg->m_edit.MoveWindow(xPos,yPos+dummyRect.Height()+1,m_CEntityDetailsAreaDlg->m_EditButtonRect.Width(),m_CEntityDetailsAreaDlg->m_EditButtonRect.Height());	

	xPos = m_CEntityDetailsAreaDlg->m_SaveButtonRect.left;
	yPos = m_CEntityDetailsAreaDlg->m_SaveButtonRect.top;
	m_CEntityDetailsAreaDlg->m_save.MoveWindow(xPos,yPos+dummyRect.Height()+1,m_CEntityDetailsAreaDlg->m_SaveButtonRect.Width(),m_CEntityDetailsAreaDlg->m_SaveButtonRect.Height());	


	xPos = m_CEntityDetailsAreaDlg->m_DeleteButtonRect.left;
	yPos = m_CEntityDetailsAreaDlg->m_DeleteButtonRect.top;
	m_CEntityDetailsAreaDlg->m_delete.MoveWindow(xPos,yPos+dummyRect.Height()+1,m_CEntityDetailsAreaDlg->m_DeleteButtonRect.Width(),m_CEntityDetailsAreaDlg->m_DeleteButtonRect.Height());	

	//xPos = m_CEntityDetailsAreaDlg->m_DeleteMemberButtonRect.left;
	//yPos = m_CEntityDetailsAreaDlg->m_DeleteMemberButtonRect.top;
	//m_CEntityDetailsAreaDlg->m_deleteMember.MoveWindow(xPos,yPos+dummyRect.Height()+1,m_CEntityDetailsAreaDlg->m_DeleteMemberButtonRect.Width(),m_CEntityDetailsAreaDlg->m_DeleteMemberButtonRect.Height());	

	xPos = m_CEntityDetailsAreaDlg->m_CancelButtonRect.left;
	yPos = m_CEntityDetailsAreaDlg->m_CancelButtonRect.top;
	m_CEntityDetailsAreaDlg->m_cancel.MoveWindow(xPos,yPos+dummyRect.Height()+1,m_CEntityDetailsAreaDlg->m_CancelButtonRect.Width(),m_CEntityDetailsAreaDlg->m_CancelButtonRect.Height());	



	//--Fix TD#14320
	// display resource data in edit controls
	m_CEntityDetailsAreaDlg->refresh(IDS_RR_DYNGROUP);

	//check if operator has rights to configure this resource
	CRadioManagerDlg* pMainWnd = dynamic_cast<CRadioManagerDlg*>(AfxGetMainWnd());    
	ASSERT_VALID(pMainWnd);
	m_CEntityDetailsAreaDlg->m_bEditRights = pMainWnd->checkRights(TA_Base_Bus::aca_RADIO_DYNAMICREGROUP);


	// Enable/Disable controls on Edit Mode
	m_CEntityDetailsAreaDlg->enableControl(FALSE);

	m_CEntityDetailsAreaDlg->m_descriptionEL.SetLimitText(12);

	FUNCTION_EXIT;
}


void DynamicRegroupEntityDetailsAreaStrategy::deleteResource()
{
    FUNCTION_ENTRY("deleteResource");
    
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	The_RadioOperations::instance()->deleteEntity(m_CEntityDetailsAreaDlg->m_RadioResource);

	FUNCTION_EXIT;
}


void DynamicRegroupEntityDetailsAreaStrategy::edit()
{
    FUNCTION_ENTRY("edit");
    
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	m_CEntityDetailsAreaDlg->enableControl(TRUE);
	//TD18693 marc_bugfix, make the ID edit box read only
	m_CEntityDetailsAreaDlg->m_idEL.SetReadOnly(true);
	The_RadioOperations::instance()->editMode(m_CEntityDetailsAreaDlg->m_RadioResource,false);

	FUNCTION_EXIT;
}


void DynamicRegroupEntityDetailsAreaStrategy::save()
{
    FUNCTION_ENTRY("save");
    
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	// Store previous resource data
	RadioResource editResource = m_CEntityDetailsAreaDlg->m_RadioResource;
	
	// set m_RadioResource to input data; only alias & members are editable
	CString data;
	m_CEntityDetailsAreaDlg->m_descriptionEL.GetWindowText(data);
	editResource.alias = data;

	// update Talkgroup Assigned to DGNA in m_dropMembers
	CString alias;
	m_CEntityDetailsAreaDlg->m_dgTalkgroupEL.GetWindowText(alias);
	if (!alias.IsEmpty()) {
		std::vector<RadioResource>::iterator iter = m_dropMembers.begin();
		for(; iter != m_dropMembers.end(); iter++)
		{
			if ((*iter).type == RR_TALKGROUP) break;
		}

		std::vector<RadioResource> resourceList = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_TALKGROUP);
		std::vector<RadioResource>::iterator iterR = resourceList.begin();
		for ( ; iterR != resourceList.end(); ++iterR)
		{
			if ((*iterR).alias.compare(alias) == 0) break;
		}

		if (iter == m_dropMembers.end()) {
			m_dropMembers.push_back(*iterR);
		} else {
			(*iter) = *iterR;
		}
	}


	if ( The_RadioOperations::instance()->saveEntity(editResource,m_dropMembers) )
	{
		m_CEntityDetailsAreaDlg->m_RadioResource = editResource;

		m_CEntityDetailsAreaDlg->m_members = m_dropMembers;
		m_dropMembers.clear();
	}
	else
	{
		MessageBox( NULL, "Saving of resource failed", "DynamicRegroupEntityDetailsAreaStrategy", MB_OK|MB_ICONERROR );
		The_RadioOperations::instance()->m_observerHelper.notifyChildWindows(WM_OPERATION_MODE);
	}

	m_CEntityDetailsAreaDlg->enableControl(FALSE);

	FUNCTION_EXIT;
}


void DynamicRegroupEntityDetailsAreaStrategy::cancel()
{
    FUNCTION_ENTRY("cancel");
    
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	The_RadioOperations::instance()->operationMode();

	m_CEntityDetailsAreaDlg->enableControl(FALSE);

	m_dropMembers.clear();

	m_CEntityDetailsAreaDlg->refresh(IDS_RR_DYNGROUP,false);

	FUNCTION_EXIT;
}


void DynamicRegroupEntityDetailsAreaStrategy::OnDropFiles(RadioResource& resource)
{
    FUNCTION_ENTRY("OnDropFiles");
    
	if ( resource.type == RR_RADIO )
	{
		m_dropMembers.push_back(resource);
	}
	else
	{
		MessageBox( NULL, "Cannot add resource to Dynamic Regroup", "DynamicRegroupEntityDetailsAreaStrategy", MB_OK|MB_ICONERROR );
	}

	FUNCTION_EXIT;
}
