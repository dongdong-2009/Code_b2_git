/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/radiomanager.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"
#include "app/radio/radio_manager_gui/src/RadioErrorMsg.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/RadioAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/TcpMethodTimeoutException.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/TcpException.h"
#include "app/radio/radio_manager_gui/src/TrainAgentInterface/TrainAgentIF.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TA_Base_Core::ReEntrantThreadLockable RadioOperations::m_lock;

RadioOperations::RadioOperations():
	m_resourceListCurrentIndex(0),
	m_auditSender(NULL),
	m_entityKey(0),
	m_CallForwardingState(false),
	m_trainAgentIF (NULL)
{
    FUNCTION_ENTRY("RadioOperations");

	initialise();

    FUNCTION_EXIT;
}


RadioOperations::~RadioOperations()
{
    FUNCTION_ENTRY("~RadioOperations");

	if (m_auditSender != NULL)
	{
		delete m_auditSender;
		m_auditSender = NULL;
	}
	
	if (m_trainAgentIF)
	{
		delete m_trainAgentIF;
		m_trainAgentIF = NULL;
	}

    FUNCTION_EXIT;
	
}



void RadioOperations::initialise()
{
    FUNCTION_ENTRY("initialise");

	m_session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

	try {
		m_entityKey = RadioMFTAgentHelper::getRadioMFTAgentEntityKey();
	}
	catch (TA_Base_Core::TransactiveException e) {
		m_entityKey = 0;
	}

	try {
		m_auditSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::RadioAudit::Context);
	}
	catch (...) {
		m_auditSender = NULL;
	}


	connectFail = false;
	connected = false;
	notifyConFail = false;
	driverCallState.clear();

    FUNCTION_EXIT;
}

bool RadioOperations::getResource_isActive(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	return The_RadioSystemStatus::instance()->getResource_isActive(resource);
}


PrivateCallState RadioOperations::getResource_isInPrivateCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	return The_RadioSystemStatus::instance()->getResource_isInPrivateCall(resource);
}


GroupConnectionState RadioOperations::getResource_isGroupConnected(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	return The_RadioSystemStatus::instance()->getResource_isGroupConnected(resource);
}


bool RadioOperations::getResource_isInAmbienceListeningCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );

	return The_RadioSystemStatus::instance()->getResource_isInAmbienceListeningCall(resource);
}


bool RadioOperations::getResource_isBeingMonitored(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    
	return The_RadioSystemStatus::instance()->getResource_isBeingMonitored(resource);
}


std::vector<RadioResourceType> RadioOperations::getCreateableResourceTypes()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("getCreateableResourceTypes");
    
	std::vector<RadioResourceType> rTypeList;

	//RR_PATCHGROUP, RR_DYNGROUP, RR_MSELECT,RR_MSITE
	rTypeList.push_back(RR_PATCHGROUP);
	rTypeList.push_back(RR_DYNGROUP);
	rTypeList.push_back(RR_MSELECT);
	rTypeList.push_back(RR_MSITE);

	FUNCTION_EXIT;

	return rTypeList;
}


void RadioOperations::onResourceAssignmentEvent(RadioResource& resource, unsigned int buttonRef)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onResourceAssignmentEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_RESOURCE_ASSIGNMENT,(WPARAM)res, buttonRef);

	FUNCTION_EXIT;
}


void RadioOperations::onResourceDeAssignmentEvent(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onResourceDeAssignmentEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_RESOURCE_DE_ASSIGNMENT,(WPARAM)res);

	FUNCTION_EXIT;
}


void RadioOperations::onVolumeChangedEvent(RadioResource& resource, unsigned int volume)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onVolumeChangedEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_VOLUME_CHANGED,(WPARAM)res,volume);

	FUNCTION_EXIT;
}


void RadioOperations::onSelectSpeakerSelectedEvent(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onSelectSpeakerSelectedEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_SELECT_SPEAKER_SELECTED,(WPARAM)res);

	FUNCTION_EXIT;
}


void RadioOperations::onSelectSpeakerUnselectedEvent(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onSelectSpeakerUnselectedEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_SELECT_SPEAKER_UNSELECTED,(WPARAM)res);

	FUNCTION_EXIT;
}


void RadioOperations::onInboundCallEvent(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onInboundCallEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_INBOUND_CALL,(WPARAM)res);

	FUNCTION_EXIT;
}


void RadioOperations::onInboundCallCanceledEvent(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onInboundCallCanceledEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_INBOUND_CALL_CANCELED,(WPARAM)res);

	FUNCTION_EXIT;

}

void RadioOperations::onPrivateCallRinging(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onPrivateCallRinging");

	// If the corresponding MFT is on call forwarding state, ignore all the incoming call 
	// todo: handle driver call situation
	if (getCallForwardingState()) return;
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_PRIVATE_CALL_RINGING,(WPARAM)res);

	//huangxin TD14286
	TA_Base_Core::DescriptionParameters descParams;

	TA_Base_Core::NameValuePair param1("ResourceName",resource.alias);
    descParams.push_back(&param1);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioIncomingPrivateCall,descParams,resource);
	//huangxin TD14286

	FUNCTION_EXIT;
}

void RadioOperations::onPrivateCallEstablished(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onPrivateCallEstablished");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_PRIVATE_CALL_ESTABLISHED,(WPARAM)res);

	FUNCTION_EXIT;
}


void RadioOperations::onPrivateCallEnded(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onPrivateCallEnded");

	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_PRIVATE_CALL_ENDED,(WPARAM)res);

	//huangxin TD14286
	TA_Base_Core::DescriptionParameters descParams;
	TA_Base_Core::NameValuePair param1("ResourceName",resource.alias);
    descParams.push_back(&param1);

	if (The_RadioSystemStatus::instance()->getResource_isInAmbienceListeningCall(resource))
	{
		auditEventPosted(TA_Base_Core::RadioAudit::RadioAmbienceListeningTerminated,descParams);
	}
	else
	{
		auditEventPosted(TA_Base_Core::RadioAudit::RadioEndPrivateCall,descParams,resource);
	}
	//huangxin TD14286
	FUNCTION_EXIT;
}

// Fixed TD#14131
void RadioOperations::onPrivateCallProcessed(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onPrivateCallProcessed");
    
	RadioResource* res = getNextResource(resource);
	m_observerHelper.notifyChildWindows(WM_PRIVATE_CALL_PROCESSED,(WPARAM)res);
	
	FUNCTION_EXIT;	
}
void RadioOperations::onPrivateCallRejected(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("onPrivateCallRejected");

	RadioResource* res = getNextResource(resource);
	m_observerHelper.notifyChildWindows(WM_PRIVATE_CALL_REJECTED,(WPARAM)res);

	FUNCTION_EXIT;	
}



void RadioOperations::onAmbienceListeningEvent(RadioResource& resource, AmbienceListeningCallState state)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onAmbienceListeningEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_AMBIENCE_LISTENING,(WPARAM)res, (LPARAM)state);

	FUNCTION_EXIT;	
}

void RadioOperations::onGroupConnectionStateEvent(RadioResource& resource, GroupConnectionState pState)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onGroupConnectionStateEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_CONNECT_GROUP,(WPARAM)res, (LPARAM)pState);

	FUNCTION_EXIT;	
}


void RadioOperations::onInboundPTTEvent(RadioResource& listener,RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onInboundPTTEvent");
    
	RadioResource* receiver = getNextResource(listener);

	RadioResource* src = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_INBOUND_PTT,(WPARAM)receiver,(WPARAM)src);

	FUNCTION_EXIT;
}


void RadioOperations::onInboundPTTReleasedEvent(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onInboundPTTReleasedEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_INBOUND_PTT_RELEASED,(WPARAM)res);
	FUNCTION_EXIT;
}


