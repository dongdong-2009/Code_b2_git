/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/AtsPlatformAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * AtsPlatformAccessFactory is a singleton that is used to retrieve AtsPlatform objects either from the
  * database or newly created. All AtsPlatform objects returned will adhear to the IAtsPlatform interface.
  */



#if !defined(AtsPlatformAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define AtsPlatformAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/ats/src/IAtsPlatform.h"
#include "core/data_access_interface/ats/src/IConfigAtsPlatform.h"




namespace TA_IRS_Core
{

	class AtsPlatformAccessFactory
	{


	private:

        // Disable the following methods
		AtsPlatformAccessFactory() { };
		AtsPlatformAccessFactory( const AtsPlatformAccessFactory& theAtsPlatformAccessFactory);
		AtsPlatformAccessFactory& operator=(const AtsPlatformAccessFactory &);


	public:

      
        /** 
         * ~AtsPlatformAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~AtsPlatformAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an AtsPlatformAccessFactory object.
         */
		static AtsPlatformAccessFactory& getInstance();


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
         * getAtsPlatform
         *
         * Retrieves the specified AtsPlatform and returns the associated data as an 
         * object conforming to the IAtsPlatform interface.
         *
         * @param   idKey      the primary key/id of the AtsPlatform to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigAtsPlatform objects or set to false to return 
         *                     readonly IAtsPlatform objects.
         *
         * @return  A pointer to an object conforming to the IAtsPlatform interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IAtsPlatform object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no AtsPlatform matching the supplied key
         *          - if there is more than one AtsPlatform matching the supplied key. 
         *
         */
		IAtsPlatform* getAtsPlatform(const unsigned long idKey, const bool readWrite = false);

        /**
         * getAllAtsPlatforms
         *
         * Retrieves all AtsPlatforms and returns the associated data as a vector of  
         * objects conforming to the IAtsPlatform interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigAtsPlatform objects or set to false to return 
         *                     readonly IAtsPlatform objects.
         *
         * @return  a vector of pointers to objects conforming to the IAtsPlatform interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IAtsPlatform object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no AtsPlatform matching the supplied key
         *          - if there is more than one AtsPlatform matching the supplied key. 
         *
         */
        IAtsPlatforms getAllAtsPlatforms( const bool readWrite = false );


        /**
         * getAtsPlatformsWhere (UNTESTED)
         *
         * Retrieves all AtsPlatforms matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ATMSSZ_ID from AT_MSSZONE where 
         *                                   STATION_ID=<stationId> and
		 *									 MSS_ZONE=<mssZone>
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of pointers to objects conforming to the IAtsPlatform interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IAtsPlatform
         * objects
         *
         * @param   readWrite
         *          - true returns IConfigAtsPlatform objects
         *          - false returns IAtsPlatform objects
		 * @param   platformId
         *          - "where PLATFORM_ID=<platformId>" clause.
         *          - specify -1 to omit.
		 * @param   stationId
         *          - "where STATION_ID=<stationId>" clause.
         *          - specify -1 to omit.
         * @param   platformName
         *          - case sensitive "where PLATFORM_NAME=<platformName>" clause.
         *          - specify "" to omit.
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IAtsPlatforms getAtsPlatformsWhere( const bool readWrite = false,
										  long platformId = -1,
                                          long stationId = -1, 
										  const std::string& platformName = "");
 
        /**
         * getAtsPlatformIdsWhere (UNTESTED)
         *
         * Retrieves all AtsPlatformIds (primary key) matching the specified where clauses.  
         * The factory will execute: 
         *
         * "select ATMSSZ_ID from AT_MSSZONE where 
         *                                   STATION_ID=<stationId> and
		 *									 MSS_ZONE=<mssZone>
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of AtsPlatformIds (primary keys) 
         *
		 * @param   platformId
         *          - "where PLATFORM_ID=<platformId>" clause.
         *          - specify -1 to omit.
		 * @param   stationId
         *          - "where STATION_ID=<stationId>" clause.
         *          - specify -1 to omit.
         * @param   platformName
         *          - case sensitive "where PLATFORM_NAME=<platformName>" clause.
         *          - specify "" to omit.
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        std::vector<int> getAtsPlatformIdsWhere( long platformId = -1,
                                          long stationId = -1, 
										  const std::string& platformName = "");

       
        /**
         * createAtsPlatform
         * 
         * Creates an empty IConfigAtsPlatform object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigAtsPlatform object.
         *
         * @return A pointer to an IConfigAtsPlatform object
         *
         */
		IConfigAtsPlatform* createAtsPlatform();


        /**
         * copyAtsPlatform
         * 
         * Creates a new configuration AtsPlatform object using the AtsPlatform passed in
         *
         * @param  AtsPlatformToCopy the ConfigAtsPlatform to copy
         *
         * @return the new object as a pointer to an IConfigAtsPlatform object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigAtsPlatform* copyAtsPlatform(const IConfigAtsPlatform* AtsPlatformToCopy);


    private:
        static AtsPlatformAccessFactory* m_instance;
	};

} // closes TA_Core

#endif // !defined(AtsPlatformAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
