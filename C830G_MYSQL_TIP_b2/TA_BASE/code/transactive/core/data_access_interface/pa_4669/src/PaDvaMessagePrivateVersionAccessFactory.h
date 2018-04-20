/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaDvaMessagePrivateVersionAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * PaDvaMessagePrivateVersionAccessFactory is a singleton that is used to retrieve PaDvaMessagePrivateVersion objects either from the
  * database or newly created. All PaDvaMessagePrivateVersion objects returned will adhere to the IPaDvaMessagePrivateVersion interface.
  */



#if !defined PaDvaMessagePrivateVersionAccessFactory_H
#define PaDvaMessagePrivateVersionAccessFactory_H

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/pa_4669/src/IPaDvaMessagePrivateVersion.h"
#include "core/data_access_interface/pa_4669/src/IConfigPaDvaMessagePrivateVersion.h"



namespace TA_Base_Core
{

    

	class PaDvaMessagePrivateVersionAccessFactory
	{


	private:
        
        // Disable the following methods
		PaDvaMessagePrivateVersionAccessFactory() { };
		PaDvaMessagePrivateVersionAccessFactory( const PaDvaMessagePrivateVersionAccessFactory& thePaDvaMessagePrivateVersionAccessFactory);
		PaDvaMessagePrivateVersionAccessFactory& operator=(const PaDvaMessagePrivateVersionAccessFactory &);


	public:
        
        
        /** 
         * ~PaDvaMessagePrivateVersionAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaDvaMessagePrivateVersionAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an PaDvaMessagePrivateVersionAccessFactory object.
         */
		static PaDvaMessagePrivateVersionAccessFactory& getInstance();


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
         * getPaDvaMessagePrivateVersion
         *
         * Retrieves the specified PaDvaMessagePrivateVersion and returns the associated data as an 
         * object conforming to the IPaDvaMessagePrivateVersion interface.
         *
         * @param   key        the primary key/id of the PaDvaMessagePrivateVersion to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessagePrivateVersion objects or set to false to return 
         *                     readonly IPaDvaMessagePrivateVersion objects.
         *
         * @return  A pointer to an object conforming to the IPaDvaMessagePrivateVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessagePrivateVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaDvaMessagePrivateVersion matching the supplied key
         *          - if there is more than one PaDvaMessagePrivateVersion matching the supplied key. 
         *
         */
		IPaDvaMessagePrivateVersion* getPaDvaMessagePrivateVersion(const unsigned long key, const bool readWrite);

        /**
         * getPaDvaMessagePrivateVersionByLocationKey
         *
         * Retrieves the specified PaDvaMessagePrivateVersion and returns the associated data as an 
         * object conforming to the IPaDvaMessagePrivateVersion interface.
         *
         * @param   key        the locationKey of the PaDvaMessagePrivateVersion to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessagePrivateVersion objects or set to false to return 
         *                     readonly IPaDvaMessagePrivateVersion objects.
         *
         * @return  A pointer to an object conforming to the IPaDvaMessagePrivateVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessagePrivateVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaDvaMessagePrivateVersion matching the supplied key
         *          - if there is more than one PaDvaMessagePrivateVersion matching the supplied key. 
         *
         */
		IPaDvaMessagePrivateVersion* getPaDvaMessagePrivateVersionByLocationKey(const unsigned long locationKey, const bool readWrite);

        /**
         * getAllPaDvaMessagePrivateVersions
         *
         * Retrieves all PaDvaMessagePrivateVersions and returns the associated data as a vector of  
         * objects conforming to the IPaDvaMessagePrivateVersion interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessagePrivateVersion objects or set to false to return 
         *                     readonly IPaDvaMessagePrivateVersion objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaDvaMessagePrivateVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessagePrivateVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaDvaMessagePrivateVersions getAllPaDvaMessagePrivateVersions( const bool readWrite = false );
        

        /**
         * createPaDvaMessagePrivateVersion
         * 
         * Creates an empty IConfigPaDvaMessagePrivateVersion object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigPaDvaMessagePrivateVersion object.
         *         
         * @return A pointer to an IConfigPaDvaMessagePrivateVersion object
         *
         */
		IConfigPaDvaMessagePrivateVersion* createPaDvaMessagePrivateVersion();


        /**
         * copyPaDvaMessagePrivateVersion
         * 
         * Creates a new configuration PaDvaMessagePrivateVersion object using the PaDvaMessagePrivateVersion passed in
         *
         * @param  PaDvaMessagePrivateVersionToCopy the ConfigPaDvaMessagePrivateVersion to copy
         *
         * @return the new object as a pointer to an IConfigPaDvaMessagePrivateVersion object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
       IConfigPaDvaMessagePrivateVersion* copyPaDvaMessagePrivateVersion(const IConfigPaDvaMessagePrivateVersion* paDvaMessagePrivateVersionGroupToCopy);


    private:

        static PaDvaMessagePrivateVersionAccessFactory* m_instance;

        /**
         * getPaDvaMessagePrivateVersionUsing
         *
         * Retrieves the specified PaDvaMessagePrivateVersion and returns the associated data as an 
         * object conforming to the IPaDvaMessagePrivateVersion interface.
         *
         * @param   colName    the name of the column to test for equality
         * @param   colValue   the value to test against the column
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessagePrivateVersion objects or set to false to return 
         *                     readonly IPaDvaMessagePrivateVersion objects.
         *
         * @return  A pointer to an object conforming to the IPaDvaMessagePrivateVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessagePrivateVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no PaDvaMessagePrivateVersion matching the supplied key
         *          - if there is more than one PaDvaMessagePrivateVersion matching the supplied key. 
         *
         */
        IPaDvaMessagePrivateVersion* getPaDvaMessagePrivateVersionUsing(const std::string& colName,
                                                            unsigned long colValue,
                                                            bool readWrite);


        /**
         * getPaDvaMessagePrivateVersionsReturnedFromStatement
         *
         * Retrieves all PaDvaMessagePrivateVersions that are filtered by input clause, should be
         * of the form 'WHERE LOCATIONKEY = 1' or empty string if want to return all.  The whereclause
         * is simply appended to the basic select from statement (although a space is automatically inserted)
         *
         * @param whereClause the sql statements used to define the messages to return, no leading space required
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigPaDvaMessagePrivateVersion objects or set to false to return 
         *                     readonly IPaDvaMessagePrivateVersion objects.
         *
         * @return  a vector of pointers to objects conforming to the IPaDvaMessagePrivateVersion interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IPaDvaMessagePrivateVersion object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         *
         */
        IPaDvaMessagePrivateVersions getPaDvaMessagePrivateVersionsReturnedFromStatement(const std::string& whereClause = "", const bool readWrite = false);
	};

} // closes TA_Base_Core

#endif // !defined PaDvaMessagePrivateVersionAccessFactory_H
