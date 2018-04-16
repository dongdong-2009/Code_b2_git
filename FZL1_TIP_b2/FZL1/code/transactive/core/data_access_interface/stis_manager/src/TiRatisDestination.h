 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/stis_manager/src/TiRatisDestination.h $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * TiRatisDestination is an implementation of ITiRatisDestination. It holds the data specific to an TiRatisDestination entry
  * in the database, and allows read-only access to that data.
  */


#ifndef TI_RATIS_DESTIONATION_H_INCLUDED
#define TI_RATIS_DESTIONATION_H_INCLUDED

#include <string>

#include "core/data_access_interface/stis_manager/src/TiRatisDestinationHelper.h"
#include "core/data_access_interface/stis_manager/src/ITiRatisDestination.h"

namespace TA_IRS_Core
{

    class TiRatisDestination : public ITiRatisDestination
    {

    public:


        /**
         * TiRatisDestination (constructor)
         *
         * Construct an TiRatisDestination class based around a key.
         *
         * @param key The key of this TiRatisDestination in the database
         */
        TiRatisDestination(const unsigned long key);

        /**
         * TiRatisDestination (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see TiRatisDestinationHelper::TiRatisDestinationHelper(IData*)
         *
         */
        TiRatisDestination(unsigned long row, TA_Base_Core::IData& data);

        virtual ~TiRatisDestination();

	    virtual unsigned long getKey();

	     /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA Station this is 
         * just the key in a string format e.g. "PA Station 1".
         *
         * @return The key description for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();


        /**
         * invalidate
         *
         * Make the data contained by this TiRatisDestination as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

		virtual std::string	getDestination();

    private:

        // Assignment TiRatisDestination not used so it is made private
		TiRatisDestination& operator=(const TiRatisDestination &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the TiRatisDestinationHelper
        // has only been written for ConfigLocation objects and will not copy the TiRatisDestination helper
        // for a read-only object (ie this one).
        TiRatisDestination( const TiRatisDestination& theTiRatisDestination);  

        TA_IRS_Core::TiRatisDestinationHelper * m_tiRatisDestinationHelper;
    };
} // closes TA_IRS_Core

#endif // !defined(TiRatisDestination_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
