/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/CEntityDetailsAreaDlg.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioManager.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/CEntityDetailsAreaDlg.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityDetails/EntityDetailsAreaStrategyFactory.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <algorithm>

CEntityDetailsAreaDlg::CEntityDetailsAreaDlg(CWnd* pParent)
	: CDialog(CEntityDetailsAreaDlg::IDD, pParent), 
	  m_pEntityDetailsAreaStrategy(NULL),
	  m_bEditMode(false)
{
	FUNCTION_ENTRY("CEntityDetailsAreaDlg");

	FUNCTION_EXIT;
}


CEntityDetailsAreaDlg::~CEntityDetailsAreaDlg()
{
	FUNCTION_ENTRY("~CEntityDetailsAreaDlg");

	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_BEING_CREATED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_SELECTION_CHANGED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_CONNECT_GROUP);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_EMPTY_SELECTION);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_DB_UPDATE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_DB_DELETE);

    delete m_pEntityDetailsAreaStrategy;
    m_pEntityDetailsAreaStrategy = NULL;

	FUNCTION_EXIT;
}


void CEntityDetailsAreaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CEntityDetailsAreaDlg)
	DDX_Control(pDX, IDC_STATIC_BLANK, m_staticBlank);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_staticType);
	DDX_Control(pDX, IDC_STATIC_ID, m_staticID);
	DDX_Control(pDX, IDC_STATIC_DESC, m_staticDesc);
	DDX_Control(pDX, IDC_STATIC_TEXT, m_staticText);
	DDX_Control(pDX, IDC_TYPE, m_typeEL);
	DDX_Control(pDX, IDC_LIST2, m_listLB);
	DDX_Control(pDX, IDC_ID, m_idEL);
	DDX_Control(pDX, IDC_DESCRIPTION, m_descriptionEL);
	DDX_Control(pDX, IDC_EDIT_BTN, m_edit);
	DDX_Control(pDX, IDC_SAVE_BTN, m_save);
	DDX_Control(pDX, IDC_DELETE_BTN, m_delete);
	DDX_Control(pDX, IDC_CANCEL_BTN, m_cancel);
	DDX_Control(pDX, IDC_DG_TALKGROUP,m_dgTalkgroupEL);
	DDX_Control(pDX, IDC_DG_TEXT,m_dgText);
	//DDX_Control(pDX, IDC_DELETE_MEMBER_BTN,m_deleteMember);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEntityDetailsAreaDlg, CDialog)
	//{{AFX_MSG_MAP(CEntityDetailsAreaDlg)
	ON_BN_CLICKED(IDC_DELETE_BTN, OnDelete)
	ON_BN_CLICKED(IDC_EDIT_BTN, OnEdit)
	ON_BN_CLICKED(IDC_SAVE_BTN, OnSave)
	ON_BN_CLICKED(IDC_CANCEL_BTN, OnCancel)
	ON_MESSAGE(WM_ENTITY_BEING_CREATED,onNotifiedEntityBeingCreated)
	ON_MESSAGE(WM_ENTITY_SELECTION_CHANGED,onNotifiedEntitySelectionChanged)
	ON_MESSAGE(WM_CONNECT_GROUP,onNotifiedConnectGroup)
	ON_MESSAGE(WM_ENTITY_EMPTY_SELECTION, onNotifiedEntityEmptySelection)
	ON_MESSAGE(WM_DB_UPDATE, onNotifiedDbUpdate)
	ON_MESSAGE(WM_DB_DELETE, onNotifiedDbDelete)
	ON_COMMAND(ID_DELETE_MENU, OnDeleteMenu)
	ON_COMMAND(IDC_DELETE_MEMBER_BTN, OnDeleteMenu)
	ON_WM_CONTEXTMENU()
	ON_WM_DROPFILES()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CEntityDetailsAreaDlg::OnInitDialog() 
{
	FUNCTION_ENTRY("OnInitDialog");

	CDialog::OnInitDialog();

	//Show some buttons & hide some buttons according to different radio resources;
	initialise();
	//++Fixed TD#14320
	m_listLB.GetWindowRect(m_listRect);
	this->ScreenToClient(&m_listRect);

	m_dgTalkgroupEL.GetWindowRect(m_editRect);
	this->ScreenToClient(&m_editRect);
	//--Fixed TD#14320

	m_edit.GetWindowRect(m_EditButtonRect);
	this->ScreenToClient(&m_EditButtonRect);

	m_save.GetWindowRect(m_SaveButtonRect);
	this->ScreenToClient(&m_SaveButtonRect);


	m_delete.GetWindowRect(m_DeleteButtonRect);
	this->ScreenToClient(&m_DeleteButtonRect);

	//m_deleteMember.GetWindowRect(m_DeleteMemberButtonRect);
	//this->ScreenToClient(&m_DeleteMemberButtonRect);

	m_cancel.GetWindowRect(m_CancelButtonRect);
	this->ScreenToClient(&m_CancelButtonRect);


	// Listen to the following two messages;
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_BEING_CREATED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_SELECTION_CHANGED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_CONNECT_GROUP);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_EMPTY_SELECTION);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DB_UPDATE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DB_DELETE);

	if (m_listLB.isReadOnly())
	{
		m_listLB.SetBkColor(GetSysColor(COLOR_3DFACE));
		m_listLB.SetTextBkColor(::GetSysColor(COLOR_3DFACE));
	}
	
	m_listLB.InsertDefColoumn();
	m_listLB.SetDefColWidth();

	FUNCTION_EXIT;
	
	return TRUE;  
}


