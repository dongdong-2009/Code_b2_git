/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve MMS Scheduling information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786 4284)

#include "app/configuration/config_plugins/mms_scheduling/src/StdAfx.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingDatabaseAccessor.h"

#include "core/data_access_interface/mms_dai/src/IConfigMmScheduling.h"
#include "core/data_access_interface/mms_dai/src/MmSchedulingAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    MmsSchedulingDatabaseAccessor::MmsSchedulingDatabaseAccessor()
    {
    }


    MmsSchedulingDatabaseAccessor::~MmsSchedulingDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // Must explicitly clean up the factory on exit, as it maintains
            // a cache of the data it returns (and is thus responsible for 
            // clearning up that memory)
            TA_IRS_Core::MmSchedulingAccessFactory::getInstance().cleanUp();           
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void MmsSchedulingDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");
        
        FUNCTION_EXIT;
    }


    TA_IRS_Core::IConfigMmScheduling* MmsSchedulingDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the item matching the key passed in.
        ScheduleMap::iterator matching = m_scheduleMap.find(key);

        if (matching != m_scheduleMap.end() )
        {   
            TA_IRS_Core::IConfigMmScheduling* result = dynamic_cast<TA_IRS_Core::IConfigMmScheduling*>(matching->second);
            if (0 != result)
            {
                return result;
            }
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the schedule with key %lu",key);
        FUNCTION_EXIT;
        return NULL;
    }



    bool MmsSchedulingDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& rightsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");
                
        TA_ASSERT(rightsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (0 == m_scheduleMap.size())
        {
            return false;
        }

        bool haveChangesBeenMade = false;

        // Step through all the objects and see if any have changed but not yet been applied.
        for (ScheduleMap::iterator iter = m_scheduleMap.begin(); iter != m_scheduleMap.end();  ++iter)
        {
            TA_IRS_Core::IConfigMmScheduling* pData = dynamic_cast<TA_IRS_Core::IConfigMmScheduling*>(iter->second);

            if( (pData!= NULL) && pData->hasChanged() )
            {
                haveChangesBeenMade = true;
                break;
            }
        }

        if (haveChangesBeenMade)
        {
            // We push back a blank entry because it is this component overall that has changed not an individual
            // part of it.
            rightsNotApplied.push_back( "" );
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;

        return haveChangesBeenMade;        
    }


    void MmsSchedulingDatabaseAccessor::invalidateData()
    {
        // Tell the scheduler factory to invalidate its cached data
        TA_IRS_Core::MmSchedulingAccessFactory::getInstance().invalidateCache();
    }


    ScheduleMap MmsSchedulingDatabaseAccessor::getAllSchedules()
    {
        FUNCTION_ENTRY("getAllSchedules");

        m_scheduleMap.clear();

        // First retrieve all the schedules from the database 
        std::vector<TA_IRS_Core::IMmSchedulingData*> data;

        try
        {
            bool readWrite = true;
            data = TA_IRS_Core::MmSchedulingAccessFactory::getInstance().getAllRecords(readWrite);
        }
        catch(const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve the schedule data. None will be displayed to the user");
            FUNCTION_EXIT;
            return m_scheduleMap;
        }
        catch(const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve the schedule data. None will be displayed to the user");
            FUNCTION_EXIT;
            return m_scheduleMap;
        }
        catch (...)
        {
            // General error, don't allow plugin to crash (happened when location key unset, 
            // meaning unable to subscribe for updates)
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Could not retrieve the schedule data. None will be displayed to the user");
            FUNCTION_EXIT;
            return m_scheduleMap;
        }

        // Now step through each one and apply into map - because the data is cached by the factory, 
        // it is fine to maintain a hold on the pointers
        for (std::vector<TA_IRS_Core::IMmSchedulingData*>::iterator iter = data.begin(); iter != data.end(); ++iter)
        {
            unsigned int key = (*iter)->getKey();

            TA_IRS_Core::IConfigMmScheduling* configItem = dynamic_cast<TA_IRS_Core::IConfigMmScheduling*>(*iter);
                
            m_scheduleMap[key] = configItem;
        }

        FUNCTION_EXIT;
        return m_scheduleMap;
    }


}
