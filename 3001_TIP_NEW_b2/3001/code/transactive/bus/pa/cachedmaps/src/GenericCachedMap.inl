/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $Workfile:$
  * @author Jade Lee
  * @version $Revision: 1.1.2.2 $
  * Last modification: $Modtime:$
  * Last modified by: $Author: christiandw $
  * 
  * Inline definitions
  *
  */

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Bus
{

    /////////////////////////////
    // Helper template function
    template <class ArrayType> 
    void deleteAllItemsInArray(ArrayType& array)
    {
        typename
        ArrayType::iterator it = array.begin(); 
        for (; it != array.end(); ++it)
        {
            delete (*it);
        }

        array.clear();
    }
    /////////////////////////


    template <class DbRecordType, class DaiRecordInterface>
    GenericCachedMap<DbRecordType, DaiRecordInterface>::GenericCachedMap(const TA_Base_Core::EObjectType& databaseUpdateListenerType,
                                                                         const std::string& tableName)
    :
    GenericCachedMapBase(databaseUpdateListenerType, tableName),
    m_isKeyToRecordMapInitialised(false)
    {
    }


    template <class DbRecordType, class DaiRecordInterface>
    bool GenericCachedMap<DbRecordType, DaiRecordInterface>::updateRecordMapWithItemFromDatabase(DaiRecordInterface& item, bool isNew)
                //throw (TA_Base_Core::InvalidPasConfigurationException)
    {
        FUNCTION_ENTRY("updateRecordMapWithItemFromDatabase");

        bool itemAlreadyExists = doesRecordExistInMap(item.getKey());
    
        /*
        // TD #2692: Exception was being thrown because already existed on local instance of PA Manager
        if (isNew && itemAlreadyExists)
        {
            TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Item is not new, already exists"));        
        }
        else if (!isNew && !itemAlreadyExists)
        {
            TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Item to update not found"));
        }*/

        try
        {
            DbRecordType newRecord = createRecordFromInterface(item);
            
            // Allow any invalid configuration exceptions to propogate
            return updateRecordMapWithItem(item.getKey(), newRecord);
        }
        catch (TA_Base_Core::TransactiveException&)
        {
            std::ostringstream msg;
            msg << "Exception caught while extracting information from table " 
                << getTableName().c_str() << " (pkey=" << item.getKey() << ")";
            LOG( SourceInfo, 
                    TA_Base_Core::DebugUtil::ExceptionCatch, 
                    "TransactiveException",
                    msg.str().c_str());
            throw;
        }

        FUNCTION_EXIT;
    }


    template <class DbRecordType, class DaiRecordInterface>
    std::vector<unsigned long> GenericCachedMap<DbRecordType, DaiRecordInterface>::getPrimaryKeyArray()
    {
        TA_Base_Core::ThreadGuard guard( m_lockForKeyToRecordMap );

        std::vector<unsigned long> resultKeys;

        for (KeyToRecordMapIt itr = m_keyToRecordMap.begin();
                itr != m_keyToRecordMap.end();
                itr ++)
        {
            // Return all the keys into map (representing primary keys in database)
            resultKeys.push_back(itr->first);
        }

        return resultKeys;
    }


    template <class DbRecordType, class DaiRecordInterface>
    bool GenericCachedMap<DbRecordType, DaiRecordInterface>::updateRecordMapWithItem(unsigned long pKey, const DbRecordType& record)
                //throw (TA_Base_Core::InvalidPasConfigurationException)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForKeyToRecordMap );

        typedef typename std::map<unsigned long, DbRecordType>::iterator iterator;
        iterator itr = m_keyToRecordMap.find(pKey);

        if (itr != m_keyToRecordMap.end() && itr->second == record)
        {
            // Item already exists, and matches the data we're updating with - no changes necessary
            return false;        
        }
  
        // Allow exceptions to propogate from validation function
        validateRecordData(record);

        if (itr == m_keyToRecordMap.end())
        {
            // Item doesn't exist in map, add in fresh
            m_keyToRecordMap[pKey] = record;
        }
        else
        {
            // Otherwise update the existing data in the map
            itr->second = record;
        }
        return true;
    }


    template <class DbRecordType, class DaiRecordInterface>
    bool GenericCachedMap<DbRecordType, DaiRecordInterface>::processDatabaseUpdateCreateNotification(
                                            const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        unsigned long pKey = updateEvent.getKey();                
        bool isNewRecord = (TA_Base_Core::Create == updateEvent.getModifications());
        
        // Note that isNewRecord can be deceiving, if the application that sent out
        // the update updates it's cache before sending out the notification, then
        // even though isNewRecord is true, the item will already be in the cache
        // (ie it will only be new for foreign applications)
        
        // Fetch the update for this item from the database     
        // In some situations the local machine may be the source of this 
        // notification (in which case it will have already been processed)
        // - but won't hurt to do again
        
        // Otherwise typical update, where primary key has not changed
        std::auto_ptr<DaiRecordInterface> record(getRecordFromDatabaseUsingKey(pKey, false));
    
        if (record->getKey() != pKey)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Key mismatch" );
            return false;
        }
        
        if (0 == record.get())
        {
            onDatabaseUpdateProcessError(pKey);
            return false;
        }
                
        return updateRecordMapWithItemFromDatabase(*(record.get()), isNewRecord);         
    }


    template <class DbRecordType, class DaiRecordInterface>
    bool GenericCachedMap<DbRecordType, DaiRecordInterface>::processDatabaseRemovalNotification(unsigned long pKey)
    {
        // In some situations the local machine may be the source of this 
        // notification (in which case it will have already been processed)
        // - so check if this record exists or not..
        if (doesRecordExistInMap(pKey))
        {
            removeRecordMapItem(pKey);
        }

        return true;
    }


    template <class DbRecordType, class DaiRecordInterface>
    bool GenericCachedMap<DbRecordType, DaiRecordInterface>::doesRecordExistInMap(unsigned long pKey)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForKeyToRecordMap );
        return m_keyToRecordMap.end() != m_keyToRecordMap.find(pKey);
    }


    template <class DbRecordType, class DaiRecordInterface>
    void GenericCachedMap<DbRecordType, DaiRecordInterface>::removeRecordMapItem(unsigned long pKey)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForKeyToRecordMap );

        KeyToRecordMapIt it = m_keyToRecordMap.find(pKey);

        if (it == m_keyToRecordMap.end())
        {
            TA_THROW(TA_Base_Core::CachedMappingNotFoundException("Item being removed does not exist"));
        }

        m_keyToRecordMap.erase(it);
    }


    template <class DbRecordType, class DaiRecordInterface>
    void GenericCachedMap<DbRecordType, DaiRecordInterface>::clearKeyToRecordMap()
    {
        TA_Base_Core::ThreadGuard guard( m_lockForKeyToRecordMap );
        m_keyToRecordMap.clear();
    }


    template <class DbRecordType, class DaiRecordInterface>
    unsigned long GenericCachedMap<DbRecordType, DaiRecordInterface>::refreshKeyToRecordMapUsing(DaiRecordInterfaceSet& records) 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {

        FUNCTION_ENTRY("refreshKeyToRecordMap");

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Stopped listening");
        
        // Clear the map first - ensuring we release the lock early
        // (because the insertion requires possession of the nonreentrant lock)
        clearKeyToRecordMap();

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Cleared map");

        std::vector<unsigned long> primaryKeys;
    
        unsigned long failedLoads = 0;
      
        typedef typename DaiRecordInterfaceSet::iterator iterator;
        for (iterator it = records.begin(); it != records.end(); ++it)
        {      
            DaiRecordInterface* pRec = (*it);
        
            try
            {
                updateRecordMapWithItemFromDatabase(*pRec, true);
            }
            catch (const TA_Base_Core::InvalidPasConfigurationException& e)
            {
                failedLoads ++;

                LOG( SourceInfo, 
                    TA_Base_Core::DebugUtil::ExceptionCatch, 
                    "InvalidPasConfigurationException",
                    e.what());
            }

            primaryKeys.push_back(pRec->getKey());
        }
    
        m_isKeyToRecordMapInitialised = true;

        FUNCTION_EXIT;

        return failedLoads;
    }


    template <class DbRecordType, class DaiRecordInterface>
    DbRecordType GenericCachedMap<DbRecordType, DaiRecordInterface>::getCachedDbRecordFromKey(
                                                                       unsigned long key)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return getConfigurableCachedDbRecordFromKey(key).getLockedData();
    }


    template <class DbRecordType, class DaiRecordInterface>
    LockedPacket<DbRecordType> GenericCachedMap<DbRecordType, DaiRecordInterface>::
                    getConfigurableCachedDbRecordFromKey(unsigned long key)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        FUNCTION_ENTRY("getConfigurableCachedDbRecordFromKey");
        if (!m_isKeyToRecordMapInitialised)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "KeyToPaZoneRecordMap not initialised" );
            TA_THROW(TA_Base_Core::CachedMappingNotFoundException("KeyToPaZoneRecordMap not initialised"));
        }
    
        // Allocate new guard that can be passed into locked result
        std::auto_ptr<TA_Base_Core::ThreadGuard> 
                guard( new TA_Base_Core::ThreadGuard(m_lockForKeyToRecordMap) );

        KeyToRecordMapIt it = m_keyToRecordMap.find(key);

	    if (it == m_keyToRecordMap.end())
	    {
            std::ostringstream msg;
            msg << "Failed to locate " 
                << getTableName() 
                << " record for key=" 
                << key;
	        TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
	    }

        // Trust the default assignement operator
        LockedPacket<DbRecordType> lockedData(guard, it->second); 
        FUNCTION_EXIT;

        return lockedData;
    }


    template <class DbRecordType, class DaiRecordInterface>
        typename GenericCachedMap<DbRecordType, DaiRecordInterface>::DbRecordTypeSet 
            GenericCachedMap<DbRecordType, DaiRecordInterface>::getAllRecords
            (
             const ICachedMapFilter<DbRecordType>* filter,
             bool breakOnFirstMatch
            )
    {
        FUNCTION_ENTRY("getAllRecords");
    
        TA_Base_Core::ThreadGuard guard( m_lockForKeyToRecordMap );

        DbRecordTypeSet result;

        if (!m_isKeyToRecordMapInitialised)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "KeyToPaZoneRecordMap not initialised" );

            FUNCTION_EXIT;
            return result;
        }

        for (KeyToRecordMapIt it = m_keyToRecordMap.begin();
                                         it != m_keyToRecordMap.end(); 
                                         it ++)
        {
            // Add record to result by default if no filter present..
            if (0 == filter || filter->isMatch(it->second))
            {
                result.push_back(it->second);

                if (breakOnFirstMatch)
                {
                    // May return early
                    break;
                }
            }
        }

        FUNCTION_EXIT;

        return result;
    }


    template <class DbRecordType, class DaiRecordInterface>
        typename GenericCachedMap<DbRecordType, DaiRecordInterface>::DbRecordTypeSet 
            GenericCachedMap<DbRecordType, DaiRecordInterface>::getFirstMatchingRecord(
                                            const ICachedMapFilter<DbRecordType>& filter)
    {
        return getAllRecords(&filter, true);
    }


} // end namespace TA_IRS_Bus