void RadioOperations::onResourceActivityEvent(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onResourceActivityEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_RESOURCE_ACTIVITY,(WPARAM)res);

	FUNCTION_EXIT;
}


void RadioOperations::onResourceActivityEndedEvent(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onResourceActivityEndedEvent");
    
	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_RESOURCE_ACTIVITY_ENDED,(WPARAM)res);

	FUNCTION_EXIT;
}


void RadioOperations::onResourceEmergencyEvent(RadioResource& resource,RadioResource& radioUnit)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onResourceEmergencyEvent");
    
	// TD17224
	TA_Base_Core::DescriptionParameters descParams;

	std::ostringstream radioId;
	radioId << radioUnit.ssi;

	TA_Base_Core::NameValuePair param1("Radio ID",radioId.str());
    descParams.push_back(&param1);

	radioUnit.id = radioUnit.ssi;
	
	if (radioUnit.ssi != 0) // TD18118
	{
		auditEventPosted(TA_Base_Core::RadioAudit::RadioInitiateEmergency,descParams,radioUnit);
	}
	// TD17224

	RadioResource* res = getNextResource(resource);
	RadioResource* rUnit = getNextResource(radioUnit); // TD18118
	m_observerHelper.notifyChildWindows(WM_RESOURCE_EMERGENCY,(WPARAM)res,(LPARAM)rUnit);

	FUNCTION_EXIT;
}


void RadioOperations::onResourceEmergencyAcknowledgedEvent(RadioResource& resource,RadioResource& radioUnit)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onResourceEmergencyAcknowledgedEvent");

	RadioResource* res = getNextResource(resource);
	RadioResource* rUnit = getNextResource(radioUnit); // TD18118

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Notifying Ack Emergency for resource id : %d",resource.id);

	m_observerHelper.notifyChildWindows(WM_RESOURCE_EMERGENCY_ACKNOWLEDGED,(WPARAM)res,(LPARAM)rUnit);

	FUNCTION_EXIT;
}


void RadioOperations::onResourceEmergencyEndedEvent(RadioResource& resource,RadioResource& radioUnit)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onResourceEmergencyEndedEvent");
    
	// TD17224
	TA_Base_Core::DescriptionParameters descParams;

	std::ostringstream radioId;
	radioId << radioUnit.ssi;

	std::ostringstream rid;
	rid << radioUnit.id;

	TA_Base_Core::NameValuePair param1("Radio ID",radioId.str());
	TA_Base_Core::NameValuePair param2("ResourceId",rid.str());
    descParams.push_back(&param1);
	descParams.push_back(&param2);

	radioUnit.id = radioUnit.ssi;

	if (radioUnit.ssi != 0) // TD18118
	{
		auditEventPosted(TA_Base_Core::RadioAudit::RadioEmergencyEnded,descParams,radioUnit);
	}
	// TD17224

	RadioResource* res = getNextResource(resource);
	RadioResource* rUnit = getNextResource(radioUnit); // TD18118
	m_observerHelper.notifyChildWindows(WM_RESOURCE_EMERGENCY_ENDED,(WPARAM)res,(LPARAM)rUnit);

	FUNCTION_EXIT;
}


//void RadioOperations::onPredefinedMessageEvent(RadioUpdateMessageType eventType, PredefinedMessage* messageData)
//{
//	TA_Base_Core::ThreadGuard guard( m_lock );
//    FUNCTION_ENTRY("onPredefinedMessageEvent");
//    
//	switch(eventType)
//	{
//		case RM_NEW:
//			predefinedMessageAdded(messageData);
//			break;
//		case RM_UPDATE:
//			predefinedMessageModified(messageData);
//			break;
//		case RM_DELETE:
//			predefinedMessageRemoved(messageData);
//	}
//
//	FUNCTION_EXIT;
//}
//
//
//void RadioOperations::predefinedMessageAdded(PredefinedMessage* messageData)
//{
//    FUNCTION_ENTRY("predefinedMessageAdded");
//    
//	//notifyChildWindows - WM_PREDEFINEDMSG_ADDED
//	m_observerHelper.notifyChildWindows(WM_PREDEFINEDMSG_ADDED,(WPARAM)messageData);
//
//	FUNCTION_EXIT;
//}


//void RadioOperations::predefinedMessageModified(PredefinedMessage* messageData)
//{
//    FUNCTION_ENTRY("predefinedMessageModified");
//    	
//	//notifyChildWindows - WM_PREDEFINEDMSG_MODIFIED
//	m_observerHelper.notifyChildWindows(WM_PREDEFINEDMSG_MODIFIED,(WPARAM)messageData);
//
//	FUNCTION_EXIT;
//}
//
//
//void RadioOperations::predefinedMessageRemoved(PredefinedMessage* messageData)
//{
//    FUNCTION_ENTRY("predefinedMessageRemoved");
//    
//	//notifyChildWindows - WM_PREDEFINEDMSG_REMOVED
//	m_observerHelper.notifyChildWindows(WM_PREDEFINEDMSG_REMOVED,(WPARAM)messageData);
//
//	FUNCTION_EXIT;
//}

/* Fixed TD#14297 
   added Radio resource parameter for auditEventPosted() */
void RadioOperations::onIncomingTextMessageEvent(MessageInbox* messageData,RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onIncomingTextMessageEvent");

	TA_Base_Core::DescriptionParameters descParams;

	std::ostringstream sKey;
	sKey << messageData->key;

	TA_Base_Core::NameValuePair param1("MessageID",sKey.str());
	TA_Base_Core::NameValuePair param2("ResourceName",messageData->messageSource);
    descParams.push_back(&param1);
	descParams.push_back(&param2);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioTextMessageReceive,descParams,resource);
    
	//notifyChildWindows - WM_TEXTMSG_RECEIVED
	m_observerHelper.notifyChildWindows(WM_TEXTMSG_RECEIVED,(WPARAM)messageData);
	
	FUNCTION_EXIT;
}


void RadioOperations::operationMode()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("operationMode");
    
	//notifyChildWindows - WM_OPERATION_MODE
	m_observerHelper.notifyChildWindows(WM_OPERATION_MODE);

	FUNCTION_EXIT;
}


void RadioOperations::editMode(RadioResource& res, bool bOnDispatchNotReady/* =false */)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("editMode");
    
	//notifyChildWindows - WM_ENTITY_EDIT_MODE
	RadioResource* resource = getNextResource(res);
	m_observerHelper.notifyChildWindows(WM_ENTITY_EDIT_MODE,bOnDispatchNotReady, (LPARAM) resource);

	FUNCTION_EXIT;
}


void RadioOperations::newEntityBeingCreated(RadioResource& newResource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("newEntityBeingCreated");
    
	RadioResource* resource = getNextResource(newResource);

	m_observerHelper.notifyChildWindows(WM_ENTITY_BEING_CREATED,(WPARAM)resource);

	FUNCTION_EXIT;
}


void RadioOperations::entitySelectionChange(RadioResource& resource, bool selected)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("entitySelectionChange");
    
	if ( resource.type != RR_NIL )
	{
		RadioResource* res = getNextResource(resource);
		
		m_observerHelper.notifyChildWindows(WM_ENTITY_SELECTION_CHANGED,(WPARAM)res, (LPARAM) selected);
	}
	
	FUNCTION_EXIT;
}


bool RadioOperations::locateSelectSpeaker()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("locateSelectSpeaker");
    
	RadioResource resource = The_RadioSystemStatus::instance()->locateSelectSpeaker();

	if ( resource.type != RR_NIL )
	{
		RadioResource* res = getNextResource(resource);
		m_observerHelper.notifyChildWindows(WM_ENTITY_SELECTION_CHANGED,(WPARAM)res);
	}
	else
		return false;

	FUNCTION_EXIT;

	return true;
}


