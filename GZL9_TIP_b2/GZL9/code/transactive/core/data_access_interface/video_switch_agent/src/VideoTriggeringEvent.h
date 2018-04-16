 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/VideoTriggeringEvent.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * VideoTriggeringEvent is an implementation of IVideoTriggeringEvent. It holds the data specific to an VideoTriggeringEvent entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(VideoTriggeringEvent_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define VideoTriggeringEvent_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/video_switch_agent/src/VideoTriggeringEventHelper.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoTriggeringEvent.h"

namespace TA_IRS_Core
{
    class VideoTriggeringEvent : public IVideoTriggeringEvent
    {

    public:

        /**
         * VideoTriggeringEvent (constructor)
         *
         * Construct an VideoTriggeringEvent class based around a key.
         *
         * @param key The key of this VideoTriggeringEvent in the database
         */
        VideoTriggeringEvent(const unsigned long key);


        /**
         * VideoTriggeringEvent (constructor)
         *
         * Constructs a new instance using the information provided in TA_Base_Core::IData interface
         *
         * @see VideoTriggeringEventHelper::VideoTriggeringEventHelper(TA_Base_Core::IData*)
         *
         */
        VideoTriggeringEvent(unsigned long row, TA_Base_Core::IData& data);

        virtual ~VideoTriggeringEvent();


        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception TA_Base_Core::DataException A TA_Base_Core::DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();


		virtual unsigned long getStationId();
        virtual unsigned long getEventId();
        virtual std::string getLocation();
        virtual std::string getCctvTriggering();
        virtual bool getManualClosing();
		// TD 15200
        virtual unsigned long getEntityKey();
        // TD 15200
        virtual void invalidate();
		
    private:

        // Assignment VideoTriggeringEvent not used so it is made private
		VideoTriggeringEvent& operator=(const VideoTriggeringEvent &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the VideoTriggeringEventHelper
        // has only been written for ConfigLocation objects and will not copy the VideoTriggeringEvent helper
        // for a read-only object (ie this one).
        VideoTriggeringEvent( const VideoTriggeringEvent& theVideoTriggeringEvent);  


        TA_IRS_Core::VideoTriggeringEventHelper * m_VideoTriggeringEventHelper;
    };
} // closes TA_IRS_Core

#endif // !defined(VideoTriggeringEvent_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
