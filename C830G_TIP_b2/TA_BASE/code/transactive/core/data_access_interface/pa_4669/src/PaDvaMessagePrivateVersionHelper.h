/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersionHelper.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * PaDvaMessagePrivateVersionHelper is an object that is held by PaDvaMessagePrivateVersion and ConfigPaDvaMessagePrivateVersion objects. 
  * It contains all data used by PaDvaMessagePrivateVersions, and manipulation
  * methods for the data. It helps avoid re-writing code for both PaDvaMessagePrivateVersion and ConfigPaDvaMessagePrivateVersion.
  *
  */

#if !defined PaDvaMessagePrivateVersionHelper_H
#define PaDvaMessagePrivateVersionHelper_H

#include <string>
#include <vector>
#include <climits>
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/pa_4669/src/IPaDvaMessagePrivateVersion.h"
#include "core/data_access_interface/pa_4669/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/pa_4669/src/GenericDaiHelper.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_Base_Core
{

    class PaDvaMessagePrivateVersionHelper
        : public GenericDaiHelper<PaDvaMessagePrivateVersionHelper>
    {

    public:

        // Basic definition of what kind of set a message belongs to
		/*
        enum EBasicSet
        {
            PRIVATE,
            PUBLIC
        };
		*/

        /** 
         * PaDvaMessagePrivateVersionHelper
         *
         * This constructor creates a new PaDvaMessagePrivateVersionHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        PaDvaMessagePrivateVersionHelper(const unsigned long key);

        
        /** 
         * PaDvaMessagePrivateVersionHelper
         *
         * Copy constructor.
         *
         * @param   thePaDvaMessagePrivateVersionHelper the original PaDvaMessagePrivateVersionHelper to copy.
         *       
         */
        PaDvaMessagePrivateVersionHelper( const PaDvaMessagePrivateVersionHelper& thePaDvaMessagePrivateVersionHelper);

        /** 
         * PaDvaMessagePrivateVersionHelper
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
        PaDvaMessagePrivateVersionHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * PaDvaMessagePrivateVersionHelper
         *
         * This constructor creates a new PaDvaMessagePrivateVersionHelper for configuration, initialising
         * the primary key to 0.  This primary key will be automatically set to a valid value 
         * when writePaDvaMessagePrivateVersionData() is called for the first time.
         *
         */
        PaDvaMessagePrivateVersionHelper();


        /** 
         * ~PaDvaMessagePrivateVersionHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaDvaMessagePrivateVersionHelper();


        /**
         * isNew
         *
         * This returns whether this is a new PaDvaMessagePrivateVersion.  A new PaDvaMessagePrivateVersion is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new PaDvaMessagePrivateVersion
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getKey
         *
         * Returns the primary key of this PaDvaMessagePrivateVersion.
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
         *           (e.g. the key should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessagePrivateVersion record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();


        /**
         * getLocationKey
         *
         * Returns the Location Key of this PA DVA Message Version.  If this 
         * PA DVA Message Version record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PA DVA Message Version has been requested, all the data is loaded from the database.
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
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PA DVA Message Version record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getLocationKey();

        /**
         * getVersionData
         *
         * Returns relevant version info for this PA Dva Message Version set.  
         * If this PA DVA Message record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PA DVA Message Version has been requested, all the data is loaded from the database.
         *
         * pre:    The data exists in the database or has been set by a call to the equivalent 
         *         set... method.
         *
         * @param column the specific column we want the version to be taken from
         *
         * @return the version information corresponding to the specified column
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PA DVA Message Version record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual std::string getVersionData(IPaDvaMessagePrivateVersion::EVersionColumn column);


        /**
         * getMessageVersion
         *
         * Determines what version is associated with the input message Id
         *      
         * @param location the location the query originated from
         *
         * @param stationDvaMessageId the id of the station DVA Message
         *
         * @return a string representation of the version
         *
         */
        std::string getMessageVersion(IPaDvaMessagePrivateVersion::ELocation location,
                                                unsigned long stationDvaMessageId);


        /**
         * setLocationKey
         *
         * Sets the Location Key of this PA DVA Message Version. The set values are not written to the database
         * until writePaDvaMessagePrivateVersionGroupData() is called.  This method should only be used by the 
         * ConfigPaDvaMessagePrivateVersionGroup class.  For data to be correctly written to the database, the referenced 
         * PKEY in the LOCATION table must exist.
         *
         * @param  locationKey the new Location Key for this PA DVA Message Version
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PA DVA Message Version record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setLocationKey(const unsigned long locationKey);

        /**
         * setVersionData
         *
         * Sets new version data for this PA DVA Message Version set. The values are not written to the database
         * until writePaDvaMessagePrivateVersionGroupData() is called.  This method should only be used by the 
         * ConfigPaDvaMessagePrivateVersionGroup class.  For data to be correctly written to the database, the referenced 
         * PKEY in the LOCATION table must exist.
         *
         * @param newVersion the new Location Key for this PA DVA Message Version
         *
         * @param column the specific version column we want to update
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Location Key should be an unsigned long)
         *         - NO_VALUE if the PA DVA Message Version record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setVersionData(const std::string& newVersion,
                                        IPaDvaMessagePrivateVersion::EVersionColumn column);

        
        /**
         * getDateCreated
         *
         * Returns the date created for this PaDvaMessagePrivateVersion.  If this PaDvaMessagePrivateVersion record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessagePrivateVersion has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PaDvaMessagePrivateVersion as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessagePrivateVersion record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PaDvaMessagePrivateVersion.  If this PaDvaMessagePrivateVersion record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PaDvaMessagePrivateVersion has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this PaDvaMessagePrivateVersion as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the Location Key should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessagePrivateVersion record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();


        /**
         * invalidate
         *
         * Make the data contained by this PaDvaMessagePrivateVersion as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writePaDvaMessagePrivateVersionData() has been called prior to calling
         * this method on a new PaDvaMessagePrivateVersion, as it does not make any sense to invalidate an
         * PaDvaMessagePrivateVersion that has not yet been written to the database.
         *
         * pre:    This PaDvaMessagePrivateVersion was initially loaded from the database
         *         or writePaDvaMessagePrivateVersionData() has already been called
         */
        void invalidate();


        /**
         * writePaDvaMessagePrivateVersionData
         * 
         * Write this PaDvaMessagePrivateVersion to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the Location Key should be an unsigned long)
         *          - For new messages, the primary key hasn't been set or is non unique
         *          - NO_VALUE if the PaDvaMessagePrivateVersion record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the PaDvaMessagePrivateVersion returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the PaDvaMessagePrivateVersion object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writePaDvaMessagePrivateVersionData(); //throw(TA_Core::TransactiveException);


        /**
         * deletePaDvaMessagePrivateVersion
         *
         * Remove this PaDvaMessagePrivateVersion from the database. Once this method has been executed, the
         * PaDvaMessagePrivateVersionHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This PaDvaMessagePrivateVersion was initially loaded from the database
         *         or writePaDvaMessagePrivateVersionData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException
         *
         */
        void deletePaDvaMessagePrivateVersion();

        /**
         * getBasicQueryData
         *
         * Required static implementation for template base class
         *
         * @see GenericDaiHelper::getBasicQueryData
         *
         */
        static const DatabaseQueryHelper::QueryData getBasicQueryData();

    protected:

        /**
         * reloadUsing (interface implementation)
         *
         * @see GenericDaiHelper::reloadUsing for more details
         *
         */
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);
	
    private:

        // Made private as it is not used
		PaDvaMessagePrivateVersionHelper& operator=(const PaDvaMessagePrivateVersionHelper &);
        
        /**
         * modifyExistingPaDvaMessagePrivateVersion
         *
         * This will update an existing PaDvaMessagePrivateVersion in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaDvaMessagePrivateVersion object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this message have not been set
         */
        void modifyExistingPaDvaMessagePrivateVersion(); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewPaDvaMessagePrivateVersion
         *
         * This will add a new PaDvaMessagePrivateVersion in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PaDvaMessagePrivateVersion object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this message have not been set
         *
         */
        void addNewPaDvaMessagePrivateVersion(); //throw(TA_Core::TransactiveException);
    
        /**
         * getMessageVersionUsingParams
         *         
         * Determines what version is associated with input parameters
         *  for example use getMessageVersionUsingParams(1, 1, 500, PRIVATE)
         *  to get the PRIVATE_ADHOC1 message from Set A [1-500]
         *
         * @pre startRange <= stationDvaMessageId <= endRange
         *
         * @param stationDvaMessageId the id of the station DVA Message
         *
         * @param startRange the starting range of the message set (inclusive)
         *
         * @param endRange the ending range of message set (inclusive)
         *
         * @param set the basic set type (PRIVATE or PUBLIC)
         *
         * @return a string representation of the version
         *
         */
        std::string getMessageVersionUsingParams(unsigned long stationDvaMessageId,
                                                 unsigned long startRange,
                                                 unsigned long endRange);             

        /**
         * getKeyDirect (base class pure virtual implementation)
         *
         * @see GenericDaiHelper::getKeyDirect
         *
         */
        virtual unsigned long getKeyDirect() const { return m_key.getValue(); }
        
        // The actual key (as returned from database - or set by user)        
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_key;     

        TA_Base_Core::PrimitiveWrapper<unsigned long> m_locationKey;

        // Array of strings (one for each version column)
        std::string m_versionData[IPaDvaMessagePrivateVersion::VERSION_COLUMN_MAX];
        
        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew;

        /**
         * getVersionDataColumns
         *
         * @return all the Version column names in a comma separated
         *   list (without a trailing comma) - eg COL1, COL2, COL3
         *
         */
        std::string getVersionDataColumns();
        std::string getLocalDatabaseName();

    };
} // closes TA_Core

#endif // !defined(PaDvaMessagePrivateVersionHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
