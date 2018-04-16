 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ecs/src/ECSMasterMode.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * ECSMasterMode is an implementation of IECSMasterMode. It holds the data specific to an ECSMasterMode entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(ECSMasterMode_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ECSMasterMode_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/ecs/src/ECSMasterModeHelper.h"
#include "core/data_access_interface/ecs/src/IECSMasterMode.h"

namespace TA_IRS_Core
{


    class ECSMasterMode : public IECSMasterMode
    {

    public:



        /**
         * ECSMasterMode (constructor)
         *
         * Construct an ECSMasterMode class based around a key.
         *
         * @param key The key of this ECSMasterMode in the database
         */
        ECSMasterMode(const unsigned long key);

        /**
         * ECSMasterMode (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see ECSMasterModeHelper::ECSMasterModeHelper(IData*)
         *
         */
        ECSMasterMode(unsigned long row, TA_Base_Core::IData& data);

        virtual ~ECSMasterMode();

        virtual unsigned long getKey();
        virtual unsigned long getMasterMode();
        virtual std::string getCondition();
        virtual int getECSZoneId();
        virtual std::string getEvacuationDirection();
        virtual std::string getAirflowDirection();
		virtual std::string getSituation();
        virtual std::string getDisplayDirection();
		virtual std::string getVerificationMessage();

        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the ECS Master Mode this is 
         * just the key in a string format e.g. "ECS Master Mode 1".
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
		virtual std::vector<IECSStationMode*> getAssociatedStationModes();

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
		virtual std::vector<unsigned long> getAssociatedStationModeKeys();
        

        /**
         * invalidate
         *
         * Make the data contained by this ECSMasterMode as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

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
        virtual void setAssociatedStationMode( unsigned long StationModeKey, bool isAssociated);

    private:

        // Assignment ECSMasterMode not used so it is made private
		ECSMasterMode& operator=(const ECSMasterMode &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the ECSMasterModeHelper
        // has only been written for ConfigLocation objects and will not copy the ECSMasterMode helper
        // for a read-only object (ie this one).
        ECSMasterMode( const ECSMasterMode& theECSMasterMode);  


        TA_IRS_Core::ECSMasterModeHelper * m_ECSMasterModeHelper;
    };
} // closes TA_Core

#endif // !defined(ECSMasterMode_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
