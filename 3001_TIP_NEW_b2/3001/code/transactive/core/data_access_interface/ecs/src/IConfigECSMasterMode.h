/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ecs/src/IConfigECSMasterMode.h $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2013/09/18 15:18:25 $
  * Last modified by: $Author: huangjian $
  * 
  * IConfigECSMasterMode - This is the interface supported by objects that allow configuration of
  * ECSMasterModes. It provides methods for retrieving, settings, and writing data for an ECSMasterMode object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigECSMasterMode_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigECSMasterMode_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/ecs/src/IECSMasterMode.h"

namespace TA_IRS_Core
{

    class IConfigECSMasterMode : public virtual TA_Base_Core::IConfigItem, public virtual IECSMasterMode
    {

    public:


        virtual ~IConfigECSMasterMode() {};

        virtual void setMasterMode(const unsigned long mode) =0;
        virtual void setCondition(const std::string& Condition) =0;
        virtual void setECSZoneId(const int zoneId) =0;
        virtual void setEvacuationDirection(const std::string& evacDir) =0;
        virtual void setAirflowDirection(const std::string& airflowDir) =0;
        virtual void setDisplayDirection(const std::string& displayDir) =0;
		virtual void setVerificationMessage(const std::string& msg) = 0;
		virtual void setSituation(const std::string& situation) =0;

        /**
         * setAssociatedStationMode
         *
         * Changes a StationMode association for this MasterMode
         *
         * @param unsigned long - This is the key of the StationMode to add or remove from the associations
         * @param bool - This indicates whether this StationMode is to be added or removed. True if the
         *               StationMode is to be added to the associations, false if the StationMode is to be
         *               removed.
         *
         * pre: This MasterMode has not been deleted
         */
        virtual void setAssociatedStationMode( unsigned long StationModeKey, bool isAssociated) =0;

        /**
         * getAssociatedStationModeKeys
         *
         * Returns the StationMode keys that are triggered by this MasterMode
         *
         * @return The StationModes triggered by this MasterMode, as a vector of keys
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		virtual std::vector<unsigned long> getAssociatedStationModeKeys() =0;

        /**
         * getDateCreated
         *
         * Returns the date created for this ECSMasterMode
         *
         * @return The date created for this ECSMasterMode as a time_t.
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
         * Returns the date modified for this ECSMasterMode.
         *
         * @return The date modified for this ECSMasterMode as a time_t.
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
         * Removes this ECSMasterMode from the database. 
         * The calling application MUST then delete this ECSMasterMode object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this ECSMasterMode was initially loaded from the database
         *      OR     - writeECSMasterModeData() has already been called
         *      This ECSMasterMode has not been deleted
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

    };
} //close namespace TA_IRS_Core
#endif // !defined(IConfigECSMasterMode_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