void CEntityDetailsAreaDlg::initialise()
{
	FUNCTION_ENTRY("initialise");

	if ( m_pEntityDetailsAreaStrategy != NULL )
	{
		delete m_pEntityDetailsAreaStrategy;
		m_pEntityDetailsAreaStrategy = NULL;
	}

	//m_RadioResource should be dynamically updated;
	m_pEntityDetailsAreaStrategy = EntityDetailsAreaStrategyFactory::getEntityDetailsAreaStrategy( m_RadioResource.type );
	TA_ASSERT( m_pEntityDetailsAreaStrategy != NULL, "m_pEntityDetailsAreaStrategy is NULL");

	IEntityDetailsAreaStrategy* pEntityDetailsAreaStrategy = const_cast<IEntityDetailsAreaStrategy*>(m_pEntityDetailsAreaStrategy);

	//All cases use CEntityDetailsAreaDlg to do corresponding initialization;
	pEntityDetailsAreaStrategy->initialise(this);

	FUNCTION_EXIT
}


void CEntityDetailsAreaDlg::OnEdit() 
{
	FUNCTION_ENTRY("OnEdit");

	//The real implementation is done in detailed strategy;
	TA_ASSERT( m_pEntityDetailsAreaStrategy!= NULL, "m_pEntityDetailsAreaStrategy is NULL");

	if (m_RadioResource.id == 0 && The_RadioSystemStatus::instance()->getResourceCollectioForType(m_RadioResource.type).size() >= RadioSystemStatus::MAX_NUMBER_GROUP) {
		::MessageBox(this->m_hWnd, "Max number of groups (10) reached", "Warning Message", MB_OK|MB_ICONWARNING);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Max number of groups (10) reached");
		FUNCTION_EXIT;
		return;
	}

	IEntityDetailsAreaStrategy* pEntityDetailsStrategy = const_cast<IEntityDetailsAreaStrategy*>(m_pEntityDetailsAreaStrategy);

	pEntityDetailsStrategy->edit();

	// set flag  to Edit mode
	m_bEditMode = true;

	enableListCtrl(true);

	FUNCTION_EXIT;
}


