/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/CResourceButton.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */


#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/CResourceButton.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/IResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/RadioResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/TalkGroupRadioResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/ResourceButtonStrategyFactory.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/CRadioStatusPage.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

const long CResourceButton::RADIO_BUTTON_REFERENCE = 1024;

//All the 14 message handlers are listed here;
BEGIN_MESSAGE_MAP(CResourceButton, CDecoratorButton)
	//{{AFX_MSG_MAP(CResourceButton)
	ON_MESSAGE(WM_ENTITY_EDIT_MODE, onNotifiedEditMode)
	ON_MESSAGE(WM_OPERATION_MODE, onNotifiedOperationMode)
	ON_MESSAGE(WM_RESOURCE_ASSIGNMENT, onNotifiedResourceAssignment)
	ON_MESSAGE(WM_RESOURCE_DE_ASSIGNMENT, onNotifiedResourceDeAssignment)
	ON_MESSAGE(WM_SELECT_SPEAKER_SELECTED, onNotifiedSelectSpeakerSelected)
	ON_MESSAGE(WM_SELECT_SPEAKER_UNSELECTED, onNotifiedSelectSpeakerUnselected)
	ON_MESSAGE(WM_INBOUND_CALL, onNotifiedInboundCall)
	ON_MESSAGE(WM_INBOUND_CALL_CANCELED, onNotifiedInboundCallCanceled)
	ON_MESSAGE(WM_PRIVATE_CALL_ESTABLISHED, onNotifiedPrivateCallEstablished)
	ON_MESSAGE(WM_PRIVATE_CALL_RINGING, onNotifiedPrivateCallRinging)
	ON_MESSAGE(WM_PRIVATE_CALL_ENDED, onNotifiedPrivateCallEnded)
	ON_MESSAGE(WM_RESOURCE_ACTIVITY, onNotifiedResourceActivity)
	ON_MESSAGE(WM_RESOURCE_ACTIVITY_ENDED, onNotifiedResourceActivityEnded)
	ON_MESSAGE(WM_RESOURCE_EMERGENCY, onNotifiedResourceEmergency)
	//ON_MESSAGE(WM_RESOURCE_ACTIVITY, onNotifiedResourceActivity) lichao++
	ON_MESSAGE(WM_RESOURCE_EMERGENCY_ACKNOWLEDGED, onNotifiedResourceEmergencyAcknowledged)
	ON_MESSAGE(WM_RESOURCE_EMERGENCY_ENDED, onNotifiedResourceEmergencyEnded)
	ON_MESSAGE(WM_TIMER_TRIGGERED, onNotifiedTimerTriggered)
	ON_COMMAND(ID_STOP_MONITOR_MENU, OnStopMonitorMenu)
	ON_COMMAND(ID_ACKNOWLEDGE_EMERGENCY_MENU, OnAcknowledgeEmergencyMenu)
	ON_COMMAND(ID_KNOCKDOWN_EMERGENCY_MENU, OnKnockdownEmergencyMenu)
	ON_COMMAND(ID_START_EMERGENCY_MENU, OnStartEmergencyMenu)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


CResourceButton::CResourceButton()
:	m_isProfileButton(false),
	m_isFlashOff(true),
	m_pResourceButtonContainer(NULL),
	m_pResourceButtonStrategy(NULL),
	m_buttonRef(0),
	m_isAckEmergency(false),
    m_isActive(false)
{
	FUNCTION_ENTRY("CResourceButton");

	IResourceButtonStrategy* pResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(m_RadioResource.type);
	TA_ASSERT( pResourceButtonStrategy != NULL, "Failed to Create IResourceButtonStrategy!");

	setResourceButtonStrategy(pResourceButtonStrategy);

	pResourceButtonStrategy->initialise ( this );


	// register observers for use-defined messages
	addObservers();

	FUNCTION_EXIT;
}


CResourceButton::~CResourceButton()
{
	FUNCTION_ENTRY("~CResourceButton");

	removeObservers();

	//The returned Strategy from ResourceButtonStrategyFactory should be deleted here;
	if( m_pResourceButtonStrategy != NULL )
	{
		delete m_pResourceButtonStrategy;

		m_pResourceButtonStrategy = NULL;
	}

	FUNCTION_EXIT;
}


void CResourceButton::setResourceButtonStrategy(IResourceButtonStrategy* pResourceButtonStrategy)
{
	FUNCTION_ENTRY("setResourceButtonStrategy");
	
	if ( m_pResourceButtonStrategy != NULL )
	{
		delete m_pResourceButtonStrategy;
		m_pResourceButtonStrategy = NULL;
	}
	
	m_pResourceButtonStrategy  = pResourceButtonStrategy;

	setButtonBehaviorStrategy(pResourceButtonStrategy);

	FUNCTION_EXIT;
}


