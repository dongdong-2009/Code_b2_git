/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectoryAccessFactory.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by:  $Author: weikun.lin $
  *
  * TelephoneSubDirectoryAccessFactory is a singleton that is used to retrieve TelephoneSubDirectory objects either from the
  * database or newly created. All TelephoneSubDirectory objects returned will adhear to the ITelephoneSubDirectory interface.
  */

#if !defined(TelephoneSubDirectoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define TelephoneSubDirectoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/data_access_interface/telephone/telephone_directory_dai/src/ITelephoneSubDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/IConfigTelephoneSubDirectory.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryItemStatus.h"

namespace TA_IRS_Core
{

	class TelephoneSubDirectoryAccessFactory
	{
	private:

		TelephoneSubDirectoryAccessFactory() { };

        // Disable the following methods
		TelephoneSubDirectoryAccessFactory( const TelephoneSubDirectoryAccessFactory& theTelephoneSubDirectoryAccessFactory);
		TelephoneSubDirectoryAccessFactory& operator=(const TelephoneSubDirectoryAccessFactory &);

	public:
        /** 
         * ~TelephoneSubDirectoryAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~TelephoneSubDirectoryAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an TelephoneSubDirectoryAccessFactory object.
         */
		static TelephoneSubDirectoryAccessFactory& getInstance();

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
         * getTelephoneSubDirectory
         *
         * Retrieves the specified TelephoneSubDirectory and returns the associated data as an 
         * object conforming to the ITelephoneSubDirectory interface.
         *
         * @return  A pointer to an object conforming to the ITelephoneDirectory interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITelephoneDirectory object
         *
         * @param   idKey           the primary key/id of the TelephoneSubDirectory to retrieve
         * @param   readWrite       (optional) set to true to return writeable
         *                          IConfigTelephoneDirectory objects or set to false to return 
         *                          readonly ITelephoneDirectory objects.
         * @param   stateOfRecords  (optional) value will determine the state of the record that 
         *                          will be retrieved
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
		ITelephoneSubDirectory* getTelephoneSubDirectory(const unsigned long idKey, 
                                                         const bool readWrite = false, 
                                                         const TelephoneDirectoryItemStatus stateOfRecords = TELDIR_STATUS_ACTIVE);

        /**
         * getAllTelephoneSubDirectories
         *
         * Retrieves all Sub Directory reocords and returns the associated data as a vector of  
         * objects conforming to the ITelephoneSubDirectory interface.
         *
         * @return  a vector of pointers to objects conforming to the ITelephoneDirectory interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITelephoneDirectory objects
         *
         * @param   readWrite       (optional) set to true to return writeable
         *                          IConfigTelephoneDirectory objects or set to false to return 
         *                          readonly ITelephoneDirectory objects.
         * @param   stateOfRecords  (optional) value will determine the state of the record that 
         *                          will be retrieved
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        ITelephoneSubDirectories getAllTelephoneSubDirectories( const bool readWrite = false, 
                                                                const TelephoneDirectoryItemStatus stateOfRecords = TELDIR_STATUS_ACTIVE );

        /**
         * getTelephoneSubDirectoryWhere
         *
         * Retrieves a sub directory record that matches the specified where clauses.  The factory will
         * execute: 
         *
         * "select TESUBD_ID, NAME, STATUS from TE_SUBDIRECTORY 
         *      where STATUS=<stateOfRecords> and 
         *      NAME=<dirName> 
         *
         * If a particular clause is not required, specify with a null for strings.  
         *
         * @return  A pointer to object conforming to the ITelephoneSubDirectory interface.
         *          N.B.: It is the responsibility of the *client* to delete the returned ITelephoneSubDirectory
         *          objects
         *
         * @param   readWrite       
         *          (optional) set to true to return writeable
         *          IConfigTelephoneDirectory objects or set to false to return 
         *          readonly ITelephoneDirectory objects.
         * @param   dirName
         *          - The directory name of the record to be retrieved
         *          - specify "" to omit.
         * @param   stateOfRecords  
         *          (optional) value will determine the state of the record that 
         *          will be retrieved
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - NOT_UNIQUE, if there is more than one TelephoneDirectory matching the supplied key. 
         */
        ITelephoneSubDirectory* getTelephoneSubDirectoryWhere(  const bool readWrite = false,
                                                                const std::string& dirName = "",
                                                                const TelephoneDirectoryItemStatus stateOfRecords = TELDIR_STATUS_ACTIVE);
        
