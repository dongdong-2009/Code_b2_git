/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ecs/src/ECSStationModeHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * ECSStationModeHelper is an object that is held by ECSStationMode and ConfigECSStationMode objects. 
  * It contains all data used by ECSStationModes, and manipulation
  * methods for the data. It helps avoid re-writing code for both ECSStationMode and ConfigECSStationMode.
  */

#if !defined(ECSStationModeHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define ECSStationModeHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"

#include <string>
#include <vector>
#include <climits>

namespace TA_Base_Core
{
	class IData;
}

namespace TA_IRS_Core
{
    class ECSStationModeHelper
    {

    public:


        /** 
         * ECSStationModeHelper
         *
         * This constructor creates a new ECSStationModeHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        ECSStationModeHelper(const unsigned long key);

        
        /** 
         * ECSStationModeHelper
         *
         * Copy constructor.
         *
         * @param   theECSStationModeHelper the original ECSStationModeHelper to copy.
         *
         */
        ECSStationModeHelper( const ECSStationModeHelper& theECSStationModeHelper);


        /** 
         * ECSStationModeHelper
         *
         * This constructor creates a new ECSStationModeHelper for configuration, initiailising
         * the primary key to 0.  This primary key will be automatically set to a valid value 
         * when writeECSStationModeData() is called for the first time.
         *
         */
        ECSStationModeHelper();


        /** 
         * ECSStationModeHelper
         *
         * This constructor creates a new object using the input data
         *      which is representative of a row returned from SQL statement
         *
         * @param row the row of data in the data object that we should collect data from
         *          in order to construct ourselves
         *
         * @param data the IData interface that should have been obtained using the 
         *              getBasicQueryData function
         *
         * @see getBasicQueryData         
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the primary key returns multiple records 
         *
         */
        ECSStationModeHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * ~ECSStationModeHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~ECSStationModeHelper();


        /**
         * isNew
         *
         * This returns whether this is a new ECSStationMode.  A new ECSStationMode is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new ECSStationMode
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the primary key of this ECSStationMode.
         *
         * pre:    the data exists in the database
         *
         * @return the primary key 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();


        /**
         * getStationMode
         *
         * Returns the Station Mode of this ECSStationMode.  If this ECSStationMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSStationMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Station Mode 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getStationMode();


        /**
         * getLocationKey
         *
         * Returns the Location Key of this ECSStationMode.  If this ECSStationMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSStationMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Location Key
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getLocationKey();


        /**
         * getOperation
         *
         * Returns the Operation of this ECSStationMode.  If this ECSStationMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSStationMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Operation description
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getOperation();

        
        /**
         * getTrack
         *
         * Returns the Track of this ECSStationMode.  If this ECSStationMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSStationMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Track description
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getTrack();


        /**
         * getCondition
         *
         * Returns the Condition of this ECSStationMode.  If this ECSStationMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSStationMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Condition description 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getCondition();

        /**
         * getCongestion Logic
         *
         * Returns the Congestion Logic of this ECSStationMode.  If this ECSStationMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSStationMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Congestion Logic  
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getCongestionLogic();
        
        /**
         * setStationMode
         *
         * Sets the Station Mode of this ECSStationMode. The set values are not written to the database
         * until writeECSStationModeData() is called.  This method should only be used by the 
         * ConfigECSStationMode class.
         *
         * @param  stationMode the Station Mode for this ECSStationMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setStationMode(const unsigned long mode);


        /**
         * setLocationKey
         *
         * Sets the Location Key of this ECSStationMode. The set values are not written to the database
         * until writeECSStationModeData() is called.  This method should only be used by the 
         * ConfigECSStationMode class.  For data to be correctly written to the database, the referenced 
         * PKEY in the LOCATION table must exist.
         *
         * @param  location key the Location Key for this ECSStationMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLocationKey(const unsigned long locationKey);


        /**
         * setOperation
         *
         * Sets the Operation of this ECSStationMode. The set values are not written to the database
         * until writeECSStationModeData() is called.  This method should only be used by the 
         * ConfigECSStationMode class.
         *
         * @param  operation the Operation description for this ECSStationMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setOperation(const std::string& operation);


        /**
         * setTrack
         *
         * Sets the Track of this ECSStationMode. The set values are not written to the database
         * until writeECSStationModeData() is called.  This method should only be used by the 
         * ConfigECSStationMode class.  Note that the track field can be NULL and doesn't require
         * a value before writing to the database.
         *
         * @param  track the Track description for this ECSStationMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setTrack(const std::string& track);


        /**
         * setCondition
         *
         * Sets the Condition of this ECSStationMode. The set values are not written to the database
         * until writeECSStationModeData() is called.  This method should only be used by the 
         * ConfigECSStationMode class.
         *
         * @param  condition the Condition description for this ECSStationMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setCondition(const std::string& condition);
  

        /**
         * setCongestion Logic
         *
         * Sets the Congestion Logic of this ECSStationMode. The set values are not written to the database
         * until writeECSStationModeData() is called.  This method should only be used by the 
         * ConfigECSStationMode class.
         *
         * @param  congestionLogic the Congestion Logic for this ECSStationMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setCongestionLogic(const std::string& congestionLogic);      

        
        /**
         * getDateCreated
         *
         * Returns the date created for this ECSStationMode.  If this ECSStationMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSStationMode has been requested, all the data is loaded from the database.
         *
         * @return the date created for this ECSStationMode as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this ECSStationMode.  If this ECSStationMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSStationMode has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this ECSStationMode as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();

        
        /**
         * invalidate
         *
         * Make the data contained by this ECSStationMode as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeECSStationModeData() has been called prior to calling
         * this method on a new ECSStationMode, as it does not make any sense to invalidate an
         * ECSStationMode that has not yet been written to the database.
         *
         * pre:    This ECSStationMode was initially loaded from the database
         *         or writeECSStationModeData() has already been called
         */
        void invalidate();


