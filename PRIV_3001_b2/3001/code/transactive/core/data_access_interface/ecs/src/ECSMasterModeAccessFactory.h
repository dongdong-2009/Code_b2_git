/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ecs/src/ECSMasterModeAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * ECSMasterModeAccessFactory is a singleton that is used to retrieve ECSMasterMode objects either from the
  * database or newly created. All ECSMasterMode objects returned will adhear to the IECSMasterMode interface.
  */



#if !defined(ECSMasterModeAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define ECSMasterModeAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#ifdef _MSC_VER
#pragma warning( disable : 4284 )
#endif // _MSC_VER

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/ecs/src/IECSMasterMode.h"
#include "core/data_access_interface/ecs/src/IConfigECSMasterMode.h"



namespace TA_IRS_Core
{

	class ECSMasterModeAccessFactory
	{


	private:

        // Disable the following methods
		ECSMasterModeAccessFactory() { };
		ECSMasterModeAccessFactory( const ECSMasterModeAccessFactory& theECSMasterModeAccessFactory);
		ECSMasterModeAccessFactory& operator=(const ECSMasterModeAccessFactory &);


	public:


        /** 
         * ~ECSMasterModeAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~ECSMasterModeAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an ECSMasterModeAccessFactory object.
         */
		static ECSMasterModeAccessFactory& getInstance();


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
         * getECSMasterMode
         *
         * Retrieves the specified ECSMasterMode and returns the associated data as an 
         * object conforming to the IECSMasterMode interface.
         *
         * @param   key        the primary key/id of the ECSMasterMode to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigECSMasterMode objects or set to false to return 
         *                     readonly IECSMasterMode objects.
         *
         * @return  A pointer to an object conforming to the IECSMasterMode interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IECSMasterMode object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no ECSMasterMode matching the supplied key
         *          - if there is more than one ECSMasterMode matching the supplied key. 
         *
         */
		IECSMasterMode* getECSMasterMode(const unsigned long key, const bool readWrite);


		IECSMasterModes getECSMasterModeDetails(std::vector< unsigned long > keylist);
        /**
         * getAllECSMasterModes
         *
         * Retrieves all ECSMasterModes and returns the associated data as a vector of  
         * objects conforming to the IECSMasterMode interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigECSMasterMode objects or set to false to return 
         *                     readonly IECSMasterMode objects.
         *
         * @return  a vector of pointers to objects conforming to the IECSMasterMode interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IECSMasterMode object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no ECSMasterMode matching the supplied key
         *          - if there is more than one ECSMasterMode matching the supplied key. 
         *
         */
        IECSMasterModes getAllECSMasterModes( const bool readWrite = false );


        /**
         * getMasterModeKeysWhere (UNTESTED)
         *
         * Retrieves all primary keys matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ECMMOD_ID from EC_MASTER_MODE where  MASTER_MODE=<masterMode> and 
         *                                              CONDITION=<condition> and 
         *                                              ECZONE_ID=<zoneID> and 
         *                                              EVACUATION_DIRECTION=<evacuationDirection> and
		 *												AIRFLOW_DIRECTION=<airflowDirection> and
		 *												DISPLAY_DIRECTION=<displayDirection>"
         *
         * If a particular clause is not required, specify with a null for strings or 0 for locationKey.  
         *
         * @return A vector of MasterMode keys (primary keys) 
         *
         * @param   masterMode
         *          - "where MASTER_MODE=<masterMode>" clause.
         *          - specify -1 to omit.
         * @param   condition
         *          - case sensitive "where CONDITION=<condition>" clause.
         *          - specify "" to omit.
         * @param   zoneID
         *          - case sensitive "where ECZONE_ID=<zoneID>" clause.
         *          - specify "" to omit.
         * @param   evacuationDirection
         *          - case sensitive "where EVACUATION_DIRECTION=<evacuationDirection>" clause.
         *          - specify "" to omit.
         * @param   airflowDirection
         *          - case sensitive "where AIRFLOW_DIRECTION=<airflowDirection>" clause.
         *          - specify "" to omit.
         * @param   displayDirection
         *          - case sensitive "where DISPLAY_DIRECTION=<displayDirection>" clause.
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
		std::vector<unsigned long> getECSMasterModeKeysWhere( unsigned long masterMode = -1, 
                                                               const std::string& condition = "",
                                                               int zoneID = -1, 
                                                               const std::string& evacuationDirection = "",
															   const std::string& airflowDirection = "",
															   const std::string& displayDirection = "" );


        /**
         * getMasterModeKeysWhereNot (UNTESTED)
         *
         * Retrieves all primary keys matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ECMMOD_ID from EC_MASTER_MODE where  MASTER_MODE<><masterMode> and 
         *                                              CONDITION<><condition> and 
         *                                              ECZONE_ID<><zoneID> and 
         *                                              EVACUATION_DIRECTION<><evacuationDirection> and
		 *												AIRFLOW_DIRECTION<><airflowDirection> and
		 *												DISPLAY_DIRECTION<><displayDirection>"
         *
         * If a particular clause is not required, specify with a null for strings or 0 for locationKey.  
         *
         * @return A vector of MasterMode keys (primary keys) 
         *
         * @param   masterMode
         *          - "where MASTER_MODE<><masterMode>" clause.
         *          - specify -1 to omit.
         * @param   condition
         *          - case sensitive "where CONDITION<><condition>" clause.
         *          - specify "" to omit.
         * @param   zoneID
         *          - case sensitive "where ECZONE_ID<><zoneID>" clause.
         *          - specify -1 to omit.
         * @param   evacuationDirection
         *          - case sensitive "where EVACUATION_DIRECTION<><evacuationDirection>" clause.
         *          - specify "" to omit.
         * @param   airflowDirection
         *          - case sensitive "where AIRFLOW_DIRECTION<><airflowDirection>" clause.
         *          - specify "" to omit.
         * @param   displayDirection
         *          - case sensitive "where DISPLAY_DIRECTION<><displayDirection>" clause.
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
		std::vector<unsigned long> getECSMasterModeKeysWhereNot( unsigned long masterMode = -1, 
                                                               const std::string& condition = "",
                                                               int zoneID = -1, 
                                                               const std::string& evacuationDirection = "",
															   const std::string& airflowDirection = "",
															   const std::string& displayDirection = "" );


        /**
         * createECSMasterMode
         * 
         * Creates an empty IConfigECSMasterMode object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigECSMasterMode object.
         *
         * @return A pointer to an IConfigECSMasterMode object
         *
         */
		IConfigECSMasterMode* createECSMasterMode();


        /**
         * copyECSMasterMode
         * 
         * Creates a new configuration ECSMasterMode object using the ECSMasterMode passed in
         *
         * @param  ECSMasterModeToCopy the ConfigECSMasterMode to copy
         *
         * @return the new object as a pointer to an IConfigECSMasterMode object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigECSMasterMode* copyECSMasterMode(const IConfigECSMasterMode* ECSMasterModeToCopy);


    private:
        static ECSMasterModeAccessFactory* m_instance;
	};

} // closes TA_Core

#endif // !defined(ECSMasterModeAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
