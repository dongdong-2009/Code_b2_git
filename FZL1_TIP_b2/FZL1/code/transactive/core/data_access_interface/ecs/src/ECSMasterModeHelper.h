/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ecs/src/ECSMasterModeHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * ECSMasterModeHelper is an object that is held by ECSMasterMode and ConfigECSMasterMode objects. 
  * It contains all data used by ECSMasterModes, and manipulation
  * methods for the data. It helps avoid re-writing code for both ECSMasterMode and ConfigECSMasterMode.
  */

#if !defined(ECSMasterModeHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define ECSMasterModeHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"

#include <string>
#include <vector>
#include <climits>
#include "core/data_access_interface/ecs/src/IECSMasterMode.h"

namespace TA_Base_Core
{
	class IData;
}

namespace TA_IRS_Core
{
    class ECSMasterModeHelper
    {

    public:
        /** 
         * ECSMasterModeHelper
         *
         * This constructor creates a new ECSMasterModeHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        ECSMasterModeHelper(const unsigned long key);


        /** 
         * ECSMasterModeHelper
         *
         * Copy constructor.
         *
         * @param   theECSMasterModeHelper the original ECSMasterModeHelper to copy.
         *
         */
        ECSMasterModeHelper( const ECSMasterModeHelper& theECSMasterModeHelper);


        /** 
         * ECSMasterModeHelper
         *
         * This constructor creates a new ECSMasterModeHelper for configuration, initiailising
         * the primary key to 0.  This primary key will be automatically set to a valid value 
         * when writeECSMasterModeData() is called for the first time.
         *
         */
        ECSMasterModeHelper();


        /** 
         * ECSMasterModeHelper
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
        ECSMasterModeHelper(unsigned long row, TA_Base_Core::IData& data);


        /** 
         * ~ECSMasterModeHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~ECSMasterModeHelper();


        /**
         * isNew
         *
         * This returns whether this is a new ECSMasterMode.  A new ECSMasterMode is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new ECSMasterMode
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };

        /**
         * getKey
         *
         * Returns the primary key of this ECSMasterMode.
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
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();

        
        /**
         * getMasterMode
         *
         * Returns the Master Mode of this ECSMasterMode.  If this ECSMasterMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSMasterMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Master Mode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual int getMasterMode();
        
        
        /**
         * getCondition
         *
         * Returns the Condition of this ECSMasterMode.  If this ECSMasterMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSMasterMode has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getCondition();
        
        
        /**
         * getECSZoneId
         *
         * Returns the ECS Zone Id of this ECSMasterMode.  If this ECSMasterMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSMasterMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the ECS Zone Id.  Note that the ECSZoneId field in the database can be set to
         *         NULL as this is used to indicate the special Track/ECS Locations NA/All.  To 
         *         indicate that this is NULL, we return a -1.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getECSZoneId();
        
        
        /**
         * getEvacuationDirection
         *
         * Returns the Station Mode of this ECSMasterMode.  If this ECSMasterMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSMasterMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Evacuation Direction description
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getEvacuationDirection();
        
        
        /**
         * getAirflowDirection
         *
         * Returns the Airflow Direction of this ECSMasterMode.  If this ECSMasterMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSMasterMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Airflow Direction description
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getAirflowDirection();

		virtual std::string getSituation();
        
        /**
         * getDisplayDirection
         *
         * Returns the Display Direction of this ECSMasterMode.  If this ECSMasterMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSMasterMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the Display direction
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getDisplayDirection();


        /**
         * getVerificationMessage
         *
         * Returns the verification message of this ECSMasterMode.  If this ECSMasterMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSMasterMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @return the verification message if the entry in the database is not null.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         */   
		virtual std::string getVerificationMessage();


