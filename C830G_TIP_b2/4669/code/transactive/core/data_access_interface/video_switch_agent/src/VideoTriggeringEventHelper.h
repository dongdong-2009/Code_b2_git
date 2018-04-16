/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/video_switch_agent/src/VideoTriggeringEventHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * VideoTriggeringEventHelper is an object that is held by VideoTriggeringEvent and ConfigVideoTriggeringEvent objects. 
  * It contains all data used by VideoTriggeringEvents, and manipulation
  * methods for the data. It helps avoid re-writing code for both VideoTriggeringEvent and ConfigVideoTriggeringEvent.
  */

#if !defined(VideoTriggeringEventHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define VideoTriggeringEventHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4290) // C++ Exception Specification ignored
#endif

#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/IData.h"

#include <string>
#include <vector>
#include <climits>

namespace TA_IRS_Core
{

    class VideoTriggeringEventHelper
    {

    public:


        /** 
         * VideoTriggeringEventHelper
         *
         * This constructor creates a new VideoTriggeringEventHelper for the specified TriggeringEvent (primary key).
         *
         * @param  idKey the Station Id (primary key)
         *
         */
        VideoTriggeringEventHelper(const unsigned long key);
         
        
        /** 
         * VideoTriggeringEventHelper
         *
         * Copy constructor.
         *
         * @param   theVideoTriggeringEventHelper the original VideoTriggeringEventHelper to copy.
         *
         */
        VideoTriggeringEventHelper( const VideoTriggeringEventHelper& theVideoTriggeringEventHelper);

        /** 
         * VideoTriggeringEventHelper
         *
         * This constructor creates a new object using the input data
         *      which is representative of a row returned from SQL statement
         *
         * @param row the row of data in the data object that we should collect data from
         *          in order to construct ourselves
         *
         * @param data the TA_Base_Core::IData interface that should have been obtained using the 
         *              getBasicQueryData function
         *
         * @see getBasicQueryData         
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the primary key returns multiple records 
         *
         */
        VideoTriggeringEventHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * VideoTriggeringEventHelper
         *
         * This constructor creates a new VideoTriggeringEventHelper for configuration, initiailising
         * to 0/defaults. All fields should be set before writing to the database for the first time.
         *
         */
        VideoTriggeringEventHelper();

        
        /** 
         * ~VideoTriggeringEventHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~VideoTriggeringEventHelper();

        /**
         * isNew
         *
         * This returns whether this is a new VideoTriggeringEvent.  A new VideoTriggeringEvent is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new VideoTriggeringEvent
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        // See the interface description for comments on these
        unsigned long getKey();
        std::string getName();

        unsigned long getStationId();
        unsigned long getEventId();
        std::string getLocation();
		std::string getTriggeringEvent();
        bool getManualClosing();
		// TD 15200
		unsigned long getEntityKey();
        // TD 15200
        
        void setStationId(unsigned long stationId);
        void setEventId(unsigned long eventId);
        void setLocation(const std::string& location);
        void setTriggeringEvent(const std::string& triggeringEvent);
        void setManualClosing(bool manualClosing);
		// TD 15200
		void setEntityKey(unsigned long entityKey);
        // TD 15200

        /**
         * getDateCreated
         *
         * Returns the date created for this VideoTriggeringEvent.  If this VideoTriggeringEvent record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing VideoTriggeringEvent has been requested, all the data is loaded from the database.
         *
         * @return the date created for this VideoTriggeringEvent as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the VideoTriggeringEvent record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this VideoTriggeringEvent.  If this VideoTriggeringEvent record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing VideoTriggeringEvent has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this VideoTriggeringEvent as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the VideoTriggeringEvent record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();

        void invalidate();


        void writeVideoTriggeringEventData() throw(TA_Base_Core::TransactiveException);
        void deleteVideoTriggeringEvent(bool cascade = false);
		

    private:
        // Made private as it is not used
		VideoTriggeringEventHelper& operator=(const VideoTriggeringEventHelper &);

        void reload();
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);
        
        void modifyExistingVideoTriggeringEvent() throw(TA_Base_Core::TransactiveException);
        void addNewVideoTriggeringEvent() throw(TA_Base_Core::TransactiveException);


        unsigned long m_pkey;
        unsigned long m_stationId;
        unsigned long m_eventId;
		std::string m_location;
		std::string m_triggeringEvent;
        bool m_manualClosing;
		// TD 15200
		unsigned long m_entitykey;
		// TD 15200
        time_t m_dateCreated;
        time_t m_dateModified;

        bool m_isValidData;
        bool m_isNew; 
        

    };
} // closes TA_IRS_Core

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#endif // !defined(VideoTriggeringEventHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
