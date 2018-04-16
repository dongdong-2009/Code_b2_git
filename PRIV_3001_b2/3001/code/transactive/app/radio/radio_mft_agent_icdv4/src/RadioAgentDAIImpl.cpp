/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioAgentDAIImpl.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioAgentDAIImpl.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.h"
#include "bus/radio/radio_agent/IDL/src/CommonRadioAgentIDL.h"

#include "core/data_access_interface/radio/src/RadioBtnCfgFactory.h"
#include "core/data_access_interface/radio/src/IRadioBtnCfg.h"
#include "core/data_access_interface/radio/src/RadioPredefinedTextMessageFactory.h"
#include "core/data_access_interface/radio/src/IRadioPredefinedTextMessage.h"
#include "core/data_access_interface/radio/src/RadioTextMessageInboxFactory.h"
#include "core/data_access_interface/radio/src/IRadioTextMessageInbox.h"
#include "core/data_access_interface/radio/src/IRadioDirectoryEntry.h"
#include "core/data_access_interface/radio/src/RadioDirectoryEntryFactory.h"
#include "core/data_access_interface/radio/src/RadioDirectoryMemberFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"

using namespace std;

RadioAgentDAIImpl::RadioAgentDAIImpl()
{
    FUNCTION_ENTRY("RadioAgentDAIImpl");

	TA_Base_Core::IEntityData* entityData = RadioMFTAgentHelper::getRadioMFTAgentEntity();
    m_entityKey       = entityData->getKey();
    m_entityLocation  = entityData->getLocation();
    m_entitySubsystem = entityData->getSubsystem();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "ENTITY    = %d",m_entityKey);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "LOCATION  = %d",m_entityLocation);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "SUBSYSTEM = %d",m_entitySubsystem);

	delete entityData;

    m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::RadioComms::Context);

    FUNCTION_EXIT;
}


RadioAgentDAIImpl::~RadioAgentDAIImpl()
{
    FUNCTION_ENTRY("~RadioAgentDAIImpl");

    delete m_commsSender;
    m_commsSender = NULL;

    FUNCTION_EXIT;
}

std::vector<ResourceButton> RadioAgentDAIImpl::getResourceButtonCollection(const std::string& session)
{
    std::vector<ResourceButton> result;

    try
    {
        RadioResource tmp;
        ResourceButton rButton;
        
        std::vector<TA_IRS_Core::IRadioBtnCfg*> buttons = TA_IRS_Core::RadioBtnCfgFactory::getInstance().getAllRadioBtnCfg(session);
        for (std::vector<TA_IRS_Core::IRadioBtnCfg*>::iterator it = buttons.begin();
             it != buttons.end(); ++it)
        {
            rButton.key = (*it)->getKey();
            rButton.isProfile = (*it)->isProfile();
            rButton.buttonRef = (*it)->getButtonRef();
            rButton.type = (RadioResourceType) (*it)->getResourceType();
            rButton.id = (*it)->getResourceId();

            tmp.type = rButton.type;
            tmp.id = rButton.id;
            if (The_StatusManager::instance()->setFeatureAttributeButton(tmp, rButton.isProfile, rButton.buttonRef))
            {
    			result.push_back(rButton);
            }
            delete *it; // clean up
        }
    }
    catch (exception& e)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Error catched while trying to get the resource button with session: %s.", session.c_str());
        throw;
    }

    return result;
}

std::vector<PredefinedMessage> RadioAgentDAIImpl::getPredefinedMessageList(const std::string& session)
{
    FUNCTION_ENTRY("getPredefinedMessageList");
    std::vector<PredefinedMessage> ret;
    try
    {
		//fixed TD13419, added the 2nd params
        std::vector<TA_IRS_Core::IRadioPredefinedTextMessage*> messages = 
            TA_IRS_Core::RadioPredefinedTextMessageFactory::getInstance().getAllRadioPredefinedTextMessages(session, false);
        for (std::vector<TA_IRS_Core::IRadioPredefinedTextMessage*>::iterator it = messages.begin();
        it != messages.end(); ++it)
        {
            PredefinedMessage tmp;
            tmp.key = (*it)->getKey();
            tmp.shortName = (*it)->getShortName();
            tmp.messageText = (*it)->getMessageText();
			tmp.type = (*it)->getType(); //fixed TD14312
			
            ret.push_back(tmp);
            delete (*it); // cleanup
        }
    }
    catch (exception& e) 
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Error catched while trying to get predefined text message with session: %s.", session.c_str() );
        throw;
    }
    FUNCTION_EXIT;
    return ret;
}

