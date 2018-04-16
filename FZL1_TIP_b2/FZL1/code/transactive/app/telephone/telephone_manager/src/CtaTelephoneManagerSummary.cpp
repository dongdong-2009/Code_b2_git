/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/CtaTelephoneManagerSummary.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Refer to 9999Q01020201-Outsourcer_Guideline.doc
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/message/types/TelephoneManagerAudit_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/message/types/TelephoneManagerSpeedDialBroadcastMessage_MessageTypes.h"
#include "core/message/types/TelephoneManagerDirectoryBroadcastMessage_MessageTypes.h"

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"
#include "app/telephone/telephone_manager/src/CtaTelephoneManagerSummary.h"

std::vector<IGenericObserverListener*> GenericSubscriberPublisher<TA_Base_Core::CommsMessageCorbaDef>::m_genericListeners;

CtaTelephoneManagerSummary::CtaTelephoneManagerSummary(TA_Base_Core::IEntityDataPtr agentConfiguration)
    : CtaUserMessage(),
      CtaRuntime(agentConfiguration),
      CtaAuditing(agentConfiguration->getKey(), TA_Base_Core::TelephoneManagerAudit::Context),
      TelephoneDirectoryMessageSubscriber(),
      TelephoneDirectLinesMessageSubscriber(),
      m_semaphore( 0 )
{
    FUNCTION_ENTRY( "CtaTelephoneManagerSummary" );
    FUNCTION_EXIT;
}


CtaTelephoneManagerSummary::~CtaTelephoneManagerSummary()
{
    FUNCTION_ENTRY( "~CtaTelephoneManagerSummary" );
    FUNCTION_EXIT;
}


void CtaTelephoneManagerSummary::Subscribe()
{
    FUNCTION_ENTRY( "Subscribe" );

    TelephoneDirectoryMessageSubscriber::Subscribe();
    TelephoneDirectLinesMessageSubscriber::Subscribe();

    FUNCTION_EXIT;
}


void CtaTelephoneManagerSummary::ExtractMessageData(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY( "ExtractMessageData" );

    // Call the approriate inherited function based on message.messageTypeKey
    std::string messageTypeKey(message.messageTypeKey);
    if (messageTypeKey == TA_Base_Core::TelephoneManagerSpeedDialBroadcastMessage::DirectLineModifiedEvent.getTypeKey())
    {
        TelephoneDirectLinesMessageSubscriber::ExtractMessageData(message);
    }
    else
    {
        TelephoneDirectoryMessageSubscriber::ExtractMessageData(message);
    }

    FUNCTION_EXIT;
}


void CtaTelephoneManagerSummary::PublishToListener(const TA_Base_Core::CommsMessageCorbaDef& message, IGenericObserverListener* listener)
{
    FUNCTION_ENTRY( "PublishToListener" );

    // Call the approriate inherited function based on message.messageTypeKey
    std::string messageTypeKey(message.messageTypeKey);
    if (messageTypeKey == TA_Base_Core::TelephoneManagerSpeedDialBroadcastMessage::DirectLineModifiedEvent.getTypeKey())
    {
        TelephoneDirectLinesMessageSubscriber::PublishToListener(message, listener);
    }
    else
    {
        TelephoneDirectoryMessageSubscriber::PublishToListener(message, listener);
    }

    FUNCTION_EXIT;
}


TA_Base_Bus::ItaRights* CtaTelephoneManagerSummary::getItaRights()
{
    FUNCTION_ENTRY( "getItaRights" );

    TA_THREADGUARD( m_rightsLock );

    if ( !m_rights )
    {
        m_semaphore.wait();
    }

    FUNCTION_EXIT;
    return m_rights.get();
}


