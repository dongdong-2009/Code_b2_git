/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/CEntityOperationsAreaDlg.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/resource.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/CEntityOperationsAreaDlg.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/EntityOperationsAreaStrategyFactory.h"
#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/IEntityOperationsAreaStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


/////////////////////////////////////////////////////////////////////////////
// CEntityOperationsAreaDlg dialog

CEntityOperationsAreaDlg::CEntityOperationsAreaDlg(CWnd* pParent)
	: CDialog(CEntityOperationsAreaDlg::IDD, pParent),
	  m_pEntityOperationsAreaStrategy(NULL),
	  m_bEditMode(false)
{
	FUNCTION_ENTRY("CEntityOperationsAreaDlg");
	

	FUNCTION_EXIT;
}

CEntityOperationsAreaDlg::~CEntityOperationsAreaDlg()
{
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_OPERATION_MODE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_SELECTION_CHANGED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_BEING_CREATED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_UPDATED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_CONNECT_GROUP);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_PROCESSED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_RINGING);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_ESTABLISHED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_ENDED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_EMPTY_SELECTION);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_DB_DELETE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ESTABLISHED_DRIVER_CALL);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INCOMING_DRIVER_CALL);

    delete m_pEntityOperationsAreaStrategy;
    m_pEntityOperationsAreaStrategy = NULL;
}

void CEntityOperationsAreaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CEntityOperationsAreaDlg)
	DDX_Control(pDX, IDC_STATIC_VOLUME, m_staticText);
	DDX_Control(pDX, IDC_VOLUME_SLIDER, m_volumeSlider);
	DDX_Control(pDX, IDC_CALL_BTN, m_callButton);
	DDX_Control(pDX, IDC_ANSWER_BTN, m_answerButton);
	DDX_Control(pDX, IDC_ENDCALL_BTN, m_endCallButton);
	DDX_Control(pDX, IDC_CONNECT_BTN, m_connectButton);
	DDX_Control(pDX, IDC_SPEAKER_BTN, m_selectedSpeakerButton);
	DDX_Control(pDX, IDC_RELEASE_BTN, m_releaseButton);
	DDX_Control(pDX, IDC_SENDTEXT_BTN, m_sendTextButton);
	DDX_Control(pDX, IDC_PATCH_BTN, m_patchButton);
	DDX_Control(pDX, IDC_AMBIENCE_BTN, m_ambienceButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEntityOperationsAreaDlg, CDialog)
	//{{AFX_MSG_MAP(CEntityOperationsAreaDlg)
	ON_MESSAGE(WM_ENTITY_EDIT_MODE, OnNotifiedEditMode)
	ON_MESSAGE(WM_OPERATION_MODE, OnNotifiedOperationMode)
	ON_MESSAGE(WM_ENTITY_SELECTION_CHANGED, OnNotifiedEntitySelectionChanged)
	ON_MESSAGE(WM_ENTITY_BEING_CREATED,OnNotifiedEntityBeingCreated)
	ON_MESSAGE(WM_ENTITY_UPDATED,OnNotifiedEntityUpdated)
	ON_MESSAGE(WM_CONNECT_GROUP,OnNotifiedConnectGroup)
	ON_MESSAGE(WM_ENTITY_EMPTY_SELECTION, onNotifiedEntityEmptySelection)

	ON_MESSAGE(WM_PRIVATE_CALL_PROCESSED,OnNotifiedPrivateCallEvent)
	ON_MESSAGE(WM_PRIVATE_CALL_RINGING,OnNotifiedPrivateCallEvent)
	ON_MESSAGE(WM_PRIVATE_CALL_ESTABLISHED,OnNotifiedPrivateCallEvent)
	ON_MESSAGE(WM_PRIVATE_CALL_ENDED,OnNotifiedPrivateCallEvent)
	ON_MESSAGE(WM_ESTABLISHED_DRIVER_CALL,OnNotifiedPrivateCallEvent)
	ON_MESSAGE(WM_INCOMING_DRIVER_CALL,OnNotifiedPrivateCallEvent)
	ON_MESSAGE(WM_DB_DELETE, onNotifiedDbDelete)
	
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_VOLUME_SLIDER, OnReleasedCaptureVolumeSlider)
	ON_BN_CLICKED(IDC_AMBIENCE_BTN, OnAmbienceListening)
	ON_BN_CLICKED(IDC_ANSWER_BTN, OnAnswer)
	ON_BN_CLICKED(IDC_CALL_BTN, OnCall)
	ON_BN_CLICKED(IDC_CONNECT_BTN, OnConnect)
	ON_BN_CLICKED(IDC_ENDCALL_BTN, OnEndCall)
	ON_BN_CLICKED(IDC_PATCH_BTN, OnPatch)
	ON_BN_CLICKED(IDC_RELEASE_BTN, OnRelease)
	//ON_BN_CLICKED(IDC_SENDTEXT_BTN, OnSendText)
	ON_BN_CLICKED(IDC_SPEAKER_BTN, OnSpeaker)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEntityOperationsAreaDlg message handlers

BOOL CEntityOperationsAreaDlg::OnInitDialog() 
{
	FUNCTION_ENTRY("OnInitDialog");

	CDialog::OnInitDialog();


	//Show some buttons & hide some buttons according to different radio resources;
	initialise();
	
	/**
	 *  The volume slider shall represent the full range of values 
	 *	that are permissible in the Motorola system. 
	 *	ie. Values 1 to 8 inclusive, in integral steps. 
	 **/
	//fixed TD14123, also changed the property of this slide control
	m_volumeSlider.SetRange(1,8);
	m_volumeSlider.SetTicFreq(1);
	//fixed TD14123

	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_OPERATION_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_SELECTION_CHANGED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_BEING_CREATED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_UPDATED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_CONNECT_GROUP);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_PROCESSED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_RINGING);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_ESTABLISHED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_ENDED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_EMPTY_SELECTION);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DB_DELETE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ESTABLISHED_DRIVER_CALL);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INCOMING_DRIVER_CALL);

	FUNCTION_EXIT;
	
	return TRUE;  
}


void CEntityOperationsAreaDlg::initialise()
{
	FUNCTION_ENTRY("initialise");

	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		delete m_pEntityOperationsAreaStrategy;

		m_pEntityOperationsAreaStrategy = NULL;
	}

	//m_RadioResource should be dynamically updated;
	m_pEntityOperationsAreaStrategy = EntityOperationsAreaStrategyFactory::getEntityOperationsAreaStrategy( m_RadioResource.type );

	TA_ASSERT( m_pEntityOperationsAreaStrategy != NULL, "m_pEntityOperationsAreaStrategy is NULL");

	IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);

	//All cases use CEntityOperationsAreaDlg to do corresponding initialization;
	pEntityOperationsStrategy->initialise(this);

	FUNCTION_EXIT
}

RadioResource& CEntityOperationsAreaDlg::getCurrentResource()
{
    return m_RadioResource;
}


void CEntityOperationsAreaDlg::OnReleasedCaptureVolumeSlider(NMHDR* pNMHDR, LRESULT* pResult) 
{
	FUNCTION_ENTRY("OnReleasedCaptureVolumeSlider");

	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);

		pEntityOperationsStrategy->adjustVolume();
	}

	if (NULL != pResult)
		*pResult = 0;

	FUNCTION_EXIT;
}


void CEntityOperationsAreaDlg::OnSpeaker() 
{
	FUNCTION_ENTRY("OnSelectSpeaker");

	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);

		if (m_selectedSpeakerButton.GetCheck() > 0)
			pEntityOperationsStrategy->selectSpeaker();
		else
			pEntityOperationsStrategy->unselectSpeaker();		
	}

	FUNCTION_EXIT;
}


void CEntityOperationsAreaDlg::OnCall() 
{
	FUNCTION_ENTRY("OnCall");

	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);

		pEntityOperationsStrategy->call();
	}

	FUNCTION_EXIT;
}


void CEntityOperationsAreaDlg::OnAnswer() 
{
	FUNCTION_ENTRY("OnAnswer");

	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);

		pEntityOperationsStrategy->answer();
	}

	FUNCTION_EXIT;
}


void CEntityOperationsAreaDlg::OnEndCall() 
{
	FUNCTION_ENTRY("OnEndCall");

	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);

		pEntityOperationsStrategy->endCall();
	}

	FUNCTION_EXIT;
}

//void CEntityOperationsAreaDlg::OnSendText() 
//{
//	FUNCTION_ENTRY("OnSendText");
//
//	if ( m_pEntityOperationsAreaStrategy != NULL )
//	{
//		IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);
//
//		pEntityOperationsStrategy->sendText();
//	}
//
//	FUNCTION_EXIT;
//}


void CEntityOperationsAreaDlg::OnPatch() 
{
	FUNCTION_ENTRY("OnPatch");

	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);

		pEntityOperationsStrategy->patch();
	}	

	FUNCTION_EXIT;
}


void CEntityOperationsAreaDlg::OnAmbienceListening() 
{
	FUNCTION_ENTRY("OnAmbienceListening");

	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);

		// TD18119
		if (m_ambienceButton.GetCheck() > 0)
			pEntityOperationsStrategy->ambienceListening();
		else
			pEntityOperationsStrategy->endCall();
	}

	FUNCTION_EXIT;
}