void RadioAgentDAIImpl::changePredefinedMessage(const std::string& session, PredefinedMessage& message)
{
    FUNCTION_ENTRY("changePredefinedMessage");
    try
    {
        if (message.key == 0) // new message
        {
            TA_IRS_Core::IRadioPredefinedTextMessage* ret = 
                TA_IRS_Core::RadioPredefinedTextMessageFactory::getInstance().createNew(session, 
                    message.shortName, message.messageText);

            // assign new key to message
            if (ret != NULL)
            {
                message.key = ret->getKey();
                delete ret;
            }

            TA_IRS_Bus::Radio::PredefinedMessage predefinedMessage;
            predefinedMessage.key = message.key;
            predefinedMessage._cxx_operator = 0;  // TODO: assign operator
            predefinedMessage.shortName = message.shortName.c_str();
            predefinedMessage.messageText = message.messageText.c_str();
			predefinedMessage.type = message.type; //fixed TD14312
            
			CORBA::Any msg;
            msg <<= predefinedMessage;

            m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::PredefinedTextMessageAdded,
                m_entityKey, msg, 
                m_entitySubsystem, m_entityLocation);
        }
        else
        {
            TA_IRS_Core::RadioPredefinedTextMessageFactory::getInstance().changedPredefinedTextMessage(
                session, message.key, message.shortName, message.messageText);

            TA_IRS_Bus::Radio::PredefinedMessage predefinedMessage;
            predefinedMessage.key = message.key;
            predefinedMessage._cxx_operator = 0;   // TODO: assign operator
            predefinedMessage.shortName = CORBA::string_dup(message.shortName.c_str());
            predefinedMessage.messageText = CORBA::string_dup(message.messageText.c_str()); 
            predefinedMessage.type = message.type; //fixed TD14312

			CORBA::Any msg;
            msg <<= predefinedMessage;

            m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::PredefinedTextMessageModified,
                m_entityKey, msg, 
                m_entitySubsystem, m_entityLocation);
        }
    }
    catch (exception& e) 
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Error catched while trying to change the predefined text message with id: %d.", message.key);
        throw;
    }

    FUNCTION_EXIT;
}

void RadioAgentDAIImpl::deletePredefinedMessage(const std::string& session, unsigned long messageKey)
{
    FUNCTION_ENTRY("deletePredefinedMessage");

    try
    {
        TA_IRS_Core::RadioPredefinedTextMessageFactory::getInstance().deletePredefinedTextMessage(session,messageKey);

            TA_IRS_Bus::Radio::PredefinedMessage predefinedMessage;
            predefinedMessage.key = messageKey;
            predefinedMessage._cxx_operator = 0;   // TODO: assign operator
            predefinedMessage.shortName = CORBA::string_dup("");
            predefinedMessage.messageText = CORBA::string_dup("");
            predefinedMessage.type = false; //fixed TD14312

			CORBA::Any msg;
            msg <<= predefinedMessage;

            m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::PredefinedTextMessageDeleted,
                m_entityKey, msg, 
                m_entitySubsystem, m_entityLocation);
    }
    catch (exception& e) 
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Error catched while trying to delete the predefined text message with id: %d.", messageKey );
        throw;
    }

    FUNCTION_EXIT;    
}


