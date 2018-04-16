/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryHelper.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  *
  * TelephoneDirectoryHelper is an object that is held by TelephoneDirectory and ConfigTelephoneDirectory objects. 
  * It contains all data used by TelephoneDirectory and ConfigTelephoneDirectory, and manipulation
  * methods for the data. It helps avoid re-writing code for both TelephoneDirectory and ConfigTelephoneDirectory.
  */
#if !defined(TelephoneDirectoryHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define TelephoneDirectoryHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"

#include <string>
#include <vector>

namespace TA_Base_Core
{
    class IData;
}

namespace TA_IRS_Core
{
    class ITelephoneSubDirectory;

    class TelephoneDirectoryHelper
    {

    public:

        // Members will hold the name of the different columns in the TE_DIRECTORY table
        static const std::string DIR_SEQ;
        static const std::string DIR_TABLE;
        static const std::string DIR_PRI_KEY;
        static const std::string DIR_SUB_DIR_PRI_KEY;
        static const std::string DIR_TEL_NUM;
        static const std::string DIR_FULL_NAME;
        static const std::string DIR_LOCATION;
        static const std::string DIR_STATUS;

        /** 
         * TelephoneDirectoryHelper
         *
         * This constructor creates a new TelephoneDirectoryHelper for configuration, initiailising
         * the TelephoneDirectory to a new record.  
         *
         */
        TelephoneDirectoryHelper();

        /** 
         * TelephoneDirectoryHelper
         *
         * This constructor creates a new TelephoneDirectoryHelper for the specified TelephoneDirectory (primary key).
         * The Helper class will initialize the record to an existing record
         *
         * @param  const unsigned long idKey 
         *         the primary key for the TE_DIRECTORY table
         *
         */
        TelephoneDirectoryHelper(const unsigned long idKey);
         
        
        /** 
         * TelephoneDirectoryHelper
         *
         * Copy constructor.
         * The Helper class will treat the record as a new record. The primary key of the 
         * to-be-copied object will not be copied into this new object
         *
         * @param   const TelephoneDirectoryHelper& theTelephoneDirectoryHelper
         *          the original TelephoneDirectoryHelper to copy.
         *
         */
        TelephoneDirectoryHelper( const TelephoneDirectoryHelper& theTelephoneDirectoryHelper);


        /** 
         * TelephoneDirectoryHelper
         *
         * This constructor creates a new object using the input data
         * which is representative of a row returned from SQL statement
         *
         * @param row 
         *        The row of data in the data object that we should collect data from
         *        in order to construct ourselves
         *
         * @param data 
         *        The IData interface that should have been obtained using the 
         *        getBasicQueryData function.  It is important for the data to contain the 
         *        following column inforamtion:
         *        TEDENT_ID, TESUBD_ID, TEL_NUMBER, FULLNAME, LOCATION, STATUS 
         *          
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *
         */
        TelephoneDirectoryHelper(unsigned long row, TA_Base_Core::IData& data);

        
        /** 
         * ~TelephoneDirectoryHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~TelephoneDirectoryHelper();

        /**
         * isNew
         *
         * This returns whether this is a new TelephoneDirectory record.  A new TelephoneDirectory is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new TelephoneDirectory
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /**
         * getPrimaryKey
         *
         * Returns the primary key for this Telephone Directory Entry.  If this record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing TelephoneDirectory has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setPrimaryKey()
         *
         * @return primary key of the Telephone Directory Entry Record 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         */
        unsigned long getPrimaryKey();


        /**
         * getSubDirKey
         *
         * Returns the key of the sub-directory that this Telephone Directory Entry record belongs  
         * to.  If the record does not exist in the database (constructed without an Id),
         * then data will be retrieved from within the class. Otherwise, if this is the first
         * time data for this existing Telephone Directory Entry has been requested, all the 
         * data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setSubDirKey()
         *
         * @return the key of the sub-directory that the record belongs to. 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         */
        unsigned long getSubDirKey();


        /**
         * getTelephoneNum
         *
         * Returns the telephone Number that for the Telephone Directory Entry record.   
         * If the record does not exist in the database (constructed without an Id),
         * then data will be retrieved from within the class. Otherwise, if this is the first
         * time data for this existing Telephone Directory Entry has been requested, all the 
         * data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setTelephoneNum()
         *
         * @return the Telephone Numebr 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         */
        std::string getTelephoneNum();
         
