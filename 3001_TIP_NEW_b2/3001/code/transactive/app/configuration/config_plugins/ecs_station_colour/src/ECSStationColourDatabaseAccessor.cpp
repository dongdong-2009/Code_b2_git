/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class uses the Data Access Interface to retrieve ECSStationColour information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ecs_station_colour/src/StdAfx.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourDatabaseAccessor.h"

#include "core/data_access_interface/src/IConfigECSStationColourData.h"
#include "core/data_access_interface/src/IECSStationColourData.h"
#include "core/data_access_interface/src/ECSStationColourAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    ECSStationColourDatabaseAccessor::ECSStationColourDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    ECSStationColourDatabaseAccessor::~ECSStationColourDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedECSStationColours::iterator iter = m_ecsStationColours.begin(); iter != m_ecsStationColours.end(); ++iter)
            {
                if( (iter->second) != NULL )
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void ECSStationColourDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the AlarmSeverities and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedECSStationColours::iterator ECSStationColourToDelete = m_ecsStationColours.begin(); ECSStationColourToDelete != m_ecsStationColours.end(); ++ECSStationColourToDelete)
        {
            if( (ECSStationColourToDelete->second) != NULL )
            {
                delete ECSStationColourToDelete->second;
                ECSStationColourToDelete->second = NULL;
            }
        }
        m_ecsStationColours.clear();


        // Now reload all the AlarmSeverities
        std::vector<TA_Base_Core::IECSStationColourData*> alarmSeverities = TA_Base_Core::ECSStationColourAccessFactory::getInstance().getAllECSStationColours( true);

        // And insert the alarmSeverities into our map. At the same time cast them to IConfigECSStationColourData*
        // instead of IECSStationColourData*.
        for (std::vector<TA_Base_Core::IECSStationColourData*>::iterator iter = alarmSeverities.begin();
             iter != alarmSeverities.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigECSStationColourData* castECSStationColour = dynamic_cast<TA_Base_Core::IConfigECSStationColourData*>(*iter);
             if( castECSStationColour != NULL )
             {
                m_ecsStationColours.insert( LoadedECSStationColours::value_type(castECSStationColour->getUniqueIdentifier(), castECSStationColour) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::map<CString,unsigned long> ECSStationColourDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::map<CString,unsigned long> names; 

        // The maximum range is the number of operators times by 2. This is because we want to step for each
        // ECSStationColour while it is loaded and then again while each ECSStationColour is added to the list.
        progress.SetRange( 0, m_ecsStationColours.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedECSStationColours::iterator iter = m_ecsStationColours.begin();
             iter != m_ecsStationColours.end();
             ++iter)
        {
            if (iter->second != NULL)
            {
                std::string name = iter->second->getName();

                names.insert( std::map<CString, unsigned long>::value_type( name.c_str(), iter->second->getUniqueIdentifier() ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return names;
    }


    TA_Base_Core::IConfigItem* ECSStationColourDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the ECSStationColour matching the key passed in.
        LoadedECSStationColours::iterator matching = m_ecsStationColours.find(key);
        if (matching == m_ecsStationColours.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ECSStationColour with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void ECSStationColourDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Alarm Severities cannot be deleted.

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ECSStationColourDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");
        FUNCTION_EXIT;

        // Alarm Severities cannot be created.
        return NULL;

    }


    TA_Base_Core::IConfigItem* ECSStationColourDatabaseAccessor::copyItem(const unsigned long idOfECSStationColourToCopy)
    {
        FUNCTION_ENTRY("copyItem");
        FUNCTION_EXIT;

        // Alarm Severities cannot be copied.
        return NULL;
    }


    bool ECSStationColourDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& alarmSeveritiesNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(alarmSeveritiesNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedECSStationColours::iterator iter = m_ecsStationColours.begin(); iter != m_ecsStationColours.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    alarmSeveritiesNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this ECSStationColour that has not had it's changes applied yet");
                    alarmSeveritiesNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this ECSStationColour that has not had it's changes applied yet");
                    alarmSeveritiesNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !alarmSeveritiesNotApplied.empty();
    }
}