void CEntityDetailsAreaDlg::OnDelete() 
{
	FUNCTION_ENTRY("OnDelete");

    std::ostringstream bufstr;

	bufstr<<m_RadioResource.alias << " is being monitored. It should be unmonitored first.";

	if(The_RadioOperations::instance()->getResource_isBeingMonitored(m_RadioResource))
	{
		MessageBox(bufstr.str().c_str(), "Cannot delete", MB_OK);
		return;
	}

	bufstr.str("");
    bufstr << "Delete " << m_RadioResource.alias << " ?";
        
    if (IDNO == MessageBox("Are you sure you wish to delete the group?",bufstr.str().c_str(), MB_YESNO))
    {
        return;
    }

//TD18869
	RadioResourceType resourceType = m_RadioResource.type;
//TD18869

	//The real implementation is done in detailed strategy;
	IEntityDetailsAreaStrategy* pEntityDetailsStrategy = const_cast<IEntityDetailsAreaStrategy*>(m_pEntityDetailsAreaStrategy);
	pEntityDetailsStrategy->deleteResource();

	// clear members list
	if ( m_members.size() > 0 )
	{
		m_members.clear();
	}

//TD18869
	The_RadioOperations::instance()->entityBeingDeleted(resourceType);
//TD18869

	m_bEditMode = false;

	FUNCTION_EXIT;
}


void CEntityDetailsAreaDlg::OnSave() 
{
	FUNCTION_ENTRY("OnSave");

	//The real implementation is done in detailed strategy;
	TA_ASSERT( m_pEntityDetailsAreaStrategy!= NULL, "m_pEntityDetailsAreaStrategy is NULL");

	IEntityDetailsAreaStrategy* pEntityDetailsStrategy = const_cast<IEntityDetailsAreaStrategy*>(m_pEntityDetailsAreaStrategy);

	// get newly dropped members in ListBox
	for( int i=0; i < m_listLB.GetCount(); i++ )
	{
		DWORD rData = m_listLB.GetItemData(i);

		//TD17061
		RadioResourceType memType = RR_TALKGROUP;
		if (m_RadioResource.type == RR_DYNGROUP) 
		{
			memType = RR_RADIO;
		}
		else if (m_RadioResource.type == RR_MSITE)
		{
			memType = RR_SITE;
		}
		//TD17061
		
		RadioResource resource = The_RadioSystemStatus::instance()->getResourceByRID(rData,memType);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[OnSave] %s", resource.alias.c_str());
		if (resource.type != RR_NIL)
		{
			pEntityDetailsStrategy->OnDropFiles(resource);
		}
	}

	// update resource and its members

	pEntityDetailsStrategy->save();

	// set flag  to Operation mode
	m_bEditMode = false;

	enableListCtrl(false);

	FUNCTION_EXIT;
}

void CEntityDetailsAreaDlg::OnCancel() 
{
	FUNCTION_ENTRY("OnCancel");

	//The real implementation is done in detailed strategy;
	TA_ASSERT( m_pEntityDetailsAreaStrategy!= NULL, "m_pEntityDetailsAreaStrategy is NULL");
	IEntityDetailsAreaStrategy* pEntityDetailsStrategy = const_cast<IEntityDetailsAreaStrategy*>(m_pEntityDetailsAreaStrategy);

	pEntityDetailsStrategy->cancel();

	// set flag  to Operation mode
	m_bEditMode = false;
	enableListCtrl(false);
	FUNCTION_EXIT;
}


void CEntityDetailsAreaDlg::OnDropFiles(HDROP hdrop)
{
	FUNCTION_ENTRY("OnDropFiles");

    DragFinish ( hdrop )	;

	FUNCTION_EXIT;
}


void CEntityDetailsAreaDlg::enableControl(BOOL flag)
{
	FUNCTION_ENTRY("enableControl");

	m_descriptionEL.SetReadOnly(!flag);
	m_idEL.SetReadOnly(!flag);
	m_listLB.setReadOnly(!flag);

	if (!m_bEditRights || CONNECTED == The_RadioOperations::instance()->getResource_isGroupConnected(m_RadioResource))
	{
		m_edit.EnableWindow(FALSE);
		m_edit.EnableWindow(FALSE);
	}
	else
	{
		m_edit.EnableWindow(!flag);
		m_delete.EnableWindow(!flag);
	}

	m_save.EnableWindow(flag);
	
	//m_deleteMember.EnableWindow(flag);
	m_cancel.EnableWindow(flag);

	FUNCTION_EXIT;
}


