/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/AtsFixedBlockMapAccessFactory.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * AtsFixedBlockMapAccessFactory is a singleton that is used to retrieve AtsFixedBlockMap objects either from the
  * database or newly created. All AtsFixedBlockMap objects returned will adhear to the IAtsFixedBlockMap interface.
  */



#if !defined(AtsFixedBlockMapAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
#define AtsFixedBlockMapAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/ats/src/IAtsFixedBlockMap.h"
#include "core/data_access_interface/ats/src/IAtsFbMapRecord.h"
#include "core/data_access_interface/ats/src/IConfigAtsFbMapRecord.h"

namespace TA_IRS_Core
{
	class AtsFixedBlockMapAccessFactory
	{

	public:
        
        // public constants
        static const std::string CctvZoneType;
        static const std::string RadioZoneType;
        static const std::string EcsZoneType;

	public:
        virtual ~AtsFixedBlockMapAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an AtsFixedBlockMapAccessFactory object.
         */
		static AtsFixedBlockMapAccessFactory& getInstance();


        /**
         * getAtsFixedBlockMap
         *
         * Retrieves the map that is specific to the specified zone type
         *
         * @param zoneType the zone type
         *
         * @return A pointer to an object conforming to the IAtsFixedBlockMap interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IAtsFixedBlockMap 
         * object
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no AtsFixedBlockMap
         *            matching the supplied PKEY, or if there is more than one AtsFixedBlockMap
         *            matching the supplied PKEY. 
         */
        IAtsFixedBlockMap* getAtsFixedBlockMap(const std::string zoneType);

		/**
         * getAllAtsFixedBlockMaps
         *
         * Retrieves all the maps
         *
         * @return Avector of pointers to objects conforming to the IAtsFixedBlockMap interface.
         * N.B.: It is the responsibility of the *client* to delete the returned IAtsFixedBlockMap 
         * objects
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException A DataException will be thrown if there is no AtsFixedBlockMap
         *            matching the supplied PKEY, or if there is more than one AtsFixedBlockMap
         *            matching the supplied PKEY. 
         */
		std::vector<IAtsFixedBlockMap*> getAllAtsFixedBlockMaps();

		/**
         * getAllAtsFbMapRecords
         *
         * Retrieves all AtsFbMapRecords for a zone type and returns the associated data as a vector of  
         * objects conforming to the IAtsFbMapRecord interface.
         *
		 * @param	zoneType   The Zone Type to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigAtsFbMapRecord objects or set to false to return 
         *                     readonly IAtsFbMapRecord objects.
         *
         * @return  a vector of pointers to objects conforming to the IAtsFbMapRecord interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IAtsFbMapRecord object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no AtsFbMapRecord matching the supplied key
         *          - if there is more than one AtsFbMapRecord matching the supplied key. 
         *
         */
        IAtsFbMapRecords getAllAtsFbMapRecords( const bool readWrite = false );

		
		/**
         * createAtsFbMapRecord
         * 
         * Creates an empty IConfigAtsFbMapRecord object, and returns a pointer to it.
         * Note: It is the responsibility of the *client* to delete the returned  
         * IConfigAtsFbMapRecord object.
         *
         * @return A pointer to an IConfigAtsFbMapRecord object
         *
         */
		IConfigAtsFbMapRecord* createAtsFbMapRecord();


        /**
         * copyAtsFbMapRecord
         * 
         * Creates a new configuration AtsFbMapRecord object using the AtsFbMapRecord passed in
         *
         * @param  AtsFbMapRecordToCopy the ConfigAtsFbMapRecord to copy
         *
         * @return the new object as a pointer to an IConfigAtsFbMapRecord object.
         *
         * @exception DatabaseException 
         *            - if there is a database problem.
         */
        IConfigAtsFbMapRecord* copyAtsFbMapRecord(const IConfigAtsFbMapRecord* AtsFbMapRecordToCopy);


	protected:
		/**
         * getAllAtsFbMapRecordsForZoneType
         *
         * Retrieves all AtsFbMapRecords for a zone type and returns the associated data as a vector of  
         * objects conforming to the IAtsFbMapRecord interface.
         *
		 * @param	zoneType   The Zone Type to retrieve
         * @param   readWrite  (optional) set to true to return writeable
         *                     IConfigAtsFbMapRecord objects or set to false to return 
         *                     readonly IAtsFbMapRecord objects.
         *
         * @return  a vector of pointers to objects conforming to the IAtsFbMapRecord interface.
         *          Note: It is the responsibility of the *client* to delete the 
         *          returned IAtsFbMapRecord object
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possilble reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no AtsFbMapRecord matching the supplied key
         *          - if there is more than one AtsFbMapRecord matching the supplied key. 
         *
         */
        IAtsFbMapRecords getAllAtsFbMapRecordsForZoneType( const std::string& zoneType, const bool readWrite = false );

    private:
        //
        // These are private as this method is a singleton
        //
		AtsFixedBlockMapAccessFactory() { };
		AtsFixedBlockMapAccessFactory( const AtsFixedBlockMapAccessFactory& theAtsFixedBlockMapAccessFactory);
		AtsFixedBlockMapAccessFactory& operator=(const AtsFixedBlockMapAccessFactory &);

        static AtsFixedBlockMapAccessFactory* m_instance;
    };

} // closes TA_Core

#endif // !defined(AtsFixedBlockMapAccessFactory_A16AAC60_54BE_4756_B682_2C67E80990D6__INCLUDED_)
