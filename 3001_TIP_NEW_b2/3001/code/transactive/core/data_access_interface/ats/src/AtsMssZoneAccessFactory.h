/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/AtsMssZoneAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * AtsMssZoneAccessFactory is a singleton that is used to retrieve AtsMssZone objects either from the
  * database or newly created. All AtsMssZone objects returned will adhear to the IAtsMssZone interface.
  */



#if !defined(AtsMssZoneAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define AtsMssZoneAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/ats/src/IAtsMssZone.h"
#include "core/data_access_interface/ats/src/IConfigAtsMssZone.h"




namespace TA_IRS_Core
{

	class AtsMssZoneAccessFactory
	{


	private:

        // Disable the following methods
		AtsMssZoneAccessFactory() { };
		AtsMssZoneAccessFactory( const AtsMssZoneAccessFactory& theAtsMssZoneAccessFactory);
		AtsMssZoneAccessFactory& operator=(const AtsMssZoneAccessFactory &);


	public:

      
        /** 
         * ~AtsMssZoneAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~AtsMssZoneAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an AtsMssZoneAccessFactory object.
         */
		static AtsMssZoneAccessFactory& getInstance();


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
         * getAtsMssZone
         *
         * Retrieves the specified AtsMssZone and returns the associated data as an 
         * object conforming to the IAtsMssZone interface.
         *
         * @param   idKey      the primary key/id of the AtsMssZone to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigAtsMssZone objects or set to false to return 
         *                     readonly IAtsMssZone objects.
         *
         * @return  A pointer to an object conforming to the IAtsMssZone interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IAtsMssZone object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no AtsMssZone matching the supplied key
         *          - if there is more than one AtsMssZone matching the supplied key. 
         *
         */
		IAtsMssZone* getAtsMssZone(const unsigned long idKey, const bool readWrite = false);

        /**
         * getAllAtsMssZones
         *
         * Retrieves all AtsMssZones and returns the associated data as a vector of  
         * objects conforming to the IAtsMssZone interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigAtsMssZone objects or set to false to return 
         *                     readonly IAtsMssZone objects.
         *
         * @return  a vector of pointers to objects conforming to the IAtsMssZone interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IAtsMssZone object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no AtsMssZone matching the supplied key
         *          - if there is more than one AtsMssZone matching the supplied key. 
         *
         */
        IAtsMssZones getAllAtsMssZones( const bool readWrite = false );


        /**
         * getAtsMssZonesWhere (UNTESTED)
         *
         * Retrieves all AtsMssZones matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ATMSSZ_ID from AT_MSSZONE where 
         *                                   STATION_ID=<stationId> and
		 *									 MSS_ZONE=<mssZone>
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of pointers to objects conforming to the IAtsMssZone interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IAtsMssZone
         * objects
         *
         * @param   readWrite
         *          - true returns IConfigAtsMssZone objects
         *          - false returns IAtsMssZone objects
		 * @param   stationId
         *          - "where STATION_ID=<stationId>" clause.
         *          - specify -1 to omit.
         * @param   track
         *          - case sensitive "where MSS_ZONE=<mssZone>" clause.
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
        IAtsMssZones getAtsMssZonesWhere( const bool readWrite = false,
                                          long stationId = -1, 
										  const std::string& mssZone = "");
 
        /**
         * getAtsMssZoneIdsWhere (UNTESTED)
         *
         * Retrieves all AtsMssZoneIds (primary key) matching the specified where clauses.  
         * The factory will execute: 
         *
         * "select ATMSSZ_ID from AT_MSSZONE where 
         *                                   STATION_ID=<stationId> and
		 *									 MSS_ZONE=<mssZone>
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of AtsMssZoneIds (primary keys) 
         *
		 * @param   stationId
         *          - "where STATION_ID=<stationId>" clause.
         *          - specify -1 to omit.
         * @param   track
         *          - case sensitive "where MSS_ZONE=<mssZone>" clause.
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
        std::vector<int> getAtsMssZoneIdsWhere( long stationId = -1, 
												const std::string& mssZone = "");

       
        /**
         * createAtsMssZone
         * 
         * Creates an empty IConfigAtsMssZone object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigAtsMssZone object.
         *
         * @return A pointer to an IConfigAtsMssZone object
         *
         */
		IConfigAtsMssZone* createAtsMssZone();


        /**
         * copyAtsMssZone
         * 
         * Creates a new configuration AtsMssZone object using the AtsMssZone passed in
         *
         * @param  AtsMssZoneToCopy the ConfigAtsMssZone to copy
         *
         * @return the new object as a pointer to an IConfigAtsMssZone object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigAtsMssZone* copyAtsMssZone(const IConfigAtsMssZone* AtsMssZoneToCopy);


    private:
        static AtsMssZoneAccessFactory* m_instance;
	};

} // closes TA_Core

#endif // !defined(AtsMssZoneAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
