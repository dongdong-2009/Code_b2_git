/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsDssPlanMapAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * AtsDssPlanMapAccessFactory is a singleton that is used to retrieve AtsDssPlanMap objects either from the
  * database or newly created. All AtsDssPlanMap objects returned will adhear to the IAtsDssPlanMap interface.
  */



#if !defined(AtsDssPlanMapAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define AtsDssPlanMapAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/ats/src/IAtsDssPlanMap.h"
#include "core/data_access_interface/ats/src/IConfigAtsDssPlanMap.h"




namespace TA_IRS_Core
{
	class IndexedAtsDssPlanMaps;

	class AtsDssPlanMapAccessFactory
	{


	private:

        // Disable the following methods
		AtsDssPlanMapAccessFactory() { };
		AtsDssPlanMapAccessFactory( const AtsDssPlanMapAccessFactory& theAtsDssPlanMapAccessFactory);
		AtsDssPlanMapAccessFactory& operator=(const AtsDssPlanMapAccessFactory &);


	public:

      
        /** 
         * ~AtsDssPlanMapAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~AtsDssPlanMapAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an AtsDssPlanMapAccessFactory object.
         */
		static AtsDssPlanMapAccessFactory& getInstance();


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
         * getAtsDssPlanMap
         *
         * Retrieves the specified AtsDssPlanMap and returns the associated data as an 
         * object conforming to the IAtsDssPlanMap interface.
         *
         * @param   idKey      the primary key/id of the AtsDssPlanMap to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigAtsDssPlanMap objects or set to false to return 
         *                     readonly IAtsDssPlanMap objects.
         *
         * @return  A pointer to an object conforming to the IAtsDssPlanMap interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IAtsDssPlanMap object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no AtsDssPlanMap matching the supplied key
         *          - if there is more than one AtsDssPlanMap matching the supplied key. 
         *
         */
		IAtsDssPlanMap* getAtsDssPlanMap(const unsigned long idKey, const bool readWrite = false);

        /**
         * getAllAtsDssPlanMaps
         *
         * Retrieves all AtsDssPlanMaps and returns the associated data as a vector of  
         * objects conforming to the IAtsDssPlanMap interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigAtsDssPlanMap objects or set to false to return 
         *                     readonly IAtsDssPlanMap objects.
         *
         * @return  a vector of pointers to objects conforming to the IAtsDssPlanMap interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IAtsDssPlanMap object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no AtsDssPlanMap matching the supplied key
         *          - if there is more than one AtsDssPlanMap matching the supplied key. 
         *
         */
        IAtsDssPlanMaps getAllAtsDssPlanMaps( const bool readWrite = false );


        /**
         * getAtsDssPlanMapsWhere (UNTESTED)
         *
         * Retrieves all AtsDssPlanMaps matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ATMSSZ_ID from AT_MSSZONE where 
         *                                   DSS_INPUT=<dssInput> and
		 *									 PLAN_PATH=<planPath>
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of pointers to objects conforming to the IAtsDssPlanMap interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IAtsDssPlanMap
         * objects
         *
         * @param   readWrite
         *          - true returns IConfigAtsDssPlanMap objects
         *          - false returns IAtsDssPlanMap objects
		 * @param   dssEvent
         *          - "where DSS_EVENT=<dssEvent>" clause.
         *          - specify -1 to omit.
		 * @param   planPath
         *          - "where PLAN_PATH=<planPath>" clause.
         *          - specify NULL to omit.
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IAtsDssPlanMaps getAtsDssPlanMapsWhere( const bool readWrite = false,
										  long dssEvent = -1,
										  const char* const planPath = NULL);
 
        /**
         * getAtsDssPlanMapIdsWhere (UNTESTED)
         *
         * Retrieves all AtsDssPlanMapIds (primary key) matching the specified where clauses.  
         * The factory will execute: 
         *
         * "select ATMSSZ_ID from AT_MSSZONE where 
         *                                   DSS_INPUT=<dssInput> and
		 *									 PLAN_PATH=<planPath>
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of AtsDssPlanMapIds (primary keys) 
         *
		 * @param   dssEvent
         *          - "where DSS_EVENT=<dssEvent>" clause.
         *          - specify -1 to omit.
		 * @param   planPath
         *          - "where PLAN_PATH=<planPath>" clause.
         *          - specify NULL to omit.
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        std::vector<int> getAtsDssPlanMapIdsWhere( long dssEvent = -1,
										  const char* const planPath = NULL);
        /**
         * getIndexedAtsDssPlanMaps
         *
         * Retrieves all AtsDssPlanMaps and returns the associated data indexed by DSS Event and Input
         *
         * @return  a pointer to a IndexedAtsDssPlanMaps object
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IndexedAtsDssPlanMaps object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no AtsDssPlanMap matching the supplied key
         *          - if there is more than one AtsDssPlanMap matching the supplied key. 
         *
         */
		IndexedAtsDssPlanMaps* getIndexedAtsDssPlanMaps();
       
        /**
         * createAtsDssPlanMap
         * 
         * Creates an empty IConfigAtsDssPlanMap object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigAtsDssPlanMap object.
         *
         * @return A pointer to an IConfigAtsDssPlanMap object
         *
         */
		IConfigAtsDssPlanMap* createAtsDssPlanMap();


        /**
         * copyAtsDssPlanMap
         * 
         * Creates a new configuration AtsDssPlanMap object using the AtsDssPlanMap passed in
         *
         * @param  AtsDssPlanMapToCopy the ConfigAtsDssPlanMap to copy
         *
         * @return the new object as a pointer to an IConfigAtsDssPlanMap object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigAtsDssPlanMap* copyAtsDssPlanMap(const IConfigAtsDssPlanMap* AtsDssPlanMapToCopy);


    private:
        static AtsDssPlanMapAccessFactory* m_instance;
	};

} // closes TA_Core

#endif // !defined(AtsDssPlanMapAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