void CEntityOperationsAreaDlg::OnConnect() 
{
	FUNCTION_ENTRY("OnConnect");

	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);

		pEntityOperationsStrategy->connect();
	}	

	FUNCTION_EXIT;
}


void CEntityOperationsAreaDlg::OnRelease() 
{
	FUNCTION_ENTRY("OnRelease");

	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);

		pEntityOperationsStrategy->release();

		m_selectedSpeakerButton.SetCheck(0);

	}	

	FUNCTION_EXIT;
}


LRESULT CEntityOperationsAreaDlg::OnNotifiedEntitySelectionChanged(WPARAM pRadioResource, LPARAM unused)
{
	// Dialog is not changed when an entity is selected in Edit Mode
	if ( m_bEditMode )
		return 1;

	//Different and new radio resource is selected or changed;
	RadioResource* radioResource = (RadioResource*)pRadioResource;
	m_RadioResource = *radioResource;

	//Clean the old strategy;
	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		delete m_pEntityOperationsAreaStrategy;
		m_pEntityOperationsAreaStrategy = NULL;
	}	

	//According to the new radio resource type to get new radio resource strategy;
	m_pEntityOperationsAreaStrategy = EntityOperationsAreaStrategyFactory::getEntityOperationsAreaStrategy( m_RadioResource.type );
	TA_ASSERT( m_pEntityOperationsAreaStrategy != NULL, "m_pEntityOperationsAreaStrategy is NULL");

	IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);
	pEntityOperationsStrategy->initialise(this);

	return 0;
}

LRESULT CEntityOperationsAreaDlg::OnNotifiedEditMode(WPARAM wParam, LPARAM lParam)
{
	// set flag to Edit mode
	m_bEditMode = true;

	//In edit mode, disable all controls;
	m_callButton.EnableWindow(FALSE);
	m_selectedSpeakerButton.EnableWindow(FALSE);
	m_answerButton.EnableWindow(FALSE);
	m_endCallButton.EnableWindow(FALSE);
	m_sendTextButton.EnableWindow(FALSE);
	m_patchButton.EnableWindow(FALSE);
	m_ambienceButton.EnableWindow(FALSE);
	m_connectButton.EnableWindow(FALSE);
	m_releaseButton.EnableWindow(FALSE);
	m_volumeSlider.EnableWindow(FALSE);

	return 0;
}

LRESULT CEntityOperationsAreaDlg::OnNotifiedOperationMode(WPARAM wParam, LPARAM lParam)
{

	// MUST enable buttons
	m_callButton.EnableWindow(TRUE);
	m_selectedSpeakerButton.EnableWindow(TRUE);
	m_answerButton.EnableWindow(TRUE);
	m_endCallButton.EnableWindow(TRUE);
	m_sendTextButton.EnableWindow(TRUE);
	m_patchButton.EnableWindow(TRUE);
	m_ambienceButton.EnableWindow(TRUE);
	m_connectButton.EnableWindow(TRUE);
	m_releaseButton.EnableWindow(TRUE);
	m_volumeSlider.EnableWindow(TRUE);

	IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);
	pEntityOperationsStrategy->initialise(this);
	
	// set flag to Operation mode
	m_bEditMode = false;

	return 0;
}
LRESULT CEntityOperationsAreaDlg::OnNotifiedEntityBeingCreated(WPARAM resource, LPARAM unused)
{
	// Dialog is not changed when an entity is selected in Edit Mode
	if ( m_bEditMode )
		return 1;

	m_connectButton.EnableWindow(FALSE);
	m_selectedSpeakerButton.EnableWindow(FALSE);
	m_releaseButton.EnableWindow(FALSE);
	m_patchButton.EnableWindow(FALSE);
	m_volumeSlider.EnableWindow(FALSE);
	
	//A new radio resource is selected or changed;
	RadioResource* radioResource = (RadioResource*)resource;
	m_RadioResource = *radioResource;

	//Clean the old strategy;
	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		delete m_pEntityOperationsAreaStrategy;
		m_pEntityOperationsAreaStrategy = NULL;
	}	

	//According to the new radio resource type to get new radio resource strategy;
	m_pEntityOperationsAreaStrategy = EntityOperationsAreaStrategyFactory::getEntityOperationsAreaStrategy( m_RadioResource.type );
	TA_ASSERT( m_pEntityOperationsAreaStrategy != NULL, "m_pEntityOperationsAreaStrategy is NULL");

	IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);
	pEntityOperationsStrategy->initialise(this);	

	return 0;
}


LRESULT CEntityOperationsAreaDlg::OnNotifiedEntityUpdated(WPARAM resource, LPARAM unused)
{
	RadioResource* radioResource = (RadioResource*)resource;
	m_RadioResource = *radioResource;

	return 0;
}

