/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/MultiSiteEntityDetailsAreaStrategy.cpp $
  * @author:  F. Stuart
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */


#include "app/radio/radio_manager_gui_icdv4/src/StdAfx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/CRadioManagerDlg.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/MultiSiteEntityDetailsAreaStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/utilities/src/TAAssert.h"
#include <sstream>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MultiSiteEntityDetailsAreaStrategy::MultiSiteEntityDetailsAreaStrategy()
{
	m_CEntityDetailsAreaDlg = NULL;
}

MultiSiteEntityDetailsAreaStrategy::~MultiSiteEntityDetailsAreaStrategy()
{

}


void MultiSiteEntityDetailsAreaStrategy::initialise(CEntityDetailsAreaDlg* pentityDetailsArea)
{
	TA_ASSERT(pentityDetailsArea != NULL, "pentityDetailsArea is NULL");

	m_CEntityDetailsAreaDlg = pentityDetailsArea;


	// show controls
	m_CEntityDetailsAreaDlg->m_staticText.SetWindowText("Sites");
	m_CEntityDetailsAreaDlg->m_listLB.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_edit.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_delete.ShowWindow(SW_SHOW);
	//m_CEntityDetailsAreaDlg->m_deleteMember.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_save.ShowWindow(SW_SHOW);
	m_CEntityDetailsAreaDlg->m_cancel.ShowWindow(SW_SHOW);
	
	// display resource data in edit controls
	m_CEntityDetailsAreaDlg->refresh(IDS_RR_MSITE);

	//check if operator has rights to configure this resource
	CRadioManagerDlg* pMainWnd = dynamic_cast<CRadioManagerDlg*>(AfxGetMainWnd());    
	ASSERT_VALID(pMainWnd);
	m_CEntityDetailsAreaDlg->m_bEditRights = pMainWnd->checkRights(TA_Base_Bus::aca_RADIO_MULTISITE);

	
	// This will be enabled on Edit Mode
	m_CEntityDetailsAreaDlg->enableControl(FALSE);
	
}


void MultiSiteEntityDetailsAreaStrategy::deleteResource()
{
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	The_RadioOperations::instance()->deleteEntity(m_CEntityDetailsAreaDlg->m_RadioResource);

	m_CEntityDetailsAreaDlg->enableControl(FALSE);
}


void MultiSiteEntityDetailsAreaStrategy::edit()
{
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	m_CEntityDetailsAreaDlg->enableControl(TRUE);
	//TD18693 marc_bugfix, make the ID edit box read only
	m_CEntityDetailsAreaDlg->m_idEL.SetReadOnly(true);

	
	The_RadioOperations::instance()->editMode(m_CEntityDetailsAreaDlg->m_RadioResource, false);
}


void MultiSiteEntityDetailsAreaStrategy::save()
{
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	// Store previous resource data
	RadioResource editResource = m_CEntityDetailsAreaDlg->m_RadioResource;
	
	// set m_RadioResource to input data; only alias & members are editable
	CString data;
	m_CEntityDetailsAreaDlg->m_descriptionEL.GetWindowText(data);
	editResource.alias = data;

	if ( The_RadioOperations::instance()->saveEntity(editResource,m_dropMembers) )
	{
		m_CEntityDetailsAreaDlg->m_RadioResource = editResource;

		m_CEntityDetailsAreaDlg->m_members = m_dropMembers;
		m_dropMembers.clear();
	}
	else
	{
		MessageBox( NULL, "Saving of resource failed", "MultiSiteEntityDetailsAreaStrategy", MB_OK|MB_ICONERROR );
	}

	m_CEntityDetailsAreaDlg->enableControl(FALSE);
}


void MultiSiteEntityDetailsAreaStrategy::cancel()
{
	TA_ASSERT(m_CEntityDetailsAreaDlg != NULL, "m_CEntityDetailsAreaDlg is NULL");

	The_RadioOperations::instance()->operationMode();

	m_CEntityDetailsAreaDlg->enableControl(FALSE);

	m_dropMembers.clear();
	
	m_CEntityDetailsAreaDlg->refresh(IDS_RR_MSITE,false);
}


void MultiSiteEntityDetailsAreaStrategy::OnDropFiles(RadioResource& resource)
{
	if ( resource.type == RR_SITE )
	{
		m_dropMembers.push_back(resource);
	}
	else
	{
		MessageBox( NULL, "Cannot add resource to MultiSite", "MultiSiteEntityDetailsAreaStrategy", MB_OK|MB_ICONERROR );
	}
}