LRESULT CEntityDetailsAreaDlg::onNotifiedConnectGroup(WPARAM pRadioResource, LPARAM lParam)
{
	//TD18868
	if (CONNECTED == lParam)
	{
		m_edit.EnableWindow(FALSE);
		m_delete.EnableWindow(FALSE);
	}
	else
	{
		m_edit.EnableWindow();
		m_delete.EnableWindow();
	}
	//TD18868
	RadioResource* radioResource = (RadioResource*)pRadioResource;
	m_RadioResource.alias = radioResource->alias;
	m_RadioResource.id	  = radioResource->id;
	m_RadioResource.type  = radioResource->type;
	m_RadioResource.ssi   = radioResource->ssi;

	if ((radioResource->type == RR_DYNGROUP) && (radioResource->id == m_RadioResource.id))
	{
		IEntityDetailsAreaStrategy* pEntityDetailsStrategy = const_cast<IEntityDetailsAreaStrategy*>(m_pEntityDetailsAreaStrategy);
		pEntityDetailsStrategy->initialise(this);
	}
	return 0;
}

LRESULT CEntityDetailsAreaDlg::onNotifiedEntityBeingCreated(WPARAM pRadioResource, LPARAM unused)
{
    FUNCTION_ENTRY("onNotifiedEntityBeingCreated");
    
	if ( m_staticBlank.IsWindowVisible() )
	{
		m_staticBlank.ShowWindow(SW_HIDE);

		//show controls
		m_staticText.ShowWindow(SW_SHOW);
		m_staticType.ShowWindow(SW_SHOW);
		m_staticID.ShowWindow(SW_SHOW);
		m_staticDesc.ShowWindow(SW_SHOW);
		m_typeEL.ShowWindow(SW_SHOW);
		m_idEL.ShowWindow(SW_SHOW);
		m_descriptionEL.ShowWindow(SW_SHOW);
	}

    // clear ListBox
    m_listLB.ResetContent();

	//From the passed in "WPARAM", extract(convert) RadioResource;
	RadioResource* radioResource = (RadioResource*)pRadioResource;
	m_RadioResource.alias = radioResource->alias;
	m_RadioResource.id	  = radioResource->id;
	m_RadioResource.type  = radioResource->type;
	m_RadioResource.ssi   = radioResource->ssi;

	reInitialiseStrategy(m_RadioResource.type);

	IEntityDetailsAreaStrategy* pEntityDetailsStrategy = const_cast<IEntityDetailsAreaStrategy*>(m_pEntityDetailsAreaStrategy);

	pEntityDetailsStrategy->initialise(this);

	FUNCTION_EXIT;
	return 0;
}


LRESULT CEntityDetailsAreaDlg::onNotifiedEntitySelectionChanged(WPARAM pRadioResource, LPARAM unused)
{
    FUNCTION_ENTRY("onNotifiedEntitySelectionChanged");
    
	if ( m_bEditMode )
		return 1;


	// Show/Hide controls
	if ( m_staticBlank.IsWindowVisible() )
	{
		m_staticBlank.ShowWindow(SW_HIDE);
		m_staticText.ShowWindow(SW_SHOW);
		m_staticType.ShowWindow(SW_SHOW);
		m_staticID.ShowWindow(SW_SHOW);
		m_staticDesc.ShowWindow(SW_SHOW);
		m_typeEL.ShowWindow(SW_SHOW);
		m_idEL.ShowWindow(SW_SHOW);
		m_descriptionEL.ShowWindow(SW_SHOW);
	}
	//++Fixed TD#14320
	m_listLB.MoveWindow(m_listRect);
	m_dgTalkgroupEL.MoveWindow(m_editRect);
	
	m_dgTalkgroupEL.ShowWindow(SW_HIDE);
	m_dgText.ShowWindow(SW_HIDE);
	//--Fixed TD#14320


	m_edit.MoveWindow(m_EditButtonRect);	
	m_save.MoveWindow(m_SaveButtonRect);
	m_delete.MoveWindow(m_DeleteButtonRect);
	//m_deleteMember.MoveWindow(m_DeleteMemberButtonRect);
	m_cancel.MoveWindow(m_CancelButtonRect);

	m_edit.ShowWindow(SW_HIDE);
	m_save.ShowWindow(SW_HIDE);
	m_delete.ShowWindow(SW_HIDE);
	//m_deleteMember.ShowWindow(SW_HIDE);
	m_cancel.ShowWindow(SW_HIDE);


	// clear ListBox
	m_listLB.ResetContent();

	RadioResource* radioResource = (RadioResource*)pRadioResource;
	if (RR_TRAIN == radioResource->type) {
		m_RadioResource = The_RadioSystemStatus::instance()->getResourceBySSI(radioResource->ssi);
	} else {
		m_RadioResource.alias = radioResource->alias;
		m_RadioResource.id	  = radioResource->id;
		m_RadioResource.type  = radioResource->type;
		m_RadioResource.ssi  = radioResource->ssi;
	}

	// Initialize EntityDetailsStrategy pointer to selected resource
	reInitialiseStrategy(m_RadioResource.type);

	IEntityDetailsAreaStrategy* pEntityDetailsStrategy = const_cast<IEntityDetailsAreaStrategy*>(m_pEntityDetailsAreaStrategy);

	pEntityDetailsStrategy->initialise(this);

	FUNCTION_EXIT;

	return 0;
}