        /**
         * setVerificationMessage
         *
         * Sets the verification message of this ECSMasterMode.  If this ECSMasterMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSMasterMode has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         */ 
		virtual void setVerificationMessage(const std::string& msg);


        
        /**
         * setMasterMode
         *
         * Sets the Master Mode of this ECSMasterMode. The set values are not written to the database
         * until writeECSMasterModeData() is called.  This method should only be used by the 
         * ConfigECSMasterMode class.
         *
         * @param  mode the Master Mode description for this ECSMasterMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setMasterMode(const unsigned long mode);


        /**
         * setCondition
         *
         * Sets the Station Mode of this ECSMasterMode. The set values are not written to the database
         * until writeECSMasterModeData() is called.  This method should only be used by the 
         * ConfigECSMasterMode class.
         *
         * @param  condition the Condition description for this ECSMasterMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setCondition(const std::string& condition);


        /**
         * setECSZoneId
         *
         * Sets the ECSZoneId of this ECSMasterMode. The set values are not written to the database
         * until writeECSMasterModeData() is called.  This method should only be used by the 
         * ConfigECSMasterMode class.
         *
         * @param  zoneId the ECSZoneId for this ECSMasterMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setECSZoneId(const int zoneId);


        /**
         * setEvacuationDirection
         *
         * Sets the EvacuationDirection of this ECSMasterMode. The set values are not written to the database
         * until writeECSMasterModeData() is called.  This method should only be used by the 
         * ConfigECSMasterMode class.
         *
         * @param  evacDir the EvacuationDirection description for this ECSMasterMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setEvacuationDirection(const std::string& evacDir);


        /**
         * setAirflowDirection
         *
         * Sets the AirflowDirection of this ECSMasterMode. The set values are not written to the database
         * until writeECSMasterModeData() is called.  This method should only be used by the 
         * ConfigECSMasterMode class.
         *
         * @param  airflowDir the AirflowDirection description for this ECSMasterMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setAirflowDirection(const std::string& airflowDir);

		virtual void setSituation(const std::string& situation);
        
        /**
         * setDisplayDirection
         *
         * Sets the DisplayDirection of this ECSMasterMode. The set values are not written to the database
         * until writeECSMasterModeData() is called.  This method should only be used by the 
         * ConfigECSMasterMode class.
         *
         * @param  DisplayDir the DisplayDirection for this ECSMasterMode
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setDisplayDirection(const std::string& displayDir);

        
        /**
         * getDateCreated
         *
         * Returns the date created for this ECSMasterMode.  If this ECSMasterMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSMasterMode has been requested, all the data is loaded from the database.
         *
         * @return the date created for this ECSMasterMode as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this ECSMasterMode.  If this ECSMasterMode record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECSMasterMode has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this ECSMasterMode as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();


        /**
         * getAssociatedStationModes
         *
         * Returns the StationModes that this MasterMode triggers.
         *
         * @return The StationModes triggered by this MasterMode, as a vector of IStationMode*'s
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database
         *         - if an error is encountered while retrieving data.
         * @throws DataException 
         *         - if the data cannot be converted to the required format 
         *           (e.g. ACCESSLEVEL should be an unsigned long)
         *         - if the wrong ammount of data is retrieved.
         */ 
		virtual IECSStationModes getAssociatedStationModes();

        
        /**
         * setAssociatedStationMode
         *
         * Changes a StationMode association for this MasterMode
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
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeECSMasterModeData() has been called prior to calling
         * this method on a new ECSMasterMode, as it does not make any sense to invalidate an
         * ECSMasterMode that has not yet been written to the database.
         *
         * pre:    This ECSMasterMode was initially loaded from the database
         *         or writeECSMasterModeData() has already been called
         */
        void invalidate();


        /**
         * writeECSMasterModeData
         * 
         * Write this ECSMasterMode to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. ECSZoneId should be an unsigned long)
         *          - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the ECSMasterMode returns multiple records
         *          - if the ECSZoneId references a non-existent EC_ZONE record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the ECSMasterMode object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeECSMasterModeData() throw(TA_Base_Core::TransactiveException);


        /**
         * deleteECSMasterMode
         *
         * Remove this ECSMasterMode from the database. Once this method has been executed, the
         * ECSMasterModeHelper object should be destroyed.
         *
         * pre:    This ECSMasterMode was initially loaded from the database
         *         or writeECSMasterModeData() has already been called
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
        void deleteECSMasterMode(bool cascade = false);

		
    private:


        // Made private as it is not used
		ECSMasterModeHelper& operator=(const ECSMasterModeHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This ECSMasterMode was initially loaded from the database
         *         or writeECSMasterModeData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECSMasterMode record cannot be found for the helper 
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
         * modifyExistingECSMasterMode
         *
         * This will update an existing ECSMasterMode in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the ECSMasterMode object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ECSZoneId references a non-existent EC_ZONE record. 
         *
         */
        void modifyExistingECSMasterMode() throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewECSMasterMode
         *
         * This will add a new ECSMasterMode in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the ECSMasterMode object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ECSZoneId references a non-existent EC_ZONE record. 
         *
         */
        void addNewECSMasterMode() throw(TA_Base_Core::TransactiveException);

        /**
         * updateStationModeAssociations
         *
         * This removes and adds StationMode from/to the database to get it up-to-date with the user's changes.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.
         */
        void updateStationModeAssociations();

        void validateMasterModeUniqueness() throw(TA_Base_Core::TransactiveException);
        void validateZoneIdExistence() throw(TA_Base_Core::TransactiveException);
        void validateStationModeKeyExistence(unsigned long stationModeKey) throw(TA_Base_Core::TransactiveException);


        unsigned long m_key;
        unsigned long m_masterMode;
        std::string   m_condition;
        int m_ECSZoneId;
        //bool          m_ECSZoneIdIsNull;
        std::string   m_evacDir;
        std::string   m_airDir;
		std::string   m_situation;
        std::string   m_dispDir;
		std::string m_verificationMsg;
        time_t        m_dateCreated;
        time_t        m_dateModified;
        bool          m_isValidData;
        bool          m_isNew;

        std::vector<unsigned long> m_associatedStationModes;


    };
} // closes TA_Core

#endif // !defined(ECSMasterModeHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
