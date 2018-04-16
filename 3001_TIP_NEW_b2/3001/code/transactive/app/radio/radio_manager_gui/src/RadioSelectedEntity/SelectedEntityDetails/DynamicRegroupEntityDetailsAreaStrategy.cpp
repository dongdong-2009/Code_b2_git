/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/DynamicRegroupEntityDetailsAreaStrategy.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Constructs the appropriate IEntityDetailsAreaStrategy type.
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioManager.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/DynamicRegroupEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "app/radio/radio_manager_gui/src/CRadioManagerDlg.h"

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
	m_CEntityDetailsAreaDlg->m_listLB.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_idEL.ShowWindow(SW_HIDE);
	m_CEntityDetailsAreaDlg->m_idEL.EnableWindow(false);
	m_CEntityDetailsAreaDlg->m_listLB.setReadOnly(true);
	m_CEntityDetailsAreaDlg->m_listLB.EnableWindow(true);
	m_CEntityDetailsAreaDlg->m_edit.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_delete.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_save.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_cancel.ShowWindow(SW_SHOW);
	
	//++Fix TD#14320
	m_CEntityDetailsAreaDlg->m_dgText.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_dgTalkgroupEL.ShowWindow(SW_SHOW);

	int xPos = m_CEntityDetailsAreaDlg->m_listRect.left;
	int yPos = m_CEntityDetailsAreaDlg->m_listRect.top;
	
	CRect dummyRect = m_CEntityDetailsAreaDlg->m_editRect;

	if (The_RadioOperations::instance()->getResource_isGroupConnected(m_CEntityDetailsAreaDlg->m_RadioResource) == CONNECTED)
	{
		RadioResource dgTalkgroup = The_RadioSystemStatus::instance()->getDGTalkgroup(m_CEntityDetailsAreaDlg->m_RadioResource);

		m_CEntityDetailsAreaDlg->m_dgTalkgroupEL.SetWindowText(dgTalkgroup.alias.c_str());
	}
	else
	{
		m_CEntityDetailsAreaDlg->m_dgTalkgroupEL.SetWindowText("");
	}

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

	
	RadioResource editResource = m_CEntityDetailsAreaDlg->m_RadioResource;
	std::vector<RadioResource> members = The_RadioSystemStatus::instance()->getResourceMembers(m_CEntityDetailsAreaDlg->m_RadioResource);
	RadioResource dgTalkgroup;

	// remove members from Talkgroup by calling RadioAgent
	CString alias;
	m_CEntityDetailsAreaDlg->m_dgTalkgroupEL.GetWindowText(alias);
	if (!alias.IsEmpty()) {
		std::vector<RadioResource> resourceList = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_TALKGROUP);
		std::vector<RadioResource>::iterator iterR = resourceList.begin();
		for ( ; iterR != resourceList.end(); ++iterR)
		{
			if ((*iterR).alias.compare(alias) == 0) break;
		}
		dgTalkgroup = *iterR;
	}
	std::vector<RadioResource>::iterator iter = members.begin();
	for(; iter!=members.end(); iter++)
	{		
		if (RR_TALKGROUP != (*iter).type) {
			bool status = The_RadioOperations::instance()->releaseDynamicRegroup(m_CEntityDetailsAreaDlg->m_RadioResource, dgTalkgroup.alias, (*iter).id);
			if (!status) {
				// roll back operation 
				The_RadioOperations::instance()->cancelDeleteDynGroup(m_CEntityDetailsAreaDlg->m_RadioResource, dgTalkgroup.alias, members, iter);
				return;
			}
		}
	}

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
	
	setFlagIfNewEntry(m_CEntityDetailsAreaDlg->m_RadioResource);

	FUNCTION_EXIT;
}


void DynamicRegroupEntityDetailsAreaStrategy::save()
{
    FUNCTION_ENTRY("save");
    
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	// Store previous resource data
	RadioResource editResource = m_CEntityDetailsAreaDlg->m_RadioResource;
	std::vector<RadioResource> oldMembers = The_RadioSystemStatus::instance()->getResourceMembers(m_CEntityDetailsAreaDlg->m_RadioResource);
	RadioResource dgTalkgroup;
	
	// set m_RadioResource to input data; only alias & members are editable
	CString data;
	m_CEntityDetailsAreaDlg->m_descriptionEL.GetWindowText(data);
	editResource.alias = data;
	
	if (isDescriptionAlreadyExist(data.GetBuffer(0),RR_DYNGROUP))
	{
		char * pszData = new char [1023];
		memset(pszData, 0, 1023);
		//sprintf(pszData,"Resource <%s> already on the list",data.GetBuffer(0));
		sprintf(pszData,"DGNA group has already exist.");
		::MessageBox(NULL, pszData, "Saving Error", MB_OK|MB_ICONERROR);
		delete [] pszData;

		m_CEntityDetailsAreaDlg->refresh(IDS_RR_DYNGROUP);
		m_CEntityDetailsAreaDlg->enableControl(FALSE);
		The_RadioOperations::instance()->m_observerHelper.notifyChildWindows(WM_OPERATION_MODE);

		return;
	}

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
		dgTalkgroup = *iterR;
	} else {
		::MessageBox(NULL, "Talkgroup is not assigned to DGNA", "Error connecting", MB_OK|MB_ICONERROR);
		The_RadioOperations::instance()->m_observerHelper.notifyChildWindows(WM_OPERATION_MODE);
		m_CEntityDetailsAreaDlg->enableControl(FALSE);
		return;
	}

	if (editResource.alias != "") {
		bool status = The_RadioOperations::instance()->connectDynamicRegroup(dgTalkgroup, oldMembers, m_dropMembers);
		if (!status) {		
			The_RadioOperations::instance()->m_observerHelper.notifyChildWindows(WM_OPERATION_MODE);
			m_CEntityDetailsAreaDlg->enableControl(FALSE);
			return;
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