bool RadioOperations::saveEntity(RadioResource& resource, std::vector<RadioResource>& members)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("saveEntity");
    
	bool bFlag = The_RadioSystemStatus::instance()->updateEntity(resource,members);

	if (bFlag)
	{
		The_RadioSystemStatus::instance()->addRadioResource(resource, members, resource.id);
		RadioResource* res = getNextResource(resource);

		//Send Audit Message
		sendDGNAAuditMsge(resource,CREATE_DYNAMIC_GROUP);

		m_observerHelper.notifyChildWindows(WM_ENTITY_UPDATED,(WPARAM)res);
		m_observerHelper.notifyChildWindows(WM_OPERATION_MODE);

	}

	FUNCTION_EXIT;
	return bFlag;
}


void RadioOperations::deleteEntity(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("deleteEntity");

	if ( The_RadioSystemStatus::instance()->deleteEntity(resource))
	{
        RadioResource emptyResource;
		RadioResource* res =  getNextResource(emptyResource);

		//Send Audit Message
		sendDGNAAuditMsge(resource,RELEASE_DYNMIC_GROUP);
		
		m_observerHelper.notifyChildWindows(WM_OPERATION_MODE);
		m_observerHelper.notifyChildWindows(WM_ENTITY_SELECTION_CHANGED,(WPARAM)res);

	}

	FUNCTION_EXIT;
}


bool RadioOperations::assignResource(RadioResource& resource, unsigned int buttonRef)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("assignResource");
    
	FUNCTION_EXIT;

	
	if (RR_TALKGROUP == resource.type) 
	{
		TA_Base_Core::DescriptionParameters descParams;
		std::stringstream ss;
		ss << resource.id;
		TA_Base_Core::NameValuePair param1("Radio ID",ss.str());
		descParams.push_back(&param1);

		auditEventPosted(TA_Base_Core::RadioAudit::RadioGroupMonitored,descParams,resource);
	}

	int status = The_RadioSystemStatus::instance()->assignResource(resource,buttonRef);

	if (!status) 
	{
		cancelApiOperation(RA_ASSIGN_STATE, resource);
	}

	return status;
}


bool RadioOperations::deAssignResource(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("deAssignResource");
    
	FUNCTION_EXIT;

	int status = The_RadioSystemStatus::instance()->deAssignResource(resource);

	if (!status) 
	{
		cancelApiOperation(RA_DEASSIGN_STATE, resource);
	}

	return status;
}


void RadioOperations::makePrivateCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("makePrivateCall");
    
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"RadioOperations : makePrivate Call : %s",
				resource.alias.c_str());
	
	if ( The_RadioSystemStatus::instance()->makePrivateCall(resource) )
	{
		RadioResource* res = getNextResource(resource);

		m_observerHelper.notifyChildWindows(WM_OUTGOING_CALL,(WPARAM)res);
	}

	TA_Base_Core::DescriptionParameters descParams;

	std::stringstream ss;
	ss << resource.id;
	TA_Base_Core::NameValuePair param1("Radio ID",ss.str());
    descParams.push_back(&param1);

	//++Fixed TD#14138  
	//TA_Base_Core::NameValuePair param2("CallType","Private");
	//descParams.push_back(&param2);
	//--Fixed TD#14138  

	auditEventPosted(TA_Base_Core::RadioAudit::RadioMakePrivateCall,descParams,resource);

	FUNCTION_EXIT;
}


void RadioOperations::makeAmbienceListeningCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("makeAmbienceListeningCall");
    
	The_RadioSystemStatus::instance()->makeAmbienceListeningCall(resource);

	TA_Base_Core::DescriptionParameters descParams;

	TA_Base_Core::NameValuePair param1("ResourceName",resource.alias);
    descParams.push_back(&param1);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioAmbienceListeningActivated,descParams,resource);

	FUNCTION_EXIT;
}


void RadioOperations::answerPrivateCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("answerPrivateCall");
    
	The_RadioSystemStatus::instance()->answerPrivateCall(resource);

	//huangxin TD14286
	TA_Base_Core::DescriptionParameters descParams;
	std::stringstream ss;
	ss << resource.id;
	TA_Base_Core::NameValuePair param1("Radio ID",ss.str());
    descParams.push_back(&param1);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioPrivateCallAnswered,descParams,resource);
	//huangxin TD14286

	FUNCTION_EXIT;
}


void RadioOperations::endPrivateCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("endPrivateCall");
    
	The_RadioSystemStatus::instance()->endPrivateCall(resource);

	FUNCTION_EXIT;
}


void RadioOperations::selectSpeaker(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("selectSpeaker");
    
	The_RadioSystemStatus::instance()->selectSpeaker(resource);

	TA_Base_Core::DescriptionParameters descParams;
	std::stringstream ss;
	ss << resource.id;
	TA_Base_Core::NameValuePair param1("Radio ID",ss.str());
    descParams.push_back(&param1);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioGroupSelect,descParams,resource);

	FUNCTION_EXIT;
}


void RadioOperations::unselectSpeaker(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("unselectSpeaker");
    
	The_RadioSystemStatus::instance()->unselectSpeaker(resource);

	TA_Base_Core::DescriptionParameters descParams;
	TA_Base_Core::NameValuePair param1("ResourceName",resource.alias);
    descParams.push_back(&param1);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioGroupUnselect,descParams,resource);

	FUNCTION_EXIT;
}


void RadioOperations::changeVolume(RadioResource& resource, unsigned int volume)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("changeVolume");
    
	The_RadioSystemStatus::instance()->changeVolume(resource,volume);

	FUNCTION_EXIT;
}


bool RadioOperations::connectGroup(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("connectGroup");

	bool result = false;

	result = The_RadioSystemStatus::instance()->connectGroup(resource);

	if (!result) {
		cancelApiOperation(RA_SELECT_STATE, resource);
	}

	TA_Base_Core::DescriptionParameters descParams;
	//TA_Base_Core::NameValuePair param1("ResourceName",resource.alias);
 //   descParams.push_back(&param1);


	std::vector<RadioResource> members = The_RadioSystemStatus::instance()->getResourceMembers(resource);		
	if (RR_DYNGROUP == resource.type) {
		std::vector<RadioResource>::iterator iter = members.begin();
		for ( ; iter != members.end(); ++iter)
		{
			if (RR_TALKGROUP == (*iter).type) break;
		}
		if (iter != members.end()) {
			members.erase(iter);
		}
	}	
		
	std::vector<RadioResource>::iterator iter = members.begin();
	std::stringstream ss;
	std::string s;
	for (; iter!=members.end(); iter++) {
		ss << (*iter).id;
		ss << ",";
	}
	s = ss.str();
	TA_Base_Core::NameValuePair param2("Member", s.substr(0, s.size()-1));
	descParams.push_back(&param2);

	switch(resource.type)
	{
		case RR_PATCHGROUP:
			auditEventPosted(TA_Base_Core::RadioAudit::RadioPatchGroupConnect,descParams,resource);
			break;
		case RR_DYNGROUP:
			onGroupConnectionStateEvent(resource,CONNECTED); // Fixed TD14320
			auditEventPosted(TA_Base_Core::RadioAudit::RadioDynamicRegroupConnect,descParams,resource);
			break;
		case RR_MSELECT:
			auditEventPosted(TA_Base_Core::RadioAudit::RadioMultiselectConnect,descParams,resource);
			break;
		//case RR_SITE:
		//	auditEventPosted(TA_Base_Core::RadioAudit::RadioSiteConnect,descParams,resource);
		//	break;
		//case RR_MSITE:
		//	auditEventPosted(TA_Base_Core::RadioAudit::RadioMultiSiteConnect,descParams,resource);
	}
	
/* AZ: connection state will be sent later
	if (result)
	{
		RadioResource* res = getNextResource(resource);
		m_observerHelper.notifyChildWindows(WM_CONNECT_GROUP,(WPARAM)res);
	}
*/
	FUNCTION_EXIT;
	return result;
}


