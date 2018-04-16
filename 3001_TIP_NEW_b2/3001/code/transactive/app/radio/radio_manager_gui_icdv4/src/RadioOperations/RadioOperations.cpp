/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/radiomanager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h"

#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus\trains\TrainCorbaProxy\src\TrainInformationCorbaProxy.h"
#include "bus\trains\TrainCommonLibrary\src\CommonTypes.h"
#include "bus\trains\TrainCommonLibrary\src\TrainInformationTypes.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/RadioAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"


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
	m_entityKey(0)
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
    FUNCTION_EXIT;
	
}



void RadioOperations::initialise()
{
    FUNCTION_ENTRY("initialise");

	m_session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

    m_entityKey = RadioMFTAgentHelper::getRadioMFTAgentEntityKey();

	m_auditSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::RadioAudit::Context);

	//m_trainAgent = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainInformationCorbaProxy();

	connectFail = false;
	connected = false;
	notifyConFail = false;

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
		auditEventPosted(TA_Base_Core::RadioAudit::RadioEmergencyEnded,descParams,radioUnit);
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
    
	// TD17224
	TA_Base_Core::DescriptionParameters descParams;

	//std::ostringstream radioId;
	//radioId << radioUnit.ssi;

	//std::ostringstream rid;
	//rid << radioUnit.id;

	//TA_Base_Core::NameValuePair param1("Radio ID",radioId.str());
	//TA_Base_Core::NameValuePair param2("ResourceId",rid.str());
 //   descParams.push_back(&param1);
	//descParams.push_back(&param2);

	std::stringstream ss;
	ss << resource.id;

	TA_Base_Core::NameValuePair param1("Radio ID", ss.str());
	descParams.push_back(&param1);


	radioUnit.id = radioUnit.ssi;
	auditEventPosted(TA_Base_Core::RadioAudit::RadioEmergencyAcknowledged,descParams,radioUnit);
	// TD17224

	RadioResource* res = getNextResource(resource);

	RadioResource* rUnit = getNextResource(radioUnit); // TD18118
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


void RadioOperations::onPredefinedMessageEvent(RadioUpdateMessageType eventType, PredefinedMessage* messageData)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("onPredefinedMessageEvent");
    
	switch(eventType)
	{
		case RM_NEW:
			predefinedMessageAdded(messageData);
			break;
		case RM_UPDATE:
			predefinedMessageModified(messageData);
			break;
		case RM_DELETE:
			predefinedMessageRemoved(messageData);
	}

	FUNCTION_EXIT;
}


void RadioOperations::predefinedMessageAdded(PredefinedMessage* messageData)
{
    FUNCTION_ENTRY("predefinedMessageAdded");
    
	//notifyChildWindows - WM_PREDEFINEDMSG_ADDED
	m_observerHelper.notifyChildWindows(WM_PREDEFINEDMSG_ADDED,(WPARAM)messageData);

	FUNCTION_EXIT;
}


void RadioOperations::predefinedMessageModified(PredefinedMessage* messageData)
{
    FUNCTION_ENTRY("predefinedMessageModified");
    	
	//notifyChildWindows - WM_PREDEFINEDMSG_MODIFIED
	m_observerHelper.notifyChildWindows(WM_PREDEFINEDMSG_MODIFIED,(WPARAM)messageData);

	FUNCTION_EXIT;
}


void RadioOperations::predefinedMessageRemoved(PredefinedMessage* messageData)
{
    FUNCTION_ENTRY("predefinedMessageRemoved");
    
	//notifyChildWindows - WM_PREDEFINEDMSG_REMOVED
	m_observerHelper.notifyChildWindows(WM_PREDEFINEDMSG_REMOVED,(WPARAM)messageData);

	FUNCTION_EXIT;
}

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

	
	if (RR_TALKGROUP == resource.type) {
		TA_Base_Core::DescriptionParameters descParams;
		std::stringstream ss;
		ss << resource.id;
		TA_Base_Core::NameValuePair param1("Radio ID",ss.str());
		descParams.push_back(&param1);

		auditEventPosted(TA_Base_Core::RadioAudit::RadioGroupMonitored,descParams,resource);
	}

	return The_RadioSystemStatus::instance()->assignResource(resource,buttonRef);
}


