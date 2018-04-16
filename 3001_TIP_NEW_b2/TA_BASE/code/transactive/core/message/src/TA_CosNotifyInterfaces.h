#if !defined(TA_CosNotifyInterfaces_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_)
#define TA_CosNotifyInterfaces_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/message/src/TA_CosNotifyInterfaces.h $
  * @author:  Lachlan Partington
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/12/17 14:00:24 $
  * Last modified by:  $Author: shurui.liu $
  * 
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <vector>

namespace CosNotification
{
    struct StructuredEvent;
    class EventTypeSeq;
};

namespace TA_Base_Core
{
    // Refer to TA_CosNotifyChannel.h for more information about these interfaces.


    // IEventHandler is implemented by components that process batches of events (a batch
    // can contain one or more events).  
    struct IEventHandler
    {
        // Events in the batch must be processed transactionally - ie. the implementor should 
        // either process ALL or NONE of the events in the batch.  false should be returned
        // if a failure occurred and the events were not processed.
        // NOTE that the caller of onEventsReceived() keeps ownership of the events in the 
        // vector (ie. the framework).
        // Exceptions must NOT be thrown.
    	virtual bool onEventsReceived(const std::vector<const CosNotification::StructuredEvent*>& events) = 0;

        virtual ~IEventHandler() {}
    };


    // Subscription change notifications are sent to all suppliers by an event channel when
    // a consumer modifies the filters attached to its proxy supplier.
    struct ISubChangeHandler
    {
        // 'added' and 'removed' will NOT be empty.
        // Exceptions must NOT be thrown.
		virtual void onSubscriptionChange(const CosNotification::EventTypeSeq& added, const CosNotification::EventTypeSeq& removed) = 0;

        // Indicates that the subscription change deltas previously sent by onSubscriptionChange() 
        // are no longer valid.
        virtual void onSubscriptionReset() = 0;

        virtual ~ISubChangeHandler() {}
    };

}; // TA_Base_Core

#endif // !defined(TA_CosNotifyInterfaces_D7D3B769_3E7E_4621_9507_B6CF420E7E69__INCLUDED_)
