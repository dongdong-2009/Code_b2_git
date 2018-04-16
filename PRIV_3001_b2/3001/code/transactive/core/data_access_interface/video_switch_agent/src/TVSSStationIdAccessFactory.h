/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/core/data_access_interface/video_switch_agent/src/TVSSStationIdAccessFactory.h $
  * @author Zhang hongzhi
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/02/5 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * TVSSStationIdAccessFactory is a singleton that is used to retrieve TVSSStationId objects either from the
  * database or newly created. All TVSSStationId objects returned will adhear to the ITVSSStationId interface.
  */



#if !defined(TVSSStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define TVSSStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <map>


#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/video_switch_agent/src/ITVSSStationId.h"
#include "core/data_access_interface/video_switch_agent/src/IConfigTVSSStationId.h"




namespace TA_IRS_Core
{

	class TVSSStationIdAccessFactory
	{


	private:

        // Disable the following methods
		TVSSStationIdAccessFactory() { };
		TVSSStationIdAccessFactory( const TVSSStationIdAccessFactory& theTVSSStationIdAccessFactory);
		TVSSStationIdAccessFactory& operator=(const TVSSStationIdAccessFactory &);


	public:

        // used to map between TA locations and station ids
        typedef std::map<unsigned long, unsigned long> StationIdToLocationKeyMap;
        typedef std::map<unsigned long, unsigned long> LocationKeyToStationIdMap;

      
        /** 
         * ~TVSSStationIdAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~TVSSStationIdAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an TVSSStationIdAccessFactory object.
         */
		static TVSSStationIdAccessFactory& getInstance();


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
         * getTVSSStationId
         *
         * Retrieves the specified TVSSStationId and returns the associated data as an 
         * object conforming to the ITVSSStationId interface.
         *
         * @param   idKey      the primary key/id of the TVSSStationId to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigTVSSStationId objects or set to false to return 
         *                     readonly ITVSSStationId objects.
         *
         * @return  A pointer to an object conforming to the ITVSSStationId interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITVSSStationId object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  TA_Base_Core::DataException 
         *          - if there is no TVSSStationId matching the supplied key
         *          - if there is more than one TVSSStationId matching the supplied key. 
         *
         */
		ITVSSStationId* getTVSSStationId(const unsigned long idKey, const bool readWrite = false);

        /**
         * getAllTVSSStationIds
         *
         * Retrieves all TVSSStationIds and returns the associated data as a vector of  
         * objects conforming to the ITVSSStationId interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigTVSSStationId objects or set to false to return 
         *                     readonly ITVSSStationId objects.
         *
         * @return  a vector of pointers to objects conforming to the ITVSSStationId interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITVSSStationId object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  TA_Base_Core::DataException 
         *          - if there is no TVSSStationId matching the supplied key
         *          - if there is more than one TVSSStationId matching the supplied key. 
         *
         */
        ITVSSStationIds getAllTVSSStationIds( const bool readWrite = false );


        /**
         * getTVSSStationIdsWhere (UNTESTED)
         *
         * Retrieves all TVSSStationIds matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ATBTRA_ID from AT_BRANCH_TRACK where 
         *                                   TRACK=<track> 
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of pointers to objects conforming to the ITVSSStationId interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ITVSSStationId
         * objects
         *
         * @param   readWrite
         *          - true returns IConfigTVSSStationId objects
         *          - false returns ITVSSStationId objects
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
        ITVSSStationId* getTVSSStationIdByLocation(unsigned long taLocation, const bool readWrite = false);
 
        /**
         * getTVSSStationIdIdsWhere (UNTESTED)
         *
         * Retrieves all TVSSStationIdIds (primary key) matching the specified where clauses.  
         * The factory will execute: 
         *
         * "select ATBTRA_ID from AT_BRANCH_TRACK where  
         *                                   TRACK=<track>"
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of TVSSStationIdIds (primary keys) 
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
        std::vector<int> getTVSSStationIdIdsWhere( const std::string& track="" );

       
        /**
         * createTVSSStationId
         * 
         * Creates an empty IConfigTVSSStationId object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigTVSSStationId object.
         *
         * @return A pointer to an IConfigTVSSStationId object
         *
         */
		IConfigTVSSStationId* createTVSSStationId();


        /**
         * copyTVSSStationId
         * 
         * Creates a new configuration TVSSStationId object using the TVSSStationId passed in
         *
         * @param  TVSSStationIdToCopy the ConfigTVSSStationId to copy
         *
         * @return the new object as a pointer to an IConfigTVSSStationId object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigTVSSStationId* copyTVSSStationId(const IConfigTVSSStationId* TVSSStationIdToCopy);


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
          *          - if there is no TVSSStationId matching the supplied key
          *          - if there is more than one TVSSStationId matching the supplied key. 
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
          *          - if there is no TVSSStationId matching the supplied key
          *          - if there is more than one TVSSStationId matching the supplied key. 
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
        static TVSSStationIdAccessFactory* m_instance;
	};

} // closes TA_IRS_Core

#endif // !defined(TVSSStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
