/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/mms_dai/src/MmSchedulingAccessFactory.cpp $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * MmSchedulingAccessFactory is a singleton that is used to retrieve 
  * IMmSchedulingData objects from the database. It is able to provide 
  * read-only access.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <ctime>
#include <algorithm>

#include "core/data_access_interface/mms_dai/src/MmSchedulingAccessFactory.h"
#include "core/data_access_interface/mms_dai/src/MmSchedulingData.h"
#include "core/data_access_interface/mms_dai/src/ConfigMmScheduling.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
    MmSchedulingAccessFactory* MmSchedulingAccessFactory::s_instance = 0;
   
	MmSchedulingAccessFactory::MmSchedulingAccessFactory()
    :
    m_schedulesLoaded(false),
    m_hasCacheChanged(false),
    m_readWriteCache(false),
    m_forceRefresh(false)
	{
	}


	MmSchedulingAccessFactory::~MmSchedulingAccessFactory()
	{
		try
		{
	        // Confirm we've stopped listening out for updates prior to clearing the cache
	        // (otherwise we'll be attempting to update cached objects that no longer exist!)
	        if (isListening())
	        {
	            TA_ASSERT(false, "Still registered to online updates on exit!");
	        }
		
			// Clear the cache
			clearCache();
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");		
		}
	}


    void MmSchedulingAccessFactory::clearCache()
    {
        TA_Base_Core::ThreadGuard guard(m_listenerLock);        

        for( ScheduleList::iterator iter = m_schedules.begin(); iter != m_schedules.end(); ++iter)
        {
            if (*iter != NULL)
            {
                delete *iter;
                *iter = NULL;
            }
        }

        m_schedules.clear();
        m_schedulesLoaded = false;
    }


    void MmSchedulingAccessFactory::cleanUp()
    {
        if (isListening())
        {
            try
            {
				TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::MMS_SCHEDULE, *this);
            }
            catch (...)
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "from deregisterAllInterests");
            }
        }
        
        if (s_instance != NULL)
        {
            delete s_instance;
            s_instance = NULL;
        }
    }


    MmSchedulingAccessFactory& MmSchedulingAccessFactory::getInstance()
    {
        if( s_instance == 0 )
        {
            s_instance = new MmSchedulingAccessFactory();
        }

        return *s_instance;
    }


	bool MmSchedulingAccessFactory::registerInterest(TA_Base_Core::IOnlineUpdatable& callback)
	{
		TA_Base_Core::ThreadGuard guard(m_listenerLock);      

		std::vector<TA_Base_Core::IOnlineUpdatable*>::iterator itr = std::find(m_callbacks.begin(),
																	m_callbacks.end(),
																	&callback);
		if (m_callbacks.end() == itr)
		{
			// Add in new item
			m_callbacks.push_back(&callback);
			return true;
		}
		else
		{
			// Already added
			return false;
		}
	}


	bool MmSchedulingAccessFactory::deregisterInterest(TA_Base_Core::IOnlineUpdatable& callback)
	{
		TA_Base_Core::ThreadGuard guard(m_listenerLock);      

		std::vector<TA_Base_Core::IOnlineUpdatable*>::iterator itr = std::find(m_callbacks.begin(),
																	m_callbacks.end(),
																	&callback);
		if (m_callbacks.end() == itr)
		{
			// Item to remove not found
			return false;
		}
		else
		{
			// Found item to remove
			m_callbacks.erase(itr);
			return true;
		}
	}


    bool MmSchedulingAccessFactory::isListening()
    {
        try
        {
            return TA_Base_Core::OnlineUpdateListener::getInstance().isObjectTypeRegistered(TA_Base_Core::MMS_SCHEDULE, *this);
        }
        catch (...)
        {
            return false;
        }
    }


	std::vector<IMmSchedulingData*> MmSchedulingAccessFactory::getAllRecords( const bool readWrite /*= false*/ )
	{
		FUNCTION_ENTRY("getAllRecords");

        if (!isListening())
        {
            try
            {
                TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::MMS_SCHEDULE ,*this);
            }
            catch (...)
            {
                // This is handled for the most part gracefully - just means data is re-fetched
                // from database whenever user makes request.  However online updates will not be posted
                // to listeners
            	LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "from registerAllInterests");
		    }
        }        
                            
        // Lock out any online update listener responses while loading the cache
        TA_Base_Core::ThreadGuard guard(m_listenerLock);      
        
        // Determine if we need to clear the cache and re-fetch all the data
        // - if we're not currently listening out for online updates, we must
        // refetch to guarantee we have up to date data (we can't know whether it's changed or not)
        bool refetchRequired = m_forceRefresh || !isListening();

        if (refetchRequired)
        {
            // Force a refresh of cache data (performing full fetch)
            clearCache();
            m_forceRefresh = false;
        }  

        if (m_schedulesLoaded && 
            m_readWriteCache == readWrite)
        {
            // Cache data loaded previously (for correct r/w type), can return that data
            return m_schedules;
        }        

		// Get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::MmsScheduling_Cd, TA_Base_Core::Read);

        std::ostringstream sql;
        sql << "select MMSCHE_ID, PHYSICALKEY, TO_CHAR(TIME_OF_DAY,'YYYYMMDDHH24MISS'), ";
		sql << " TO_CHAR(LAST_SENT,'YYYYMMDDHH24MISS')";
        sql << " from MM_SCHEDULING";

        // Set up the columnNames vector to be passed to executeQuery()
		std::string keyColumn = "KEY";
        std::string subKeyColumn = "SUB_KEY";
        std::string timeColumn = "TIME_OF_DAY";
        std::string dateColumn = "LAST_SENT";

        std::vector<std::string> columnNames;
		columnNames.push_back(keyColumn);
        columnNames.push_back(subKeyColumn);
        columnNames.push_back(timeColumn);
        columnNames.push_back(dateColumn);

		TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(), columnNames);
                
	    // Create the vector to return
        std::vector<IMmSchedulingData*> records;

 	    if (data->getNumRows() == 0) // No records found
        {
            // clean up the pointer
            delete data;
            data = NULL;

            if (readWrite)
            {
                // Cache the data prior to exit, if requesting readWrite (to be available on next call)
                cacheConfigurableData(records, readWrite);
            }

            return records;
        }

        // Loop for each row returned
        do
        {
			unsigned int numRows = data->getNumRows();
            
			for (unsigned long i = 0; i < numRows; i++)
            {
                // The getUnsignedLongData() call can throw an exception. Need to catch
                // it to do pointer clean up.
                try
                {
					unsigned long key = data->getUnsignedLongData( i, keyColumn );
					unsigned long subsystemKey = data->getUnsignedLongData( i, subKeyColumn );
					time_t scheduledTime = data->getDateData( i, timeColumn );
					time_t lastRunDate = data->getDateData( i, dateColumn );
					std::string subsystemName = retrieveSubName(subsystemKey);

                    if (readWrite)
                    {
                        records.push_back( new ConfigMmScheduling( key, subsystemKey, scheduledTime, lastRunDate, subsystemName ));
                    }
                    else
                    {
                        records.push_back( new MmSchedulingData( key, subsystemKey, scheduledTime, lastRunDate, subsystemName ));
                    }
                }
                catch (TA_Base_Core::DataException&)
                {
                    // Clean up the data pointer
                    delete data;
                    data = NULL;
                    throw;
                }
            }

			delete data;
			data = NULL;
        }
        while (databaseConnection->moreData(data));

		FUNCTION_EXIT;
        
        if (readWrite)
        {
            // Cache the data prior to exit, if requesting readWrite (to be available on next call)
            cacheConfigurableData(records, readWrite);
        }

        return records;
	}


    void MmSchedulingAccessFactory::cacheConfigurableData(std::vector<IMmSchedulingData*> data, bool readWrite)
    {
        TA_Base_Core::ThreadGuard guard(m_listenerLock);        

        clearCache();

        m_schedules         = data;
        m_schedulesLoaded   = true;
        m_readWriteCache    = readWrite;
    }


    std::string MmSchedulingAccessFactory::retrieveSubName(unsigned long entityKey)
    {
        FUNCTION_ENTRY("retrieveSubName");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::OnlineUpdatable_Cd, TA_Base_Core::Read);

        std::ostringstream sql;
        sql << "select NAME from SUBSYSTEM WHERE PKEY = " << entityKey;


        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn = "NAME";

        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery( sql.str(), columnNames );

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
            reasonMessage << "No record found for key = " << entityKey;
			TA_THROW( TA_Base_Core::DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "More than one record found for key = " << entityKey;
			TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		std::string name;
		name = data->getStringData( 0, nameColumn );
 
 
        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;        

        FUNCTION_EXIT;
        return name;
    }


    bool MmSchedulingAccessFactory::hasChanged()
    {
        TA_Base_Core::ThreadGuard guard(m_listenerLock);
        
        if (!isListening())
        {
            // If we're not currently listening, we can't make any assumptions on
            // whether the data has changed, so we must return true to indicate it may have changed
            return true;
        }

        return m_hasCacheChanged;
    }


    void MmSchedulingAccessFactory::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("processUpdate");

        TA_Base_Core::ThreadGuard guard(m_listenerLock);

		try
		{
			handleUpdateNotification(updateEvent);
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "from handleUpdateNotification");
		}
        
		// Dispatch to listeners last (once invalidated internally)
		for (UpdateCallbackList::iterator itr = m_callbacks.begin(); itr != m_callbacks.end(); ++itr )
		{
			try
			{
				(*itr)->processUpdate(updateEvent);
			}
			catch (...)
			{
				// Ignore all exceptions
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "processUpdate to callback");
			}
		}

		FUNCTION_EXIT
	}


	void MmSchedulingAccessFactory::handleUpdateNotification(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("handleUpdateNotification");

		TA_Base_Core::ThreadGuard guard(m_listenerLock);
	
		m_hasCacheChanged = true;
	
        // Receiving updates before managed to read from DB, may ignore
        if (!m_schedulesLoaded)
        {
            return;
        }

        if (updateEvent.getType() != TA_Base_Core::MMS_SCHEDULE)
        {
            // Only interested in MMS Scheduling updates
            FUNCTION_EXIT;
            return;
        }

        if (-1 == updateEvent.getKey())
        {
            // special case - single notification sent out for a batch of changes
            invalidateCache();
            FUNCTION_EXIT;
            return;
        }

        switch(updateEvent.getModifications())
        {
            case(TA_Base_Core::Create):
            {
                // Add a new alarm plan object to our list, provided not already there
                // (only rare cases it will already be present..)
                for( ScheduleList::iterator iter = m_schedules.begin(); iter != m_schedules.end(); ++iter)
                {
                    if( (*iter)->getKey() == updateEvent.getKey() )
                    {
                        // Already loaded this schedule record, nothing to do
                        return;
                    }
                }
            
                // Otherwise add record to cache - currently no easy way
                // to construct new individual object.  Because there is no way to
                // currently send a Create notification, we will never get this message at present,
                // but to handle we just clear the cache and force a complete refresh..
                invalidateCache();
                
                FUNCTION_EXIT;
                return;
            }

            case(TA_Base_Core::Update):
            {
                // Find the object being updated and invalidate it
                for( ScheduleList::iterator iter = m_schedules.begin(); iter != m_schedules.end(); ++iter)
                {
                    if( (*iter)->getKey() == updateEvent.getKey() )
                    {
                        (*iter)->invalidate();
                        break;
                    }
                }
                FUNCTION_EXIT;
                return;
            }

            case(TA_Base_Core::Delete):
            {
                // Find the object being deleted and remove it
                for( ScheduleList::iterator iter = m_schedules.begin(); iter != m_schedules.end(); ++iter)
                {
                    if( (*iter)->getKey() == updateEvent.getKey() )
                    {
                        delete *iter;
                        *iter = NULL;
                        m_schedules.erase(iter);
                        break;
                    }
                }
                FUNCTION_EXIT;
                return;
            }
        }

        FUNCTION_EXIT;
    }


    void MmSchedulingAccessFactory::invalidateCache()
    {
        m_forceRefresh = true;
    }
                
} // closes TA_IRS_Core
