/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/bus/pa/cachedmaps/src/GenericCachedMap.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Generic base class for <TABLE>CachedMap classes
  * This class automatically managed the local cache copy by fetching
  * from database initially, then listening for ConfigEditor updates
  * and modifying the cache as required (calling any registered observers
  * with the update notification information)
  *
  *     Try not to include this file from within other headers (except
  *         of course for classes derived from this class) - otherwise
  *         there is a high chance of a fatal C1076 compiler error
  *
  */

#if !defined(GenericCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define GenericCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

#include <list>
#include <map>

#include "bus/pa/cachedmaps/src/GenericCachedMapBase.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/pa/common/src/LockedPacket.h"
#include "bus/pa/cachedmaps/src/ICachedMapFilter.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/PasExceptions.h"


namespace TA_Base_Core
{
    enum EObjectType;
}

namespace TA_IRS_Bus
{
    class GenericCachedMapTestCases;

    template <class DbRecordType, class DaiRecordInterface>
    class GenericCachedMap : public GenericCachedMapBase
    {
        // Unit test class
        friend class GenericCachedMapTestCases;
    public:
    
        typedef std::vector<DaiRecordInterface*> DaiRecordInterfaceSet;

        typedef std::list<DbRecordType>                 DbRecordTypeSet;
        typedef typename DbRecordTypeSet::iterator      DbRecordTypeSetIt;

        /**
         * GenericCachedMap (constructor)
         *
         * @see GenericCachedMapBase for more details
         *
         */
        GenericCachedMap(const TA_Base_Core::EObjectType& databaseUpdateListenerType,
                         const std::string& tableName);

        virtual ~GenericCachedMap() {}


        /** 
         * getCachedDbRecordFromKey
         *
         * Returns the record associated with this key.
         *
         * pre: refreshKeyToRecordMapUsing() has been called successfully.
         *      or cached map has been manually initialised in conjunction with flagAsInitialised
         *
         * @param  key the primary key of the record.
         *
         * @return the specified record
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases.
         *
         */
	     DbRecordType getCachedDbRecordFromKey(unsigned long key);
                //throw(TA_Base_Core::CachedMappingNotFoundException);

        /** 
         * getConfigurableCachedDbRecordFromKey
         *
         * Returns the record associated with this key (which is editable)
         *      all modifications will be immediately reflect in the map
         *      hence the internal map is locked for lifetime of returned object
         *      - The destructor must be called on the returned object
         *              before internal map is unlocked
         *
         * pre: refreshKeyToRecordMapUsing() has been called successfully.
         *
         * @param  key the primary key of the record.
         *
         * @return the specified record
         *
         * @exception CachedMappingNotFoundException if the mapping could not be found.  
         *         This indicates invalid configuration in the databases.
         *
         */
	     LockedPacket<DbRecordType> 
             getConfigurableCachedDbRecordFromKey(unsigned long key);
                //throw(TA_Base_Core::CachedMappingNotFoundException);

         /**
          * getAllRecords
          *
          * @param filter (optional) to only return records matching the
          *         input filter (filter->isMatch will be called on each element)
          *         If null, all records are returned
          *
          * @param breakOnFirstMatch (optional) set to true if the function should
          *         return after finding the first matching record (returning
          *         a set of records no greater than one) - provided for efficiency reasons
          *
          * @return all the records contained in the cached map matching
          *         the input filter (if provided)
          *
          * pre: refreshKeyToRecordMapUsing() has been called successfully.
          *
          */
        DbRecordTypeSet getAllRecords(const ICachedMapFilter<DbRecordType>* filter = 0,
                                        bool breakOnFirstMatch = false);

    
        /**
         * getFirstMatchingRecord
         *
         * Passthrough function to getAllRecords, except a special optional flag is set
         *  to break out of the record collection as soon as the first filtered
         *  condition is met
         *
         * @see getAllRecords
         *
         */
        DbRecordTypeSet getFirstMatchingRecord(const ICachedMapFilter<DbRecordType>& filter);
    

        /**
         * updateRecordMapWithItem
         *
         * Updates the key to record map with the input item
         * The record may or may not exist in the map currently (if it doesn't, it's
         *  inserted).
         *
         * @param pKey the primary key identifying the insertion position into map
         *
         * @record the data to insert into the record (accessed by pKey)
         *
         * @return true if actual changes occurred to the element in the cached map as
         *       a result of this operation (false if the record was the same, and no changes made)     *

         * @exception InvalidPasConfigurationException if the record is not valid
         *              (as defined by validateRecordData)
         * 
         */
        bool    updateRecordMapWithItem(unsigned long pKey, const DbRecordType& record);
	    //throw (TA_Base_Core::InvalidPasConfigurationException);


         /**
          * clearKeyToRecordMap
          *
          * Clears all entries in the map
          *
          */
         void    clearKeyToRecordMap();

         /**
          * Called when external client code wants to initialise the cached map
          *
          */
         void    flagAsInitialised() { m_isKeyToRecordMapInitialised = true; }

