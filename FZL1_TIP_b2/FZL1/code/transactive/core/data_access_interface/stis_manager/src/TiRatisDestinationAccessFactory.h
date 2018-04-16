/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/stis_manager/src/TiRatisDestinationAccessFactory.h $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * TiRatisDestinationAccessFactory is a singleton that is used to retrieve TiRatisDestination objects either from the
  * database or newly created. All TiRatisDestination objects returned will adhere to the ITiRatisDestination interface.
  */


#ifndef TI_RATIS_DESTIONATION_ACCESS_FACTORY_H_INCLUDED
#define TI_RATIS_DESTIONATION_ACCESS_FACTORY_H_INCLUDED


#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/stis_manager/src/ITiRatisDestination.h"
#include "core/data_access_interface/stis_manager/src/IConfigTiRatisDestination.h"


namespace TA_IRS_Core
{

	class TiRatisDestinationAccessFactory
	{
    public:
    
	private:
        
        // Disable the following methods
		TiRatisDestinationAccessFactory() { };

		TiRatisDestinationAccessFactory( const TiRatisDestinationAccessFactory& theTiRatisDestinationAccessFactory);

		TiRatisDestinationAccessFactory& operator=(const TiRatisDestinationAccessFactory &);


	public:
        
        
        /** 
         * ~TiRatisDestinationAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~TiRatisDestinationAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an TiRatisDestinationAccessFactory object.
         */
		static TiRatisDestinationAccessFactory& getInstance();


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
         * getTiRatisDestination
         *
         * Retrieves the specified TiRatisDestination and returns the associated data as an 
         * object conforming to the ITiRatisDestination interface.
         *
         * @param   key        the primary key/id of the TiRatisDestination to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigTiRatisDestination objects or set to false to return 
         *                     readonly ITiRatisDestination objects.
         *
         * @return  A pointer to an object conforming to the ITiRatisDestination interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITiRatisDestination object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no TiRatisDestination matching the supplied key
         *          - if there is more than one TiRatisDestination matching the supplied key. 
         *
         */
		ITiRatisDestination* getTiRatisDestination(const unsigned long key, const bool readWrite);


        /**
         * getAllTiRatisDestinations
         *
         * Retrieves all TiRatisDestinations and returns the associated data as a vector of  
         * objects conforming to the ITiRatisDestination interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigTiRatisDestination objects or set to false to return 
         *                     readonly ITiRatisDestination objects.
         *
         * @return  a vector of pointers to objects conforming to the ITiRatisDestination interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITiRatisDestination object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no TiRatisDestination matching the supplied key
         *          - if there is more than one TiRatisDestination matching the supplied key. 
         *
         */
        ITiRatisDestinations getAllTiRatisDestinations( const bool readWrite = false );

		void setDestination(std::string newDestination);


        /**
         * createTiRatisDestination
         * 
         * Creates an empty IConfigTiRatisDestination object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigTiRatisDestination object.
         *
         * @return A pointer to an IConfigTiRatisDestination object
         *
         */
		IConfigTiRatisDestination* createTiRatisDestination();


        /**
         * copyTiRatisDestination
         * 
         * Creates a new configuration TiRatisDestination object using the TiRatisDestination passed in
         *
         * @param  TiRatisDestinationToCopy the ConfigTiRatisDestination to copy
         *
         * @return the new object as a pointer to an IConfigTiRatisDestination object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigTiRatisDestination* copyTiRatisDestination(const IConfigTiRatisDestination* TiRatisDestinationToCopy);
        

    private:

        static TiRatisDestinationAccessFactory* m_instance;

        /**
         * getTiRatisDestinationsReturnedFromStatement
         *
         * Retrieves all TiRatisDestinations that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the TiRatisDestinations to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigTiRatisDestination objects or set to false to return 
         *                     readonly ITiRatisDestination objects.
         *
         * @return  a vector of pointers to objects conforming to the ITiRatisDestination interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITiRatisDestination object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        ITiRatisDestinations getTiRatisDestinationsReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);

	};

} // closes TA_IRS_Core

#endif // !defined(TiRatisDestinationAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
