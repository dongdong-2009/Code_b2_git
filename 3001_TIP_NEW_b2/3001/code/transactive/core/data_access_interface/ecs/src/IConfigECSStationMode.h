/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ecs/src/IConfigECSStationMode.h $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * IConfigECSStationMode - This is the interface supported by objects that allow configuration of
  * ECSStationModes. It provides methods for retrieving, settings, and writing data for an ECSStationMode object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigECSStationMode_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigECSStationMode_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/ecs/src/IECSStationMode.h"

namespace TA_IRS_Core
{

    class IConfigECSStationMode : public virtual TA_Base_Core::IConfigItem, public virtual IECSStationMode
    {

    public:


        virtual ~IConfigECSStationMode() {};


        virtual void setStationMode(const unsigned long mode) =0;
        virtual void setLocationKey(const unsigned long locationKey) =0;
        virtual void setOperation(const std::string& operation) =0;
        virtual void setTrack(const std::string& track) =0;
        virtual void setCondition(const std::string& condition) =0;
        virtual void setCongestionLogic(const std::string& congestionLogic) =0;
        

        
        /**
         * getDateCreated
         *
         * Returns the date created for this ECSStationMode
         *
         * @return The date created for this ECSStationMode as a time_t.
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
         * Returns the date modified for this ECSStationMode.
         *
         * @return The date modified for this ECSStationMode as a time_t.
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
         * Removes this ECSStationMode from the database. 
         * The calling application MUST then delete this ECSStationMode object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this ECSStationMode was initially loaded from the database
         *      OR     - writeECSStationModeData() has already been called
         *      This ECSStationMode has not been deleted
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

    };
} //close namespace TA_IRS_Core
#endif // !defined(IConfigECSStationMode_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