    protected:
    
        /**
         * getPrimaryKeyArray (interface implementation)
         *
         * @see GenericCachedMapBase::getPrimaryKeyArray for more details
         *
         */
        std::vector<unsigned long> getPrimaryKeyArray();

        /** 
         * refreshKeyToRecordMapUsing
         *
         * Takes the input array of records and populates the record map, using the primary
         *  key value for each record (as the key)
         *
         * @param  records the array of records to add to the map (existing items in map will be cleared)
         *
         * @return the number of records that failed to load, generally this means
         *              the record(s) in question had an invalid format
         *
         */
        unsigned long refreshKeyToRecordMapUsing(DaiRecordInterfaceSet& records) ;
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException);


        /**
         * getRecordFromDatabaseUsingKey
         * 
         * @return the record from the database - it is the responsibility of the client
         *      code to delete the returned point after finished with it
         *
         * @param key the primary key used to get the item from database
         * 
         * @param readWrite false to return a readonly record, true for writable
         *
         * @throws  DatabaseException 
         *          - if there are any problems in communicating with the database.
         *            The possible reasons are:
         *            1) Invalid connection string (as stored in RunParams)
         *            2) Database is not/cannot be opened
         *            3) Error while attempting to execute a database query
         * @throws  DataException 
         *          - if there is no record matching the supplied key
         *          - if there is more than one record matching the supplied key. 
         *   
         */
        virtual DaiRecordInterface* getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite) = 0;
                //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException) = 0;

        /**
         * validateRecordData
         *
         * Validates the input record, throws exception if errors found
         *
         * @exception InvalidPasConfigurationException if the input data is corrupt/invalid
         *
         */
        virtual void    validateRecordData(const DbRecordType& item) = 0;

        /**
         * createRecordFromInterface
         *
         * @return a new record (basic type) from the input interface
         *
         * @param item the interface to the record (where params extracted from into result)
         *
         * @exception InvalidPasConfigurationException if the input data is corrupt/invalid
         *
         */
        virtual DbRecordType createRecordFromInterface(DaiRecordInterface& item)= 0;
                            //throw (TA_Base_Core::InvalidPasConfigurationException)= 0;

        /**
         * processDatabaseRemovalNotification (interface implementation)
         *
         * @see GenericCachedMapBase::processDatabaseRemovalNotification for more details
         *
         */
        virtual bool processDatabaseRemovalNotification(unsigned long pKey);

        /**
         * processDatabaseUpdateCreateNotification (interface implementation)
         *
         * @see GenericCachedMapBase::processDatabaseUpdateCreateNotification for more details
         *
         */
        virtual bool processDatabaseUpdateCreateNotification(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        /**
         * removeRecordMapItem
         *
         * Updates the key to record map by removing the input item
         *
         * @param pKey the primary key of the item to be removed
         *
         * @exception CachedMappingNotFoundException if item doesn't exist
         *
         */
        void   removeRecordMapItem(unsigned long pKey);
    
        /**
         * doesRecordExistInMap
         *
         * @return true if the specified record exists in the map
         *
         * @param pKey the primary key of the item to be removed
         *
         */
        bool    doesRecordExistInMap(unsigned long pKey);

        /**
         * updateRecordMapWithItemFromDatabase
         *
         * Updates the key to record map with the input item
         *
         * @param item the details of the new item
         *
         * @param isNew true if the item is being added, false if being 
         *              updated (ie it already exists).  This is only
         *              used for internal checks
         *
         * @return true if actual changes occurred to the element in the cached map as
         *       a result of this operation (false if the record was the same, and no changes made)
         *
         * @exception InvalidPasConfigurationException if isNew is false, and item doesn't already
         *                  exist, or isNew, and item already exists, 
         *                  or the data contained in the item is invalid (as defined by derived class
         *                  implementation of validateRecordData)
         *
         */
        bool    updateRecordMapWithItemFromDatabase(DaiRecordInterface& item, bool isNew);
                        //throw (TA_Base_Core::InvalidPasConfigurationException);

    protected:


        typedef std::map<unsigned long, DbRecordType>         KeyToRecordMap;
        typedef typename KeyToRecordMap::iterator             KeyToRecordMapIt;
    
        // Lock for making updates to m_keyToRecordMap
        // Need to make it non-reentrant because there exist functions that can
        // return this lock (wrapped in LockedPacket), and they must not
        // be allowed to modify the map until they release lock (so cannot
        // obtain lock again, being in the same thread)
        TA_Base_Core::NonReEntrantThreadLockable                   m_lockForKeyToRecordMap;

        KeyToRecordMap                                        m_keyToRecordMap;
        bool                                                  m_isKeyToRecordMapInitialised; 
    };

} // end namespace TA_IRS_Bus

#include "bus/pa/cachedmaps/src/GenericCachedMap.inl"

#endif //!defined(GenericCachedMap_4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
