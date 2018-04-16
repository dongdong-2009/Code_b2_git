/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_Review_Branch_DBSync/3001/transactive/core/data_access_interface/ecs/src/IECSMasterMode.h $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2013/09/26 17:01:01 $
  * Last modified by: $Author: qi.huang $
  * 
  * IECSMasterMode is an interface to an ECSMasterMode object. It allows the ECSMasterMode object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IECSMasterMode_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IECSMasterMode_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/ecs/src/IECSStationMode.h"
#include "core/data_access_interface/src/IItem.h"



namespace TA_IRS_Core
{
    class IECSStationMode;

    class IECSMasterMode : public virtual TA_Base_Core::IItem
    {

    public:

        virtual unsigned long getKey() =0;
        virtual unsigned long getMasterMode() =0;
        virtual std::string getCondition() =0;
        virtual int getECSZoneId() =0;
        virtual std::string getEvacuationDirection() =0;
        virtual std::string getAirflowDirection() =0;
		virtual std::string getSituation() = 0;
        virtual std::string getDisplayDirection() =0;
		virtual std::string getVerificationMessage() = 0;

        virtual ~IECSMasterMode() {};
        
        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

        /**
         * getAssociatedStationModes
         *
         * Returns the StationModes that this MasterMode triggers.
         *
         * @return The StationModes triggered by this MasterMode, as a vector of IStationMode*'s
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		virtual IECSStationModes getAssociatedStationModes() = 0;

        /**
         * getAssociatedStationModeKeys
         *
         * Returns the StationMode primary keys (EC_STATION_MODE.ECSMOD_ID) that are triggered by this 
         * MasterMode
         *
         * @return the StationModes triggered by this MasterMode, as a vector of keys
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database
         *         - if an error is encountered while retrieving data.
         * @throws DataException 
         *         - if the data cannot be converted to the required format 
         *           (e.g. ACCESSLEVEL should be an unsigned long)
         *         - if the wrong ammount of data is retrieved.
         */ 
		virtual std::vector<unsigned long> getAssociatedStationModeKeys() = 0;
        
		//zhangnan       
		/**
         * setAssociatedStationMode
         *
         * Changes a StationMode association for this MasterMode, only call ECSMasterModeHelper the same function
         *
         * @param  StationModeKey the primary key of the StationMode (EC_STATION_MODE.ECSMOD_ID) to add or 
         *                        remove from the associations.
         * @param  isAssociated   indicates whether this StationMode is to be added or removed. True if 
         *                        the StationMode is to be added to the associations, false if the 
         *                        StationMode is to be removed.
         *
         * pre: This MasterMode has not been deleted
         */
        virtual void setAssociatedStationMode( unsigned long StationModeKey, bool isAssociated) = 0;

    };

    typedef std::vector<IECSMasterMode*> IECSMasterModes;
    typedef IECSMasterModes::iterator    IECSMasterModesIt;
} //close namespace TA_IRS_Core

#endif // !defined(IECSMasterMode_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