LRESULT CEntityDetailsAreaDlg::onNotifiedEntityEmptySelection(WPARAM pRadioResourceType, LPARAM unused)
{
    FUNCTION_ENTRY("onNotifiedEntitySelectionChanged");
    
	if ( m_bEditMode )
		return 1;

	RadioResourceType type = (RadioResourceType) pRadioResourceType;
	if (RR_TALKGROUP == type || RR_RADIO == type) {
		type = RR_NIL;
	} 

	// Show/Hide controls
	if ( m_staticBlank.IsWindowVisible() )
	{
		m_staticBlank.ShowWindow(SW_HIDE);
		m_staticText.ShowWindow(SW_SHOW);
		m_staticType.ShowWindow(SW_SHOW);
		m_staticID.ShowWindow(SW_SHOW);
		m_staticDesc.ShowWindow(SW_SHOW);
		m_typeEL.ShowWindow(SW_SHOW);
		m_idEL.ShowWindow(SW_SHOW);
		m_descriptionEL.ShowWindow(SW_SHOW);
	}
	//++Fixed TD#14320
	m_listLB.MoveWindow(m_listRect);
	m_dgTalkgroupEL.MoveWindow(m_editRect);
	
	m_dgTalkgroupEL.ShowWindow(SW_HIDE);
	m_dgText.ShowWindow(SW_HIDE);
	//--Fixed TD#14320


	m_edit.MoveWindow(m_EditButtonRect);	
	m_save.MoveWindow(m_SaveButtonRect);
	m_delete.MoveWindow(m_DeleteButtonRect);
	//m_deleteMember.MoveWindow(m_DeleteMemberButtonRect);
	m_cancel.MoveWindow(m_CancelButtonRect);

	m_edit.ShowWindow(SW_HIDE);
	m_save.ShowWindow(SW_HIDE);
	m_delete.ShowWindow(SW_HIDE);
	//m_deleteMember.ShowWindow(SW_HIDE);
	m_cancel.ShowWindow(SW_HIDE);


	// clear ListBox
	m_listLB.ResetContent();

	m_RadioResource.alias = "";
	m_RadioResource.id	  = 0;
	m_RadioResource.type  = type;
	m_RadioResource.ssi  = 0;

	// Initialize EntityDetailsStrategy pointer to selected resource
	reInitialiseStrategy(type);

	IEntityDetailsAreaStrategy* pEntityDetailsStrategy = const_cast<IEntityDetailsAreaStrategy*>(m_pEntityDetailsAreaStrategy);

	pEntityDetailsStrategy->initialise(this);

	FUNCTION_EXIT;

	return 0;
}


void CEntityDetailsAreaDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// check if point lies within ListBox
	CRect rect;
	m_listLB.GetWindowRect(&rect);

	if ( (rect.PtInRect(point) != 0) && (m_listLB.GetCurSel() != LB_ERR) )
	{
		CMenu menu;
		menu.CreatePopupMenu();
		menu.AppendMenu(0,ID_DELETE_MENU, "Delete Member");

	   // display the popup menu
		
		menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this, rect);

		menu.DestroyMenu();
	}

}

void CEntityDetailsAreaDlg::OnDeleteMenu() 
{
	int index = m_listLB.GetCurSel();

	if (-1 == index) {
		MessageBox("No member selected", "Warning", MB_OK);
		return;
	}

	if (IDNO == MessageBox("Are you sure you wish to delete the member?","Delete member", MB_YESNO))
    {
        return;
    }

	//if (m_RadioResource.type == RR_DYNGROUP) {		
	//	CString alias;
	//	m_dgTalkgroupEL.GetWindowText(alias);
	//	std::string al((LPCTSTR)alias);
	//	unsigned long id = m_listLB.GetItemData(index);
	//	The_RadioOperations::instance()->releaseDynamicRegroup(m_RadioResource, al, id);
	//}

	if ( index != LB_ERR )
	{
		m_listLB.DeleteString(index);
	}
}


void CEntityDetailsAreaDlg::reInitialiseStrategy(RadioResourceType type)
{
	// delete m_pEntityDetailsAreaStrategy associated with the previously selected radioResource
	if ( m_pEntityDetailsAreaStrategy != NULL )
	{
		delete m_pEntityDetailsAreaStrategy;

		m_pEntityDetailsAreaStrategy = NULL;
	}	

	//m_RadioResource is changed to the new type;
	m_pEntityDetailsAreaStrategy = EntityDetailsAreaStrategyFactory::getEntityDetailsAreaStrategy(type);

	TA_ASSERT( m_pEntityDetailsAreaStrategy != NULL, "m_pEntityDetailsAreaStrategy is NULL");
}


void CEntityDetailsAreaDlg::refresh(unsigned long typeID, bool bOnInit /* = true */)
{
	// Type
	CString sType;
	sType.LoadString(typeID);
	m_typeEL.SetWindowText(sType);

	// Description
	m_descriptionEL.SetWindowText(m_RadioResource.alias.c_str());	

	// SSI
	std::ostringstream num;

//TD18975
	switch(m_RadioResource.type)
	{
	case RR_RADIO:
	case RR_TALKGROUP:
	case RR_SITE:
	case RR_ANNGROUP:
		num << m_RadioResource.id;
		break;
	case RR_MSELECT:
	case RR_PATCHGROUP:
		num << "";
		break;
	default:
		num << m_RadioResource.ssi;
	}
//TD18975

	m_idEL.SetWindowText(num.str().c_str());

	// get members of this resource
	if (bOnInit)
	{
		m_members = The_RadioSystemStatus::instance()->getResourceMembers(m_RadioResource);
	}

	//display members
	m_listLB.ResetContent();
	std::vector<RadioResource>::iterator iter = m_members.begin();

	int index = 0;
	m_dgTalkgroupEL.SetWindowText("");
	for ( ; iter != m_members.end(); ++iter)
	{
		if (RR_DYNGROUP == m_RadioResource.type && RR_TALKGROUP == (*iter).type) {
			m_dgTalkgroupEL.SetWindowText((*iter).alias.c_str());
			continue;
		}
		//m_listLB.AddString((*iter).alias.c_str());
		m_listLB.InsertString(index,(*iter).alias.c_str());
		m_listLB.SetItemData(index,(*iter).id);
		index++;
	}	
}


BOOL CEntityDetailsAreaDlg::PreTranslateMessage(MSG* pMsg) 
{
	// wong.peter, handle ENTER key for CEdit
	if (pMsg != NULL) {
		if (pMsg->hwnd == this->m_typeEL.m_hWnd || pMsg->hwnd == this->m_idEL || pMsg->hwnd == this->m_descriptionEL.m_hWnd
			|| pMsg->hwnd == this->m_dgTalkgroupEL.m_hWnd || pMsg->hwnd == this->m_listLB.m_hWnd) {
				if(pMsg->message == WM_KEYDOWN &&
					pMsg->wParam == VK_RETURN )
				{
					return TRUE;
				}
		}
		//}

		return CDialog::PreTranslateMessage(pMsg);
	}
	return false;
}

