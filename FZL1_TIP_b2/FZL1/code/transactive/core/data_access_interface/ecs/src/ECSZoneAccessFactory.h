/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ecs/src/ECSZoneAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * ECSZoneAccessFactory is a singleton that is used to retrieve ECSZone objects either from the
  * database or newly created. All ECSZone objects returned will adhear to the IECSZone interface.
  */



#if !defined(ECSZoneAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ECSZoneAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/ecs/src/IECSZone.h"
#include "core/data_access_interface/ecs/src/IConfigECSZone.h"




namespace TA_IRS_Core
{

	class ECSZoneAccessFactory
	{


	private:

        // Disable the following methods
		ECSZoneAccessFactory() { };
		ECSZoneAccessFactory( const ECSZoneAccessFactory& theECSZoneAccessFactory);
		ECSZoneAccessFactory& operator=(const ECSZoneAccessFactory &);


	public:

      
        /** 
         * ~ECSZoneAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~ECSZoneAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an ECSZoneAccessFactory object.
         */
		static ECSZoneAccessFactory& getInstance();


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
         * getECSZone
         *
         * Retrieves the specified ECSZone and returns the associated data as an 
         * object conforming to the IECSZone interface.
         *
         * @param   idKey      the primary key/id of the ECSZone to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigECSZone objects or set to false to return 
         *                     readonly IECSZone objects.
         *
         * @return  A pointer to an object conforming to the IECSZone interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IECSZone object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no ECSZone matching the supplied key
         *          - if there is more than one ECSZone matching the supplied key. 
         *
         */
		IECSZone* getECSZone(const unsigned long idKey, const bool readWrite = false);


        /**
         * getAllECSZones
         *
         * Retrieves all ECSZones and returns the associated data as a vector of  
         * objects conforming to the IECSZone interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigECSZone objects or set to false to return 
         *                     readonly IECSZone objects.
         *
         * @return  a vector of pointers to objects conforming to the IECSZone interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IECSZone object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no ECSZone matching the supplied key
         *          - if there is more than one ECSZone matching the supplied key. 
         *
         */
        IECSZones getAllECSZones( const bool readWrite = false );


        /**
         * getECSZonesWhere (UNTESTED)
         *
         * Retrieves all ECSZones matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ECZONE_ID from EC_ZONE where ECS_LOCATION=<location> and 
         *                                   TRACK=<track> and 
         *                                   STATUS_ENTITYKEY=<statusEntityKey> and 
         *                                   DIRECTION_ENTITYKEY=<directionEntityKey>"
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of pointers to objects conforming to the IECSZone interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IECSZone
         * objects
         *
         * @param   readWrite
         *          - true returns IConfigECSZone objects
         *          - false returns IECSZone objects
         * @param   location
         *          - "where ECS_LOCATION=<location>" clause.
         *          - specify "" to omit.
         * @param   track
         *          - case sensitive "where TRACK=<track>" clause.
         *          - specify "" to omit.
         * @param   occEquipmentEntityKey
         *          - "where OCC_EQUIPMENT_ENTITYKEY=<occEquipmentEntityKey>" clause.
         *          - specify -1 to omit.
         * @param   stnEquipmentEntityKey
         *          - "where STN_EQUIPMENT_ENTITYKEY=<stnEquipmentEntityKey>" clause.
         *          - specify -1 to omit.
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IECSZones getECSZonesWhere( const bool readWrite = false,
                                    const std::string& ecsLocation = "",
                                    const std::string& track="", 
                                    const unsigned long occEquipmentEntityKey=-1,
                                    const unsigned long stnEquipmentEntityKey=-1 );
 
        /**
         * getECSZoneIdsWhere (UNTESTED)
         *
         * Retrieves all ECSZoneIds (primary key) matching the specified where clauses.  
         * The factory will execute: 
         *
         * "select ECZONE_ID from EC_ZONE where ECS_LOCATION=<location> and 
         *                                   TRACK=<track> and 
         *                                   STATUS_ENTITYKEY=<statusEntityKey> and 
         *                                   DIRECTION_ENTITYKEY=<directionEntityKey>"
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of ECSZoneIds (primary keys) 
         *
         * @param   location
         *          - "where ECS_LOCATION=<location>" clause.
         *          - specify "" to omit.
         * @param   track
         *          - case sensitive "where TRACK=<track>" clause.
         *          - specify "" to omit.
         * @param   occEquipmentEntityKey
         *          - "where OCC_EQUIPMENT_ENTITYKEY=<occEquipmentEntityKey>" clause.
         *          - specify -1 to omit.
         * @param   stnEquipmentEntityKey
         *          - "where STN_EQUIPMENT_ENTITYKEY=<stnEquipmentEntityKey>" clause.
         *          - specify -1 to omit.
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        std::vector<int> getECSZoneIdsWhere( const std::string& ecsLocation = "",
                                             const std::string& track="", 
                                             const unsigned long occEquipmentEntityKey=-1,
                                             const unsigned long stnEquipmentEntityKey=-1 );

       
        /**
         * createECSZone
         * 
         * Creates an empty IConfigECSZone object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigECSZone object.
         *
         * @return A pointer to an IConfigECSZone object
         *
         */
		IConfigECSZone* createECSZone();


        /**
         * copyECSZone
         * 
         * Creates a new configuration ECSZone object using the ECSZone passed in
         *
         * @param  ECSZoneToCopy the ConfigECSZone to copy
         *
         * @return the new object as a pointer to an IConfigECSZone object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigECSZone* copyECSZone(const IConfigECSZone* ECSZoneToCopy);


    private:
        static ECSZoneAccessFactory* m_instance;
	};

} // closes TA_Core

#endif // !defined(ECSZoneAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