	    /**
	     * getAnyStateTelephoneSubDirectoryWhere
	     * 
         * Retrieves a list of sub directory records that matches the specified where clauses regardless of the status of the record.
         * The factory will execute: 
	     * 
         * "select TESUBD_ID, NAME, STATUS from TE_SUBDIRECTORY where NAME=<dirName> 
         *
         * @return  A list of pointers to object conforming to the ITelephoneSubDirectory interface.
         *          N.B.: It is the responsibility of the *client* to delete the returned ITelephoneSubDirectory
         *          objects
         *
         * @param   readWrite       
         *          (optional) set to true to return writable
         *          IConfigTelephoneDirectory objects or set to false to return 
         *          read only ITelephoneDirectory objects.
         * @param   dirName
         *          - The directory name of the record to be retrieved
         *          - specify "" to omit.
	     * 
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - NOT_UNIQUE, if there is more than one TelephoneDirectory matching the supplied key. 
         *          - NO_VALUE, if no record can be found
	     */
        ITelephoneSubDirectories getAnyStateTelephoneSubDirectoryWhere(  const bool readWrite = false,
                                                                        const std::string& dirName ="");
  
	    /**
	     * getNotInactiveTelephoneSubDirectoryWhere
	     * 
         * Retrieves a list of sub directory records that matches the specified where clauses where status of the record is inactive.
         * The factory will execute: 
	     * 
         * "select TESUBD_ID, NAME, STATUS from TE_SUBDIRECTORY 
         *          where NAME=<dirName> and STATUS=<TELDIR_STATUS_INACTIVE>
         *
         * @return  A list of pointers to object conforming to the ITelephoneSubDirectory interface.
         *          N.B.: It is the responsibility of the *client* to delete the returned ITelephoneSubDirectory
         *          objects
         *
         * @param   readWrite       
         *          (optional) set to true to return writable
         *          IConfigTelephoneDirectory objects or set to false to return 
         *          read only ITelephoneDirectory objects.
         * @param   dirName
         *          - The directory name of the record to be retrieved
         *          - specify "" to omit.
	     * 
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - NO_VALUE, if no record can be found
	     */
        ITelephoneSubDirectories getNotInactiveTelephoneSubDirectoryWhere( const bool readWrite,
                                                                          const std::string& dirName);

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
		IConfigTelephoneSubDirectory* createTelephoneSubDirectory();

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
        IConfigTelephoneSubDirectory* copyTelephoneSubDirectory(const IConfigTelephoneSubDirectory* subDirToCopy);

		//libo++
		std::map<long, std::string> getAllAssociatedSubdirectoryMap(const TelephoneDirectoryItemStatus stateOfRecords = TELDIR_STATUS_ACTIVE);
		//++libo

    private:
	    /**
	     * getSubDirectoryWhere
         * A internal helper function that will retrieve the data based in the SQL statement. 
         * Function will retrieve the following fields as such, SQL statements have to ensure that
         * they are selecting the following field as well.
         *          - TESUBD_ID
         *          - NAME
         *          - STATUS
	     * 
         * @return  A pointer to object conforming to the ITelephoneSubDirectory interface.
         *          N.B.: It is the responsibility of the *client* to delete the returned ITelephoneSubDirectory
         *          objects
         *
         * @param   sql
         *          - The SQL statement to be executed
         * @param   readWrite       
         *          (optional) set to true to return writable
         *          IConfigTelephoneDirectory objects or set to false to return 
         *          read only ITelephoneDirectory objects.
	     * 
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - NOT_UNIQUE, if there is more than one TelephoneDirectory matching the supplied key. 
         *          - NO_VALUE, if no record can be found
	     */
        static ITelephoneSubDirectory* getSubDirectoryWhere( TA_Base_Core::IDatabase* pDb, const TA_Base_Core::SQLStatement& sql, const bool readWrite = false);

	    /**
	     * getSubDirectoriesWhere
         * A internal helper function that will retrieve the data based in the SQL statement. 
         * Function will retrieve the following fields as such, SQL statements have to ensure that
         * they are selecting the following field as well.
         *          - TESUBD_ID
         *          - NAME
         *          - STATUS
	     * 
         * @return  A list of pointers to object conforming to the ITelephoneSubDirectory interface.
         *          N.B.: It is the responsibility of the *client* to delete the returned ITelephoneSubDirectory
         *          objects
         *
         * @param   sql
         *          - The SQL statement to be executed
         * @param   readWrite       
         *          (optional) set to true to return writable
         *          IConfigTelephoneDirectory objects or set to false to return 
         *          read only ITelephoneDirectory objects.
	     * 
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - NO_VALUE, if no record can be found
	     */
        static ITelephoneSubDirectories getSubDirectoriesWhere(TA_Base_Core::IDatabase* pDb, const TA_Base_Core::SQLStatement& sql, const bool readWrite = false);



    private:
        /**
	      * Self pointer for singleton implementation
          */
        static TelephoneSubDirectoryAccessFactory* m_instance;
	};
} // closes TA_Core

#endif // !defined(TelephoneSubDirectoryAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