bool RadioOperations::releaseGroup(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("releaseGroup");
    
	bool result = The_RadioSystemStatus::instance()->releaseGroup(resource);

	if (!result) {
		cancelApiOperation(RA_DESELECT_STATE, resource);
	}

	TA_Base_Core::DescriptionParameters descParams;
	TA_Base_Core::NameValuePair param1("ResourceName",resource.alias);
    descParams.push_back(&param1);

	if (resource.type ==RR_PATCHGROUP)
	{
		auditEventPosted(TA_Base_Core::RadioAudit::RadioPatchGroupRelease,descParams,resource);
	}
	else if (resource.type == RR_DYNGROUP)
	{
		auditEventPosted(TA_Base_Core::RadioAudit::RadioDynamicRegroupRelease,descParams,resource);
	}

	if (result)
	{
		RadioResource* res = getNextResource(resource);
		m_observerHelper.notifyChildWindows(WM_CONNECT_GROUP,(WPARAM)res, (LPARAM) RELEASED);
	}

	FUNCTION_EXIT;
	return result;
}


void RadioOperations::auditEventPosted(const TA_Base_Core::MessageType& messageType,const TA_Base_Core::DescriptionParameters& descParams)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("auditEventPosted");
	
	TA_Base_Core::DescriptionParameters decoratedDesc(descParams);
	TA_Base_Core::NameValuePair param1("Value"," ");
	decoratedDesc.insert(decoratedDesc.begin(), &param1);
    
    try
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling auditsender");
		if (m_auditSender!=NULL)
		{
			m_auditSender->sendAuditMessage(
				messageType,
				m_entityKey,
				decoratedDesc,
				"",
				m_session,
				"",
				"",
				"");
		}
    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugError, "An unknown exception was caught while "
            "attempting to send an audit message");
    }

	// Fixed TD#14297
	int messageID = 0;

	if ((!descParams.empty()) && (descParams[0]->name == "MessageID"))
	{
		messageID = atoi(descParams[0]->value.c_str());
	}

	m_observerHelper.notifyChildWindows(WM_AUDIT_EVENT_OCCURRED,messageID,(LPARAM)messageType.getTypeKey().c_str());

	FUNCTION_EXIT;
}

void RadioOperations::auditEventPosted(
	const TA_Base_Core::MessageType& messageType,
	const TA_Base_Core::DescriptionParameters& descParams,
	RadioResource& resource)  
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("auditEventPosted");
	
	TA_Base_Core::DescriptionParameters decoratedDesc(descParams);
	TA_Base_Core::NameValuePair param1("Value"," ");
	decoratedDesc.insert(decoratedDesc.begin(), &param1);
    
    try
    {
		if (m_auditSender!=NULL)
		{
			m_auditSender->sendAuditMessage(
				messageType,
				m_entityKey,
				decoratedDesc,
				"",
				m_session,
				"",
				"",
				"");
		}
    }
    catch(...)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,TA_Base_Core::DebugUtil::DebugError, "An unknown exception was caught while "
            "attempting to send an audit message");
    }

	// add resource & message type to display data in History Page
	RadioResource* res = getNextResource(resource);
	m_observerHelper.notifyChildWindows(WM_AUDIT_EVENT_OCCURRED,(WPARAM)res,(LPARAM)messageType.getTypeKey().c_str());

	FUNCTION_EXIT;
}

RadioResource* RadioOperations::getNextResource(RadioResource& resource)
{
    RadioResource* res = &(m_resourceList[m_resourceListCurrentIndex++]);

    if (m_resourceListCurrentIndex >= MAX_RESOURCE_SIZE)
    {
        m_resourceListCurrentIndex = 0;
    }

    *res = resource;
    return res;
}


void RadioOperations::databaseReadyEvent()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("databaseReadyEvent");
	
	m_observerHelper.notifyChildWindows(WM_DATABASE_READY);
	connected = true;

    FUNCTION_EXIT;
}

//fixed TD14271
void RadioOperations::initialiseMode()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("initialiseMode");
    
	m_observerHelper.notifyChildWindows(WM_INITIALISE_MODE);
	
	FUNCTION_EXIT;
}

void RadioOperations::apiFailedMode()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("apiFailedMode");

	// initilize immediatly the initialization dialong
	// and hide the effect of restart
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"Initializing Init Dialog");
	m_observerHelper.notifyChildWindows(WM_INITIALISE_MODE);
	
	if (connected)
	{
		if (!notifyConFail)
		{
			notifyConFail = true;
			m_observerHelper.notifyChildWindows(WM_APIFAILURE_MODE,1);
		}
	} 
	else if (!connectFail) 
	{
		connectFail = true;    
		m_observerHelper.notifyChildWindows(WM_APIFAILURE_MODE);
	}
	
	FUNCTION_EXIT;
}

void RadioOperations::connectionFail()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("connectionFail");
	
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				"Initializing Init Dialog");
	m_observerHelper.notifyChildWindows(WM_INITIALISE_MODE);
	
	if (connected) {
		if (!notifyConFail){
		notifyConFail = true;
		m_observerHelper.notifyChildWindows(WM_APIFAILURE_MODE,1);
		}
	} else
	if (!connectFail) {
		connectFail = true;    
		m_observerHelper.notifyChildWindows(WM_APIFAILURE_MODE);
	}
	
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
		"Calling Restart Connection");

	The_RadioSystemStatus::instance()->restartConnection();

	FUNCTION_EXIT;
}

// TD16926
void RadioOperations::launchInboxPage()
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("launchInboxPage");

	//notifyChildWindows - WM_TEXTMSG_RECEIVED
	m_observerHelper.notifyChildWindows(WM_TEXTMSG_RECEIVED,1);
	
	FUNCTION_EXIT;
}
//TD18869
void RadioOperations::entityBeingDeleted(RadioResourceType rType)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("entityBeingDeleted");

	m_observerHelper.notifyChildWindows(WM_ENTITY_BEING_DELETED,(WPARAM)rType);

	FUNCTION_EXIT;

}
//TD18869

void RadioOperations::makeEmergencyCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("makeEmergencyCall");

	The_RadioSystemStatus::instance()->beginEmergency(resource);

	TA_Base_Core::DescriptionParameters descParams;
	std::stringstream ss;
	ss << resource.id;
	TA_Base_Core::NameValuePair param1("Radio ID",ss.str());
	descParams.push_back(&param1);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioInitiateEmergency,descParams,resource);

	FUNCTION_EXIT;
}

void RadioOperations::endEmergencyCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("endEmergencyCall");

	The_RadioSystemStatus::instance()->knockdownEmergency(resource);

	FUNCTION_EXIT;
}

void RadioOperations::ackEmergencyCall(RadioResource &resource)
{	
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("ackEmergencyCall");

	The_RadioSystemStatus::instance()->acknowledgeEmergency(resource);

	FUNCTION_EXIT;
}


