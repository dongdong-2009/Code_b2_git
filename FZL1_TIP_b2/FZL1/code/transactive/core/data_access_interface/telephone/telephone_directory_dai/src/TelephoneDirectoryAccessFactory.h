/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryAccessFactory.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2013/01/25 15:17:27 $
  * Last modified by:  $Author: huirong.luo $
  * 
  * TelephoneDirectoryAccessFactory is a singleton that is used to retrieve 
  * TelephoneDirectory objects either from the database or newly created. 
  * All TelephoneDirectory objects returned will adhear to the ITelephoneDirectory interface.
  */

#if !defined(TelephoneDirectoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define TelephoneDirectoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/ITelephoneDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/IConfigTelephoneDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryItemStatus.h"

namespace TA_IRS_Core
{

	class TelephoneDirectoryAccessFactory
	{
	private:
        TelephoneDirectoryAccessFactory();

        // Disable the following methods
		TelephoneDirectoryAccessFactory( const TelephoneDirectoryAccessFactory& theTelephoneDirectoryAccessFactory);
		TelephoneDirectoryAccessFactory& operator=(const TelephoneDirectoryAccessFactory &);

	public:
        /** 
         * ~TelephoneDirectoryAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~TelephoneDirectoryAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an TelephoneDirectoryAccessFactory object.
         */
		static TelephoneDirectoryAccessFactory& getInstance();

        /**
         * removeInstance
	     *
	     * Removes the instance of the class (if already created) and cleans up the members.  
         * Primarily used upon program termination (e.g. from main()) so that Purify does not 
         * consider this class and its members to be memory leaks.
         *
         */
        static void removeInstance();
        
        /**
         * getTelephoneDirectory - default we only retrieve active records 
         * new = 0
         * active = 1
         * inactive = 3
         *
         * Retrieves the specified TelephoneDirectory and returns the associated data as an 
         * object conforming to the ITelephoneDirectory interface.
         *
         * @param   idKey      the primary key/id of the TelephoneDirectory to retrieve
         * @param   readWrite  (optional) set to true to return writeable 
         *                     IConfigTelephoneDirectory objects or set to false to return 
         *                     readonly ITelephoneDirectory objects.
         *
         * @return  A pointer to an object conforming to the ITelephoneDirectory interface.
         *          If no record can be found with the idKey, a NULL object will be returned.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITelephoneDirectory object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - NOT_UNIQUE, if there is more than one TelephoneDirectory matching the supplied key. 
         *
         */
		ITelephoneDirectory* getTelephoneDirectory( const unsigned long idKey, 
                                                    const bool readWrite = false, 
                                                    const TelephoneDirectoryItemStatus stateOfRecords = TELDIR_STATUS_ACTIVE);

	    /**
	     * IsTelephoneDirectoryWithSubDir
	     * 
	     * Check if the subdirectory with the subDirKey exist in the TE_SUBDIRECTORY table
	     * 
	     * @return bool 
	     * @param : const unsigned subDirKey
	     * @param : const TelephoneDirectoryItemStatus stateOfRecords = TELDIR_STATUS_ACTIVE
	     * 
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
	     */
        bool isTelephoneDirectoryWithSubDir(const unsigned long subDirKey,
                                            const TelephoneDirectoryItemStatus stateOfRecords = TELDIR_STATUS_ACTIVE);

        /**
         * getAllTelephoneDirectories
         *
         * Retrieves all TelephoneDirectory Record with that has STATUS = stateOfRecords
         * and returns the associated data as a vector of objects conforming to the 
         * ITelephoneDirectory interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigTelephoneDirectory objects or set to false to return 
         *                     readonly ITelephoneDirectory objects.
         *
         * @return  a vector of pointers to objects conforming to the ITelephoneDirectory interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITelephoneDirectory object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         */
        ITelephoneDirectories getAllTelephoneDirectories(const bool readWrite = false, 
                                                         const TelephoneDirectoryItemStatus stateOfRecords = TELDIR_STATUS_ACTIVE);

