/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerAgentCommandImpl.cpp $
 * @author:  Anggiono
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "ace/OS.h"
#include "app/radio/RadioTetraAgent/src/RadioServerAgentCommandImpl.h"


#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADICommand.h"
#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventManager.h"


#include "bus/radio/radio_agent/IDL/src/RadioServerAgentIDL.h"
#include "bus/radio/radio_agent/IDL/src/CommonRadioAgentIDL.h"

#include "core/data_access_interface/radio/src/RadioPredefinedTextMessageFactory.h"
#include "core/data_access_interface/radio/src/IRadioPredefinedTextMessage.h"
#include "core/data_access_interface/radio/src/RadioSubscriberFactory.h"
#include "core/data_access_interface/radio/src/IRadioSubscriber.h"
#include "core/data_access_interface/radio/src/IRadioTextMessageInbox.h"
#include "core/data_access_interface/radio/src/RadioTextMessageInboxFactory.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/threads/src/Thread.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/types/RadioAlarms_MessageTypes.h"
#include "core/utilities/src/RunParams.h"

#include <sstream>


using namespace TA_IRS_App;

RadioServerAgentCommandImpl::RadioServerAgentCommandImpl(TA_Base_Core::IEntityDataPtr EntityData):
    m_isCADISessionOkay(false),
    m_commsSender(NULL),
    m_entityKey(0),
    m_entitySubsystem(0),
    m_entityLocation(0)
{
    FUNCTION_ENTRY("constructor");

	m_session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	
    m_entityKey       = EntityData->getKey();
    m_entityLocation  = EntityData->getLocation();
    m_entitySubsystem = EntityData->getSubsystem();
    m_entityTypeKey   = EntityData->getTypeKey();
    m_entityName      = EntityData->getName();

    m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().
        getCommsMessageSender(TA_Base_Core::RadioComms::Context);

    FUNCTION_EXIT;
}


RadioServerAgentCommandImpl::~RadioServerAgentCommandImpl()
{
    FUNCTION_ENTRY("destructor");

    delete m_commsSender;
    m_commsSender = NULL;

	//TD17994, Haipeng refactoring the alarm libary;
	TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
    FUNCTION_EXIT;
}

void RadioServerAgentCommandImpl::setSummary(RadioServerAgentSummary& summary)
{
    m_summary = &summary;
}

bool RadioServerAgentCommandImpl::initializeCADI()
{
    if (m_isCADISessionOkay) return true;

    if (!m_summary->loadCADIParameters())
    {
        submitAlarm(TA_Base_Core::RadioAlarms::RadioCADIAlarm);
        return false;
    }

    try
    {
	    //	To establish a session;
	    HandshakeCommand sessionHandShake;
        TokenList tokens;
        std::string cadiHost = m_summary->getCADIHost();
        std::string cadiLogin = m_summary->getCADILogin();
        std::string cadiPassword = m_summary->getCADIPassword();
        std::string cadiEncryption = m_summary->getCADIEncryption();

        if (cadiHost.empty() || cadiLogin.empty())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "CADI is not configured" );
            return false;
        }

        tokens.push_back(cadiHost);

	    if (sessionHandShake.call(tokens) == 1)
        {
            TA_Base_Core::Thread::sleep(500); // sleep before we log in
            cadiEventManager::instance()->registerObserver( *this );
            cadiEventManager::instance()->activate();

            LoginCommand loginCommand;
            tokens.clear();
        
            tokens.push_back(cadiLogin);
            tokens.push_back(cadiPassword);
            tokens.push_back(cadiEncryption);
        
            int loginStatus = loginCommand.call(tokens);
        
            //The returned code only indicates "true" or "false";
            if (loginStatus == 0 )
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "CADI Login failed" );
            }
            else
            {
                m_isCADISessionOkay = true;
            }
        }
    }
    catch (...)
    {
    }

    if (m_isCADISessionOkay)
    {
        closeAlarm(TA_Base_Core::RadioAlarms::RadioCADIAlarm);
    }
    else
    {
        submitAlarm(TA_Base_Core::RadioAlarms::RadioCADIAlarm);
    }

    return m_isCADISessionOkay;
}

void RadioServerAgentCommandImpl::cleanupCADI()
{
    if (m_isCADISessionOkay)
    {
        LogoutCommand logoutCommand;
        TokenList tokens;
    
        if (logoutCommand.call( tokens ) == 0)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Logout failed without reasons" );
	    }
        m_isCADISessionOkay = false;
    }
}