void CResourceButton::addObservers()
{
	FUNCTION_ENTRY("addObservers");

	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_OPERATION_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_ASSIGNMENT);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_DE_ASSIGNMENT);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_SELECT_SPEAKER_SELECTED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_SELECT_SPEAKER_UNSELECTED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INBOUND_CALL);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_INBOUND_CALL_CANCELED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_ACTIVITY);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_ACTIVITY_ENDED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_EMERGENCY);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_EMERGENCY_ACKNOWLEDGED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_RESOURCE_EMERGENCY_ENDED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_TIMER_TRIGGERED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_ESTABLISHED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_RINGING);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PRIVATE_CALL_ENDED);

	FUNCTION_EXIT;
}

void CResourceButton::removeObservers()
{
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_OPERATION_MODE);	
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_ASSIGNMENT);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_DE_ASSIGNMENT);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_SELECT_SPEAKER_SELECTED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_SELECT_SPEAKER_UNSELECTED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INBOUND_CALL);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_INBOUND_CALL_CANCELED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_ACTIVITY);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_ACTIVITY_ENDED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_EMERGENCY);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_EMERGENCY_ACKNOWLEDGED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_RESOURCE_EMERGENCY_ENDED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_TIMER_TRIGGERED);	
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_ESTABLISHED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_RINGING);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PRIVATE_CALL_ENDED);
}

CResourceButton * CResourceButton::undecoratedResourceButton()
{
    return dynamic_cast<CResourceButton*>(CDecoratorButton::undecoratedResourceButton());
}


LRESULT CResourceButton::onNotifiedEditMode(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedEditMode");

	EnableWindow(FALSE);

	FUNCTION_EXIT;

	return 0;
}


LRESULT CResourceButton::onNotifiedOperationMode(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedEditMode");

	EnableWindow(TRUE);

	FUNCTION_EXIT;

	return 0;
}


LRESULT CResourceButton::onNotifiedResourceAssignment(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedResourceAssignment");

	if ( wParam != 0)
	{
		RadioResource* resource = (RadioResource*)wParam;

		// check if buttonRef matches lParam & current button is empty
		if ( (lParam == m_buttonRef) && (m_RadioResource.type == RR_NIL) )
		{
			m_RadioResource = *resource;

            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

			IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);
			
			pResourceButtonStrategy->resourceAssignment();

			SetWindowText(m_RadioResource.alias.c_str());
		}
	}

	FUNCTION_EXIT;

	return 0;

}

LRESULT CResourceButton::onNotifiedResourceDeAssignment(WPARAM wParam, LPARAM lParam)
{

	FUNCTION_ENTRY("onNotifiedResourceDeAssignment");

	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ( m_RadioResource.id == resource->id )
		{
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

			IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);
			pResourceButtonStrategy->resourceDeAssignment();
    		SetWindowText("");
		}
	}
	
	FUNCTION_EXIT;

	return 0;
}


LRESULT CResourceButton::onNotifiedSelectSpeakerSelected(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedSelectSpeakerSelected");

	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ((m_RadioResource.id == resource->id) && (m_RadioResource.type == resource->type))
		{
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");
			
			IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);
			
			pResourceButtonStrategy->selectSpeakerSelected();
		}
		
	}

	FUNCTION_EXIT;

	return 0;
}


LRESULT CResourceButton::onNotifiedSelectSpeakerUnselected(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedSelectSpeakerUnselected");

	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ((m_RadioResource.id == resource->id) && (m_RadioResource.type == resource->type))
		{
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

			IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);
			
			pResourceButtonStrategy->selectSpeakerUnselected();
		}
		
	}

	FUNCTION_EXIT;

	return 0;
}


LRESULT CResourceButton::onNotifiedInboundCall(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedInboundCall");

	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ( resource->id == m_RadioResource.id )
		{
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

			IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

			pResourceButtonStrategy->inboundCall();
		}
	}

	FUNCTION_EXIT;

	return 0;
}


LRESULT CResourceButton::onNotifiedInboundCallCanceled(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedInboundCallCanceled");

	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ( resource->id == m_RadioResource.id )
		{
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

            IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

			pResourceButtonStrategy->inboundCallCanceled();
		}
	}

	FUNCTION_EXIT;

	return 0;
}

LRESULT CResourceButton::onNotifiedPrivateCallEstablished(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ( resource->id == m_RadioResource.id )
		{
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

            IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

			pResourceButtonStrategy->inboundCall();
		}
	}
	return 0;
}

LRESULT CResourceButton::onNotifiedPrivateCallRinging(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ( resource->id == m_RadioResource.id )
		{
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

            IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

			pResourceButtonStrategy->inboundCallRinging();
		}
	}
	return 0;
}