        /**
         * getFullName
         *
         * Returns the full name for this Telephone Directory Entry record.
         * If the record does not exist in the database (constructed without an Id),
         * then data will be retrieved from within the class. Otherwise, if this is the first
         * time data for this existing Telephone Directory Entry has been requested, all the 
         * data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setTelephoneNum()
         *
         * @return the Telephone Numebr 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         */
        std::string getFullName();
    
        
        /**
         * getLocation
         *
         * Returns the location for this Telephone Directory Entry record.
         * If the record does not exist in the database (constructed without an Id),
         * then data will be retrieved from within the class. Otherwise, if this is the first
         * time data for this existing Telephone Directory Entry has been requested, all the 
         * data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setLocation()
         *
         * @return the Location 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         */
        std::string getLocation();
    
        
        /**
         * getStatus
         *
         * Returns the status for this Telephone Directory Entry record.
         * If the record does not exist in the database (constructed without an Id),
         * then data will be retrieved from within the class. Otherwise, if this is the first
         * time data for this existing Telephone Directory Entry has been requested, all the 
         * data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStatus()
         *
         * @return the Location 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         */
        int getStatus();

       
        /**
         * setPrimaryKey
         *
         * Sets the Primary key of this Telephone Directory Entry. The set values are not written to the database
         * until writeTelephoneDirectoryData() is called.  This method should only be used by the 
         * ConfigTelephoneDirectory class.
         *
         * pre:    This is a new Telephone Directory Entry constructed by the default constructor.
         *
         * @param  primary key of the TelephoneDirectory for this Telephone Directory Enty
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. m_key should be an unsigned long)
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         *
         */
        void setPrimaryKey(unsigned long idKey);


        /**
         * setSubDirKey
         *
         * Sets the Sub-directory key of this Telephone Directory Entry. The set values are not written to 
         * the database until writeTelephoneDirectoryData() is called.  This method should only be used  
         * by the ConfigTelephoneDirectory class.
         *
         * @param  primary key of the SubDirectory for this Telephone Directory Enty
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. m_key should be an unsigned long)
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         *
         */
        void setSubDirKey(unsigned long entityKey);


        /**
         * setTelephoneNum
         *
         * Sets the telephone number of this Telephone Directory Entry. The set values are not written to 
         * the database until writeTelephoneDirectoryData() is called.  This method should only be used  
         * by the ConfigTelephoneDirectory class.
         *
         * @param  telephone Number for this Telephone Directory Enty
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. m_key should be an unsigned long)
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         *
         */
        void setTelephoneNum(const std::string& number);

        /**
         * setFullName
         *
         * Sets the name of this Telephone Directory Entry. The set values are not written to 
         * the database until writeTelephoneDirectoryData() is called.  This method should only   
         * be used by the ConfigTelephoneDirectory class.
         *
         * @param  full name for this Telephone Directory Enty
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. m_key should be an unsigned long)
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         *
         */
        void setFullName(const std::string& name);
        
        /**
         * setLocation
         *
         * Sets the location of this Telephone Directory Entry. The set values are not written to 
         * the database until writeTelephoneDirectoryData() is called.  This method should only   
         * be used by the ConfigTelephoneDirectory class.
         *
         * @param  location for this Telephone Directory Enty
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. m_key should be an unsigned long)
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         *
         */
        void setLocation(const std::string& location);

      
        /**
         * setStatus
         *
         * Sets the status of this Telephone Directory Entry. The set values are not written to 
         * the database until writeTelephoneDirectoryData() is called.  This method should only   
         * be used by the ConfigTelephoneDirectory class. There are altogether 3 states, for more 
         * information, please refer to TelephoneDirectoryItemStatus class
         *
         * @param  status for this Telephone Directory Enty
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. m_key should be an unsigned long)
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
         *
         */
        void setStatus(int status);
       
        /**
         * invalidate
         *
         * Make the data contained by this Telephone Directory Entry as invalid.  
         * The next call to get...() following a call to invalidate() will cause all the data to 
         * be reloaded from the database.  A possible scenario for this method to be used is when 
         * it is known that the database has been modified outside the process.
         *
         * It is a precondition that writeTelephoneDirectoryData() has been called prior to calling
         * this method on a new ECSZone, as it does not make any sense to invalidate an
         * Telephone Directory Entry that has not yet been written to the database.
         *
         * pre:    This ECSZone was initially loaded from the database
         *         or writeECSZoneData() has already been called
         */
        void invalidate();


        /**
         * writeTelephoneDirectoryData
         * 
         * Write this Telephone Directory Entry to the database.  Note that record uniqueness is enforced
         * for the primary key TEDENT_ID and the existance of the specified sub Directory. It will ensure 
         * that the following information are set before it will proceed to write to the database:
         *          - Primary key of the telephone directory entry ("TEDENT_ID")
         *          - Primary key of the sub-directory ("TESUBD_ID")
         *          - Status field ("STATUS")
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. TEDENT_ID should be an unsigned long)
         *          - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *            constructed with an TelephoneDirectory.
         *          - NOT_UNIQUE if the TelephoneDirectory returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the TelephoneDirectory object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeTelephoneDirectoryData();

        /**
         * deleteTelephoneDirectoryData
         *
         * Remove this ECSZone from the database. Once this method has been executed, the
         * TelephoneDirectoryHelper object should be destroyed.  Assertion will be raised
         * if Helper is attempting to delete a new data ( that does not exist in database)
         *
         * pre:    This Telephone Directory was initially loaded from the database
         *         or writeTelephoneDirectoryData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException 
         *         - CANNOT_DELETE if there are errors deleting the record
         *
         */
        void deleteTelephoneDirectoryData();


