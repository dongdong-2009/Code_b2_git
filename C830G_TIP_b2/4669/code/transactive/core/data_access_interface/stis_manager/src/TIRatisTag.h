 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/stis_manager/src/TIRatisTag.h $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * TiRatisTag is an implementation of ITiRatisTag. It holds the data specific to an TiRatisTag entry
  * in the database, and allows read-only access to that data.
  */


#ifndef TI_RATIS_TAG_H_INCLUDED
#define TI_RATIS_TAG_H_INCLUDED

#include <string>

#include "core/data_access_interface/stis_manager/src/TiRatisTagHelper.h"
#include "core/data_access_interface/stis_manager/src/ITiRatisTag.h"

namespace TA_IRS_Core
{

    class TiRatisTag : public ITiRatisTag
    {

    public:


        /**
         * TiRatisTag (constructor)
         *
         * Construct an TiRatisTag class based around a key.
         *
         * @param key The key of this TiRatisTag in the database
         */
        TiRatisTag(const unsigned long key);

        /**
         * TiRatisTag (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see TiRatisTagHelper::TiRatisTagHelper(IData*)
         *
         */
        TiRatisTag(unsigned long row, TA_Base_Core::IData& data);

        virtual ~TiRatisTag();

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
         * Make the data contained by this TiRatisTag as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

		virtual std::string	getTagGroup();

		virtual std::string	getDetail();

		virtual std::string	getRatisTag();

    private:

        // Assignment TiRatisTag not used so it is made private
		TiRatisTag& operator=(const TiRatisTag &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the TiRatisTagHelper
        // has only been written for ConfigLocation objects and will not copy the TiRatisTag helper
        // for a read-only object (ie this one).
        TiRatisTag( const TiRatisTag& theTiRatisTag);  

        TA_IRS_Core::TiRatisTagHelper * m_tiRatisTagHelper;
    };
} // closes TA_IRS_Core

#endif // !defined(TiRatisTag_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
