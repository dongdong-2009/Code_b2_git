/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/mms_dai/src/MmSchedulingAccessFactory.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * MmSchedulingAccessFactory is a singleton that is used to retrieve 
  * IMmSchedulingData objects from the database. 
  */

#if !defined(MM_SCHEDULING_ACCESSFACTORY_H)
#define MM_SCHEDULING_ACCESSFACTORY_H

#include <vector>

#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"


namespace TA_IRS_Core
{

    // Forward delaration to help the compiler avoid unnecessary compilation
    class IMmSchedulingData;
    class IConfigMmScheduling;



    class MmSchedulingAccessFactory : public virtual TA_Base_Core::IOnlineUpdatable
    {
    public:

        virtual ~MmSchedulingAccessFactory();


        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return A reference to an instance of an MmSchedulingAccessFactory object.
         */
		static MmSchedulingAccessFactory& getInstance();
      
        /**
         * getAllRecords
         *
         * Retrieves all the records in the database, returns cached data if present
         *      from a previous call (that will be up to date, as this class listens out
         *      for online updates).  Interleaving calls with readWrite true then false
         *      will cause sub-optimal behaviour for the cache mechanism (cache is reset
         *      each time a different readWrite parameter comes through).
         *
         * @exception DatabaseException A DatabaseException will be thrown if there are
         *            any problems in communicating with the database. The possilble 
         *            reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @exception DataException 
         */
        std::vector<IMmSchedulingData*> getAllRecords( const bool readWrite = false );

		std::string retrieveSubName(unsigned long entityKey);
		
        /**
         * cleanUp
         * 
         * Cancels online update listener subscriptions and destroys singleton instance
         *  of the factory.
         * Should call this before shutting down core subsystems.  This will ensure that
         * it closes all connections to the online update library, and destroys
         *  all cached data.
         */
        void cleanUp();

        /**
         * processUpdate
         *
         * When there is a configuration update of an mms schedule then this method
         * will be invoked to process the update accordingly.
         *
         * @param ConfigUpdateDetails& - The update details so we know what to modify
         */
        virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);
        
        /**
         * @return true if the last set of *configurable* data returned from 
         * getAllRecords has changed due to receipt of an online update -
         * meaning operator will need to make another call to getAllRecords to fetch
         *  the latest data.
         */
        bool hasChanged();

        /**
         * invalidateCache will force the next call to getAllRecords to fetch data directly
         *  from the database (all cached data will be cleared and refreshed)
         */
        void invalidateCache();
		
		/**
		 * @registers the specified callback so that it receives update notifications
		 *		when the data changes - user must have made a call to getAllRecords
		 *		in order to initialise the listener
		 * Any exceptions thrown from the callback function will be caught and logged
		 * @return false if this callback is already registered
		 */
		bool registerInterest(TA_Base_Core::IOnlineUpdatable& callback);

		/**
		 * deregisters a callback previously registered with registerInterest
		 * @return false if the specified callback wasn't recognized (not registered)
		 */
		bool deregisterInterest(TA_Base_Core::IOnlineUpdatable& callback);

        /**
         * @return true if this class is currently listening for online updates
         *  (will automatically attempt to listen out for online updates from within
         *  call to getAllRecords - but upon failure this function will continue to return false)
         */
        bool isListening();

    private:
        
        typedef std::vector<IMmSchedulingData*> ScheduleList;

		static MmSchedulingAccessFactory* s_instance;
        //
        // These are private as this method is a singleton
        //
		MmSchedulingAccessFactory();
		MmSchedulingAccessFactory( const MmSchedulingAccessFactory& theMmSchedulingAccessFactory);
		MmSchedulingAccessFactory& operator=(const MmSchedulingAccessFactory &) { };
 
        /**
         * Deletes the contents of the cache
         */
        void clearCache();
    
        /**
         * Called internally to refresh contents of cache with specified data
         *
         * @param data the data to cache
         * @param readWrite indicates the type of data being cached (configurable or otherwise)
         */
        void cacheConfigurableData(std::vector<IMmSchedulingData*> data, bool readWrite);

		/**
		 * Internal handler (helper) function for processing processUpdate notifications
		 */
		void handleUpdateNotification(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        // The MMS Access Factory caches all objects locally, and listens out for
        // updates in order to keep them up to date - but only for configurable objects
        bool    m_schedulesLoaded;
        bool    m_readWriteCache;
	    ScheduleList m_schedules;
        /// This is set to force a refresh of the cached data on the next call to getAllRecords
        /// (in the event that we can't update the cache properly and need to do a full refresh)
        bool    m_forceRefresh;

        // Queues online update listener notifications while loading data
        TA_Base_Core::ReEntrantThreadLockable m_listenerLock;
        bool    m_hasCacheChanged;

		typedef std::vector<TA_Base_Core::IOnlineUpdatable*> UpdateCallbackList;
		UpdateCallbackList m_callbacks;
    };

} // end namespace TA_IRS_Core

   
#endif // MM_SCHEDULING_ACCESSFACTORY_H

