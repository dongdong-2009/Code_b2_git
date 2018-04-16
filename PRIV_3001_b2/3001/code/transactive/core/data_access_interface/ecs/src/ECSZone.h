 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ecs/src/ECSZone.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * ECSZone is an implementation of IECSZone. It holds the data specific to an ECSZone entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(ECSZone_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ECSZone_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/ecs/src/ECSZoneHelper.h"
#include "core/data_access_interface/ecs/src/IECSZone.h"

namespace TA_IRS_Core
{
    class ECSZone : public IECSZone
    {

    public:

        /**
         * ECSZone (constructor)
         *
         * Construct an ECSZone class based around a key.
         *
         * @param key The key of this ECSZone in the database
         */
        ECSZone(const unsigned long idKey);

        /**
         * ECSZone (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see ECSZoneHelper::ECSZoneHelper(IData*)
         *
         */
        ECSZone(unsigned long row, TA_Base_Core::IData& data);

        virtual ~ECSZone();

        virtual int getECSZoneId();
        virtual std::string getECSLocation();
        virtual std::string getTrack();
        virtual unsigned long getOccEquipmentEntityKey();
        virtual unsigned long getStnEquipmentEntityKey();	
        virtual bool getIsIgnoringCongestion();	

        virtual void invalidate();

		virtual unsigned long getKey()
        {
            return getECSZoneId();
        }

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

        // Assignment ECSZone not used so it is made private
		ECSZone& operator=(const ECSZone &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the ECSZoneHelper
        // has only been written for ConfigLocation objects and will not copy the ECSZone helper
        // for a read-only object (ie this one).
        ECSZone( const ECSZone& theECSZone);  


        TA_IRS_Core::ECSZoneHelper * m_ECSZoneHelper;
    };
} // closes TA_Core

#endif // !defined(ECSZone_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