        /**
         * getAllStatusTelephoneDirectories
         *
         * Retrieves all Telephone Directory Entry record (regardless of its status) 
         * and returns the associated data as a vector of objects conforming to the 
         * ITelephoneDirectory interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigTelephoneDirectory objects or set to false to return 
         *                     readonly ITelephoneDirectory objects.
         *
         * @return  a vector of pointers to objects conforming to the ITelephoneDirectory interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITelephoneDirectory object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         */
         ITelephoneDirectories getAllStatusTelephoneDirectories(const bool readWrite = false);

        /**
         * getTelephoneDirectoriesWhere
         *
         * Retrieves all TelephoneDirectorys matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select TEDENT_ID, TESUBD_ID, TEL_NUMBER, FULLNAME, LOCATION, STATUS 
         *      from TE_DIRECTORY_ENTRY 
         *      inner join TE_SUBDIRECTORY using(TESUBD_ID)
         *      where STATUS=<stateOfRecords>       and
         *      NAME like %<dirname>%               and
         *      TEL_NUMBER=<telephoneNum>           and 
         *      UPPER(FULLNAME) like %<fullName>%           
         *
         * If a particular clause is not required, specify with a null for strings.  
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigTelephoneDirectory objects or set to false to return 
         *                     readonly ITelephoneDirectory objects.
         *
         * @return A vector of pointers to objects conforming to the ITelephoneDirectory interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ITelephoneDirectory
         * objects
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        ITelephoneDirectories getTelephoneDirectoriesWhere(const bool readWrite = false,
                                    const std::string& telephoneNum = "",
                                    const std::string& fullName = "",
                                    const std::string& dirname = "",
                                    const TelephoneDirectoryItemStatus stateOfRecords = TELDIR_STATUS_ACTIVE);
       
        /**
         * createTelephoneDirectory
         * 
         * Creates an empty IConfigTelephoneDirectory object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigTelephoneDirectory object.
         *
         * @return A pointer to an IConfigTelephoneDirectory object
         *
         */
		IConfigTelephoneDirectory* createTelephoneDirectory();

        /**
         * copyTelephoneDirectory
         * 
         * Creates a new configuration TelephoneDirectory object using the TelephoneDirectory passed in
         *
         * @param  TelephoneDirectoryToCopy the ConfigTelephoneDirectory to copy
         *
         * @return the new object as a pointer to an IConfigTelephoneDirectory object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigTelephoneDirectory* copyTelephoneDirectory(const IConfigTelephoneDirectory* TelephoneDirectoryToCopy);

		//libo++
		ITelephoneDirectories getTelephoneDirectoriesScope(const bool readWrite = false,
                            const std::string& telephoneNumScope = "",
                            const TelephoneDirectoryItemStatus stateOfRecords = TELDIR_STATUS_ACTIVE);



    private:
	    /**
	     * getAllTelephoneDirectories
	     * 
	     * A helper class that retrieves the following fields from the TE_DIRECTORY table by executing the 
         * specified SQL statement
         *          - TEDENT_ID
         *          - TESUBD_ID
         *          - TEL_NUMBER
         *          - FULLNAME
         *          - LOCATION
         *          - STATUS 
         *
	     * @return ITelephoneDirectories 
         * @param   readWrite  
         *          (optional) set to true to return writeable
         *          IConfigTelephoneDirectory objects or set to false to return 
         *          readonly ITelephoneDirectory objects.
	     * @param : const std::string& sql 
         *          SQL statement to be executed
	     * 
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
	     */
        ITelephoneDirectories getAllTelephoneDirectories(const bool readWrite, const TA_Base_Core::SQLStatement& sql, TA_Base_Core::IDatabase* pDb);

        /**
	     * Holds the pointer to itself
         */
        static TelephoneDirectoryAccessFactory* m_instance;
	};

} // closes TA_Core

#endif // !defined(TelephoneDirectoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