LRESULT CEntityOperationsAreaDlg::OnNotifiedConnectGroup(WPARAM resource, LPARAM pData)
{
	RadioResource* radioResource = (RadioResource*)resource;
    if (radioResource->type == m_RadioResource.type &&
        radioResource->id == m_RadioResource.id)
    {
	    IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);
	    pEntityOperationsStrategy->onConnectState((GroupConnectionState) pData);
//TD18785
		checkConnectGroup(*radioResource, (GroupConnectionState) pData);
//TD18785
    }

	return 0;
}

LRESULT CEntityOperationsAreaDlg::OnNotifiedPrivateCallEvent(WPARAM resource, LPARAM pData)
{
	RadioResource* radioResource = (RadioResource*)resource;
    if ((RR_TRAIN == m_RadioResource.type || RR_RADIO == m_RadioResource.type) &&
        radioResource->id == m_RadioResource.id)
    {
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "OnNotifiedPrivateCallEvent");
		IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);
	    pEntityOperationsStrategy->initialise(this);
//TD18785
		checkPrivateCall(*radioResource);
//TD18785
    }
	return 0;
}
//TD18785
void CEntityOperationsAreaDlg::checkConnectGroup(RadioResource &resource, GroupConnectionState eGroupState)
{
	switch(eGroupState)
	{
	case CONNECTED:
		The_RadioSystemStatus::instance()->addActiveResource(resource);
		break;
	case RELEASED:
		The_RadioSystemStatus::instance()->deleteActiveResource(resource);
	default:
		break;
	}
}	

void CEntityOperationsAreaDlg::checkPrivateCall(RadioResource &resource)
{
	PrivateCallState ptCallState = 
		The_RadioOperations::instance()->getResource_isInPrivateCall(resource);
	
	switch(ptCallState)
	{
	case PC_INCOMING:
	case PC_ESTABLISHED:
	case PC_OUTGOING:
		The_RadioSystemStatus::instance()->addActiveResource(resource);
		break;
	case PC_NOT_INVOLVED:
		{
			bool isAmbListen = The_RadioOperations::instance()->getResource_isInAmbienceListeningCall(resource);
			if (isAmbListen)
			{
				The_RadioSystemStatus::instance()->addActiveResource(resource);
			}
			else
			{
				The_RadioSystemStatus::instance()->deleteActiveResource(resource);
			}
			
		}
		break;
	}

	
}

//TD18785

LRESULT CEntityOperationsAreaDlg::onNotifiedEntityEmptySelection(WPARAM pRadioResourceType, LPARAM unused)
{
	// Dialog is not changed when an entity is selected in Edit Mode
	if ( m_bEditMode )
		return 1;

	RadioResourceType type = (RadioResourceType) pRadioResourceType;
	if (RR_TALKGROUP == type || RR_RADIO == type) {
		type = RR_NIL;
	}
	
	RadioResource resource;
	resource.type = (RadioResourceType)type;
	m_RadioResource = resource;

	//Clean the old strategy;
	if ( m_pEntityOperationsAreaStrategy != NULL )
	{
		delete m_pEntityOperationsAreaStrategy;
		m_pEntityOperationsAreaStrategy = NULL;
	}	

	//According to the new radio resource type to get new radio resource strategy;
	m_pEntityOperationsAreaStrategy = EntityOperationsAreaStrategyFactory::getEntityOperationsAreaStrategy( m_RadioResource.type );
	TA_ASSERT( m_pEntityOperationsAreaStrategy != NULL, "m_pEntityOperationsAreaStrategy is NULL");

	IEntityOperationsAreaStrategy* pEntityOperationsStrategy = const_cast<IEntityOperationsAreaStrategy*>(m_pEntityOperationsAreaStrategy);
	pEntityOperationsStrategy->initialise(this);

	return 0;
}

BOOL CEntityOperationsAreaDlg::PreTranslateMessage(MSG* pMsg) 
{
	// wong.peter, handle ENTER / ESC key
	if(pMsg->message == WM_KEYDOWN &&  (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) )
	{
		return TRUE;
	}
	//}

	return CDialog::PreTranslateMessage(pMsg);
}



LRESULT CEntityOperationsAreaDlg::onNotifiedDbDelete(WPARAM pOldResource, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedDbDelete");

	RadioResource* res = (RadioResource*)pOldResource;

	if (RR_TALKGROUP == m_RadioResource.type || RR_RADIO == m_RadioResource.type) {
		if (res->id == m_RadioResource.id) {
			onNotifiedEntityEmptySelection((WPARAM)m_RadioResource.type, (LPARAM)0);
		}
	}

	FUNCTION_EXIT;
	return 0;
}