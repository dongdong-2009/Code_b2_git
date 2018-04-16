/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/MultiselectEntityDetailsAreaStrategy.cpp $
  * @author:  F. Stuart
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */


#include "app/radio/radio_manager_gui/src/StdAfx.h"
#include "app/radio/radio_manager_gui/src/RadioManager.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/MultiselectEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "app/radio/radio_manager_gui/src/CRadioManagerDlg.h"

#include "core/utilities/src/TAAssert.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MultiselectEntityDetailsAreaStrategy::MultiselectEntityDetailsAreaStrategy()
{
	m_CEntityDetailsAreaDlg = NULL;
}

MultiselectEntityDetailsAreaStrategy::~MultiselectEntityDetailsAreaStrategy()
{

}


void MultiselectEntityDetailsAreaStrategy::initialise(CEntityDetailsAreaDlg* pEntityDetailsArea)
{
    FUNCTION_ENTRY("initialise");
    
	TA_ASSERT(pEntityDetailsArea != NULL, "pEntityDetailsArea is NULL");

	m_CEntityDetailsAreaDlg = pEntityDetailsArea;

	// show controls
	m_CEntityDetailsAreaDlg->m_staticText.SetWindowText("Members");
	m_CEntityDetailsAreaDlg->m_listLB.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_listLB.setReadOnly(true);
	m_CEntityDetailsAreaDlg->m_listLB.EnableWindow(true);
	m_CEntityDetailsAreaDlg->m_edit.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_delete.ShowWindow(SW_SHOW);
	//m_CEntityDetailsAreaDlg->m_deleteMember.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_save.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_cancel.ShowWindow(SW_SHOW);	
	
	m_CEntityDetailsAreaDlg->m_staticID.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_staticID.EnableWindow(true);
	m_CEntityDetailsAreaDlg->m_idEL.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_idEL.EnableWindow(true);

	// display resource data in edit controls
	m_CEntityDetailsAreaDlg->refresh(IDS_RR_MSELECT);

	
	//check if operator has rights to configure this resource
	CRadioManagerDlg* pMainWnd = dynamic_cast<CRadioManagerDlg*>(AfxGetMainWnd());    
	ASSERT_VALID(pMainWnd);
	m_CEntityDetailsAreaDlg->m_bEditRights = pMainWnd->checkRights(TA_Base_Bus::aca_RADIO_MULTISELECT);
	
	// Enable/Disable on Edit Mode
	m_CEntityDetailsAreaDlg->enableControl(FALSE);

	m_CEntityDetailsAreaDlg->m_descriptionEL.SetLimitText(12);

	FUNCTION_EXIT;	
}


void MultiselectEntityDetailsAreaStrategy::deleteResource()
{
    FUNCTION_ENTRY("deleteResource");
    
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	The_RadioOperations::instance()->deleteEntity(m_CEntityDetailsAreaDlg->m_RadioResource);

	m_CEntityDetailsAreaDlg->enableControl(FALSE);

	FUNCTION_EXIT;
}


void MultiselectEntityDetailsAreaStrategy::edit()
{
    FUNCTION_ENTRY("edit");
    
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	m_CEntityDetailsAreaDlg->enableControl(TRUE);
	The_RadioOperations::instance()->editMode(m_CEntityDetailsAreaDlg->m_RadioResource, false);
	//TD18693 marc_bugfix, make the ID edit box read only
	m_CEntityDetailsAreaDlg->m_idEL.SetReadOnly(true);

	setFlagIfNewEntry(m_CEntityDetailsAreaDlg->m_RadioResource);

	FUNCTION_EXIT;
}


void MultiselectEntityDetailsAreaStrategy::save()
{
    FUNCTION_ENTRY("save");

	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	// Store previous resource data
	RadioResource editResource = m_CEntityDetailsAreaDlg->m_RadioResource;
	
	// set m_RadioResource to input data; only alias & members are editable
	CString data;
	m_CEntityDetailsAreaDlg->m_descriptionEL.GetWindowText(data);
	editResource.alias = data;

	if (isDescriptionAlreadyExist(data.GetBuffer(0),RR_MSELECT))
	{
		char * pszData = new char [1023];
		memset(pszData, 0, 1023);
		sprintf(pszData,"Resource <%s> already on the list",data.GetBuffer(0));
		::MessageBox(NULL, pszData, "Saving Error", MB_OK|MB_ICONERROR);
		delete [] pszData;

		m_CEntityDetailsAreaDlg->refresh(IDS_RR_DYNGROUP);
		m_CEntityDetailsAreaDlg->enableControl(FALSE);
		The_RadioOperations::instance()->m_observerHelper.notifyChildWindows(WM_OPERATION_MODE);

		return;
	}

	if ( The_RadioOperations::instance()->saveEntity(editResource,m_dropMembers) )
	{
		m_CEntityDetailsAreaDlg->m_RadioResource = editResource;

		m_CEntityDetailsAreaDlg->m_members = m_dropMembers;
		m_dropMembers.clear();
	}
	else
	{
		MessageBox( NULL, "Saving of resource failed", "MultiselectEntityDetailsAreaStrategy", MB_OK|MB_ICONERROR );

		The_RadioOperations::instance()->m_observerHelper.notifyChildWindows(WM_OPERATION_MODE);
		The_RadioOperations::instance()->m_observerHelper.notifyChildWindows(WM_ENTITY_EMPTY_SELECTION);
	}

	m_CEntityDetailsAreaDlg->enableControl(FALSE);

	FUNCTION_EXIT;
}


void MultiselectEntityDetailsAreaStrategy::cancel()
{
    FUNCTION_ENTRY("cancel");
    
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	The_RadioOperations::instance()->operationMode();

	m_CEntityDetailsAreaDlg->enableControl(FALSE);

	m_dropMembers.clear();
	
	m_CEntityDetailsAreaDlg->refresh(IDS_RR_MSELECT,false);

	FUNCTION_EXIT;
}


void MultiselectEntityDetailsAreaStrategy::OnDropFiles(RadioResource& resource)
{
    FUNCTION_ENTRY("OnDropFiles");
    
	if ( resource.type == RR_TALKGROUP )
	{
		m_dropMembers.push_back(resource);
	}
	else
	{
		MessageBox( NULL, "Cannot add resource to MultiSelect", "MultiselectEntityDetailsAreaStrategy", MB_OK|MB_ICONERROR );
	}

	FUNCTION_EXIT;
}
