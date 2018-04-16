/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ecs/src/ECSZoneHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by:  $Author: weikun.lin $
  *
  * ECSZoneHelper is an object that is held by ECSZone and ConfigECSZone objects. 
  * It contains all data used by ECSZones, and manipulation
  * methods for the data. It helps avoid re-writing code for both ECSZone and ConfigECSZone.
  */

#if !defined(ECSZoneHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define ECSZoneHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

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
    class ECSZoneHelper
    {

    public:


        /** 
         * ECSZoneHelper
         *
         * This constructor creates a new ECSZoneHelper for the specified ECS Zone Id (primary key).
         *
         * @param  idKey the ECS Zone Id (primary key)
         *
         */
        ECSZoneHelper(const unsigned long idKey);
         
        
        /** 
         * ECSZoneHelper
         *
         * Copy constructor.
         *
         * @param   theECSZoneHelper the original ECSZoneHelper to copy.
         *
         */
        ECSZoneHelper( const ECSZoneHelper& theECSZoneHelper);

        /** 
         * ECSZoneHelper
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
        ECSZoneHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * ECSZoneHelper
         *
         * This constructor creates a new ECSZoneHelper for configuration, initiailising
         * the ECS Zone Id to 0.  This needs to be specified by setECSZoneId() before writing 
         * to the database for the first time.
         *
         */
        ECSZoneHelper();

        
        /** 
         * ~ECSZoneHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~ECSZoneHelper();

        /**
         * isNew
         *
         * This returns whether this is a new ECSZone.  A new ECSZone is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new ECSZone
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };

       
        
        /**
         * getECSZoneId
         *
         * Returns the ECS Zone Id of this ECS Zone.  If this ECS Zone record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECS Zone has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setECSZoneId()
         *
         * @return the ECS Zone Id 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         */
        int getECSZoneId();


        /**
         * getECSLocation
         *
         * Returns the ECS Location of this ECS Zone.  If this ECS Zone record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECS Zone has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setECSLocation()
         *
         * @return the ECS Location 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         */
        std::string getECSLocation();


        /**
         * getTrack
         *
         * Returns the Track of this ECS Zone.  If this ECS Zone record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECS Zone has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setTrack()
         *
         * @return the Track 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         */
        std::string getTrack();
       
        
        /**
         * getOccEquipmentEntityKey
         *
         * Returns the OccEquipment of this ECS Zone.  If this ECS Zone record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECS Zone has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setOccEquipmentEntityKey()
         *
         * @return the OccEquipmentEntityKey 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         */
        unsigned long getOccEquipmentEntityKey();
    
        
        /**
         * getStnEquipmentEntityKey
         *
         * Returns the StnEquipmentEntityKey of this ECS Zone.  If this ECS Zone record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECS Zone has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStnEquipmentEntityKey()
         *
         * @return the StnEquipmentEntityKey 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         */
        unsigned long getStnEquipmentEntityKey();
    
        
        /**
         * getIsIgnoringCongestion
         *
         * Returns the IsIgnoringCongestion flag for this ECS Zone.  If this ECS Zone record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECS Zone has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStnEquipmentEntityKey()
         *
         * @return the IsIgnoringCongestion flag
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         */
        bool getIsIgnoringCongestion();

       
        /**
         * getDateCreated
         *
         * Returns the date created for this ECS Zone.  If this ECS Zone record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECS Zone has been requested, all the data is loaded from the database.
         *
         * @return the date created for this ECSZone as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this ECS Zone.  If this ECS Zone record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing ECS Zone has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this ECSZone as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         */
        time_t getDateModified();


        /**
         * setECSZoneId
         *
         * Sets the ECSZoneId of this ECSZone. The set values are not written to the database
         * until writeECSZoneData() is called.  This method should only be used by the 
         * ConfigECSZone class.
         *
         * pre:    This is a new ECSZone constructed by the default constructor.
         *
         * @param  idKey the ECS Zone Id (primary key) for this ECSZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         *
         */
        void setECSZoneId(int idKey);


        /**
         * setECSLocation
         *
         * Sets the ECSLocation of this ECSZone. The set values are not written to the database
         * until writeECSZoneData() is called.  This method should only be used by the 
         * ConfigECSZone class.
         *
         *
         * @param  location the ECSLocation description for this ECSZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         *
         */
        void setECSLocation(const std::string& location);


        /**
         * setTrack
         *
         * Sets the Track of this ECSZone. The set values are not written to the database
         * until writeECSZoneData() is called.  This method should only be used by the 
         * ConfigECSZone class.
         *
         * @param  track the Track description for this ECSZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         *
         */
        void setTrack(const std::string& track);

        /**
         * setOccEquipmentEntityKey
         *
         * Sets the OccEquipmentEntityKey of this ECSZone. The set values are not written to the database
         * until writeECSZoneData() is called.  This method should only be used by the 
         * ConfigECSZone class.
         *
         * @param  entityKey the OccEquipmentEntityKey for this ECSZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         *
         */
        void setOccEquipmentEntityKey(unsigned long entityKey);

        
        /**
         * setStnEquipmentEntityKey
         *
         * Sets the StnEquipmentEntityKey of this ECSZone. The set values are not written to the database
         * until writeECSZoneData() is called.  This method should only be used by the 
         * ConfigECSZone class.
         *
         * @param  entityKey the StnEquipmentEntityKey for this ECSZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         *
         */
        void setStnEquipmentEntityKey(unsigned long entityKey);

      
        /**
         * setIsIgnoringCongestion
         *
         * Sets the IsIgnoringCongestion flag of this ECSZone. The set values are not written to the database
         * until writeECSZoneData() is called.  This method should only be used by the 
         * ConfigECSZone class.
         *
         * @param  isIgnoringCongestion the flag to ignore Congestions for this ECSZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
         *
         */
        void setIsIgnoringCongestion(bool isIgnoringCongestion);
       
        /**
         * invalidate
         *
         * Make the data contained by this ECSZone as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeECSZoneData() has been called prior to calling
         * this method on a new ECSZone, as it does not make any sense to invalidate an
         * ECSZone that has not yet been written to the database.
         *
         * pre:    This ECSZone was initially loaded from the database
         *         or writeECSZoneData() has already been called
         */
        void invalidate();


        /**
         * writeECSZoneData
         * 
         * Write this ECSZone to the database.  Note that record uniqueness is enforced
         * for the primary key ECZONE_ID and also for the case-insensitive combination of the 
         * TRACK and ECS_LOCATION fields.  i.e. every ECS Zone must have a unique TRACK / 
         * ECS_LOCATION field pair.  
         *
         * e.g. Records 2,3 & 4 will clash:
         *
         * 1. ECZONE_ID    = 1
         *    TRACK        = IB
         *    ECS_LOCATION = DBG to MSM
         * 2. ECZONE_ID    = 1                  <= clash with record 1
         *    TRACK        = OB
         *    ECS_LOCATION = DBG to MSM
         * 3. ECZONE_ID    = 2                  
         *    TRACK        = ib                 <= clash with record 1 (case insensitive)
         *    ECS_LOCATION = DBG to MSM
         * 4. ECZONE_ID    = 3                  
         *    TRACK        = IB                 
         *    ECS_LOCATION = DBG TO MSM         <= clash with record 1 (case insensitive)
         * 5. ECZONE_ID    = 4                  
         *    TRACK        = OB                 
         *    ECS_LOCATION = DBG to MSM
         * 6. ECZONE_ID    = 5                  
         *    TRACK        = OB                 
         *    ECS_LOCATION = DBG to CVC
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. ECSZoneId should be an unsigned long)
         *          - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *            constructed with an ECS Zone Id.
         *          - NOT_UNIQUE if the ECS Zone Id returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the ECSZone object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeECSZoneData();
            //throw(TA_Base_Core::TransactiveException);


        /**
         * deleteECSZone
         *
         * Remove this ECSZone from the database. Once this method has been executed, the
         * ECSZoneHelper object should be destroyed.
         *
         * pre:    This ECSZone was initially loaded from the database
         *         or writeECSZoneData() has already been called
         *
         * @param  cascade set to true to force the removal of this record by removing all 
         *         references to this record in other tables (FIXED_BLOCK_ECS_ZONE_MAP, 
         *         ECS_MASTER_MODE and indirectly in ECS_MASTER_MODE_HELPER).  This should 
         *         be set to true to prevent the DatabaseException from being thrown.
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException 
         *         - CANNOT_DELETE if references to this record exists in 
         *           FIXED_BLOCK_ECS_ZONE_MAP and ECS_MASTER_MODE tables.  These can be 
         *           removed by specifying the cascade option.
         *
         */
        void deleteECSZone(bool cascade = false);
		

    private:
        // Made private as it is not used
		ECSZoneHelper& operator=(const ECSZoneHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This ECSZone was initially loaded from the database
         *         or writeECSZoneData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. ECSZoneId should be an unsigned long)
         *         - NO_VALUE if the ECS Zone Id record cannot be found for the helper 
         *           constructed with an ECS Zone Id.
         *         - NOT_UNIQUE if the ECS Zone Id returns multiple records 
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
         * modifyExistingECSZone
         *
         * This will update an existing ECSZone in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the ECSZone object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingECSZone();//throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewECSZone
         *
         * This will add a new ECSZone in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the ECSZone object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewECSZone();//throw(TA_Base_Core::TransactiveException);

        void validateZoneIdUniqueness();//throw(TA_Base_Core::TransactiveException);
        void validateLocationUniqueness();//throw(TA_Base_Core::TransactiveException);
        void validateEquipmentExistence(unsigned long entityKey);//throw(TA_Base_Core::TransactiveException);
        void validateEquipmentUniqueness(const std::string& equipmentColumnName, unsigned long entityKey);//throw(TA_Base_Core::TransactiveException);       

        int m_idKey;
        int m_modifiedIdKey;
        std::string m_ecsLocation;
        std::string m_track;
        unsigned long m_occEquipmentEntityKey;
        unsigned long m_stnEquipmentEntityKey;
		bool m_isIgnoringCongestion;					 
		bool m_isIgnoringCongestionHasBeenSet;		 // Need special flag as isIgnoringCongestion can be false
		bool m_occEquipmentEntityKeyHasBeenSet;  // Need special flag as entityKey can be 0
        bool m_stnEquipmentEntityKeyHasBeenSet;  // Need special flag as entityKey can be 0
        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew;
        

    };
} // closes TA_Core

#endif // !defined(ECSZoneHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
