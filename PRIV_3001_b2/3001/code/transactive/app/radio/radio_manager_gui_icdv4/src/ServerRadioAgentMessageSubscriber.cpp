/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/ServerRadioAgentMessageSubscriber.cpp $
  * @author:  Glen Kidd
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */



#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioUserDefinedEvents.h"
#include "app/radio/radio_manager_gui_icdv4/src/ServerRadioAgentMessageSubscriber.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioPredefinedTextMessageData.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h"
#include "bus/radio/radio_agent/idl/src/RadioServerAgentIDL.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/types/RadioAudit_MessageTypes.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/message/src/MessagePublicationManager.h"

#include "core/utilities/src/DebugUtil.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

std::vector<IGenericObserverListener*> GenericSubscriberPublisher<TA_Base_Core::CommsMessageCorbaDef>::m_genericListeners;

ServerRadioAgentMessageSubscriber::ServerRadioAgentMessageSubscriber(TA_Base_Core::CtaRuntime* pCtaRuntime) : 
	m_pCtaRuntime(pCtaRuntime)
{

	// Fix TD13617
    try
    {
		TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(m_pCtaRuntime->getMySessionId());
		m_mftName = console->getName();
		delete console;
    }
    catch (...)
    {
    }

	// Fix TD15461
	m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().
	getCommsMessageSender(TA_Base_Core::RadioComms::Context);

	TA_Base_Core::IEntityData* entity = RadioMFTAgentHelper::getRadioMFTAgentEntity();
	m_mftentityKey	= entity->getKey();
	m_mftSubsystem	= entity->getSubsystem();
	m_mftLocation   = entity->getLocation();

	delete entity;
}

ServerRadioAgentMessageSubscriber::~ServerRadioAgentMessageSubscriber()
{
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
}

void ServerRadioAgentMessageSubscriber::addListener(IRadioAgentMessageListener* plistener)
{
    FUNCTION_ENTRY("addListener");

    addGenericListener(plistener);
    
    FUNCTION_EXIT;
}


void ServerRadioAgentMessageSubscriber::Subscribe()
{
    FUNCTION_ENTRY("Subscribe");


	if (m_pCtaRuntime != NULL)
	{
        unsigned long subsystemKey = m_pCtaRuntime->getMyEntitySubsystemKey();
        /*
        // Subscribe to Server agent messages, only one server agent instance
        {
            TA_Base_Core::IEntityData* entity = RadioMFTAgentHelper::getRadioServerAgentEntity();
            
            unsigned long locationKey = entity->getLocation();
            unsigned long serverEntityKey = entity->getKey();

            delete entity;
            entity = NULL;

		    // recvSDS
		    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
			    TA_Base_Core::RadioComms::ReceiveSDS,this,
			    serverEntityKey,
			    subsystemKey,
			    locationKey);

		    // recvText
		    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
			    TA_Base_Core::RadioComms::ReceiveText,this,
			    serverEntityKey,
			    subsystemKey,
			    locationKey);
        }
*/
        // Subscribe to MFT agent messages, same location as the manager
        {
		    // PredefinedTextMessage update
		    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
			    TA_Base_Core::RadioComms::PredefinedTextMessageAdded,this,
			    m_mftentityKey,
			    m_mftSubsystem,
			    m_mftLocation);

		    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
			    TA_Base_Core::RadioComms::PredefinedTextMessageModified,this,
			    m_mftentityKey,
			    m_mftSubsystem,
			    m_mftLocation);

		    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
			    TA_Base_Core::RadioComms::PredefinedTextMessageDeleted,this,
			    m_mftentityKey,
			    m_mftSubsystem,
			    m_mftLocation);
        }
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "m_pCtaRuntime is NULL");
	}

    FUNCTION_EXIT;
}