LRESULT CEntityDetailsAreaDlg::onNotifiedDbUpdate(WPARAM pOldResource, LPARAM pNewResource)
{
	FUNCTION_ENTRY("onNotifiedDbUpdate");

	RadioResource* oldRes = (RadioResource*)pOldResource;
	RadioResource* newRes = (RadioResource*)pNewResource;

	if (RR_TALKGROUP == m_RadioResource.type || RR_RADIO == m_RadioResource.type) {
		if (m_RadioResource.id == oldRes->id) {
			std::ostringstream num;
			num << newRes->id;
			m_idEL.SetWindowText(num.str().c_str());
			m_descriptionEL.SetWindowText(newRes->alias.c_str());
		}
	} else if (RR_DYNGROUP == m_RadioResource.type) {
		refresh(IDS_RR_DYNGROUP);
		//The_RadioSystemStatus::instance()->updateEntity(m_RadioResource,m_members);
	} else if (RR_PATCHGROUP == m_RadioResource.type) {
		refresh(IDS_RR_PATCHGROUP);
		//The_RadioSystemStatus::instance()->updateEntity(m_RadioResource,m_members);
	} else if (RR_MSELECT == m_RadioResource.type) {
		refresh(IDS_RR_MSELECT);
		//The_RadioSystemStatus::instance()->updateEntity(m_RadioResource,m_members);
	} 
	
	
	/*else if (RR_PATCHGROUP == m_RadioResource.type || RR_MSELECT == m_RadioResource.type || RR_DYNGROUP == m_RadioResource.type) {
		int index = m_listLB.FindStringExact(-1, oldRes->alias.c_str());
		if (LB_ERR != index) {
			m_listLB.DeleteString(index);
			m_listLB.InsertString(index, newRes->alias.c_str());
			m_listLB.SetItemData(index, newRes->id);
		}
	}
	
	if (RR_DYNGROUP == m_RadioResource.type) {
		CString tg;
		m_dgTalkgroupEL.GetWindowText(tg);
		if (oldRes->alias.compare(tg) == 0) {
			m_dgTalkgroupEL.SetWindowText(newRes->alias.c_str());
		}
	}*/

	FUNCTION_EXIT;
	return 0;
}

LRESULT CEntityDetailsAreaDlg::onNotifiedDbDelete(WPARAM pOldResource, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedDbDelete");

	RadioResource* res = (RadioResource*)pOldResource;
	if (RR_TALKGROUP == m_RadioResource.type || RR_RADIO == m_RadioResource.type) {
		if (res->id == m_RadioResource.id) {
			onNotifiedEntityEmptySelection((WPARAM)m_RadioResource.type, (LPARAM) 0);
		}
	}
	else if (RR_DYNGROUP == m_RadioResource.type) {
		refresh(IDS_RR_DYNGROUP);
		//The_RadioSystemStatus::instance()->updateEntity(m_RadioResource,m_members);
	} else if (RR_PATCHGROUP == m_RadioResource.type) {
		refresh(IDS_RR_PATCHGROUP);
		//The_RadioSystemStatus::instance()->updateEntity(m_RadioResource,m_members);
	} else if (RR_MSELECT == m_RadioResource.type) {
		refresh(IDS_RR_MSELECT);
		//The_RadioSystemStatus::instance()->updateEntity(m_RadioResource,m_members);
	}

	FUNCTION_EXIT;
	return 0;
}

void CEntityDetailsAreaDlg::enableListCtrl (const bool & bEnable)
{
	if (bEnable)
	{
		m_listLB.SetBkColor(RGB (255,255,255));
		m_listLB.SetTextBkColor(RGB(255,255,255));
	}
	else
	{
		m_listLB.SetBkColor(GetSysColor(COLOR_3DFACE));
		m_listLB.SetTextBkColor(GetSysColor(COLOR_3DFACE));	
	}
	Invalidate(TRUE);
}	

