/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ecs/src/ECSStationModeAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * ECSStationModeAccessFactory is a singleton that is used to retrieve ECSStationMode objects either from the
  * database or newly created. All ECSStationMode objects returned will adhear to the IECSStationMode interface.
  */



#if !defined(ECSStationModeAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ECSStationModeAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/ecs/src/IECSStationMode.h"
#include "core/data_access_interface/ecs/src/IConfigECSStationMode.h"

//zn++

//using TA_IRS_App::MappedECSDataCache::OrderedLocations;
//using TA_IRS_App::MappedECSDataCache::OrderedLocation;
//++zn

namespace TA_IRS_Core
{

    

	class ECSStationModeAccessFactory
	{


	private:
        
        // Disable the following methods
		ECSStationModeAccessFactory() { };
		ECSStationModeAccessFactory( const ECSStationModeAccessFactory& theECSStationModeAccessFactory);
		ECSStationModeAccessFactory& operator=(const ECSStationModeAccessFactory &);


	public:
        
        
        /** 
         * ~ECSStationModeAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~ECSStationModeAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an ECSStationModeAccessFactory object.
         */
		static ECSStationModeAccessFactory& getInstance();


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
         * getECSStationMode
         *
         * Retrieves the specified ECSStationMode and returns the associated data as an 
         * object conforming to the IECSStationMode interface.
         *
         * @param   key        the primary key/id of the ECSStationMode to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigECSStationMode objects or set to false to return 
         *                     readonly IECSStationMode objects.
         *
         * @return  A pointer to an object conforming to the IECSStationMode interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IECSStationMode object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no ECSStationMode matching the supplied key
         *          - if there is more than one ECSStationMode matching the supplied key. 
         *
         */
		IECSStationMode* getECSStationMode(const unsigned long key, const bool readWrite);


        /**
         * getECSStationModesByMasterModeKey
         *
         * Retreives all ECSStationModes belonging to an MasterMode with the specified key and
		 * returns the associated data as a vector of objects conforming to the IECSStationMode 
		 * interface.
         *
         * @param   key        the key of the Master Mode (EC_MASTER_MODE.ECMMOD_ID) to match in the
         *                     EC_MASTER_MODE_HELPER table.
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigECSStationMode objects or set to false to return 
         *                     readonly IECSStationMode objects.
         *
         * @return  a pointer to an object conforming to the IECSStationMode interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IECSStationMode object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no ECSStationMode matching the supplied key
         *          - if there is more than one ECSStationMode matching the supplied key. 
         *
         */
        IECSStationModes getECSStationModesByMasterModeKey(unsigned long key, const bool readWrite = false);

        /**
         * getStationModeKeysByMasterModeKey
         *
         * Retreives all ECSStationModeKeys belonging to an MasterMode with the specified key.
         *
         * @param   key        the key of the Master Mode (EC_MASTER_MODE.ECMMOD_ID) to match in the
         *                     EC_MASTER_MODE_HELPER table.
         *
         * @return  a vector of ECStationModeKeys 
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no ECSStationMode matching the supplied key
         *          - if there is more than one ECSStationMode matching the supplied key. 
         *
         */
        std::vector<unsigned long> getStationModeKeysByMasterModeKey(unsigned long key);


        /**
         * getAllECSStationModes
         *
         * Retrieves all ECSStationModes and returns the associated data as a vector of  
         * objects conforming to the IECSStationMode interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigECSStationMode objects or set to false to return 
         *                     readonly IECSStationMode objects.
         *
         * @return  a vector of pointers to objects conforming to the IECSStationMode interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IECSStationMode object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no ECSStationMode matching the supplied key
         *          - if there is more than one ECSStationMode matching the supplied key. 
         *
         */
       IECSStationModes getAllECSStationModes( const bool readWrite = false );