bool RadioOperations::includeCall(RadioResource& resource, RadioResource& dynResource)
{
	FUNCTION_ENTRY("includeCall");

	std::string talkgroupAlias = "";

	std::vector<RadioResource> mems = The_RadioSystemStatus::instance()->getResourceMembers(dynResource);

	std::vector<RadioResource>::iterator iter = mems.begin();
	for (;iter!=mems.end(); iter++) 
		if ((*iter).id == resource.id)
			break;

	if (iter == mems.end())	
		mems.push_back(resource);

	RadioResource dgTalkgroup;
	saveEntity(dynResource, mems);
	{
		std::vector<RadioResource>::iterator iter = mems.begin();
		for ( ; iter != mems.end(); ++iter)
		{
			if (RR_TALKGROUP == (*iter).type) {
				talkgroupAlias = (*iter).alias;
				dgTalkgroup = (*iter);
				break;
			}
		}
		if (iter != mems.end()) {
			mems.erase(iter);
		}
	}

	// if DGNA is ongoing
	//if (The_RadioSystemStatus::instance()->getResource_isGroupConnected(dgTalkgroup) == CONNECTED) {
		if (talkgroupAlias == "") {
			::MessageBox(NULL, "Talkgroup is not assigned to DGNA", "Error connecting", MB_OK|MB_ICONERROR);
			return false;
		}
		try {
			The_RadioSystemStatus::instance()->connectDynamicRegroup(resource, talkgroupAlias);
		}
		catch(TA_IRS_App::TcpMethodTimeoutException& e)
		{
			AddErrorMessage("Unable to establish Connection with the Radio System");
		}
		catch(TA_IRS_App::TcpException& e)
		{
			AddErrorMessage("Unable to establish Connection with the Radio System");
		}
		catch(const CORBA::Exception& e)
		{			
			AddErrorMessage("Agent cannot be contacted");
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			AddErrorMessage("Agent cannot be contacted");
		}

	//}

	FUNCTION_EXIT;
	return true;
}


bool RadioOperations::connectDynamicRegroup(RadioResource& talkgroupResource, std::vector<RadioResource>& oldMembers, std::vector<RadioResource>& newMembers)
{
	FUNCTION_ENTRY("connectDynamicRegroup");

	std::string talkgroupAlias = talkgroupResource.alias;

	
	std::vector<RadioResource>::iterator oldMember;
	std::vector<RadioResource>::iterator newMember;
	std::vector<RadioResource> newFilters;
	std::vector<RadioResource> oldFilters;
	bool newAdd;	
	bool status = true;

	std::vector<RadioResource>::iterator filter;
	std::stringstream ss;
	std::string s;

	oldMember = oldMembers.begin();
	RadioResource oldTalkgroupResource;
	oldTalkgroupResource.alias = "";
	for(;oldMember!=oldMembers.end();oldMember++) {
		if (RR_TALKGROUP == (*oldMember).type) {
			oldTalkgroupResource = *oldMember;
			break;
		}
	}

	if (oldTalkgroupResource.alias != "" && oldTalkgroupResource.alias != talkgroupResource.alias) 
	{
		oldMember = oldMembers.begin();
		for (; oldMember!=oldMembers.end(); oldMember++) {
			// call radiosystemstatus to release dynamic regroup with old talkgroup resource
			if (RR_TALKGROUP != (*oldMember).type) {
				try {
					status = The_RadioSystemStatus::instance()->releaseDynamicRegroup(*oldMember, oldTalkgroupResource.alias);
				}
				catch(TA_IRS_App::TcpException& e)
				{
					AddErrorMessage("Unable to establish Connection with the Radio System");
					LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
					status = false;
				}
				catch(const TA_Base_Core::ObjectResolutionException& e)	
				{			
					AddErrorMessage("Agent cannot be contacted");
					LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
					status = false;
				}
				catch(TA_Base_Core::TransactiveException& e)
				{
					AddErrorMessage("Unable to establish Connection with the Radio System");
					LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
					status = false;
				}
			}
			if (!status) {
				cancelReleaseDynGroup(oldTalkgroupResource.alias, oldMembers, oldMember);
				break;
			}
		}

		if (status) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Success adding the new members of DGNA");

			newMember = newMembers.begin();
			for(; newMember!=newMembers.end(); newMember++) {
				// call radiosystemstatus to connect all of members to new talkgroup resource
				if (RR_TALKGROUP != (*newMember).type) {
					try {
						status = The_RadioSystemStatus::instance()->connectDynamicRegroup(*newMember, talkgroupResource.alias);
						ss << (*newMember).id;
						ss << ",";			
					}
					catch(TA_IRS_App::TcpException& e)
					{
						AddErrorMessage("Unable to establish Connection with the Radio System");
						LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
						status = false;
					}
					catch(const TA_Base_Core::ObjectResolutionException& e)	
					{			
						AddErrorMessage("Agent cannot be contacted");
						LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
						status = false;
					}
					catch(TA_Base_Core::TransactiveException& e)
					{
						AddErrorMessage("Unable to establish Connection with the Radio System");
						LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
						status = false;
					}
				}
				if (!status) {
					cancelReleaseDynGroup(oldTalkgroupResource.alias, oldMembers, oldMembers.end());
					cancelConnectDynGroup(talkgroupResource.alias, newMembers, newMember);
					break;
				}
			}
			if (status) {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Success deleting the old members of DGNA");
			} else {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed deleting old members of DGNA. Revert back");
			}
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed adding new members of DGNA. Revert back to original one");
		}

		// Post audit events code or assign filter to the old one
	} else {
		// old flow
		newMember = newMembers.begin();
		
		for (; newMember!=newMembers.end(); newMember++) {
			oldMember = oldMembers.begin();
			newAdd = true;
			if ((*newMember).type == RR_TALKGROUP) continue;
			for (; oldMember!=oldMembers.end(); oldMember++) {
				if (((*oldMember).id == (*newMember).id &&
					(*oldMember).type == (*newMember).type) ||
					(*newMember).type == RR_TALKGROUP) {
						newAdd = false;
						break;
				}
			}
			if (newAdd) {
				newFilters.push_back(*newMember);
			}
		}
		
		filter = newMembers.begin();
		for (; filter!=newMembers.end(); filter++) {
			ss << (*filter).id;
			ss << ",";
		}

		filter = newFilters.begin();
		for (; filter!=newFilters.end(); filter++) {
			try {
				status = The_RadioSystemStatus::instance()->connectDynamicRegroup(*filter, talkgroupResource.alias);
			}		
			catch(TA_IRS_App::TcpException& e)
			{
				AddErrorMessage("Unable to establish Connection with the Radio System");
				LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
				status = false;
			}
			catch(const TA_Base_Core::ObjectResolutionException& e)	
			{			
				AddErrorMessage("Agent cannot be contacted");
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
				status = false;
			}
			catch(TA_Base_Core::TransactiveException& e)
			{
				AddErrorMessage("Unable to establish Connection with the Radio System");
				LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
				status = false;
			}
			if (!status) {
				// rollback operation
				cancelConnectDynGroup(talkgroupResource.alias, newFilters, filter);
				break;
			}
		}

		if (status) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Success adding the new members of DGNA");
			oldMember = oldMembers.begin();

			for (; oldMember!=oldMembers.end(); oldMember++) {
				newMember = newMembers.begin();
				newAdd = true;
				if ((*oldMember).type == RR_TALKGROUP) continue;
				for (; newMember!=newMembers.end(); newMember++) {
					if (((*oldMember).id == (*newMember).id &&
						(*oldMember).type == (*newMember).type) ||
						(*oldMember).type == RR_TALKGROUP) {
							newAdd = false;
							break;
					}
				}
				if (newAdd) {
					oldFilters.push_back(*oldMember);
				}
			}

			filter = oldFilters.begin();
			for (; filter!=oldFilters.end(); filter++) {
				try {
					status = The_RadioSystemStatus::instance()->releaseDynamicRegroup(*filter, talkgroupResource.alias);
				}		
				catch(TA_IRS_App::TcpException& e)
				{
					AddErrorMessage("Unable to establish Connection with the Radio System");
					LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
					status = false;
				}
				catch(const TA_Base_Core::ObjectResolutionException& e)	
				{			
					AddErrorMessage("Agent cannot be contacted");
					LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
					status = false;
				}
				catch(TA_Base_Core::TransactiveException& e)
				{
					AddErrorMessage("Unable to establish Connection with the Radio System");
					LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
					status = false;
				}
				if (!status) {
					// rollback operation
					cancelConnectDynGroup(talkgroupResource.alias, newFilters, newFilters.end());
					cancelReleaseDynGroup(talkgroupResource.alias, oldFilters, filter);
					break;
				}
			}
			if (status) {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Success deleting the old members of DGNA");
			} else {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed deleting old members of DGNA. Revert back");
			}
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed adding new members of DGNA. Revert back to original one");
		}
	}
	
	TA_Base_Core::DescriptionParameters descParams;
		
	s = ss.str();
	TA_Base_Core::NameValuePair param2("Member", s.substr(0, s.size()-1));
	descParams.push_back(&param2);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioDynamicRegroupConnect,descParams);

	FUNCTION_EXIT;
	return status;
}

