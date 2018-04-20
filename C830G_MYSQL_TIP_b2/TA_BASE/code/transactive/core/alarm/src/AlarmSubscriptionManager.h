#if !defined(AlarmSubscriptionManager_E71292CA_355A_4b1b_8D32_6DC69EFD271F__INCLUDED_)
#define AlarmSubscriptionManager_E71292CA_355A_4b1b_8D32_6DC69EFD271F__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/alarm/src/AlarmSubscriptionManager.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is used to receive new alarms. Used by the Alarm Viewer.
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/message/src/SubscriptionManager.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Core
{
	class MessageContext;
    class MessageType;
    class AbstractMessageSubscriber;

	namespace AlarmSubscriptionManager
	{
        // Comms message subscriptions are location-based (the subscription is for events
        // sent by a particular location).  
        // TODO LPP: sourceLocationKey is not currently allowed to be LOCAL_LOCATION or ANY_LOCATION.

	    void subscribeToEntityAlarms(
            const MessageContext& messageContext,
			AbstractMessageSubscriber* subscriber,
			unsigned long entityKey, 
			unsigned long sourceLocationKey);
	    
	    void subscribeToAlarmsFromLocation(
            const MessageContext& messageContext,
			AbstractMessageSubscriber* subscriber, 
			long sourceLocationKey);

	    void subscribeToAlarmsFromLocation(
            const MessageType& messageType,
			AbstractMessageSubscriber* subscriber, 
			long sourceLocationKey);

		void unsubscribeToMessages( AbstractMessageSubscriber* subscriber );

    };

}; // namespace TA_Base_Core

#endif // !defined(AlarmSubscriptionManager_E71292CA_355A_4b1b_8D32_6DC69EFD271F__INCLUDED_)
