/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ecs/src/IConfigECSZone.h $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * IConfigECSZone - This is the interface supported by objects that allow configuration of
  * ECSZones. It provides methods for retrieving, settings, and writing data for an ECSZone object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigECSZone_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigECSZone_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/ecs/src/IECSZone.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{

    class IConfigECSZone : public virtual TA_Base_Core::IConfigItem, public virtual IECSZone
    {

    public:

        
        virtual ~IConfigECSZone() {};

        virtual void setECSZoneId(int id) =0;
        virtual void setECSLocation(const std::string& ecsLocation) =0;
        virtual void setTrack(const std::string& track) =0;
        virtual void setOccEquipmentEntityKey(unsigned long key) =0;
        virtual void setStnEquipmentEntityKey(unsigned long key) =0;
		virtual void setIsIgnoringCongestion(bool isIgnoringCongestion) =0;	



        /**
         * getDateCreated
         *
         * Returns the date created for this ECSZone
         *
         * @return The date created for this ECSZone as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this ECSZone.
         *
         * @return The date modified for this ECSZone as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;

        
        
        /**
         * deleteThisObject
         *
         * Remove this ECSZone from the database. Once this method has been executed, the
         * ECSZoneHelper object should be destroyed.
         *
         * @exception DatabaseException 
         *            - if there is a problem writing the data to the database.
         *            - if references to this record exists in FIXED_BLOCK_ECS_ZONE_MAP and 
         *              ECS_MASTER_MODE tables.  These can be removed by specifying the 
         *              cascade option.
         *
         * @param cascade
         *        - force the removal of this record by removing all references to this record
         *          in other tables (FIXED_BLOCK_ECS_ZONE_MAP, ECS_MASTER_MODE 
         *          (ECS_MASTER_MODE_HELPER).  This should be set to true to prevent the 
         *          DatabaseException from being thrown.
         *
         * pre: - this ECSZone was initially loaded from the database
         *      - writeECSZoneData() has already been called
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

    };
    
    typedef std::vector<IConfigECSZone*> IConfigECSZones;
    typedef IConfigECSZones::iterator    IConfigECSZonesIt;
} //close namespace TA_IRS_Core
#endif // !defined(IConfigECSZone_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