        /**
         * writeECSStationModeData
         * 
         * Write this ECSStationMode to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. ECSZoneId should be an unsigned long)
         *          - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the ECSStationMode returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the ECSStationMode object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeECSStationModeData(); //throw(TA_Base_Core::TransactiveException);


        /**
         * deleteECSStationMode
         *
         * Remove this ECSStationMode from the database. Once this method has been executed, the
         * ECSStationModeHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This ECSStationMode was initially loaded from the database
         *         or writeECSStationModeData() has already been called
         *
         * @param  cascade set to true to force the removal of this record by removing all 
         *         references to this record in the ECS_MASTER_MODE_HELPER table.  This should 
         *         be set to true to prevent the DatabaseException from being thrown.
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException 
         *         - CANNOT_DELETE if references to this record exists in the ECS_MASTER_MODE 
         *           table.  These can be removed by specifying the cascade option.
         *
         */
        void deleteECSStationMode(bool cascade = false);
	

    private:
        // Made private as it is not used
		ECSStationModeHelper& operator=(const ECSStationModeHelper &);


        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This ECSStationMode was initially loaded from the database
         *         or writeECSStationModeData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSStationMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the primary key returns multiple records 
         *
         */
        void reload();
        

        /**
         * reloadUsing
         *
         * Fills out the local members with data contained in the input data object
         *
         * @param row the row to query in the data object
         *
         * @param data the IData interface that should have been obtained using the 
         *              getBasicQueryData function
         *         
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the primary key returns multiple records 
         *
         */
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);

        
        /**
         * modifyExistingECSStationMode
         *
         * This will update an existing ECSStationMode in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the ECSStationMode object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         *
         */
        void modifyExistingECSStationMode(); //throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewECSStationMode
         *
         * This will add a new ECSStationMode in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the ECSStationMode object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         *
         */
        void addNewECSStationMode(); //throw(TA_Base_Core::TransactiveException);

        void validateLocationKeyExistence(); //throw(TA_Base_Core::TransactiveException);
        void validateLocationStationModeUniqueness(); //throw(TA_Base_Core::TransactiveException);
        

        unsigned long m_key;
        unsigned long m_stationMode;
        unsigned long m_locationKey;
  		bool m_locationKeyHasBeenSet;  // Need special flag as locationKey can be 0
        std::string m_operation;
        std::string m_track;
        std::string m_condition;
        std::string m_congestionLogic;
        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew;

    };
} // closes TA_Core

#endif // !defined(ECSStationModeHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
