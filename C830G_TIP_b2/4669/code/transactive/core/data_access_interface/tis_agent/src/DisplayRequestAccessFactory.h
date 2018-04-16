#ifndef DISPLAY_REQUEST_ACCESS_FACTORY_H_INCLUDED
#define DISPLAY_REQUEST_ACCESS_FACTORY_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * DisplayAccessFactory is a singleton that is used to generate and retrieve 
  * IDisplayRequest objects.
  */

#include <string>
#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"

/*
namespace TA_IRS_Core
{
   class IDisplayLog;    // Forward declaration.
} */

namespace TA_IRS_Core
{
    
    class DisplayRequestAccessFactory
    {
    public:

        /**
         * ~TrainTimeScheduleAccessFactory
         *
         * Standard destructor.
         */

        virtual ~DisplayRequestAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      DisplayRequestAccessFactory&
         *              A reference to an instance of a DisplayRequestAccessFactory object.
         */

		static DisplayRequestAccessFactory& getInstance();


        /**
         * getDisplayRequests
         *
         * Retrieves all display requests in the ti_display_log table
         *
         * @return      std::vector<IDisplayRequest*>
         *              A vector of pointers to objects conforming to the IDisplayRequest interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

		std::vector<IDisplayRequest*> getDisplayRequests();
        void bulkLoadDisplayRequest( const std::vector<IDisplayRequest*>& requests );


        /** 
          * getDisplayRequestsGreaterThan
          *
          * Gets all the display requests logged since the given key.
          *
          * @param key  The key of the latest loaded item from the last load
          *
          * @return All records since key
          */
        std::vector<IDisplayRequest*> getDisplayRequestsGreaterThan(time_t time);


        /**
         * getPredefinedMessageLibrary
         *
         * Retrieves the messages librariy in the ti_display_request table
         * with the specified version and type
         *
         * @return      IPredefinedMessageLibrary*
         *              A pointer to an object conforming to the IPredefinedMessageLibrary interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */
		IDisplayRequest* getDisplayRequest( const std::string& timestamp );


		IDisplayRequest* createNewDisplayRequest();


        /** 
          * addPidsToExistingTrainDisplayRequest
          *
          * Adds PIDs to existing TTIS display requests
          *
          * @param key  The display request pkey
          * @param pids The pids to add. space separated values
          *             the enumerated type converted to a string
          *             No leading or trailing spaces.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        void addPidsToExistingTrainDisplayRequest(std::string key, const std::string& pids);


    private:
        //
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        //
        DisplayRequestAccessFactory() {};
        DisplayRequestAccessFactory& operator=(const DisplayRequestAccessFactory &) {};
        DisplayRequestAccessFactory( const DisplayRequestAccessFactory& ) {};

		std::string getLocalDatabaseName();

        /** 
          * getPredefinedMessageLibrariesWhere
          *
          * gets the pre-defined message libraries based on some criteria
          *
          * @param whereSQL an SQL where clause.
          *
          * @return the pre-defined message libraries retrieved.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        std::vector<IDisplayRequest*> getDisplayRequestsWhere( std::string whereSQL = "" );

		std::string timetToString(time_t time);

		static DisplayRequestAccessFactory* m_theClassInstance;

		//
        // Thread lock to protect singleton creation.
        //
        
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

		std::string m_localDatabase;
    };

} // end namespace TA_IRS_Core

#endif