        /**
         * getAssociatedSubdirectory
         *
         * Call TelephoneSubDirectoryAccessFactory to retrieve specified subdirectory using the m_subDirKey 
         * in this object.  
         *
         * pre:    This Telephone Directory was initially loaded from the database
         *         or writeTelephoneDirectoryData() has already been called
         *
         * @return the ITelephoneSubDirectory
         *         if the object cannot be found, NULL will be return instead.
         *         otherwise TelephoneSubDirectory will be return
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         */
        ITelephoneSubDirectory* getAssociatedSubdirectory(); 		

    private:
        // Made private as it is not used
		TelephoneDirectoryHelper& operator=(const TelephoneDirectoryHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This telephone directory entry was initially loaded from the database
         *         or writeTelephoneDirectoryData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *           constructed with an TelephoneDirectory.
         *         - NOT_UNIQUE if the TelephoneDirectory returns multiple records 
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
         * modifyExistingTelephoneDirectoryData
         *
         * This will update an existing telephone directory entry in the database with the internals currently 
         * set in this instance. (i.e. if m_isNew is not set to true)
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. TEDENT_ID should be an unsigned long)
         *          - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *            constructed with an TelephoneDirectory.
         *          - NOT_UNIQUE if the TelephoneDirectory returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         */
        void modifyExistingTelephoneDirectoryData();//throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewTelephoneDirectoryData
         *
         * This will add a new telephone directory entry in the database with the internals currently set in this 
         * instance. (i.e. if m_isNew is set to true)
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. TEDENT_ID should be an unsigned long)
         *          - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *            constructed with an TelephoneDirectory.
         *          - NOT_UNIQUE if the TelephoneDirectory returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewTelephoneDirectoryData();//throw(TA_Base_Core::TransactiveException);


	    /**
	     * validateTelephoneDirectoryDataUniqueness
	     * 
	     * Check if the primary key specified for in the helper is unique
	     * 
	     * @return void 
	     * 
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *          - NOT_UNIQUE if the TelephoneDirectory returns multiple records
	     */
        void validateTelephoneDirectoryDataUniqueness();//throw(TA_Base_Core::TransactiveException);



	    /**
	     * validateTelephoneSubDirectoryExistence
	     * 
	     * Check if the subdirectory as specified in the helper-m_subDirKey exist 
	     * 
	     * @return void 
	     * 
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *          - NO_VALUE if the SubDirectory record cannot be found for key as specified in m_subDirKey 
	     */
        void validateTelephoneSubDirectoryExistence(unsigned long entityKey);//throw(TA_Base_Core::TransactiveException);

	    /**
	     * validateTelephoneNumberUniqueness
	     * 
	     * Check if there is an existing record of the same state with the telephone number as specified in m_teleNum 
	     * 
	     * @return void 
	     * 
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *          - NOT_UNIQUE if there is an existing record of the same state as m_key, and telephone number = m_teleNum 
	     */
        void validateTelephoneNumberUniqueness();

	    /**
	     * long getNextValidKey
	     * 
	     * Get the next valid primary key for the TE_DIRECTORY_ENTRY
         * using TEDENT_SEQ (and the sequence number will be updated in database)
	     * 
	     * @return a new valid Primary key
	     * 
         * @exception DataException 
         *            if the next auto increment ID value isn't available (error
         *            occurred fetching the value
         *
         * @exception DatabaseException 
         *            If there is an error with the database connection
         *
	     */
        unsigned long getNextValidKey();


        /**
	      * Stores the primary key of the record
          */
        unsigned long m_key;

        /**
	      * Stores the sub directory key value of the record
          */
        unsigned long m_subDirKey;

        /**
	      * Stores the telephone number of the record
          */
        std::string m_teleNum;

        /**
	      * Stores the full name of the record
          */
        std::string m_fullName;

        /**
	      * Stores the location of the record
          */
        std::string m_location;

        /**
	      * Stores the status of the record
          */
        int m_status;

        /**
	      * Indicates the validity of the data.
          */
        bool m_isValidData;

        /**
	      * Indicates whether this record is a brand new/existing record
          * That is is the m_Primary key new/exisiting
          */
        bool m_isNew;

        /**
	      * Indicates whether the key has been set before
          */
        bool m_isKeySet;

        /**
	      * Indicates whether the subdirectory key has been set before
          */
        bool m_isSubDirKeySet;

        /**
	      * Indicates whether the status has been set before
          */
        bool m_isStatusSet;

        /**
	      * Stores the sub-directory name
          */
        std::string m_subDirectoryName;

    };
} // closes TA_Core

#endif // !defined(TelephoneDirectoryHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