void ServerRadioAgentMessageSubscriber::ExtractMessageData(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("ExtractMessageData");

    std::string messageTypeKey(message.messageTypeKey);

    if (messageTypeKey == TA_Base_Core::RadioComms::ReceiveSDS.getTypeKey())
	{
		TA_IRS_Bus::RadioServerAgentIDL::recvSDS* sdsMsg = NULL;
		message.messageState >>= sdsMsg;

		// Temporarily comment this IF statement since there is only one SDTS host on site
		// Remove when SDTS servers are available for each client(MFT) SSI
		//++Fix TD13617
		/*if (strcmp(sdsMsg->mftName.in(),m_mftName.c_str()) != 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SDS Message not intended for this operator");
			return;
		}*/
		//Fix TD13617++

		m_source.id = sdsMsg->originatingTrain.id;
		m_source.alias = sdsMsg->originatingTrain.alias;
		m_source.ssi = sdsMsg->originatingTrain.ssi;
		m_source.type = RR_TRAIN;

		for (int i=0; i< sdsMsg->message.length(); i++)
		{
			m_message += sdsMsg->message[i];
		}
		std::ostringstream logStr;
		logStr <<"Trainborne SDS Message Received: " << m_message<< std::endl
			   << "Source: " << sdsMsg->originatingTrain.alias.in() ;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logStr.str().c_str());
	}
	else if (messageTypeKey == TA_Base_Core::RadioComms::ReceiveText.getTypeKey())
	{
		TA_IRS_Bus::RadioServerAgentIDL::recvText* txtMsg = NULL;
		message.messageState >>= txtMsg;
		
		// Temporarily comment this IF statement since there is only one SDTS host on site
		// Remove when SDTS servers are available for each client(MFT) SSI
		//++Fix TD13617
		if (strcmp(txtMsg->mftName.in(),m_mftName.c_str()) != 0)			
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SDS Message not intended for this operator");
			return;
		}
		//Fix TD13617++

		if (!txtMsg->messageData.isRead)
		{
			MessageInbox* data = new MessageInbox(); // delete in InboxPage
			data->messageSource = txtMsg->originator.alias.in();
			data->key			= txtMsg->messageData.key;
			data->isRead		= txtMsg->messageData.isRead;
			data->messageText   = txtMsg->messageData.messageText.in();
			data->date   = txtMsg->messageData.time;

			std::ostringstream logStr;
			logStr <<"Incoming Text Message: " << data->messageText.c_str() 
				   << "Source: " << data->messageSource.c_str();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logStr.str().c_str());

			RadioResource source;
			RadioMFTAgentHelper::convertRadioResourceFromIDL(txtMsg->originator,source); // Fixed TD14297

			sendNotification(data);
			The_RadioOperations::instance()->onIncomingTextMessageEvent(data,source);
		}
	}
	else
	{
		RadioUpdateMessageType type;
		bool flag = true;
		if (messageTypeKey == TA_Base_Core::RadioComms::PredefinedTextMessageAdded.getTypeKey())
			type = RM_NEW;
        else if (messageTypeKey == TA_Base_Core::RadioComms::PredefinedTextMessageModified.getTypeKey())
			type = RM_UPDATE;
        else if (messageTypeKey == TA_Base_Core::RadioComms::PredefinedTextMessageDeleted.getTypeKey())
			type = RM_DELETE;
		else
			flag = false;

		if (flag)
		{
			TA_IRS_Bus::Radio::PredefinedMessage* predefinedMsg = NULL;
			message.messageState >>= predefinedMsg;

			PredefinedMessage* data = new PredefinedMessage(); // delete in RadioSendTextPage
			data->key  = predefinedMsg->key;
			data->shortName = predefinedMsg->shortName;
			data->messageText = predefinedMsg->messageText.in();
			data->type = predefinedMsg->type; //fixed TD14312

			The_RadioOperations::instance()->onPredefinedMessageEvent(type,data);
		}
	}

    FUNCTION_EXIT;
}


void ServerRadioAgentMessageSubscriber::PublishToListener(const TA_Base_Core::CommsMessageCorbaDef& message, IGenericObserverListener* listener)
{
    FUNCTION_ENTRY("PublishToListener");
    
    dynamic_cast<IRadioAgentMessageListener*>(listener)->onRecvSDS(m_source,m_message);

    FUNCTION_EXIT;
}


void ServerRadioAgentMessageSubscriber::sendNotification(MessageInbox* message)
{
	TA_IRS_Bus::RadioServerAgentIDL::recvText tData;			

	TA_IRS_Bus::Radio::Message msgData;
	msgData.key = message->key;
	msgData.time = message->date;
	msgData.isRead = false;
	msgData.messageText = CORBA::string_dup(message->messageText.c_str());

	tData.messageData = msgData;
	
	// send notification
	CORBA::Any msg;
    msg <<= tData;

    m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::ReceiveText,
        m_mftentityKey, msg,m_mftSubsystem, m_mftLocation);	
}


