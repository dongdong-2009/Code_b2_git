/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/stis_manager/src/TIRatisTagAccessFactory.h $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * TiRatisTagAccessFactory is a singleton that is used to retrieve TiRatisTag objects either from the
  * database or newly created. All TiRatisTag objects returned will adhere to the ITiRatisTag interface.
  */


#ifndef TI_RATIS_TAG_ACCESS_FACTORY_H_INCLUDED
#define TI_RATIS_TAG_ACCESS_FACTORY_H_INCLUDED


#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/stis_manager/src/ITiRatisTag.h"
#include "core/data_access_interface/stis_manager/src/IConfigTiRatisTag.h"


namespace TA_IRS_Core
{

	class TiRatisTagAccessFactory
	{
    public:
    
	private:
        
        // Disable the following methods
		TiRatisTagAccessFactory() { };
		TiRatisTagAccessFactory( const TiRatisTagAccessFactory& theTiRatisTagAccessFactory);
		TiRatisTagAccessFactory& operator=(const TiRatisTagAccessFactory &);


	public:
        
        
        /** 
         * ~TiRatisTagAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~TiRatisTagAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an TiRatisTagAccessFactory object.
         */
		static TiRatisTagAccessFactory& getInstance();


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
         * getTiRatisTag
         *
         * Retrieves the specified TiRatisTag and returns the associated data as an 
         * object conforming to the ITiRatisTag interface.
         *
         * @param   key        the primary key/id of the TiRatisTag to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigTiRatisTag objects or set to false to return 
         *                     readonly ITiRatisTag objects.
         *
         * @return  A pointer to an object conforming to the ITiRatisTag interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITiRatisTag object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no TiRatisTag matching the supplied key
         *          - if there is more than one TiRatisTag matching the supplied key. 
         *
         */
		ITiRatisTag* getTiRatisTag(const unsigned long key, const bool readWrite);


        /**
         * getAllTiRatisTags
         *
         * Retrieves all TiRatisTags and returns the associated data as a vector of  
         * objects conforming to the ITiRatisTag interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigTiRatisTag objects or set to false to return 
         *                     readonly ITiRatisTag objects.
         *
         * @return  a vector of pointers to objects conforming to the ITiRatisTag interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITiRatisTag object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no TiRatisTag matching the supplied key
         *          - if there is more than one TiRatisTag matching the supplied key. 
         *
         */
        ITiRatisTags getAllTiRatisTags( const bool readWrite = false );


        /**
         * createTiRatisTag
         * 
         * Creates an empty IConfigTiRatisTag object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigTiRatisTag object.
         *
         * @return A pointer to an IConfigTiRatisTag object
         *
         */
		IConfigTiRatisTag* createTiRatisTag();


        /**
         * copyTiRatisTag
         * 
         * Creates a new configuration TiRatisTag object using the TiRatisTag passed in
         *
         * @param  TiRatisTagToCopy the ConfigTiRatisTag to copy
         *
         * @return the new object as a pointer to an IConfigTiRatisTag object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigTiRatisTag* copyTiRatisTag(const IConfigTiRatisTag* TiRatisTagToCopy);
        

    private:

        static TiRatisTagAccessFactory* m_instance;

        /**
         * getTiRatisTagsReturnedFromStatement
         *
         * Retrieves all TiRatisTags that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the TiRatisTags to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigTiRatisTag objects or set to false to return 
         *                     readonly ITiRatisTag objects.
         *
         * @return  a vector of pointers to objects conforming to the ITiRatisTag interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned ITiRatisTag object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        ITiRatisTags getTiRatisTagsReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);

	};

} // closes TA_IRS_Core

#endif // !defined(TiRatisTagAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
