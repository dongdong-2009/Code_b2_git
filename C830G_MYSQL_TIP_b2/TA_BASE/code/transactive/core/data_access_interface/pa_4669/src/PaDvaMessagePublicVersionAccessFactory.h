/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaDvaMessagePublicVersionAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * PaDvaMessagePublicVersionAccessFactory is a singleton that is used to retrieve PaDvaMessagePublicVersion objects either from the
  * database or newly created. All PaDvaMessagePublicVersion objects returned will adhere to the IPaDvaMessagePublicVersion interface.
  */



#if !defined PaDvaMessagePublicVersionAccessFactory_H
#define PaDvaMessagePublicVersionAccessFactory_H

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa_4669/src/IPaDvaMessagePublicVersion.h"
#include "core/data_access_interface/pa_4669/src/IConfigPaDvaMessagePublicVersion.h"



namespace TA_Base_Core
{

    

	class PaDvaMessagePublicVersionAccessFactory
	{


	private:
        
        // Disable the following methods
		PaDvaMessagePublicVersionAccessFactory() { };
		PaDvaMessagePublicVersionAccessFactory( const PaDvaMessagePublicVersionAccessFactory& thePaDvaMessagePublicVersionAccessFactory);
		PaDvaMessagePublicVersionAccessFactory& operator=(const PaDvaMessagePublicVersionAccessFactory &);


	public:
        
        
        /** 
         * ~PaDvaMessagePublicVersionAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaDvaMessagePublicVersionAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaDvaMessagePublicVersionAccessFactory object.
         */
		static PaDvaMessagePublicVersionAccessFactory& getInstance();


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
         * getPaDvaMessagePublicVersion
         *
         * Retrieves the specified PaDvaMessagePublicVersion and returns the associated data as an 
         * object conforming to the IPaDvaMessagePublicVersion interface.
         *
         * @param   key        the primary key/id of the PaDvaMessagePublicVersion to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessagePublicVersion objects or set to false to return 
         *                     readonly IPaDvaMessagePublicVersion objects.
         *
         * @return  A pointer to an object conforming to the IPaDvaMessagePublicVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessagePublicVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaDvaMessagePublicVersion matching the supplied key
         *          - if there is more than one PaDvaMessagePublicVersion matching the supplied key. 
         *
         */
		IPaDvaMessagePublicVersion* getPaDvaMessagePublicVersion(const unsigned long key, const bool readWrite);

        /**
         * getPaDvaMessagePublicVersionByLocationKey
         *
         * Retrieves the specified PaDvaMessagePublicVersion and returns the associated data as an 
         * object conforming to the IPaDvaMessagePublicVersion interface.
         *
         * @param   key        the locationKey of the PaDvaMessagePublicVersion to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessagePublicVersion objects or set to false to return 
         *                     readonly IPaDvaMessagePublicVersion objects.
         *
         * @return  A pointer to an object conforming to the IPaDvaMessagePublicVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessagePublicVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaDvaMessagePublicVersion matching the supplied key
         *          - if there is more than one PaDvaMessagePublicVersion matching the supplied key. 
         *
         */
		IPaDvaMessagePublicVersion* getPaDvaMessagePublicVersionByLocationKey(const unsigned long locationKey, const bool readWrite);

        /**
         * getAllPaDvaMessagePublicVersions
         *
         * Retrieves all PaDvaMessagePublicVersions and returns the associated data as a vector of  
         * objects conforming to the IPaDvaMessagePublicVersion interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessagePublicVersion objects or set to false to return 
         *                     readonly IPaDvaMessagePublicVersion objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaDvaMessagePublicVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessagePublicVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaDvaMessagePublicVersions getAllPaDvaMessagePublicVersions( const bool readWrite = false );
        

        /**
         * createPaDvaMessagePublicVersion
         * 
         * Creates an empty IConfigPaDvaMessagePublicVersion object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPaDvaMessagePublicVersion object.
         *         
         * @return A pointer to an IConfigPaDvaMessagePublicVersion object
         *
         */
		IConfigPaDvaMessagePublicVersion* createPaDvaMessagePublicVersion();


        /**
         * copyPaDvaMessagePublicVersion
         * 
         * Creates a new configuration PaDvaMessagePublicVersion object using the PaDvaMessagePublicVersion passed in
         *
         * @param  PaDvaMessagePublicVersionToCopy the ConfigPaDvaMessagePublicVersion to copy
         *
         * @return the new object as a pointer to an IConfigPaDvaMessagePublicVersion object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPaDvaMessagePublicVersion* copyPaDvaMessagePublicVersion(const IConfigPaDvaMessagePublicVersion* paDvaMessagePublicVersionGroupToCopy);


    private:

        static PaDvaMessagePublicVersionAccessFactory* m_instance;

        /**
         * getPaDvaMessagePublicVersionUsing
         *
         * Retrieves the specified PaDvaMessagePublicVersion and returns the associated data as an 
         * object conforming to the IPaDvaMessagePublicVersion interface.
         *
         * @param   colName    the name of the column to test for equality
         * @param   colValue   the value to test against the column
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessagePublicVersion objects or set to false to return 
         *                     readonly IPaDvaMessagePublicVersion objects.
         *
         * @return  A pointer to an object conforming to the IPaDvaMessagePublicVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessagePublicVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaDvaMessagePublicVersion matching the supplied key
         *          - if there is more than one PaDvaMessagePublicVersion matching the supplied key. 
         *
         */
        IPaDvaMessagePublicVersion* getPaDvaMessagePublicVersionUsing(const std::string& colName,
                                                            unsigned long colValue,
                                                            bool readWrite);


        /**
         * getPaDvaMessagePublicVersionsReturnedFromStatement
         *
         * Retrieves all PaDvaMessagePublicVersions that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the messages to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessagePublicVersion objects or set to false to return 
         *                     readonly IPaDvaMessagePublicVersion objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaDvaMessagePublicVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessagePublicVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaDvaMessagePublicVersions getPaDvaMessagePublicVersionsReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);
	};

} // closes TA_Base_Core

#endif // !defined PaDvaMessagePublicVersionAccessFactory_H
