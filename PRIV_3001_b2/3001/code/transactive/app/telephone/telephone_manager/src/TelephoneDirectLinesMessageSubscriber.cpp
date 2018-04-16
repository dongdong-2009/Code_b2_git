/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/TelephoneDirectLinesMessageSubscriber.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class will receive all notifications of changes posted on change of the
  * Telephone Speed Dial buttons. The class also acts as the publisher for
  * typically GUI components interested in notifications of these changes according
  * to the Observer pattern.
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"

#include "core/message/src/IMessageSubscriber.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/TelephoneManagerSpeedDialBroadcastMessage_MessageTypes.h"

#include "app/telephone/telephone_manager/src/TelephoneDirectLinesMessageSubscriber.h"
#include "app/telephone/telephone_manager/src/ITelephoneDirectLinesListener.h"

#include "bus/telephone/telephone_manager/IDL/src/ITelephoneManagerCommonCorbaDef.h"


TelephoneDirectLinesMessageSubscriber::TelephoneDirectLinesMessageSubscriber()
{
    FUNCTION_ENTRY( "TelephoneDirectLinesMessageSubscriber" );
    FUNCTION_EXIT;
}


TelephoneDirectLinesMessageSubscriber::~TelephoneDirectLinesMessageSubscriber()
{
    FUNCTION_ENTRY( "~TelephoneDirectLinesMessageSubscriber" );
    FUNCTION_EXIT;
}


void TelephoneDirectLinesMessageSubscriber::addListener(ITelephoneDirectLinesListener* plistener)
{
    FUNCTION_ENTRY( "addListener" );

    addGenericListener(plistener);

    FUNCTION_EXIT;
}


void TelephoneDirectLinesMessageSubscriber::Subscribe()
{
    FUNCTION_ENTRY( "Subscribe" );

    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
        TA_Base_Core::TelephoneManagerSpeedDialBroadcastMessage::Context,
        this);

    FUNCTION_EXIT;
}


void TelephoneDirectLinesMessageSubscriber::ExtractMessageData(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY( "ExtractMessageData" );

    TA_IRS_Bus::DirectLineEvent *directLineEvent;
    message.messageState >>= directLineEvent;
    m_speedDialButtonDetails.SetName = directLineEvent->SetName;
    m_speedDialButtonDetails.buttonPosition = directLineEvent->buttonPosition;
    m_speedDialButtonDetails.telephoneNumber = directLineEvent->telephoneNumber;
    m_speedDialButtonDetails.label = directLineEvent->label;

    FUNCTION_EXIT;
}


void TelephoneDirectLinesMessageSubscriber::PublishToListener(const TA_Base_Core::CommsMessageCorbaDef& message, IGenericObserverListener* listener)
{
    FUNCTION_ENTRY( "PublishToListener" );

    dynamic_cast<ITelephoneDirectLinesListener*>(listener)->onDirectLineModifiedEvent(this, m_speedDialButtonDetails);

    FUNCTION_EXIT;
}


