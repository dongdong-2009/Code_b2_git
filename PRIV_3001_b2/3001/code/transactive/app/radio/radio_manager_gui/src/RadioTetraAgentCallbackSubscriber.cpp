#pragma once
#include "app\radio\radio_manager_gui\src\RadioTetraAgentCallbackSubscriber.h"

using namespace TA_Base_Core;


RadioTetraAgentCallbackSubscriber::RadioTetraAgentCallbackSubscriber() : m_profileKey(0)
{
	initialise();
}

RadioTetraAgentCallbackSubscriber::~RadioTetraAgentCallbackSubscriber()
{
	MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
}

void RadioTetraAgentCallbackSubscriber::initialise()
{
	FUNCTION_ENTRY("initialise");

	std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	TA_Base_Core::ISession* session = NULL;
	try {
		session  = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId);
	}
	catch(TA_Base_Core::TransactiveException& e)
	{
		delete session;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TransactiveException");
		return;
	}
	m_profileKey = session->getProfileKey();

	ThreadGuard guard(m_notificationsLock);

    //
    // Subscribe to Radio Broadcast notifications from the Radio Agent
    //
	MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::RadioComms::RadioDirectoryNotification,
		(SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*)(this));

	// Create the train agent proxies
	createProxies();

	m_trainDriverCallCorbaProxy->addObserver( this );

	The_RadioOperations::instance()->setTrainProxy(m_trainInformationCorbaProxy);
	FUNCTION_EXIT;
}


void RadioTetraAgentCallbackSubscriber::processOccCallRequest( const CallTypes::OccCallRequest& event )
{
	FUNCTION_ENTRY("processOccCallRequest");

	The_RadioOperations::instance()->onIncomingDriverCall(event.trainId);
	
	FUNCTION_EXIT;
}

void RadioTetraAgentCallbackSubscriber::processOccCallReset( const CallTypes::OccCallReset& event )
{
	FUNCTION_ENTRY("processOccCallReset");

	The_RadioOperations::instance()->onResetDriverCall(event.trainId, event.bResetDueToFailure);

	FUNCTION_EXIT;
}

void RadioTetraAgentCallbackSubscriber::createProxies()
{
	FUNCTION_ENTRY("createProxies");

	std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

	// get a reference to the proxy factory
	TrainAgentProxyFactory& trainAgentProxyFactory = TrainAgentProxyFactory::getInstance();

	// initialise it in case it isnt already
	trainAgentProxyFactory.guiInitialise( sessionId );

	// get the proxy objects
	m_trainInformationCorbaProxy = trainAgentProxyFactory.createTrainInformationCorbaProxy();
	m_trainDriverCallCorbaProxy = trainAgentProxyFactory.createTrainDriverCallCorbaProxy();

	FUNCTION_EXIT;
}

void RadioTetraAgentCallbackSubscriber::receiveSpecialisedMessage(const CommsMessageCorbaDef& message)
{
	FUNCTION_ENTRY("receiveSpecialisedMessage");

	ThreadGuard lock(m_notificationsLock);

	if (std::string(message.messageTypeKey) != TA_Base_Core::RadioComms::RadioDirectoryNotification.getTypeKey())
	{
		FUNCTION_EXIT;
		return;
	}

	TA_IRS_Bus::RadioServerAgentIDL::ProfileTgSync* profileEvent;
	TA_IRS_Bus::RadioServerAgentIDL::DbResSync* dbEvent;

	if ((message.messageState >>= dbEvent) != 0) {
		processDbUpdate(*dbEvent);
	} 
	else if((message.messageState >>= profileEvent) != 0) {
		processProfileUpdate(*profileEvent);
	} 

	FUNCTION_EXIT;
}

void RadioTetraAgentCallbackSubscriber::processProfileUpdate(const TA_IRS_Bus::RadioServerAgentIDL::ProfileTgSync& event)
{
	FUNCTION_ENTRY("processProfileUpdate");
	if (m_profileKey != event.profileKey) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received profile update for profile key = %u, not this profile (%u). Not processing", event.profileKey, m_profileKey);
		return;
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received profile update for this profile(%u)", m_profileKey);

	std::vector<std::string> members;
	for(unsigned int index=0; index < RadioSystemStatus::PROFILE_TALKGROUP_SIZE; index++)
	{
		std::string temp;		
		temp = CORBA::string_dup((event.talkgroups)[index]);
		members.push_back(temp);
	}

	The_RadioOperations::instance()->notifyProfileUpdate(members);

	FUNCTION_EXIT;
}

void RadioTetraAgentCallbackSubscriber::processDbUpdate(const TA_IRS_Bus::RadioServerAgentIDL::DbResSync& event)
{
	FUNCTION_ENTRY("processDbUpdate");

	RadioResource oldResource, newResource;

	RadioMFTAgentHelper::convertRadioResourceFromIDL(event.oldResource, oldResource);
	RadioMFTAgentHelper::convertRadioResourceFromIDL(event.newResource, newResource);

	// notify mft agent
	The_RadioOperations::instance()->notifyDbChange(oldResource, newResource);

	// notify GUI

	FUNCTION_EXIT;
}