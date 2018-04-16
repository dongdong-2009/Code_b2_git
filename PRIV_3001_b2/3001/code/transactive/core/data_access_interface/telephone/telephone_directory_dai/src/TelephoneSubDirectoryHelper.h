/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectoryHelper.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * TelephoneSubDirectoryHelper is an object that is held by TelephoneSubDirectory and ConfigTelephoneSubDirectory objects. 
  * It contains all data used by TE_SUBDIRECTORY, and manipulation
  * methods for the data. It helps avoid re-writing code for both TelephoneSubDirectory and ConfigTelephoneSubDirectory.
  */


#if !defined(TelephoneSubDirectoryHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define TelephoneSubDirectoryHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"

#include <string>
#include <vector>

namespace TA_Base_Core
{
    class IData;
}

namespace TA_IRS_Core
{
    class TelephoneSubDirectoryHelper
    {
    public:
        static const std::string SUB_DIR_SEQ;
        static const std::string SUB_DIR_TABLE;
        static const std::string SUB_DIR_PRI_KEY;
        static const std::string SUB_DIR_NAME;
        static const std::string SUB_DIR_STATUS;

        /** 
         * TelephoneSubDirectoryHelper
         *
         * This constructor creates a new TelephoneSubDirectoryHelper for configuration, initiailising
         * the subdirectory key to -1.  This needs to be specified by setPrimaryKey() before writing 
         * to the database for the first time.
         * m_isNew is set to true and m_isValidData is set to false 
         *
         */
        TelephoneSubDirectoryHelper();

         /** 
         * TelephoneSubDirectoryHelper
         *
         * This constructor creates a new TelephoneSubDirectoryHelper for the specified TESUBD_ID (primary key).
         * m_isNew and m_isValidData is set to false 
         *
         * @param  idKey the TESUBD_ID (primary key)
         *
         */
        TelephoneSubDirectoryHelper(const unsigned long idKey);
         
        
        /** 
         * TelephoneSubDirectoryHelper
         *
         * Copy constructor.
         * m_isNew is set to true and m_isValidData is set to false 
         *
         * @param   theTelephoneSubDirectoryHelper the original TelephoneSubDirectoryHelper to copy.
         *
         */
        TelephoneSubDirectoryHelper( const TelephoneSubDirectoryHelper& theTelephoneSubDirectoryHelper);

        /** 
         * TelephoneSubDirectoryHelper
         *
         * This constructor creates a new object using the input data
         * which is representative of a row returned from SQL statement
         * m_isNew and m_isValidData is set to false 
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
        TelephoneSubDirectoryHelper(unsigned long row, TA_Base_Core::IData& data);

       
        /** 
         * ~TelephoneSubDirectoryHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~TelephoneSubDirectoryHelper();

        /**
         * isNew
         *
         * This returns whether this is a new subdirectory.  A new subdirectory is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new subdirectory
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };

       
        
        /**
         * getPrimaryKey
         *
         * Returns the subdirectory Primary Key.  If this record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing subdirectory has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setPrimaryKey()
         *
         * @return the TE_SUBDIRECTORY primary key
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
        long getPrimaryKey();


        /**
         * getSubDirName
         *
         * Returns the name of this subdirectory.  If this subdirectory record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing subdirectory has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setSubDirName()
         *
         * @return the subdirectory name 
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
        std::string getSubDirName();


        /**
         * getStatus
         *
         * Returns the status of this subdirectory.  If this subdirectory record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing subdirectory has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStatus()
         *
         * @return the status 
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
         * Sets the primary key of this subdirectory. The set values are not written to the database
         * until writeTelephoneSubDirData() is called.  This method should only be used by the 
         * ConfigTelephoneSubDirectory class.
         *
         * pre:    This is a new subdirectory constructed by the default constructor.
         *
         * @param  key the subdirectory key (primary key) for this subdirectory
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
        void setPrimaryKey(long Key);


        /**
         * setSubDirName
         *
         * Sets the name of this subdirectory. The set values are not written to the database
         * until writeTelephoneSubDirData() is called.  This method should only be used by the 
         * ConfigTelephoneSubDirectory class.
         *
         *
         * @param  name the name of this subdirectory
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
        void setSubDirName(const std::string& name);


        /**
         * setStatus
         *
         * Sets the Status of this subdirectory. The set values are not written to the database
         * until writeTelephoneSubDirData() is called.  This method should only be used by the 
         * ConfigTelephoneSubDirectory class.
         *
         * @param  status the status for this subdirectory
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
        void setStatus(const int status);
       
        /**
         * invalidate
         *
         * Make the data contained by this subdirectory as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeTelephoneSubDirData() has been called prior to calling
         * this method on a new subdirectory, as it does not make any sense to invalidate an
         * subdirectory that has not yet been written to the database.
         *
         * pre:    This subdirectory was initially loaded from the database
         *         or writeTelephoneSubDirData() has already been called
         */
        void invalidate();