bool RadioOperations::deAssignResource(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("deAssignResource");
    
	FUNCTION_EXIT;
	return The_RadioSystemStatus::instance()->deAssignResource(resource);
}


void RadioOperations::makePrivateCall(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("makePrivateCall");
    
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


bool RadioOperations::selectSpeaker(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("selectSpeaker");
    
	bool status = The_RadioSystemStatus::instance()->selectSpeaker(resource);

	TA_Base_Core::DescriptionParameters descParams;
	std::stringstream ss;
	ss << resource.id;
	TA_Base_Core::NameValuePair param1("Radio ID",ss.str());
    descParams.push_back(&param1);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioGroupSelect,descParams,resource);

	FUNCTION_EXIT;
	return status;
}


bool RadioOperations::unselectSpeaker(RadioResource& resource)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("unselectSpeaker");
    
	bool status = The_RadioSystemStatus::instance()->unselectSpeaker(resource);

	TA_Base_Core::DescriptionParameters descParams;
	TA_Base_Core::NameValuePair param1("ResourceName",resource.alias);
    descParams.push_back(&param1);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioGroupUnselect,descParams,resource);

	FUNCTION_EXIT;
	return status;
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
		case RR_SITE:
			auditEventPosted(TA_Base_Core::RadioAudit::RadioSiteConnect,descParams,resource);
			break;
		case RR_MSITE:
			auditEventPosted(TA_Base_Core::RadioAudit::RadioMultiSiteConnect,descParams,resource);
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
    
    try
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling auditsender");
		if (m_auditSender!=NULL)
		{
			m_auditSender->sendAuditMessage(
				messageType,
				m_entityKey,
				descParams,
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
    
    try
    {
		if (m_auditSender!=NULL)
		{
			m_auditSender->sendAuditMessage(
				messageType,
				m_entityKey,
				descParams,
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
	m_observerHelper.notifyChildWindows(WM_INITIALISE_MODE);

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


	saveEntity(dynResource, mems);
	{
		std::vector<RadioResource>::iterator iter = mems.begin();
		for ( ; iter != mems.end(); ++iter)
		{
			if (RR_TALKGROUP == (*iter).type) {
				talkgroupAlias = (*iter).alias;
				break;
			}
		}
		if (iter != mems.end()) {
			mems.erase(iter);
		}
	}

	// if DGNA is ongoing
	if (The_RadioSystemStatus::instance()->getResource_isGroupConnected(dynResource) == CONNECTED) {
		if (talkgroupAlias == "") {
			::MessageBox(NULL, "Talkgroup is not assigned to DGNA", "Error connecting", MB_OK|MB_ICONERROR);
			return false;
		}
		The_RadioSystemStatus::instance()->connectDynamicRegroup(resource, talkgroupAlias);
	}

	FUNCTION_EXIT;
	return true;
}

bool RadioOperations::callForwarding(bool cond, unsigned long forwardingISSI, unsigned long forwardedISSI)
{
	FUNCTION_ENTRY("callForwarding");

	bool status = The_RadioSystemStatus::instance()->callForwarding(cond, forwardingISSI, forwardedISSI);

	if (status) {
		setCallForwardingState(true);
	} else {
		setCallForwardingState(false);
	}

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

	if (status) {
		setCallForwardingState(false);
	}

	//TA_Base_Core::DescriptionParameters descParams;
	//std::stringstream ss;
	//ss << ISSI;
	//TA_Base_Core::NameValuePair param2("Radio ID",ss.str());
	//descParams.push_back(&param2);

	//auditEventPosted(TA_Base_Core::RadioAudit::RadioCallForwarding,descParams);

	FUNCTION_EXIT;
	return status;
}


void RadioOperations::makeDriverCall(unsigned long trainId)
{
	TA_Base_Core::ThreadGuard guard( m_lock );
    FUNCTION_ENTRY("makeDriverCall");

	TA_IRS_Bus::TrainInformationTypes::TrainDetails tDetails = m_trainAgent->getTrainDetails((char)trainId);

	// get primaryTSI/radio id from train_agent
	RadioResource resource;
	
	std::stringstream out;
	out << trainId;
	resource.alias = "Train " + out.str();
	resource.id = atol(tDetails.primaryTsi.c_str());
	resource.ssi = atol(tDetails.primaryTsi.c_str());
	resource.type = RR_TRAIN;

	if ( The_RadioSystemStatus::instance()->makePrivateCall(resource) )
	{
		RadioResource* res = getNextResource(resource);

		m_observerHelper.notifyChildWindows(WM_OUTGOING_CALL,(WPARAM)res);
	}

	// Event Audit for private call
	TA_Base_Core::DescriptionParameters descParams;

	std::stringstream ss;
	ss << resource.id;
	TA_Base_Core::NameValuePair param1("Radio ID",ss.str());
    descParams.push_back(&param1);

	auditEventPosted(TA_Base_Core::RadioAudit::RadioMakePrivateCall,descParams,resource);

	FUNCTION_EXIT;
}

void RadioOperations::onIncomingDriverCall(unsigned long trainId)
{	
	FUNCTION_ENTRY("onIncomingDriverCall");
	TA_Base_Core::ThreadGuard guard( m_lock );
	
	TA_IRS_Bus::TrainInformationTypes::TrainDetails tDetails = m_trainAgent->getTrainDetails((char)trainId);

	RadioResource resource;
	
	std::stringstream out;
	out << trainId;
	resource.alias = "Train " + out.str();
	resource.id = atol(tDetails.primaryTsi.c_str());
	resource.ssi = atol(tDetails.primaryTsi.c_str());
	resource.type = RR_TRAIN;

	RadioResource* res = getNextResource(resource);

	m_observerHelper.notifyChildWindows(WM_INCOMING_DRIVER_CALL,(WPARAM)res);

	FUNCTION_EXIT;
}

void RadioOperations::onResetDriverCall(unsigned long trainId)
{
	FUNCTION_ENTRY("onResetDriverCall");
	TA_Base_Core::ThreadGuard guard( m_lock );
	
	TA_IRS_Bus::TrainInformationTypes::TrainDetails tDetails = m_trainAgent->getTrainDetails((char)trainId);

	RadioResource resource;
	
	std::stringstream out;
	out << trainId;
	resource.alias = "Train " + out.str();
	resource.id = atol(tDetails.primaryTsi.c_str());
	resource.ssi = atol(tDetails.primaryTsi.c_str());
	resource.type = RR_TRAIN;

	RadioResource* res = getNextResource(resource);

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

void RadioOperations::setCallForwardingState(bool state)
{
	FUNCTION_ENTRY("setCallForwardingState");
	m_CallForwardingState = state;
	FUNCTION_EXIT;
}

bool RadioOperations::getCallForwardingState()
{
	FUNCTION_ENTRY("getCallForwardingState");
	FUNCTION_EXIT;
	return m_CallForwardingState;
}

void RadioOperations::setTrainProxy(TA_IRS_Bus::TrainInformationCorbaProxyPtr trainAgent) 
{
	m_trainAgent = trainAgent;
}

void RadioOperations::notifyFailOp(RadioResource& resource, int ref/*optional*/)
{
	FUNCTION_ENTRY("notifyFailOp");
	if (RR_NIL != resource.type) {
		RadioResource* res = getNextResource(resource);

		m_observerHelper.notifyChildWindows(WM_OPERATION_FAIL, (WPARAM)res, (LPARAM) ref);
	}
	FUNCTION_EXIT;
}