bool RadioOperations::releaseDynamicRegroup(RadioResource& dynResource, std::string talkgroupAlias, unsigned long radioId) {
	std::vector<RadioResource> members = The_RadioSystemStatus::instance()->getResourceMembers(dynResource);
	std::vector<RadioResource>::iterator iter = members.begin();
	bool newAdd = true;
	bool status = true;
	for (; iter!=members.end(); iter++) {
		newAdd = true;
		if ((*iter).id == radioId) {
				newAdd = false;
				break;
		}
	}
	RadioResource res;
	res.id = radioId;
	res.ssi = radioId;
	res.type = RR_RADIO;
	if (!newAdd) {
		try {
			status = The_RadioSystemStatus::instance()->releaseDynamicRegroup(res, talkgroupAlias);
		}
		catch(TA_IRS_App::TcpException& e)
		{
			AddErrorMessage("Unable to establish Connection with the Radio System");
			LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
			status = false;
		}
		catch(const TA_Base_Core::ObjectResolutionException& e)	
		{			
			AddErrorMessage("Agent cannot be contacted");
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
			status = false;
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			AddErrorMessage("Unable to establish Connection with the Radio System");
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			status = false;
		}
	}
	return status;
}

void RadioOperations::cancelDeleteDynGroup(RadioResource& dynResource, std::string talkgroupAlias, std::vector<RadioResource>& members, std::vector<RadioResource>::iterator errorMember)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("cancelDeleteDynGroup");
	
	std::vector<RadioResource> oriMembers = The_RadioSystemStatus::instance()->getResourceMembers(dynResource);
	std::vector<RadioResource>::iterator oriIter;
	std::vector<RadioResource>::iterator resIter = errorMember;
	bool newAdd = true;
	RadioResource res;
	bool status = true;

	while (resIter != members.begin()) {
		--resIter;

		newAdd = true;
		oriIter = oriMembers.begin();
		for (; oriIter!=oriMembers.end(); oriIter++) {
			if ((*oriIter).id == (*resIter).id) {
				newAdd = false;
				break;
			}
		}

		if (!newAdd) {
			res.id = (*resIter).id;
			res.ssi = (*resIter).ssi;
			res.alias = (*resIter).alias;
			res.type = RR_RADIO;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Restore connecting back DGNA group radio alias %s to talkgroup %s", res.alias.c_str(), talkgroupAlias.c_str());
			try {
				status = The_RadioSystemStatus::instance()->connectDynamicRegroup(res, talkgroupAlias);
			}
			catch(TA_IRS_App::TcpException& e)
			{
				AddErrorMessage("Unable to establish Connection with the Radio System");
				LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
				status = false;
			}
			catch(const TA_Base_Core::ObjectResolutionException& e)	
			{			
				AddErrorMessage("Agent cannot be contacted");
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
				status = false;
			}
			catch(TA_Base_Core::TransactiveException& e)
			{
				AddErrorMessage("Unable to establish Connection with the Radio System");
				LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
				status = false;
			}
		}
		if (!status) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed restoring connection of DGNA radio alias %s to talkgroup %s", res.alias.c_str(), talkgroupAlias.c_str());
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Successfully restoring connection of DGNA radio alias %s to talkgroup %s", res.alias.c_str(), talkgroupAlias.c_str());
		}
	}


	FUNCTION_EXIT;
}

void RadioOperations::cancelConnectDynGroup(std::string talkgroupAlias, std::vector<RadioResource>& members, std::vector<RadioResource>::iterator errorMember)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("cancelConnectDynGroup");

	std::vector<RadioResource>::iterator resIter = errorMember;
	RadioResource res;
	bool status = true;

	while (resIter != members.begin()) {
		--resIter;
		res.id = (*resIter).id;
		res.ssi = (*resIter).ssi;
		res.alias = (*resIter).alias;
		res.type = RR_RADIO;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Restore release DGNA group radio alias %s to talkgroup %s", res.alias.c_str(), talkgroupAlias.c_str());
		status = true;
		try {
			status = The_RadioSystemStatus::instance()->releaseDynamicRegroup(res, talkgroupAlias);
		}
		catch(TA_IRS_App::TcpException& e)
		{
			AddErrorMessage("Unable to establish Connection with the Radio System");
			LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
			status = false;
		}
		catch(const TA_Base_Core::ObjectResolutionException& e)	
		{			
			AddErrorMessage("Agent cannot be contacted");
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
			status = false;
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			AddErrorMessage("Unable to establish Connection with the Radio System");
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			status = false;
		}
		if (!status) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed deleting of DGNA radio alias %s to talkgroup %s", res.alias.c_str(), talkgroupAlias.c_str());
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Successfully deleting of DGNA radio alias %s to talkgroup %s", res.alias.c_str(), talkgroupAlias.c_str());
		}
	}
	FUNCTION_EXIT;
}

void RadioOperations::cancelReleaseDynGroup(std::string talkgroupAlias, std::vector<RadioResource>& members, std::vector<RadioResource>::iterator errorMember)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
	FUNCTION_ENTRY("cancelReleaseDynGroup");

	std::vector<RadioResource>::iterator resIter = errorMember;
	RadioResource res;
	bool status = true;

	while (resIter != members.begin()) {
		--resIter;
		res.id = (*resIter).id;
		res.ssi = (*resIter).ssi;
		res.alias = (*resIter).alias;
		res.type = RR_RADIO;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Restore connectin DGNA group radio alias %s to talkgroup %s", res.alias.c_str(), talkgroupAlias.c_str());
		status = true;
		try {
			status = The_RadioSystemStatus::instance()->connectDynamicRegroup(res, talkgroupAlias);
		}
		catch(TA_IRS_App::TcpException& e)
		{
			AddErrorMessage("Unable to establish Connection with the Radio System");
			LOG_EXCEPTION_CATCH(SourceInfo, "TcpException", e.what());
			status = false;
		}
		catch(const TA_Base_Core::ObjectResolutionException& e)	
		{			
			AddErrorMessage("Agent cannot be contacted");
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", e.what());
			status = false;
		}
		catch(TA_Base_Core::TransactiveException& e)
		{
			AddErrorMessage("Unable to establish Connection with the Radio System");
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			status = false;
		}
		if (!status) {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed connecting DGNA radio alias %s to talkgroup %s", res.alias.c_str(), talkgroupAlias.c_str());
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Successfully connecting DGNA radio alias %s to talkgroup %s", res.alias.c_str(), talkgroupAlias.c_str());
		}
	}
	FUNCTION_EXIT;
}