std::vector<MessageInbox> RadioAgentDAIImpl::getMessageInbox(const std::string& session)
{
    FUNCTION_ENTRY("getMessageInbox");
    std::vector<MessageInbox> ret;
    try
    {
        std::vector<TA_IRS_Core::IRadioTextMessageInbox*> messages = 
                    TA_IRS_Core::RadioTextMessageInboxFactory::getInstance().getAllRadioTextMessageInboxs(session);
        for (std::vector<TA_IRS_Core::IRadioTextMessageInbox*>::iterator it = messages.begin();
             it != messages.end(); ++it)
             {
                 MessageInbox tmp;
                 tmp.date = (*it)->getTimeStamp();
                 tmp.isRead = (*it)->isRead();
                 tmp.key = (*it)->getKey();
                 tmp.messageText = (*it)->getMessageText();
				 tmp.messageSource = (*it)->getSourceAlias();

                 ret.push_back(tmp);
				 
                 delete (*it); // cleanup
             }
    }
    catch (exception& e) 
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Error catched while trying to change the predefined text message with session: %s.", session.c_str() );
        throw;
    }
    FUNCTION_EXIT;
    return ret;
}

void RadioAgentDAIImpl::associateButtonWithResource(const std::string& session, const RadioResource& resource, bool isProfileButton, unsigned int buttonRef)
{
    FUNCTION_ENTRY("associateButtonWithResource");
    try
    {
		The_StatusManager::instance()->setFeatureAttributeButton(resource,isProfileButton,buttonRef);
		
        //TA_IRS_Core::RadioBtnCfgFactory::getInstance().associateButtonWithResource(session, isProfileButton, buttonRef,resource);
    }
    catch (exception& e) 
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Error catched while trying to associate button with resource (session: %s).", session.c_str() );
        throw;
    }

    FUNCTION_EXIT;
}

void RadioAgentDAIImpl::clearButton(const std::string& session, const RadioResource& resource, bool isProfileButton, unsigned int buttonRef)
{
    FUNCTION_ENTRY("clearButton");
    try
    {
		The_StatusManager::instance()->setFeatureAttributeButton(resource, false, 0);

        //TA_IRS_Core::RadioBtnCfgFactory::getInstance().clearButtonRef(session, isProfileButton, buttonRef);   

    }
    catch (exception& e) 
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Error catched while trying to clear button with resource with (session: %s).", session.c_str() );
        throw;
    }
    FUNCTION_EXIT;    
}

void RadioAgentDAIImpl::updateEntity(RadioResource& resource, std::vector<RadioResource>& members)
{
    FUNCTION_ENTRY("updateEntity");
    try
    {
		TA_IRS_Core::IRadioDirectoryEntry* data = NULL;

		data = TA_IRS_Core::RadioDirectoryEntryFactory::getInstance().modifyDirectory(resource.type,resource.alias);

		if ( data != NULL)
		{
			for (std::vector<RadioResource>::iterator it = members.begin(); it != members.end(); ++it)
			{
				TA_IRS_Core::RadioDirectoryMemberFactory::getInstance().createNew(data->getKey(),it->type,it->id);
			}

			if (resource.id==0)
			{
				resource.id = The_StatusManager::instance()->getNextTransResourceId();
			}

			The_StatusManager::instance()->addResource(resource,members, resource.id);
			
			delete data;
		}
    }
    catch (exception& e)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Error catched while trying to update the entity with (resource_id: %d).", resource.id);
        throw;        
    }
    FUNCTION_EXIT;    
}

void RadioAgentDAIImpl::deleteEntity(RadioResource& resource)
{
    FUNCTION_ENTRY("deleteEntity");
    try
    {
		TA_IRS_Core::RadioDirectoryEntryFactory::getInstance().deleteDirectory(resource.type,resource.alias);

		The_StatusManager::instance()->delResource(resource);
    }
    catch (exception& e)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Error catched while trying to update the entity with (resource_id: %d).", resource.id);
        throw;        
    }
    
    FUNCTION_EXIT;    
}

void RadioAgentDAIImpl::textMessageRead(unsigned long key, bool flag)
{
    FUNCTION_ENTRY("textMessageRead");

    try
    {
        TA_IRS_Core::RadioTextMessageInboxFactory::getInstance().setRead(key,flag);
    }
    catch (exception& e) 
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Error caught while trying to change the setRead text message with key: %d.", key);
        throw;
    }
	
    FUNCTION_EXIT;
}


