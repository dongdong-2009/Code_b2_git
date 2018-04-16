 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/video_switch_agent/src/VideoStationId.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * VideoStationId is an implementation of IVideoStationId. It holds the data specific to an VideoStationId entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(VideoStationId_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define VideoStationId_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/video_switch_agent/src/VideoStationIdHelper.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoStationId.h"

namespace TA_IRS_Core
{
    class VideoStationId : public IVideoStationId
    {

    public:

        /**
         * VideoStationId (constructor)
         *
         * Construct an VideoStationId class based around a key.
         *
         * @param key The key of this VideoStationId in the database
         */
        VideoStationId(const unsigned long idKey);

        /**
         * VideoStationId (constructor)
         *
         * Constructs a new instance using the information provided in TA_Base_Core::IData interface
         *
         * @see VideoStationIdHelper::VideoStationIdHelper(TA_Base_Core::IData*)
         *
         */
        VideoStationId(unsigned long row, TA_Base_Core::IData& data);

        virtual ~VideoStationId();

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
        virtual unsigned long getTaLocation();
 
        virtual void invalidate();
		
    private:

        // Assignment VideoStationId not used so it is made private
		VideoStationId& operator=(const VideoStationId &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the VideoStationIdHelper
        // has only been written for ConfigLocation objects and will not copy the VideoStationId helper
        // for a read-only object (ie this one).
        VideoStationId( const VideoStationId& theVideoStationId);  


        TA_IRS_Core::VideoStationIdHelper * m_VideoStationIdHelper;
    };
} // closes TA_IRS_Core

#endif // !defined(VideoStationId_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
