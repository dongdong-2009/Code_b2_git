/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/IConfigVideoTriggeringEvent.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * IConfigVideoTriggeringEvent - This is the interface supported by objects that allow configuration of
  * VideoTriggeringEvents. It provides methods for retrieving, settings, and writing data for an VideoTriggeringEvent object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigVideoTriggeringEvent_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigVideoTriggeringEvent_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/video_switch_agent/src/IVideoTriggeringEvent.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{

    class IConfigVideoTriggeringEvent : public virtual TA_Base_Core::IConfigItem,
                                        public virtual IVideoTriggeringEvent
    {

    public:

        
        virtual ~IConfigVideoTriggeringEvent() {};



        /** 
          * setStationKey
          *
          * This sets the associated station record by supplying the id
          * of the station in the VI_STATIOD_ID table.
          * There must be a matching record in the station id table, otherwise saving this
          * record will fail - as it is the pkey of the item in the VI_STATIOD_ID
          * table that is saved, not the station id itself.
          *
          * @param stationId  The id of the station
          */
        virtual void setStationId(unsigned long stationId) =0;


        /** 
          * setEventId
          *
          * Sets the event ID
          *
          * @param eventId
          */
        virtual void setEventId(unsigned long eventId) =0;


        /** 
          * setLocation
          *
          * Sets the event location
          *
          * @param location
          */
        virtual void setLocation(const std::string& location) =0;


        /** 
          * setCctvTriggering
          *
          * sets the description
          *
          * @param triggering
          */
        virtual void setCctvTriggering(const std::string& triggering) =0;


        /** 
          * setManualClosing
          *
          * Sets whether this event must be manually closed by the operator.
          *
          * @param manualClosing
          */
        virtual void setManualClosing(bool manualClosing) =0;

		// TD 15200 
		/** 
          * setEntityKey
          *
          * Sets which entity to trigger the event.
          *
          * @param entitykey
          */
        virtual void setEntityKey(unsigned long entityKey) =0;
        // TD 15200 


        /**
         * getDateCreated
         *
         * Returns the date created for this VideoTriggeringEvent
         *
         * @return The date created for this VideoTriggeringEvent as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception TA_Base_Core::DataException A TA_Base_Core::DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this VideoTriggeringEvent.
         *
         * @return The date modified for this VideoTriggeringEvent as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception TA_Base_Core::DataException A TA_Base_Core::DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;
        
        
        /**
         * deleteThisObject
         *
         * Remove this VideoTriggeringEvent from the database. Once this method has been executed, the
         * VideoTriggeringEventHelper object should be destroyed.
         *
         * @exception DatabaseException 
         *            - if there is a problem writing the data to the database.
         *            - if references to this record exists in AT_FIXED_BLOCK_MAP table.  
		 *				These can be removed by specifying the cascade option.
         *
         * @param cascade
         *        - force the removal of this record by removing all references to this record
         *          in other tables (AT_FIXED_BLOCK_MAP).  This should be set to true to prevent the 
         *          DatabaseException from being thrown.
         *
         * pre: - this VideoTriggeringEvent was initially loaded from the database
         *      - writeVideoTriggeringEventData() has already been called
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

    };
    
    typedef std::vector<IConfigVideoTriggeringEvent*> IConfigVideoTriggeringEvents;
    typedef IConfigVideoTriggeringEvents::iterator    IConfigVideoTriggeringEventsIt;
} //close namespace TA_IRS_Core
#endif // !defined(IConfigVideoTriggeringEvent_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