        /**
         * writeTelephoneSubDirData
         * 
         * Write this Telephone Directory Entry to the database.  Note that record uniqueness is enforced
         * for the primary key TESUBD_ID and the uniqueness of the name of the sub directory, NAME. 
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. TESUBD_ID should be an unsigned long)
         *          - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *            constructed with an TelephoneDirectory.
         *          - NOT_UNIQUE if the TelephoneDirectory returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the TelephoneDirectory object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         */
        void writeTelephoneSubDirData();
            //throw(TA_Base_Core::TransactiveException);


        /**
         * deleteTelephoneSubDir
         *
         * Remove this subdirectory from the database. Once this method has been executed, the
         * TelephoneSubDirectoryHelper object should be destroyed.
         *
         * pre:    This subdirectory was initially loaded from the database
         *         or writeTelephoneSubDirData() has already been called
         *
         * @param  cascade set to true to force the removal of this record by removing all 
         *         references to this record in other tables (TE_DIRECTORY).  This should 
         *         be set to true to prevent the DatabaseException from being thrown. 
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException 
         *         - CANNOT_DELETE if references to this record exists in 
         *           TE_DIRECTORY table. 
         *
         */
        void deleteTelephoneSubDir(bool cascade = false);
		

    private:
        // Made private as it is not used
		TelephoneSubDirectoryHelper& operator=(const TelephoneSubDirectoryHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This sub directory was initially loaded from the database
         *         or writeTelephoneSubDirData() has already been called
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
         * Fills out the local members with data contained in the input data object.
         * It will attempt to load the following information: TESUBD_ID, NAME, STATUS
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
         * modifyExistingTelephoneSubDir
         *
         * This will update an existing subdirectory in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - NOT_UNIQUE when there is already an existing record with the same Primary Key
         *            or when name of the directory is not unique
         *
         */
        void modifyExistingTelephoneSubDir();//throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewTelephoneSubDir
         *
         * This will add a new subdirectory in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - NOT_UNIQUE when there is already an existing record with the same Primary Key
         *            or when name of the directory is not unique
         *
         */
        void addNewTelephoneSubDir();//throw(TA_Base_Core::TransactiveException);

	    /**
	     * validateSubDirIdExistence
	     * 
	     * Checks that the primary key in m_key exist
	     * 
	     * @return void 
	     * 
         * @throws  DatabaseException 
         *          - if there is a problem retrieving data from the database.  
         * @throws  DataException 
         *          - if there is a problem retrieving data from the database.
         *          - NO_VALUE when there are no records with the same Primary Key
	     */
        void validateSubDirIdExistence();

	    /**
	     * validateSubDirIdUniqueness
	     * 
	     * Checks that the primary key in m_key is unique
	     * 
	     * @return void 
	     * 
         * @throws  DatabaseException 
         *          - if there is a problem retrieving data from the database.  
         * @throws  DataException 
         *          - if there is a problem retrieving data from the database.
         *          - NOT_UNIQUE when there is already an existing record with the same Primary Key
	     */
        void validateSubDirIdUniqueness();//throw(TA_Base_Core::TransactiveException);


	    /**
	     * validateSubDirNameUniqueness
	     * 
	     * Checks that the name of the directory in m_subDirName is unique
	     * 
	     * @return void 
	     * 
         * @throws  DatabaseException 
         *          - if there is a problem writing retrieving data from the database.  
         * @throws  DataException 
         *          - if there is a problem writing retrieving data from the database.
         *          - NOT_UNIQUE when name of the directory is not unique
         *
    	 */
        void validateSubDirNameUniqueness();

	    /**
	     * long getNextValidKey
	     * 
	     * Get the next valid primary key for the TE_SUBDIRECTORY
         * using TESUBD_SEQ (and the sequence number will be updated in database)
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
        static unsigned long getNextValidKey();

        /**
	      * Stores the primary key of the record
          */
        long m_key;  

        /**
	      * Stores the sub directory name of the record
          */
        std::string m_subDirName; 

        /**
	      * Stores status of the record
          */
        int m_status;

        /**
	      * Indicates if a data is valid.
          */
        bool m_isValidData;

        /**
	      * True will indicate that the record stored is a new record
          * False will indicated that record is an existing record
          */
        bool m_isNew;
    };
} // closes TA_Core

#endif // !defined(TelephoneSubDirectoryHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
