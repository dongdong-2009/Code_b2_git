/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/SubsystemAccessFactory.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * SubsystemAccessFactory is a singleton that is used to retrieve Subsystem objects either from the
  * database or newly created. All Subsystem objects returned will adhear to the ISubsystem interface.
  */



#if !defined(SubsystemAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define SubsystemAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <vector>

#include "core/data_access_interface/src/ConfigSubsystem.h"
#include "core/data_access_interface/src/IConfigSubsystem.h"


namespace TA_Base_Core
{
    class ISubsystem;

	class SubsystemAccessFactory
	{

	public:
        virtual ~SubsystemAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an SubsystemAccessFactory object.
         */
		static SubsystemAccessFactory& getInstance();

		void getSubsystemBySql(const std::string& sql, std::vector<ISubsystem*>& subsystems, const bool readWrite);

        /**
         * getAllSubsystems
         *
         * Retrieves all specified Subsystems and returns the associated data as a vector of  
         * objects conforming to the ISubsystem interface.
         *
         * @return A vector of pointers to objects conforming to the ISubsystem interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISubsystem 
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<ISubsystem*> getAllSubsystems( const bool readWrite = false );


        /**
         * getAllPhysicalSubsystems
         *
         * Retrieves all physical Subsystems and returns the associated data as a vector of  
         * objects conforming to the ISubsystem interface.
         *
         * @return A vector of pointers to objects conforming to the ISubsystem interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISubsystem 
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<ISubsystem*> getAllPhysicalSubsystems( const bool readWrite = false );


        /** 
          * getSubsystemByKey
          *
          * Get a subsystem based on the pkey
          *
          * @return ISubsystem* the subsystem
          *
          * @param unsigned long key
		  *
          * @exception DatabaseException A DatabaseException will be thrown if there are
          *            any problems in communicating with the database. The possilble 
          *            reasons are:
          *            1) Invalid connection string (as stored in RunParams)
          *            2) Database is not/cannot be opened
          *            3) Error while attempting to execute a database query
          * @exception DataException 
          */
		ISubsystem* getSubsystemByKey( const unsigned long key, const bool readWrite = false );


        /**
         * getSubsystemsAtLocation
         *
         * Retrieves all Subsystems associated with a given region. Returns the data
		 * as a vector of objects conforming to the ISubsystem interface.
         *
         * @return A vector of pointers to objects conforming to the ISubsystem interface.
         * N.B.: It is the responsibility of the *client* to delete the returned ISubsystem 
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<ISubsystem*> getSubsystemsByRegion( unsigned long regionKey,
			const bool readWrite = false );


        /**
         * createSubsystem
         * 
         * Creates a new configuration subsystem object.
         *
         * @return The configuration subsystem object as a pointer to an
         *         IConfigSubsystem object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         * @exception DataException Thrown if there is bad data.
         */
        IConfigSubsystem* createSubsystem();


        /**
         * copySubsystem
         * 
         * Creates a new configuration subsystem object using
         * the subsystem passed in
         *
         * @param IConfigSubsystem* The subsystem to copy
         *
         * @return The configuration subsystem object as a pointer
         *         to an IConfigSubsystem object.
         *
         * @exception DatabaseException Thrown if there is a database problem.
         */
        IConfigSubsystem* copySubsystem(const IConfigSubsystem* subsystemToCopy);


    private:
        //
        // These are private as this method is a singleton
        //
		SubsystemAccessFactory() {};
		SubsystemAccessFactory( const SubsystemAccessFactory& theSubsystemAccessFactory);
		SubsystemAccessFactory& operator=(const SubsystemAccessFactory &) { };

		static SubsystemAccessFactory* m_instance;
    };

} // closes TA_Base_Core

#endif // !defined(SubsystemAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
