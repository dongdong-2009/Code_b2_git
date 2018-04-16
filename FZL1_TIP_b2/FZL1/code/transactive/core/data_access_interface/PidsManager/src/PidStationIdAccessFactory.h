/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/PidsManager/src/PidStationIdAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * PidStationIdAccessFactory is a singleton that is used to retrieve PidStationId objects either from the
  * database or newly created. All PidStationId objects returned will adhear to the IPidStationId interface.
  */



#if !defined(PidStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define PidStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <map>


#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/PidsManager/src/IPidStationId.h"
#include "core/data_access_interface/PidsManager/src/IConfigPidStationId.h"




namespace TA_IRS_Core
{

	class PidStationIdAccessFactory
	{


	private:

        // Disable the following methods
		PidStationIdAccessFactory() { };
		PidStationIdAccessFactory( const PidStationIdAccessFactory& thePidStationIdAccessFactory);
		PidStationIdAccessFactory& operator=(const PidStationIdAccessFactory &);


	public:

        // used to map between TA locations and station ids
        typedef std::map<unsigned long, unsigned long> StationIdToLocationKeyMap;
        typedef std::map<unsigned long, unsigned long> LocationKeyToStationIdMap;

      
        /** 
         * ~PidStationIdAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PidStationIdAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PidStationIdAccessFactory object.
         */
		static PidStationIdAccessFactory& getInstance();


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
         * getPidStationId
         *
         * Retrieves the specified PidStationId and returns the associated data as an 
         * object conforming to the IPidStationId interface.
         *
         * @param   idKey      the primary key/id of the PidStationId to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPidStationId objects or set to false to return 
         *                     readonly IPidStationId objects.
         *
         * @return  A pointer to an object conforming to the IPidStationId interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPidStationId object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  TA_Base_Core::DataException 
         *          - if there is no PidStationId matching the supplied key
         *          - if there is more than one PidStationId matching the supplied key. 
         *
         */
		IPidStationId* getPidStationId(const unsigned long idKey, const bool readWrite = false);

        /**
         * getAllPidStationIds
         *
         * Retrieves all PidStationIds and returns the associated data as a vector of  
         * objects conforming to the IPidStationId interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPidStationId objects or set to false to return 
         *                     readonly IPidStationId objects.
         *
         * @return  a vector of pointers to objects conforming to the IPidStationId interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPidStationId object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  TA_Base_Core::DataException 
         *          - if there is no PidStationId matching the supplied key
         *          - if there is more than one PidStationId matching the supplied key. 
         *
         */
        IPidStationIds getAllPidStationIds( const bool readWrite = false );


        /**
         * getPidStationIdsWhere (UNTESTED)
         *
         * Retrieves all PidStationIds matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ATBTRA_ID from AT_BRANCH_TRACK where 
         *                                   TRACK=<track> 
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of pointers to objects conforming to the IPidStationId interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IPidStationId
         * objects
         *
         * @param   readWrite
         *          - true returns IConfigPidStationId objects
         *          - false returns IPidStationId objects
         * @param   track
         *          - case sensitive "where TRACK=<track>" clause.
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
        IPidStationId* getPidStationIdByLocation(unsigned long taLocation, const bool readWrite = false);
 
        /**
         * getPidStationIdIdsWhere (UNTESTED)
         *
         * Retrieves all PidStationIdIds (primary key) matching the specified where clauses.  
         * The factory will execute: 
         *
         * "select ATBTRA_ID from AT_BRANCH_TRACK where  
         *                                   TRACK=<track>"
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of PidStationIdIds (primary keys) 
         *
         * @param   track
         *          - case sensitive "where TRACK=<track>" clause.
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
        std::vector<int> getPidStationIdIdsWhere( const std::string& track="" );

       
        /**
         * createPidStationId
         * 
         * Creates an empty IConfigPidStationId object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPidStationId object.
         *
         * @return A pointer to an IConfigPidStationId object
         *
         */
		IConfigPidStationId* createPidStationId();


        /**
         * copyPidStationId
         * 
         * Creates a new configuration PidStationId object using the PidStationId passed in
         *
         * @param  PidStationIdToCopy the ConfigPidStationId to copy
         *
         * @return the new object as a pointer to an IConfigPidStationId object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigPidStationId* copyPidStationId(const IConfigPidStationId* PidStationIdToCopy);


        /** 
          * getStationIdToLocationKeyMap
          *
          * Gets a map of station ids to transactive location keys.
          *
          * @return A map
          *
          * @throws  DatabaseException 
          *          - if there are any problems in communicating with the database.
          *            The possilble reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @throws  TA_Base_Core::DataException 
          *          - if there is no PidStationId matching the supplied key
          *          - if there is more than one PidStationId matching the supplied key. 
          */
        StationIdToLocationKeyMap getStationIdToLocationKeyMap();

        /** 
          * getStationIdToLocationKeyMap
          *
          * Same as above - but skip database loading and reverse the already loaded map.
          *
          * @param mapToReverse a map from getLocationKeyToStationIdMap
          *
          * @return the inverse of mapToReverse
          */
        StationIdToLocationKeyMap getStationIdToLocationKeyMap(const LocationKeyToStationIdMap& mapToReverse);

        /** 
          * getLocationKeyToStationIdMap
          *
          * Gets a map of transactive location keys to station ids.
          *
          * @return A map
          *
          * @throws  DatabaseException 
          *          - if there are any problems in communicating with the database.
          *            The possilble reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @throws  TA_Base_Core::DataException 
          *          - if there is no PidStationId matching the supplied key
          *          - if there is more than one PidStationId matching the supplied key. 
          */
        LocationKeyToStationIdMap getLocationKeyToStationIdMap();
        /** 
          * getLocationKeyToStationIdMap
          *
          * Same as above - but skip database loading and reverse the already loaded map.
          *
          * @param mapToReverse a map from getLocationKeyToStationIdMap
          *
          * @return the inverse of mapToReverse
          */
        LocationKeyToStationIdMap getLocationKeyToStationIdMap(const StationIdToLocationKeyMap& mapToReverse);


        /** 
          * stationIdExists
          *
          * This will return true if there is a record in the 
          * database that matches this station id. There will never be more
          * than one record. Optionally the pkey of the current item can be excluded from the check.
          * ie if the current item is the matching one in the database then this isnt counted.
          *
          * @param  stationId   The ID to check.
          * @param  pkey        The pkey of the current item if applicable
          *
          * @return Whether the key exists..
          *
          * @exception DatabaseException 
          *            - if there is a database problem.
          */
        bool stationIdExists(unsigned long stationId, unsigned long pkey = -1);


        /** 
          * locationIdExists
          *
          * This will return true if there is a record in the 
          * database that matches this location id. There will never be more
          * than one record. Optionally the pkey of the current item can be excluded from the check.
          * ie if the current item is the matching one in the database then this isnt counted.
          *
          * @param  locationKey The location to check.
          * @param  pkey        The pkey of the current item if applicable
          *
          * @return Whether the key exists..
          *
          * @exception DatabaseException 
          *            - if there is a database problem.
          */
        bool locationIdExists(unsigned long locationKey, unsigned long pkey = -1);


    private:
        static PidStationIdAccessFactory* m_instance;
	};

} // closes TA_IRS_Core

#endif // !defined(PidStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
