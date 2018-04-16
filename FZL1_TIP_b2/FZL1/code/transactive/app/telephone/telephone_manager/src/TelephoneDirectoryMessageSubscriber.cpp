/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/TelephoneDirectoryMessageSubscriber.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class will receive all notifications of changes posted on change of the
  * Telephone directory. The class also acts as the publisher for typically GUI
  * components interested in notifications of these changes according to the
  * Observer pattern.
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"
#include "app/telephone/telephone_manager/src/TelephoneDirectoryMessageSubscriber.h"
#include "app/telephone/telephone_manager/src/ITelephoneDirectoryListener.h"
#include "bus/telephone/telephone_manager/IDL/src/ITelephoneManagerCommonCorbaDef.h"

#include "core/message/src/IMessageSubscriber.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/TelephoneManagerDirectoryBroadcastMessage_MessageTypes.h"

using namespace std;

TelephoneDirectoryMessageSubscriber::TelephoneDirectoryMessageSubscriber()
{
    FUNCTION_ENTRY( "TelephoneDirectoryMessageSubscriber" );
    FUNCTION_EXIT;
}


TelephoneDirectoryMessageSubscriber::~TelephoneDirectoryMessageSubscriber()
{
    FUNCTION_ENTRY( "~TelephoneDirectoryMessageSubscriber" );
    FUNCTION_EXIT;
}


void TelephoneDirectoryMessageSubscriber::addListener(ITelephoneDirectoryListener* plistener)
{
    FUNCTION_ENTRY( "addListener" );

    addGenericListener(plistener);

    FUNCTION_EXIT;
}


void TelephoneDirectoryMessageSubscriber::ExtractMessageData(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY( "ExtractMessageData" );

    // Extract m_directoryEntryData/m_directoryData from message
    std::string messageTypeKey(message.messageTypeKey);

    if (messageTypeKey == TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryEvent.getTypeKey())
    {
        TA_IRS_Bus::DirectoryEvent* directory;
        message.messageState >>= directory;

        m_directoryData.ID = directory->ID;
        m_directoryData.name = directory->name;
    }
    else if (messageTypeKey == TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryEntryEvent.getTypeKey())
    {
        TA_IRS_Bus::DirectoryEntryEvent* directoryEntry;
        message.messageState >>= directoryEntry;

        m_directoryEntryData.ID = directoryEntry->ID;
        m_directoryEntryData.number = directoryEntry->number;
        m_directoryEntryData.fullName = directoryEntry->fullName;
        m_directoryEntryData.location = directoryEntry->location;
        m_directoryEntryData.directoryDataName = directoryEntry->directoryDataName;
    }

    FUNCTION_EXIT;
}


void TelephoneDirectoryMessageSubscriber::Subscribe()
{
    FUNCTION_ENTRY( "Subscribe" );

    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::Context,
        this);

    FUNCTION_EXIT;
}


void TelephoneDirectoryMessageSubscriber::PublishToListener(const TA_Base_Core::CommsMessageCorbaDef& message, IGenericObserverListener* listener)
{
    FUNCTION_ENTRY( "PublishToListener" );

    std::string messageTypeKey(message.messageTypeKey);
    ITelephoneDirectoryListener::EventType type;
    if (messageTypeKey == TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryEvent.getTypeKey())
    {
        TA_IRS_Bus::DirectoryEvent* directory;
        message.messageState >>= directory;

        switch(directory->type)
        {
        case TA_IRS_Bus::NEW_ITEM:
            type = ITelephoneDirectoryListener::New;
            break;

        case TA_IRS_Bus::UPDATE_ITEM:
            type = ITelephoneDirectoryListener::Update;
            break;

        case TA_IRS_Bus::DELETE_ITEM:
            type = ITelephoneDirectoryListener::Delete;
            break;

        default:
            TA_ASSERT(false, "New Event type?");
        }

        dynamic_cast<ITelephoneDirectoryListener*>(listener)->onDirectoryEvent(this, type, m_directoryData);
    }
    else if (messageTypeKey == TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryEntryEvent.getTypeKey())
    {
        TA_IRS_Bus::DirectoryEntryEvent* directoryEntry;
        message.messageState >>= directoryEntry;

        switch(directoryEntry->type)
        {
        case TA_IRS_Bus::NEW_ITEM:
            type = ITelephoneDirectoryListener::New;
            break;

        case TA_IRS_Bus::UPDATE_ITEM:
            type = ITelephoneDirectoryListener::Update;
            break;

        case TA_IRS_Bus::DELETE_ITEM:
            type = ITelephoneDirectoryListener::Delete;
            break;

        default:
            TA_ASSERT(false, "New Event type?");
        }

        dynamic_cast<ITelephoneDirectoryListener*>(listener)->onDirectoryEntryEvent(this, type, m_directoryEntryData);
    }
    else if (messageTypeKey == TA_Base_Core::TelephoneManagerDirectoryBroadcastMessage::DirectoryUploadEvent.getTypeKey())
    {
        dynamic_cast<ITelephoneDirectoryListener*>(listener)->onDirectoryUploadEvent();
    }
    else
    {
        TA_ASSERT(false, "New Events?");
    }

    FUNCTION_EXIT;
}


