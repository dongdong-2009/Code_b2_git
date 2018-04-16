/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class uses the Data Access Interface to retrieve DSS Plan Map information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_dss_plan_map/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapDatabaseAccessor.h"


#include "core/data_access_interface/ats/src/IConfigAtsDssPlanMap.h"
#include "core/data_access_interface/ats/src/IAtsDssPlanMap.h"
#include "core/data_access_interface/ats/src/AtsDssPlanMapAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    AtsDssPlanMapDatabaseAccessor::AtsDssPlanMapDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    AtsDssPlanMapDatabaseAccessor::~AtsDssPlanMapDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedAtsDssPlanMaps::iterator iter = m_atsDssPlanMaps.begin(); iter != m_atsDssPlanMaps.end(); ++iter)
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


    void AtsDssPlanMapDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the DSS Plan Maps and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedAtsDssPlanMaps::iterator assocToDelete = m_atsDssPlanMaps.begin(); assocToDelete != m_atsDssPlanMaps.end(); ++assocToDelete)
        {
            if( (assocToDelete->second) != NULL )
            {
                delete assocToDelete->second;
                assocToDelete->second = NULL;
            }
        }
        m_atsDssPlanMaps.clear();


        // Now reload all the DSS Plan Maps
        std::vector<TA_IRS_Core::IAtsDssPlanMap*> associations = TA_IRS_Core::AtsDssPlanMapAccessFactory::getInstance().getAllAtsDssPlanMaps(true);

        // And insert the DSS Plan Maps into our map. At the same time cast them to IConfigAtsDssPlanMap*
        // instead of IAtsDssPlanMap*.
        for (std::vector<TA_IRS_Core::IAtsDssPlanMap*>::iterator iter = associations.begin();
             iter != associations.end();
             ++iter
             )
        {
             TA_IRS_Core::IConfigAtsDssPlanMap* castItem = dynamic_cast<TA_IRS_Core::IConfigAtsDssPlanMap*>(*iter);
             if( castItem != NULL )
             {
                m_atsDssPlanMaps.insert( LoadedAtsDssPlanMaps::value_type(castItem->getUniqueIdentifier(), castItem) );
             }
        }

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::map<CString,unsigned long> AtsDssPlanMapDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for DSS Plan Maps");

        std::map<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,AtsDssPlanMapDatabaseAccessor::AtsDssPlanMap> AtsDssPlanMapDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,AtsDssPlanMap> details; 

        // The maximum range is the number of locations times by 2. This is because we want to step for each
        // Location while it is loaded and then again while each Location is added to the list.
        progress.SetRange( 0, m_atsDssPlanMaps.size() * 2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedAtsDssPlanMaps::iterator iter = m_atsDssPlanMaps.begin();
             iter != m_atsDssPlanMaps.end();
             ++iter)
        {
            if (iter->second != NULL)
            {
                AtsDssPlanMap item;
                
				item.dssEvent = iter->second->getDssEvent();
				std::stringstream strStream;
				strStream << item.dssEvent;
				item.dssEventCString = strStream.str().c_str();

                item.plan = iter->second->getPlanPath().c_str();

                details.insert( std::map<unsigned long, AtsDssPlanMap>::value_type( iter->second->getUniqueIdentifier(), item ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    AtsDssPlanMapDatabaseAccessor::AtsDssPlanMap AtsDssPlanMapDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the DSS Plan Map matching the key passed in.
        LoadedAtsDssPlanMaps::iterator matching = m_atsDssPlanMaps.find(identifier);
        if (matching == m_atsDssPlanMaps.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the DSS Plan Map with identifier %lu",identifier);
            FUNCTION_EXIT;
            AtsDssPlanMap item;
            return item;
        }

        AtsDssPlanMap item;

		item.dssEvent = matching->second->getDssEvent();
		std::ostringstream eventStream;
		eventStream << item.dssEvent;
		item.dssEventCString = eventStream.str().c_str();

        item.plan =      matching->second->getPlanPath().c_str();

        FUNCTION_EXIT;
        return item;
    }


    TA_Base_Core::IConfigItem* AtsDssPlanMapDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the DSS Plan Map matching the key passed in.
        LoadedAtsDssPlanMaps::iterator matching = m_atsDssPlanMaps.find(key);
        if (matching == m_atsDssPlanMaps.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the DSS Plan Map with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void AtsDssPlanMapDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the DSS Plan Map matching the key passed in.
        LoadedAtsDssPlanMaps::iterator matching = m_atsDssPlanMaps.find(key);
        if (matching == m_atsDssPlanMaps.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the DSS Plan Map with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored DSS Plan Map was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisObject();
        }
        delete matching->second;
        matching->second = NULL;
        m_atsDssPlanMaps.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* AtsDssPlanMapDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_IRS_Core::IConfigAtsDssPlanMap* newAssoc = TA_IRS_Core::AtsDssPlanMapAccessFactory::getInstance().createAtsDssPlanMap();

        m_atsDssPlanMaps.insert( LoadedAtsDssPlanMaps::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );

        return newAssoc;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* AtsDssPlanMapDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_IRS_Core::IConfigAtsDssPlanMap* newAssoc = NULL;

        // Attempt to find the DSS Plan Map matching the key passed in.
        LoadedAtsDssPlanMaps::iterator matching = m_atsDssPlanMaps.find(idOfItemToCopy);

        TA_ASSERT(matching != m_atsDssPlanMaps.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigAtsDssPlanMap stored in the map");

        newAssoc = TA_IRS_Core::AtsDssPlanMapAccessFactory::getInstance().copyAtsDssPlanMap(matching->second);

        if (newAssoc != NULL)
        {
            m_atsDssPlanMaps.insert( LoadedAtsDssPlanMaps::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
        }
        
        return newAssoc;

        FUNCTION_EXIT;
    }


    bool AtsDssPlanMapDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(itemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedAtsDssPlanMaps::iterator iter = m_atsDssPlanMaps.begin(); iter != m_atsDssPlanMaps.end();  ++iter)
        {
            if( (iter->second != NULL) && (iter->second->hasChanged() || iter->second->isNew()) )
            {
                // We just add a single empty item if we know a change has been made. We don't list each
                // individual change for DSS Plan Maps
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }
}
