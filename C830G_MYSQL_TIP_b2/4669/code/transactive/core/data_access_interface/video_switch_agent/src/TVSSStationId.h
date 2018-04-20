 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/core/data_access_interface/video_switch_agent/src/TVSSStationId.h $
  * @author Zhang hongzhi
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/02/5 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * TVSSStationId is an implementation of ITVSSStationId. It holds the data specific to an TVSSStationId entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(TVSSStationId_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define TVSSStationId_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/video_switch_agent/src/TVSSStationIdHelper.h"
#include "core/data_access_interface/video_switch_agent/src/ITVSSStationId.h"

namespace TA_IRS_Core
{
    class TVSSStationId : public ITVSSStationId
    {

    public:

        /**
         * TVSSStationId (constructor)
         *
         * Construct an TVSSStationId class based around a key.
         *
         * @param key The key of this TVSSStationId in the database
         */
        TVSSStationId(const unsigned long idKey);

        /**
         * TVSSStationId (constructor)
         *
         * Constructs a new instance using the information provided in TA_Base_Core::IData interface
         *
         * @see TVSSStationIdHelper::TVSSStationIdHelper(TA_Base_Core::IData*)
         *
         */
        TVSSStationId(unsigned long row, TA_Base_Core::IData& data);

        virtual ~TVSSStationId();

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

        // Assignment TVSSStationId not used so it is made private
		TVSSStationId& operator=(const TVSSStationId &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the TVSSStationIdHelper
        // has only been written for ConfigLocation objects and will not copy the TVSSStationId helper
        // for a read-only object (ie this one).
        TVSSStationId( const TVSSStationId& theTVSSStationId);  


        TA_IRS_Core::TVSSStationIdHelper * m_TVSSStationIdHelper;
    };
} // closes TA_IRS_Core

#endif // !defined(TVSSStationId_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