bool RadioOperations::callForwarding(bool cond, unsigned long forwardingISSI, unsigned long forwardedISSI)
{
	FUNCTION_ENTRY("callForwarding");

	bool status = The_RadioSystemStatus::instance()->callForwarding(cond, forwardingISSI, forwardedISSI);

	//if (status) {
	//	setCallForwardingState(1);
	//} else {
	//	setCallForwardingState(0);
	//}

	TA_Base_Core::DescriptionParameters descParams;
	std::string s;
	if (cond) {
		s = "Immediate";
	} else {
		s = "Not Reachable";
	}
	
	TA_Base_Core::NameValuePair param1("Condition", s);
		descParams.push_back(&param1);
	std::stringstream ss;
	ss << forwardedISSI;
	TA_Base_Core::NameValuePair param2("Radio ID",ss.str());
	descParams.push_back(&param2);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioSetCallForwarding,descParams);

	FUNCTION_EXIT;
	return status;
}

bool RadioOperations::cancelCallForwarding(unsigned long ISSI)
{
	FUNCTION_ENTRY("cancelCallForwarding");
	
	bool status = The_RadioSystemStatus::instance()->cancelCallForwarding(ISSI);

	//if (status) {
	//	setCallForwardingState(0);
	//}

	TA_Base_Core::DescriptionParameters descParams;
	//std::stringstream ss;
	//ss << ISSI;
	//TA_Base_Core::NameValuePair param2("Radio ID",ss.str());
	//descParams.push_back(&param2);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioCallForwardingCancel,descParams);

	FUNCTION_EXIT;
	return status;
}


void RadioOperations::makeDriverCall(unsigned long trainId)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("makeDriverCall");

	TA_IRS_Bus::TrainInformationTypes::TrainDetails tDetails;
	try {
		 tDetails = m_trainAgent->getTrainDetails((char)trainId);
	}
	catch(TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
		return;
	}
	// get primaryTSI/radio id from train_agent
	RadioResource resource;
	
	std::stringstream out;
	out << trainId;
	resource.alias = "Train " + out.str();
	resource.id = atol(tDetails.primaryTsi.c_str());
	resource.ssi = atol(tDetails.primaryTsi.c_str());
	resource.type = RR_TRAIN;

	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Make Driver Call, Calling Train: Resource Alias : %s,Resource Id :%d",
				resource.alias.c_str(),resource.id);
	bool bResult = m_trainAgentIF->answerDriverCall(trainId);

	if (bResult)
	{
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Make Driver Call: Resource Alias : %s,Resource Id :%d Succeeded performing audit",
					resource.alias.c_str(),resource.id);
		RadioResource* res = getNextResource(resource);
		m_observerHelper.notifyChildWindows(WM_OUTGOING_CALL,(WPARAM)res);

		TA_Base_Core::DescriptionParameters descParams;
		
		// Event Audit for private call
		std::stringstream ss;
		ss << resource.id;
		TA_Base_Core::NameValuePair param1("Radio ID",ss.str());
		descParams.push_back(&param1);

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Make Driver Call: Resource Alias : %s,Resource Id :%d",
					resource.alias.c_str(),resource.id);


		auditEventPosted(TA_Base_Core::RadioAudit::RadioMakePrivateCall,descParams,resource);
	}
	else
	{
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Exception occured, driver call cannot proceed");
	}
	
	
	FUNCTION_EXIT;
}

void RadioOperations::makeDriverCall(RadioResource& resource)
{
    FUNCTION_ENTRY("makeDriverCallFromCallStack");
	
	unsigned long trainId = driverCallState[resource.id];

	if (0 == trainId) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Train Id not found for radio Id %u, with alias %s", resource.ssi, resource.alias.c_str());
		return;	
	}

	// get primaryTSI/radio id from train_agent
	RadioResource trainResource = resource;
	
	std::stringstream out;
	out << trainId;
	trainResource.alias = "Train " + out.str();
	trainResource.type = RR_TRAIN;
	
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Make Driver Call, Calling Train: Resource Alias : %s,Resource Id :%d",
				resource.alias.c_str(),resource.id);
	bool bResult = m_trainAgentIF->answerDriverCall(trainId);

	if ( bResult)
	{
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Make Driver Call: Resource Alias : %s,Resource Id :%d Succeeded performing audit",
					 resource.alias.c_str(),resource.id);
		RadioResource* res = getNextResource(resource);

		m_observerHelper.notifyChildWindows(WM_OUTGOING_CALL,(WPARAM)res);
	
		// Event Audit for private call
		TA_Base_Core::DescriptionParameters descParams;

		std::stringstream ss;
		ss << resource.id;
		TA_Base_Core::NameValuePair param1("Radio ID",ss.str());
		descParams.push_back(&param1);

		auditEventPosted(TA_Base_Core::RadioAudit::RadioMakePrivateCall,descParams,resource);
	}
	else
	{
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Exception occured, driver call cannot proceed");
	}


	FUNCTION_EXIT;
}

void RadioOperations::endDriverCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("endDriverCall");
	bool callForwarding = getCallForwardingState();
	setCallForwardingState(false);

	The_RadioSystemStatus::instance()->notifyEndDriverCall(resource);

	setCallForwardingState(callForwarding);

	FUNCTION_EXIT;
}

void RadioOperations::onIncomingDriverCall(unsigned long trainId)
{	
	FUNCTION_ENTRY("onIncomingDriverCall");
	TA_Base_Core::ThreadGuard guard( m_lock );
	
	TA_IRS_Bus::TrainInformationTypes::TrainDetails tDetails;
	try{
		 tDetails = m_trainAgent->getTrainDetails((char)trainId);
	}
	catch(TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
		return;
	}

	RadioResource resource, trainResource;
	
	std::stringstream out;
	out << trainId;
	resource.alias = "Train " + out.str();
	resource.id = atol(tDetails.primaryTsi.c_str());
	resource.ssi = trainId;
	resource.type = RR_TRAIN;

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"onIncomingDriverCall : Train Calling : %d with resource id : %d",
				trainId,resource.id);

	driverCallState[resource.id] = trainId;

	RadioResource* res = getNextResource(resource);
	
	trainResource = resource;
	trainResource.type = RR_RADIO;

	The_RadioSystemStatus::instance()->setResource_isInPrivateCall(trainResource,PC_INCOMING);

	m_observerHelper.notifyChildWindows(WM_INCOMING_DRIVER_CALL,(WPARAM)res);

	FUNCTION_EXIT;
}

void RadioOperations::onResetDriverCall(unsigned long trainId,bool bFailure)
{
	FUNCTION_ENTRY("onResetDriverCall");
	TA_Base_Core::ThreadGuard guard( m_lock );
	
	TA_IRS_Bus::TrainInformationTypes::TrainDetails tDetails;
	try {
		 tDetails = m_trainAgent->getTrainDetails((char)trainId);
	}
	catch(TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
		return;
	}

	RadioResource resource, trainResource;
	
	std::stringstream out;
	out << trainId;
	resource.alias = "Train " + out.str();
	resource.id = atol(tDetails.primaryTsi.c_str());
	resource.ssi = trainId;
	resource.type = RR_TRAIN;

	RadioResource* res = getNextResource(resource);
	
	trainResource = resource;
	trainResource.type = RR_RADIO;
	
	// handle UI update during driver call reset due to failure
	// or normal call reset
	if(!bFailure)
	{
		if (The_RadioSystemStatus::instance()->getResource_isInPrivateCall(trainResource) != PC_ESTABLISHED)
		{
			The_RadioSystemStatus::instance()->setResource_isInPrivateCall(trainResource,PC_NOT_INVOLVED);
		}
	}
	
	m_observerHelper.notifyChildWindows(WM_ESTABLISHED_DRIVER_CALL,(WPARAM)res);

	FUNCTION_EXIT;
}

void RadioOperations::entityEmptySelection(RadioResourceType type)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("entityEmptySelection");
    
	m_observerHelper.notifyChildWindows(WM_ENTITY_EMPTY_SELECTION,(WPARAM)type, (LPARAM) 0);
	
	FUNCTION_EXIT;
}