        /**
         * getECSStationModesWhere (UNTESTED)
         *
         * Retrieves all ECSStationModes matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ECSMOD_ID from EC_STATION_MODE where LOCATIONKEY=<locationKey> and 
         *                                           STATION_MODE=<stationMode> and 
         *                                           OPERATION=<operation> and 
         *                                           TRACK=<track> and 
         *                                           CONDITION=<condition>"
         * 
         * If a particular clause is not required, specify with a null for strings or 0 for locationKey.  
         *
         * @return A vector of pointers to objects conforming to the IECSStationMode interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IECSStationMode
         * objects
         *
         * @param   readWrite
         *          - true returns IConfigECSStationMode objects
         *          - false returns IECSStationMode objects
         * @param   locationKey
         *          - "where LOCATIONKEY=<locationKey>" clause.
         *          - specify -1 to omit.
         * @param   stationMode
         *          - "where STATION_MODE=<stationMode>" clause.
         *          - specify -1 to omit.
         * @param   operation
         *          - case sensitive "where OPERATION=<operation>" clause.
         *          - specify "" to omit.
         * @param   track
         *          - case sensitive "where TRACK=<track>" clause.
         *          - specify "" to omit.
         * @param   condition
         *          - case sensitive "where CONDITION=<condition>" clause.
         *          - specify "" to omit.
         * @param   congestionLogic
         *          - case sensitive "where CONGESTION_LOGIC=<congestionLogic>" clause.
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
        IECSStationModes getECSStationModesWhere( const bool readWrite = false,
                                                  const unsigned long locationKey=-1, 
                                                  const unsigned long stationMode=-1,
                                                  const std::string& operation="", 
                                                  const std::string& track="", 
                                                  const std::string& condition="",
                                                  const std::string& congestionLogic="");

        /**
         * getStationModeKeysWhere (UNTESTED)
         *
         * Retrieves all primary keys matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ECSMOD_ID from EC_STATION_MODE where LOCATIONKEY=<locationKey> and 
         *                                              STATION_MODE=<stationMode> and 
         *                                              OPERATION=<operation> and 
         *                                              TRACK=<track> and 
         *                                              CONDITION=<condition>"
         *
         * If a particular clause is not required, specify with a null for strings or 0 for locationKey.  
         *
         * @return A vector of StationMode keys (primary keys) 
         *
         * @param   readWrite
         *          - true returns IConfigECSStationMode objects
         *          - false returns IECSStationMode objects
         * @param   locationKey
         *          - "where LOCATIONKEY=<locationKey>" clause.
         *          - specify -1 to omit.
         * @param   stationMode
         *          - "where STATION_MODE=<stationMode>" clause.
         *          - specify -1 to omit.
         * @param   operation
         *          - case sensitive "where OPERATION=<operation>" clause.
         *          - specify "" to omit.
         * @param   track
         *          - case sensitive "where TRACK=<track>" clause.
         *          - specify "" to omit.
         * @param   condition
         *          - case sensitive "where CONDITION=<condition>" clause.
         *          - specify "" to omit.
         * @param   congestionLogic
         *          - case sensitive "where CONGESTION_LOGIC=<congestionLogic>" clause.
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
        std::vector<unsigned long> getECSStationModeKeysWhere( const unsigned long locationKey=-1, 
                                                               const unsigned long stationMode=-1,
                                                               const std::string& operation="", 
                                                               const std::string& track="", 
                                                               const std::string& condition="",
                                                               const std::string& congestionLogic="");

		IECSStationModes getECSStationModeDetails(std::vector< unsigned long > keylist);
        /**
         * createECSStationMode
         * 
         * Creates an empty IConfigECSStationMode object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigECSStationMode object.
         *
         * @return A pointer to an IConfigECSStationMode object
         *
         */
		IConfigECSStationMode* createECSStationMode();


        /**
         * copyECSStationMode
         * 
         * Creates a new configuration ECSStationMode object using the ECSStationMode passed in
         *
         * @param  ECSStationModeToCopy the ConfigECSStationMode to copy
         *
         * @return the new object as a pointer to an IConfigECSStationMode object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigECSStationMode* copyECSStationMode(const IConfigECSStationMode* ECSStationModeToCopy);

	   //zn++ - Launching manager optimise
	   std::vector< std::pair<unsigned long, std::string> > getOrderedLocations();
	   //++zn


    private:
        static ECSStationModeAccessFactory* m_instance;
	};

} // closes TA_Core

#endif // !defined(ECSStationModeAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
