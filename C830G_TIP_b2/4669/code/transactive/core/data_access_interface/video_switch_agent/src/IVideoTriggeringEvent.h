/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/video_switch_agent/src/IVideoTriggeringEvent.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IVideoTriggeringEvent is an interface to an VideoTriggeringEvent object. It allows the VideoTriggeringEvent object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IVideoTriggeringEvent_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IVideoTriggeringEvent_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IVideoTriggeringEvent : public virtual TA_Base_Core::IItem
    {

    public:

        /** 
          * getStationId
          *
          * Gets the station Id for this event.
          */
        virtual unsigned long getStationId() =0;


        /** 
          * getEventId
          *
          * Gets the event Id for this triggering event.
          */
        virtual unsigned long getEventId() =0;


        /** 
          * getLocation
          *
          * Gets the location description for this event
          */
        virtual std::string getLocation() =0;


        /** 
          * getCctvTriggering
          *
          * Gets the CCTV triggerning string
          */
        virtual std::string getCctvTriggering() =0;


        /** 
          * getManualClosing
          *
          * If true - then this alarm can be manually closed
          * It requires manual closing.
          */
        virtual bool getManualClosing() =0;

        // TD 15200
        /** 
          * getEntityKey
          *
          * Gets the entity key for this triggering event.
          */
        virtual unsigned long getEntityKey() =0;
		// TD 15200

        virtual ~IVideoTriggeringEvent() {};

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };


    typedef std::vector<IVideoTriggeringEvent*> IVideoTriggeringEvents;
    typedef IVideoTriggeringEvents::iterator    IVideoTriggeringEventsIt;
} //close namespace TA_IRS_Core

#endif // !defined(IVideoTriggeringEvent_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
