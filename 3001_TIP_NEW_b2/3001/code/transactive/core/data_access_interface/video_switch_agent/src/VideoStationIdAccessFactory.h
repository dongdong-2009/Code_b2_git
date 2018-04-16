/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/VideoStationIdAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * VideoStationIdAccessFactory is a singleton that is used to retrieve VideoStationId objects either from the
  * database or newly created. All VideoStationId objects returned will adhear to the IVideoStationId interface.
  */



#if !defined(VideoStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define VideoStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <map>


#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/video_switch_agent/src/IVideoStationId.h"
#include "core/data_access_interface/video_switch_agent/src/IConfigVideoStationId.h"




namespace TA_IRS_Core
{

	class VideoStationIdAccessFactory
	{


	private:

        // Disable the following methods
		VideoStationIdAccessFactory() { };
		VideoStationIdAccessFactory( const VideoStationIdAccessFactory& theVideoStationIdAccessFactory);
		VideoStationIdAccessFactory& operator=(const VideoStationIdAccessFactory &);


	public:

        // used to map between TA locations and station ids
        typedef std::map<unsigned long, unsigned long> StationIdToLocationKeyMap;
        typedef std::map<unsigned long, unsigned long> LocationKeyToStationIdMap;

      
        /** 
         * ~VideoStationIdAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~VideoStationIdAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an VideoStationIdAccessFactory object.
         */
		static VideoStationIdAccessFactory& getInstance();


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
         * getVideoStationId
         *
         * Retrieves the specified VideoStationId and returns the associated data as an 
         * object conforming to the IVideoStationId interface.
         *
         * @param   idKey      the primary key/id of the VideoStationId to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigVideoStationId objects or set to false to return 
         *                     readonly IVideoStationId objects.
         *
         * @return  A pointer to an object conforming to the IVideoStationId interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IVideoStationId object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  TA_Base_Core::DataException 
         *          - if there is no VideoStationId matching the supplied key
         *          - if there is more than one VideoStationId matching the supplied key. 
         *
         */
		IVideoStationId* getVideoStationId(const unsigned long idKey, const bool readWrite = false);

        /**
         * getAllVideoStationIds
         *
         * Retrieves all VideoStationIds and returns the associated data as a vector of  
         * objects conforming to the IVideoStationId interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigVideoStationId objects or set to false to return 
         *                     readonly IVideoStationId objects.
         *
         * @return  a vector of pointers to objects conforming to the IVideoStationId interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IVideoStationId object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  TA_Base_Core::DataException 
         *          - if there is no VideoStationId matching the supplied key
         *          - if there is more than one VideoStationId matching the supplied key. 
         *
         */
        IVideoStationIds getAllVideoStationIds( const bool readWrite = false );


        /**
         * getVideoStationIdsWhere (UNTESTED)
         *
         * Retrieves all VideoStationIds matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ATBTRA_ID from AT_BRANCH_TRACK where 
         *                                   TRACK=<track> 
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of pointers to objects conforming to the IVideoStationId interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IVideoStationId
         * objects
         *
         * @param   readWrite
         *          - true returns IConfigVideoStationId objects
         *          - false returns IVideoStationId objects
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
        IVideoStationId* getVideoStationIdByLocation(unsigned long taLocation, const bool readWrite = false);
 
        /**
         * getVideoStationIdIdsWhere (UNTESTED)
         *
         * Retrieves all VideoStationIdIds (primary key) matching the specified where clauses.  
         * The factory will execute: 
         *
         * "select ATBTRA_ID from AT_BRANCH_TRACK where  
         *                                   TRACK=<track>"
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of VideoStationIdIds (primary keys) 
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
        std::vector<int> getVideoStationIdIdsWhere( const std::string& track="" );

       
        /**
         * createVideoStationId
         * 
         * Creates an empty IConfigVideoStationId object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigVideoStationId object.
         *
         * @return A pointer to an IConfigVideoStationId object
         *
         */
		IConfigVideoStationId* createVideoStationId();


        /**
         * copyVideoStationId
         * 
         * Creates a new configuration VideoStationId object using the VideoStationId passed in
         *
         * @param  VideoStationIdToCopy the ConfigVideoStationId to copy
         *
         * @return the new object as a pointer to an IConfigVideoStationId object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigVideoStationId* copyVideoStationId(const IConfigVideoStationId* VideoStationIdToCopy);


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
          *          - if there is no VideoStationId matching the supplied key
          *          - if there is more than one VideoStationId matching the supplied key. 
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
          *          - if there is no VideoStationId matching the supplied key
          *          - if there is more than one VideoStationId matching the supplied key. 
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
        static VideoStationIdAccessFactory* m_instance;
	};

} // closes TA_IRS_Core

#endif // !defined(VideoStationIdAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