void RadioOperations::startSubsMonitoring(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("startSubsMonitoring");

	if (RR_RADIO == resource.type) {
		RadioResource* res = getNextResource(resource);

		m_observerHelper.notifyChildWindows(WM_START_SUBS_MONITORING, (WPARAM)0, (LPARAM) 0);
	}

	FUNCTION_EXIT;
}

void RadioOperations::stopSubsMonitoring(RadioResource& resource)
{	
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("startSubsMonitoring");

	if (RR_RADIO == resource.type) {
		RadioResource* res = getNextResource(resource);

		m_observerHelper.notifyChildWindows(WM_STOP_SUBS_MONITORING, (WPARAM)0, (LPARAM) 0);
	}

	FUNCTION_EXIT;
}

void RadioOperations::setCallForwardingState(short state)
{
	FUNCTION_ENTRY("setCallForwardingState");
	m_CallForwardingState = state;
	FUNCTION_EXIT;
}

short RadioOperations::getCallForwardingState()
{
	FUNCTION_ENTRY("getCallForwardingState");
	FUNCTION_EXIT;
	return m_CallForwardingState;
}

void RadioOperations::setTrainProxy(TA_IRS_Bus::TrainInformationCorbaProxyPtr trainAgent) 
{
	m_trainAgent = trainAgent;
	m_trainAgentIF = new TA_IRS_App::TrainAgentIF ();	

}

unsigned long RadioOperations::isDriverCall(unsigned long id) {
	return driverCallState[id];
}

void RadioOperations::setNoDriverCall(unsigned long id) {
	driverCallState[id] = 0;
}

void RadioOperations::cancelApiOperation(ApiState state, RadioResource resource)
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("cancelApiOperation");

	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_OPERATION_FAIL, (WPARAM)res, (LPARAM) state);


	FUNCTION_EXIT;
}

void RadioOperations::notifyProfileUpdate(std::vector<std::string>& tgs)
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("notifyProfileUpdate");
	m_profileTgName.clear();

	std::vector<std::string>::iterator iter = tgs.begin();
	for(; iter != tgs.end(); iter++) 
	{
		m_profileTgName.push_back(*iter);
	}

	m_observerHelper.notifyChildWindows(WM_PROFILE_UPDATE, (WPARAM)0, (LPARAM)0);
	FUNCTION_EXIT;
}

std::string RadioOperations::getProfileTgName(short butRef) 
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("getProfileTg");
	std::string resName = "";
	if (butRef > m_profileTgName.size()) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Profile TG for %u not found on the list (size = %u)", butRef, m_profileTgName.size());
		return resName;
	}
	resName = m_profileTgName[butRef-1];
	FUNCTION_EXIT;
	return resName;
}

bool RadioOperations::isProfileTgExist(const RadioResource& res)
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("isProfileTgExist");
	std::vector<std::string>::iterator iter = m_profileTgName.begin();
	for (; iter!=m_profileTgName.end(); iter++)
	{
		if (res.alias.compare(*iter) == 0) return true;
	}
	FUNCTION_EXIT;
	return false;
}

void RadioOperations::notifyDbChange(RadioResource &oldResource, RadioResource &newResource)
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("notifyDbChange");

	if (RR_NIL != oldResource.type && RR_NIL != newResource.type) 
	{
		// update resource
		// if need
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Updating Resource : %d",oldResource.id);
		The_RadioSystemStatus::instance()->updateResource(oldResource, newResource);
		notifyDbUpdate(oldResource, newResource);
	} 
	else if (RR_NIL == oldResource.type && RR_NIL != newResource.type) 
	{
		// new resource
		The_RadioSystemStatus::instance()->addNewResource(newResource);
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Adding Resource : %s",newResource.alias.c_str());
		notifyDbNew(newResource);
	} 
	else if (RR_NIL != oldResource.type && RR_NIL == newResource.type) 
	{
		// delete resource
		The_RadioSystemStatus::instance()->deleteResource(oldResource);
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Deleting Resource : %d",oldResource.id);
		notifyDbDelete(oldResource);
	} 

	FUNCTION_EXIT;
}

void RadioOperations::notifyDbUpdate(RadioResource& oldResource, RadioResource& newResource)
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("notifyDbUpdate");
	RadioResource* oldRes = getNextResource(oldResource);
	RadioResource* newRes = getNextResource(newResource);
	m_observerHelper.notifyChildWindows(WM_DB_UPDATE, (WPARAM) oldRes, (LPARAM) newRes);
	FUNCTION_EXIT;
}

void RadioOperations::notifyDbDelete(RadioResource& oldResource)
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("notifyDbDelete");
	RadioResource* res = getNextResource(oldResource);
	m_observerHelper.notifyChildWindows(WM_DB_DELETE, (WPARAM) res, (LPARAM) 0);
	FUNCTION_EXIT;
}

void RadioOperations::notifyDbNew(RadioResource& newResource)
{
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("notifyDbNew");
	RadioResource* res = getNextResource(newResource);
	m_observerHelper.notifyChildWindows(WM_DB_NEW, (WPARAM) res, (LPARAM) 0); 
	FUNCTION_EXIT;
}

void RadioOperations::notifyDbFinish()
{	
	TA_Base_Core::ThreadGuard guard(m_lock);
	FUNCTION_ENTRY("notifyDbFinish");
	m_observerHelper.notifyChildWindows(WM_DB_FINISH, (WPARAM) 0, (LPARAM) 0);
	FUNCTION_EXIT;
}

void RadioOperations::removeResource(RadioResource& resource)
{	
	TA_Base_Core::ThreadGuard guard(m_lock);

	FUNCTION_ENTRY("removeResource");

	if ( RR_RADIO == resource.type ) {
		RadioResource* res = getNextResource(resource);
		m_observerHelper.notifyChildWindows(WM_RESOURCE_REMOVE, (WPARAM)res, (LPARAM) 0);
	}

	FUNCTION_EXIT;
}

void RadioOperations::sendDGNAAuditMsge(RadioResource& resource,DGNA_AuditMsgType DGNAMsgeType)
{
	TA_Base_Core::ThreadGuard guard(m_lock);

	FUNCTION_ENTRY("sendDGNAAuditMsge");

	TA_Base_Core::DescriptionParameters descParams;
	std::stringstream ss;
	ss << resource.alias;
	TA_Base_Core::NameValuePair param1("DynamicGroupName",ss.str());
	descParams.push_back(&param1);

	std::vector<RadioResource> dgnaEntry = The_RadioSystemStatus::instance()->getResourceCollectioForType(RR_DYNGROUP);


	if ( CREATE_DYNAMIC_GROUP == DGNAMsgeType )
	{
		//Check if DGNA alias is already exist
		std::vector<RadioResource>::iterator iterDGNA = dgnaEntry.begin();
		bool isDGNAExist = false;

		for( iterDGNA; iterDGNA != dgnaEntry.end(); iterDGNA++ )
		{
			if ( (*iterDGNA).alias.compare(resource.alias) == 0)
			{
				//Found DGNA alias 
				isDGNAExist = true;
				break;
			}
		}
		
		//Send Audit Message for RadioDynamicGroupCreated
		if ( false == isDGNAExist )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Sending AuditMessage for event type RadioDynamicGroupCreated, alias : %s",resource.alias.c_str());
			auditEventPosted(TA_Base_Core::RadioAudit::RadioDynamicGroupCreated,descParams,resource);
		}
	}
	else if ( RELEASE_DYNMIC_GROUP == DGNAMsgeType )
	{
		//Send Audit Message for RadioDynamicGroupReleased
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Sending AuditMessage for event type RadioDynamicGroupReleased, alias : %s",resource.alias.c_str());
		auditEventPosted(TA_Base_Core::RadioAudit::RadioDynamicGroupReleased,descParams,resource);
	}

	FUNCTION_EXIT;
}
