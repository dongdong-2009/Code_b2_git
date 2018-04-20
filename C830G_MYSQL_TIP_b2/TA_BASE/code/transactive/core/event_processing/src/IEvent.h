/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/event_processing/src/IEvent.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Defines the interface that all events must adhere to.
  *
  */

#if !defined(_I_EVENT_H_)
#define _I_EVENT_H_

#include "boost/smart_ptr.hpp"

namespace TA_Base_Core
{
    namespace EventConstants
    {
        const unsigned long INVALID_KEY = 0xFFFFFFFF;
    };

    class IEvent
    {
    public:
        /** 
          * getEntityKey
          *
          * Returns the key of the entity associated with the event.
          *
          * @return unsigned long The entity key.
          */
        virtual unsigned long getEntityKey() =0;


        /** 
          * getEntityTypeKey
          *
          * Returns the key of the entity type associated with the event.
          *
          * @return unsigned long The entity type key.
          */
        virtual unsigned long getEntityTypeKey() =0;


        /** 
          * getEventTypeKey
          *
          * Returns the event type key.
          *
          * @return unsigned long The event type key.
          */
        virtual unsigned long getEventTypeKey() =0;


        /** 
          * writeToDatabase
          *
          * Saves the event into the database.
          */
        virtual void writeToDatabase() =0;


        /** 
          * serialize
          *
          * TBD: Only required when a persistant queue is needed.
          */
        virtual void serialize() /*=0*/ {};


        /** 
          * deserialize
          *
          * TBD: Only required when a persistant queue is needed.
          */
        virtual void deserialize() /*=0*/ {};
    };

    typedef boost::shared_ptr<IEvent> Event;
};

#endif // !defined(_I_EVENT_H_)

