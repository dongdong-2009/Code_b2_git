/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/AtsBranchTrackAccessFactory.h $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * AtsBranchTrackAccessFactory is a singleton that is used to retrieve AtsBranchTrack objects either from the
  * database or newly created. All AtsBranchTrack objects returned will adhear to the IAtsBranchTrack interface.
  */



#if !defined(AtsBranchTrackAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define AtsBranchTrackAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/ats/src/IAtsBranchTrack.h"
#include "core/data_access_interface/ats/src/IConfigAtsBranchTrack.h"




namespace TA_IRS_Core
{

	class AtsBranchTrackAccessFactory
	{


	private:

        // Disable the following methods
		AtsBranchTrackAccessFactory() { };
		AtsBranchTrackAccessFactory( const AtsBranchTrackAccessFactory& theAtsBranchTrackAccessFactory);
		AtsBranchTrackAccessFactory& operator=(const AtsBranchTrackAccessFactory &);


	public:

      
        /** 
         * ~AtsBranchTrackAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~AtsBranchTrackAccessFactory() {};


        /**
         * getInstance
         *
         * Creates and returns an instance of this singleton object.
         *
         * @return  a reference to an instance of an AtsBranchTrackAccessFactory object.
         */
		static AtsBranchTrackAccessFactory& getInstance();


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
         * getAtsBranchTrack
         *
         * Retrieves the specified AtsBranchTrack and returns the associated data as an 
         * object conforming to the IAtsBranchTrack interface.
         *
         * @param   idKey      the primary key/id of the AtsBranchTrack to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigAtsBranchTrack objects or set to false to return 
         *                     readonly IAtsBranchTrack objects.
         *
         * @return  A pointer to an object conforming to the IAtsBranchTrack interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IAtsBranchTrack object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no AtsBranchTrack matching the supplied key
         *          - if there is more than one AtsBranchTrack matching the supplied key. 
         *
         */
		IAtsBranchTrack* getAtsBranchTrack(const unsigned long idKey, const bool readWrite = false);

        /**
         * getAllAtsBranchTracks
         *
         * Retrieves all AtsBranchTracks and returns the associated data as a vector of  
         * objects conforming to the IAtsBranchTrack interface.
         *
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigAtsBranchTrack objects or set to false to return 
         *                     readonly IAtsBranchTrack objects.
         *
         * @return  a vector of pointers to objects conforming to the IAtsBranchTrack interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IAtsBranchTrack object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no AtsBranchTrack matching the supplied key
         *          - if there is more than one AtsBranchTrack matching the supplied key. 
         *
         */
        IAtsBranchTracks getAllAtsBranchTracks( const bool readWrite = false );


        /**
         * getAtsBranchTracksWhere (UNTESTED)
         *
         * Retrieves all AtsBranchTracks matching the specified where clauses.  The factory will
         * execute: 
         *
         * "select ATBTRA_ID from AT_BRANCH_TRACK where 
         *                                   TRACK=<track> 
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of pointers to objects conforming to the IAtsBranchTrack interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IAtsBranchTrack
         * objects
         *
         * @param   readWrite
         *          - true returns IConfigAtsBranchTrack objects
         *          - false returns IAtsBranchTrack objects
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
        IAtsBranchTracks getAtsBranchTracksWhere( const bool readWrite = false,
                                    const std::string& track="");
 
        /**
         * getAtsBranchTrackIdsWhere (UNTESTED)
         *
         * Retrieves all AtsBranchTrackIds (primary key) matching the specified where clauses.  
         * The factory will execute: 
         *
         * "select ATBTRA_ID from AT_BRANCH_TRACK where  
         *                                   TRACK=<track>"
         *
         * If a particular clause is not required, specify with a null for strings or -1 for entityKeys.  
         *
         * @return A vector of AtsBranchTrackIds (primary keys) 
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
        std::vector<int> getAtsBranchTrackIdsWhere( const std::string& track="" );

       
        /**
         * createAtsBranchTrack
         * 
         * Creates an empty IConfigAtsBranchTrack object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigAtsBranchTrack object.
         *
         * @return A pointer to an IConfigAtsBranchTrack object
         *
         */
		IConfigAtsBranchTrack* createAtsBranchTrack();


        /**
         * copyAtsBranchTrack
         * 
         * Creates a new configuration AtsBranchTrack object using the AtsBranchTrack passed in
         *
         * @param  AtsBranchTrackToCopy the ConfigAtsBranchTrack to copy
         *
         * @return the new object as a pointer to an IConfigAtsBranchTrack object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigAtsBranchTrack* copyAtsBranchTrack(const IConfigAtsBranchTrack* AtsBranchTrackToCopy);


    private:
        static AtsBranchTrackAccessFactory* m_instance;
	};

} // closes TA_Core

#endif // !defined(AtsBranchTrackAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
