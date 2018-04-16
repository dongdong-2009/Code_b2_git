#pragma once
#include "app\radio\radio_manager_gui_icdv4\src\RadioTetraAgentCallbackSubscriber.h"

using namespace TA_Base_Core;

RadioTetraAgentCallbackSubscriber::RadioTetraAgentCallbackSubscriber()
{
	initialise();
}

RadioTetraAgentCallbackSubscriber::~RadioTetraAgentCallbackSubscriber()
{
}

void RadioTetraAgentCallbackSubscriber::initialise()
{
	FUNCTION_ENTRY("initialise");
	// Create the train agent proxies
	createProxies();

	//
	// Acquire the notifications lock before subscribing to messages
	// so that any notifications are received after the initial batch of
	// authorisation requests are retrieved.
	//
	ThreadGuard guard(m_notificationsLock);

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

	The_RadioOperations::instance()->onResetDriverCall(event.trainId);

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