void RadioServerAgentCommandImpl::radioStatusMsg(unsigned long sourceSSI, unsigned long statusMsg)
{
    FUNCTION_ENTRY("radioStatusMsg");


    TA_IRS_Core::IRadioPredefinedTextMessage* msg = TA_IRS_Core::RadioPredefinedTextMessageFactory::getInstance().getRadioPredefinedTextMessage(statusMsg, true, false);

    if (msg != NULL)
    {
        std::string msgText = msg->getMessageText();
		//added by Haipeng, Fixed TD14312
        delete msg;
        msg = NULL;

		TA_IRS_Bus::Radio::RadioResource cResource;
        std::string sourceAlias;

		TA_IRS_Core::IRadioSubscriber* data = NULL;

		try
		{
			data = TA_IRS_Core::RadioSubscriberFactory::getInstance().getRadioSubscriberBySSI(sourceSSI);
		}
		catch (...)
		{
		}

        if (data == NULL)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot find radio subscriber for SSI %d", sourceSSI);
            sourceAlias = "unk";
            cResource.id = 0;
            cResource.type = TA_IRS_Bus::Radio::RR_RADIO;
        }
        else
        {
            sourceAlias = data->getAlias();
		    cResource.id = data->getResourceId();
		    cResource.type = (TA_IRS_Bus::Radio::ERadioResourceType)data->getResourceType();
    		delete data;
            data = NULL;
        }

		cResource.ssi = sourceSSI;
		cResource.alias = CORBA::string_dup(sourceAlias.c_str());

		// update DB
        time_t now = ACE_OS::time();
		TA_IRS_Core::IRadioTextMessageInbox* pText = TA_IRS_Core::RadioTextMessageInboxFactory::getInstance().addNew(
			now,m_session,false,msgText, sourceAlias, std::string("ALL"));

        
		// create notification data
		TA_IRS_Bus::RadioServerAgentIDL::recvText textData;
		textData.originator  = cResource;

		TA_IRS_Bus::Radio::Message msgData;
		if (pText != NULL)
		{
			msgData.key = pText->getKey();
			msgData.time = now;
			msgData.isRead = pText->isRead();
			msgData.messageText = CORBA::string_dup(msgText.c_str());
		}

		// send notification
		CORBA::Any msg;
        msg <<= textData;
        
        m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::ReceiveText,
            m_entityKey, msg, m_entitySubsystem, m_entityLocation);

		delete pText;
    }

    FUNCTION_EXIT;
}

void RadioServerAgentCommandImpl::connectDynamicRegroup(RadioResource resource,unsigned long talkgroupID)
{
    if (!m_isCADISessionOkay)
    {
        TA_THROW(TA_Base_Core::TransactiveException("CADI not initialized"));
    }

    //Retrieve DynGroup configuration from Radio Directory DAI
	//For each Radio Unit, perform CADI Regroup

    TA_IRS_App::TokenList token;
    std::ostringstream stream;

    stream << resource.id;
    token.push_back(stream.str());

    stream.str("");
    stream << talkgroupID;
    token.push_back(stream.str());

    TA_IRS_App::RegroupCommand regroup;
    regroup.call(token);
}

void RadioServerAgentCommandImpl::releaseDynamicRegroup(RadioResource resource,unsigned long talkgroupID)
{
    if (!m_isCADISessionOkay)
    {
        TA_THROW(TA_Base_Core::TransactiveException("CADI not initialized"));
    }

	//Retrieve DynGroup configuration from Radio Directory DAI
	//For each Radio Unit, perform CADI Cancel Regroup

    TA_IRS_App::TokenList token;

    std::ostringstream stream;
    stream << resource.id;
    token.push_back(stream.str());

    stream.str("");
    stream << talkgroupID;
    token.push_back(stream.str());
    
    TA_IRS_App::XRegroupWGidCommand cancelregroup;
    cancelregroup.call(token);
}


void RadioServerAgentCommandImpl::notify( CADIEvent &pEvent ) 
{
    CADIEvent* event = &pEvent;
    // get the type of the event

    //TODO:
    
}





void RadioServerAgentCommandImpl::submitAlarm(const TA_Base_Core::MessageType& pAlarm)
{
 	FUNCTION_ENTRY("submitAlarm");

    // Threadsafe 
    TA_Base_Core::ThreadGuard guard(m_lock);
	
    try
    {   
        // construct the DescriptionParameters list
        TA_Base_Core::DescriptionParameters parameters;

        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarm ( 
            pAlarm, 
            m_entityKey,	        	// entity key
            m_entityTypeKey,	    	// entity type key
            parameters,                 // parameters
		    m_entityName,               // entity name
            m_entityLocation, 		    // region key
            m_entitySubsystem);	        // subystem key
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to raise alarm");
    }
    
    FUNCTION_EXIT;
}

void RadioServerAgentCommandImpl::closeAlarm(const TA_Base_Core::MessageType& pAlarm)
{
 	FUNCTION_ENTRY("closeAlarm");

    // Threadsafe 
    TA_Base_Core::ThreadGuard guard(m_lock);
	
    try
    {   
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(pAlarm, m_entityKey, m_entityLocation);
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to close the alarm");
    }

    FUNCTION_EXIT;
}
