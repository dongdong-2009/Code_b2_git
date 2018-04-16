 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/AtsDssPlanMap.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * AtsDssPlanMap is an implementation of IAtsDssPlanMap. It holds the data specific to an AtsDssPlanMap entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(AtsDssPlanMap_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define AtsDssPlanMap_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/ats/src/AtsDssPlanMapHelper.h"
#include "core/data_access_interface/ats/src/IAtsDssPlanMap.h"

namespace TA_IRS_Core
{
    class AtsDssPlanMap : public IAtsDssPlanMap
    {

    public:

        /**
         * AtsDssPlanMap (constructor)
         *
         * Construct an AtsDssPlanMap class based around a key.
         *
         * @param key The key of this AtsDssPlanMap in the database
         */
        AtsDssPlanMap(const unsigned long idKey);

        /**
         * AtsDssPlanMap (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see AtsDssPlanMapHelper::AtsDssPlanMapHelper(IData*)
         *
         */
        AtsDssPlanMap(unsigned long row, TA_Base_Core::IData& data);

        virtual ~AtsDssPlanMap();

		virtual unsigned long getKey();
		virtual unsigned long getDssEvent();
		virtual std::string getPlanPath();
        
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

        // Assignment AtsDssPlanMap not used so it is made private
		AtsDssPlanMap& operator=(const AtsDssPlanMap &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the AtsDssPlanMapHelper
        // has only been written for ConfigLocation objects and will not copy the AtsDssPlanMap helper
        // for a read-only object (ie this one).
        AtsDssPlanMap( const AtsDssPlanMap& theAtsDssPlanMap);  


        TA_IRS_Core::AtsDssPlanMapHelper * m_atsDssPlanMapHelper;
    };
} // closes TA_Core

#endif // !defined(AtsDssPlanMap_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
