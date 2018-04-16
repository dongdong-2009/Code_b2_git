 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsMssZone.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * AtsMssZone is an implementation of IAtsMssZone. It holds the data specific to an AtsMssZone entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(AtsMssZone_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define AtsMssZone_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/ats/src/AtsMssZoneHelper.h"
#include "core/data_access_interface/ats/src/IAtsMssZone.h"

namespace TA_IRS_Core
{
    class AtsMssZone : public IAtsMssZone
    {

    public:

        /**
         * AtsMssZone (constructor)
         *
         * Construct an AtsMssZone class based around a key.
         *
         * @param key The key of this AtsMssZone in the database
         */
        AtsMssZone(const unsigned long idKey);

        /**
         * AtsMssZone (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see AtsMssZoneHelper::AtsMssZoneHelper(IData*)
         *
         */
        AtsMssZone(unsigned long row, TA_Base_Core::IData& data);

        virtual ~AtsMssZone();

		virtual unsigned long getStationId();
        virtual std::string getMssZone();
		virtual unsigned long getKey();
 
        virtual void invalidate();

        /**
         * getName
         *
         * Returns the name of this item. For alarm plan associations this is a name built up from the
         * entity or entity type and the alarm type.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

    private:

        // Assignment AtsMssZone not used so it is made private
		AtsMssZone& operator=(const AtsMssZone &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the AtsMssZoneHelper
        // has only been written for ConfigLocation objects and will not copy the AtsMssZone helper
        // for a read-only object (ie this one).
        AtsMssZone( const AtsMssZone& theAtsMssZone);  


        TA_IRS_Core::AtsMssZoneHelper * m_atsMssZoneHelper;
    };
} // closes TA_Core

#endif // !defined(AtsMssZone_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
