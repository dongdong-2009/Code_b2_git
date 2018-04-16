/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioNewEntityDialog/CEntityDialog.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioHelper.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioNewEntityDialog/CEntityDialog.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "core/utilities/src/DebugUtil.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEntityDialog dialog


CEntityDialog::CEntityDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CEntityDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEntityDialog)
	//}}AFX_DATA_INIT
}


void CEntityDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEntityDialog)
	DDX_Control(pDX, IDC_ENTITY_TYPE_CB, m_entityTypeCB);
	DDX_Control(pDX, IDC_DESCRIPTION, m_descriptionEL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEntityDialog, CDialog)
	//{{AFX_MSG_MAP(CEntityDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEntityDialog message handlers

BOOL CEntityDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_resourceTypelist = The_RadioOperations::instance()->getCreateableResourceTypes(); 

	std::vector<RadioResourceType>::const_iterator iter;

	CString sType;
	int nIndex = 0;
	for (iter=m_resourceTypelist.begin(); iter!=m_resourceTypelist.end(); ++iter)
	{
        sType = CRadioHelper::getTypeText(*iter);
		m_entityTypeCB.InsertString(nIndex,sType);
		nIndex++;
	}

	
	return TRUE;  
	              
}

void CEntityDialog::OnOK() 
{
    FUNCTION_ENTRY("OnOK");
	
	int nIndex = m_entityTypeCB.GetCurSel();

	if  (nIndex == CB_ERR)
    {
        AfxMessageBox("Please select a resource type.");
        m_entityTypeCB.SetFocus();
        return;
    }	

	// get resource type
    RadioResource newResource;
    newResource.id = 0;
    newResource.type = m_resourceTypelist[nIndex];

	// get resource description
	CString desc;
	m_descriptionEL.GetWindowText(desc);

	if ( desc.GetLength() > 0 )
	{
		newResource.alias = desc;
//TD18869
		if(bCheckifExisting(newResource))
		{
			AfxMessageBox("Please use a different name for the group resource.");
		    m_descriptionEL.SetFocus();
		    return; // cancel
		}
		if(!bsaveOnDatabase(newResource))
		{
			GetDlgItem(IDCANCEL)->SetFocus();
			return;
		}
//TD18869

		The_RadioOperations::instance()->newEntityBeingCreated(newResource);
	}
	else
	{
		AfxMessageBox("Description field is empty");
        m_descriptionEL.SetFocus();
        return; // cancel
	}
	
	CDialog::OnOK();


	FUNCTION_EXIT;
}


void CEntityDialog::OnCancel()
{
	CDialog::OnCancel();
}//TD18869

//TD18869
bool CEntityDialog::bCheckifExisting(RadioResource &resource)
{
	std::vector<RadioResource> resourceList = The_RadioSystemStatus::instance()->getResourceCollectioForType(resource.type);
	std::vector<RadioResource>::iterator iT;
	CString cResourceAlias(resource.alias.c_str());

	
	for(iT = resourceList.begin(); iT != resourceList.end(); iT++)
	{
		CString compareAlias(iT->alias.c_str());

		if (cResourceAlias.CompareNoCase(compareAlias) == 0)
		{
			return true;
		}
	}

	return false;
}

bool CEntityDialog::bsaveOnDatabase(RadioResource &resource)
{
	std::vector<RadioResource> members;

	members.clear();
	if ( The_RadioOperations::instance()->saveEntity(resource,members) )
	{
		return true;
	}
	else
	{
		AfxMessageBox("Unable to save group resource");
	}
	return false;
}

//TD18869