LRESULT CResourceButton::onNotifiedPrivateCallEnded(WPARAM wParam, LPARAM lParam)
{
	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if (resource->type == RR_NIL)
		{
			// no specific resource ID specified, just cancel all radio unit
			// private call
			if (m_RadioResource.type == RR_RADIO)
			{
				TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

				IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

				pResourceButtonStrategy->inboundCallCanceled();
			}
		}
		else
		{
			if ( resource->id == m_RadioResource.id )
			{
				TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

				IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

				pResourceButtonStrategy->inboundCallCanceled();
			}
		}
	}
	return 0;
}

LRESULT CResourceButton::onNotifiedResourceActivity(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedResourceActivity");

	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ( resource->id == m_RadioResource.id )
		{
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

            IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

			pResourceButtonStrategy->resourceActivity();
		}
	}

	FUNCTION_EXIT;

	return 0;
}


LRESULT CResourceButton::onNotifiedResourceActivityEnded(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedResourceActivityEnded");

	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ( resource->id == m_RadioResource.id )
		{
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

            IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

			pResourceButtonStrategy->resourceActivityEnded();
		}
	}

	FUNCTION_EXIT;

	return 0;
}


LRESULT CResourceButton::onNotifiedResourceEmergency(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedResourceEmergency");

	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ((resource->id == m_RadioResource.id))// && (!m_isAckEmergency))
		{
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

            IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

			pResourceButtonStrategy->resourceEmergency();

			m_isAckEmergency = false;
		}

	}

	FUNCTION_EXIT;

	return 0;
}


LRESULT CResourceButton::onNotifiedResourceEmergencyAcknowledged(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedResourceEmergencyAcknowledged");

	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ( resource->id == m_RadioResource.id )
		{
			m_isAckEmergency = true;
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

            IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

			pResourceButtonStrategy->resourceEmergencyAcknowledged();
		}

	}

	FUNCTION_EXIT;

	return 0;
}


LRESULT CResourceButton::onNotifiedResourceEmergencyEnded(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedResourceEmergencyEnded");

	if ( wParam != 0 )
	{
		RadioResource* resource = (RadioResource*)wParam;

		if ( resource->id == m_RadioResource.id )
		{
			m_isAckEmergency = false;
            TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

            IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

			pResourceButtonStrategy->resourceEmergencyEnded();
		}

	}

	FUNCTION_EXIT;

	return 0;
}


LRESULT CResourceButton::onNotifiedTimerTriggered(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("onNotifiedTimerTriggered");

	if ( wParam )
	{
		m_isFlashOff = false;
	}
	else
	{
		m_isFlashOff = true;
	}

    if (m_pResourceButtonStrategy)
    {
	    IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);
	    pResourceButtonStrategy->reDraw();
    }

	FUNCTION_EXIT;

	return 0;
}


void CResourceButton::OnDropResource(RadioResource& resource)
{
	FUNCTION_ENTRY("OnDropResource");

	IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);
	
	CRadioStatusPage* pButtonContainer = dynamic_cast<CRadioStatusPage*>(m_pResourceButtonContainer);

    // check whether resource type is compatible with button type
	bool compatible = true;
	
	CResourceButton* t = m_pResourceButtonStrategy->m_pResourceButton;
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"OnDropResource");
	int butRef = m_pResourceButtonStrategy->m_pResourceButton->getButtonReference();

	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"butRef = %u", butRef);
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"resource type = %c", resource.type);
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"talkgroupsize = %u", pButtonContainer->getTalkgroupSize());
	//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"cadgroupsize = %u", pButtonContainer->getCadSize());
	switch (resource.type) {
		case RR_TALKGROUP: case RR_DYNGROUP:
			if (!(butRef >= 0 && butRef <= pButtonContainer->getTalkgroupSize())) {
				compatible = false;
			}
			break;
		case RR_PATCHGROUP:
			if (!(butRef >= pButtonContainer->getTalkgroupSize() && butRef <= pButtonContainer->getTalkgroupSize()+pButtonContainer->getCadSize())) {
				compatible = false;
			}
			break;
		case RR_MSELECT:
			if (!(butRef > pButtonContainer->getTalkgroupSize()+pButtonContainer->getCadSize())) {
				compatible = false;
			}
			break;
		case RR_RADIO:
			if (butRef != RADIO_BUTTON_REFERENCE) {
				compatible = false;
			}
			break;
		default: // DGNA and individual call type
			compatible = false;
	}
	// For include call
	if (RR_RADIO == resource.type && pButtonContainer->getResourceButton(butRef)->m_RadioResource.type == RR_DYNGROUP) {
		The_RadioOperations::instance()->includeCall(resource,pButtonContainer->getResourceButton(butRef)->m_RadioResource);
		return;
	}
	if (!compatible) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"Resource not compatible with button type");
		//MessageBox("Resource not compatible with button type","Warning",MB_OK|MB_ICONERROR);
		return;
	}

	if( pResourceButtonStrategy->canDropTarget() == true )
	{	
		//if ( !The_RadioOperations::instance()->getResource_isBeingMonitored(resource) )
		if (!pButtonContainer->isMonitored(m_isProfileButton,resource))
		{
			m_RadioResource = resource;

			HDROP hDropInfo = NULL;
			pResourceButtonStrategy->dropBehaviour(hDropInfo);

			if (pButtonContainer->isMonitored(m_isProfileButton,resource)) {
				SetWindowText(resource.alias.c_str());
			}

			//if (resource.type == RR_DYNGROUP) {
			//	std::string temp = resource.alias + "*";
			//	SetWindowText(temp.c_str());
			//} else {
			//}
		}
		else
		{
			MessageBox("Resource is already monitored","Warning",MB_OK|MB_ICONERROR);
		}
	}

	FUNCTION_EXIT;
	
}


void CResourceButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	CDecoratorButton::OnLButtonUp(nFlags,point);
	if (m_pResourceButtonStrategy)
    {
	    IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);
	    pResourceButtonStrategy->reDraw();
    }
}


void CResourceButton::OnRButtonUp(UINT nFlags, CPoint point)
{
	CDecoratorButton::OnRButtonUp(nFlags,point);
}


void CResourceButton::OnStopMonitorMenu()
{
    TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

	IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);
    try
    {
		if (pResourceButtonStrategy->removeAssignedResource())
        {
    		SetWindowText("");
            m_isFlashOff = false;
            m_isAckEmergency = false;
            m_isActive = false;
        }
	}
    catch (...)
    {
    }
}

void CResourceButton::OnAcknowledgeEmergencyMenu()
{
    //The_RadioSystemStatus::instance()->acknowledgeEmergency(m_RadioResource);
	The_RadioOperations::instance()->ackEmergencyCall(m_RadioResource);
}

void CResourceButton::OnKnockdownEmergencyMenu()
{
    //The_RadioSystemStatus::instance()->knockdownEmergency(m_RadioResource);
	The_RadioOperations::instance()->endEmergencyCall(m_RadioResource);
}

void CResourceButton::OnStartEmergencyMenu()
{
	The_RadioOperations::instance()->makeEmergencyCall(m_RadioResource);
}

void CResourceButton::setResourceButtonContainer(IResourceButtonContainer* pButtonContainer)
{
	m_pResourceButtonContainer = pButtonContainer;
}


void CResourceButton::setRadioResource(RadioResource& resource)
{
	m_RadioResource = resource;
}


RadioResource CResourceButton::getRadioResource()
{
	return m_RadioResource;
}



void CResourceButton::setProfileButton(int nProfile)
{
	m_isProfileButton = nProfile;
}


void CResourceButton::setButtonReference(unsigned long buttonRef)
{
	m_buttonRef = buttonRef;
}

unsigned long CResourceButton::getButtonReference()
{
	FUNCTION_ENTRY("getButtonReference");
	FUNCTION_EXIT;
	return m_buttonRef;
}

void CResourceButton::assignButtonOnInit(RadioResource& resource)
{
    /*
	m_RadioResource = resource;

	int x = m_buttonRef;

	if (m_RadioResource.type != RR_NIL) 
	{
        TA_ASSERT(m_pResourceButtonStrategy != NULL, "NULL strategy");

		IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);
		
		pResourceButtonStrategy->resourceAssignment();

		SetWindowText(m_RadioResource.alias.c_str());
	}
    */

    // do same as on drop

	IResourceButtonStrategy* pResourceButtonStrategy = const_cast<IResourceButtonStrategy*>(m_pResourceButtonStrategy);

	if( pResourceButtonStrategy->canDropTarget() == true )
	{	
		m_RadioResource = resource;

    	//Static function to new a concrete button;
    	IResourceButtonStrategy *pTempResourceButtonStrategy  = ResourceButtonStrategyFactory::getResourceButtonStrategy(resource.type);
	    pTempResourceButtonStrategy->initialise(this);

		if(pTempResourceButtonStrategy->assignmentBehaviour(true)) {
			setResourceButtonStrategy(pTempResourceButtonStrategy);
			SetWindowText(resource.alias.c_str());
		}

/*
		CRadioStatusPage* pButtonContainer = dynamic_cast<CRadioStatusPage*>(m_pResourceButtonContainer);

		//if ( !The_RadioOperations::instance()->getResource_isBeingMonitored(resource) )
//		if (!pButtonContainer->isMonitored(m_isProfileButton,resource))
		{
			m_RadioResource = resource;

			HDROP hDropInfo = NULL;
			pResourceButtonStrategy->dropBehaviour(hDropInfo);
//    		pResourceButtonStrategy->resourceAssignment();
			
		}
        */
	}
}
