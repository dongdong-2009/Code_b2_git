/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/pa_station_dva_message/src/PAStationDvaMessageDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve DB information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"

//#include "core/data_access_interface/pa/src/IConfigPaDvaMessage.h"
#include "core/data_access_interface/pa/src/IConfigPaDvaMessage.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/DatabaseKey.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{


    PaStationDvaMessageDatabaseAccessor::PaStationDvaMessageDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configItems.clear();
    }


    PaStationDvaMessageDatabaseAccessor::~PaStationDvaMessageDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end(); ++it)
            {
                if( (it->second) != NULL )
                {
                    delete it->second;
                    it->second = NULL;
                }
            }
            m_configItems.clear();
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
        
        FUNCTION_EXIT;
    }


    void PaStationDvaMessageDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the PA Zones and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (ConfigItemsIt assocToDelete = m_configItems.begin(); assocToDelete != m_configItems.end(); ++assocToDelete)
        {
            if( (assocToDelete->second) != NULL )
            {
                delete assocToDelete->second;
                assocToDelete->second = NULL;
            }
        }
        m_configItems.clear();

		IPaDvaMessages items = PaDvaMessageAccessFactory::getInstance().getAllPaDvaMessages(true);

        // And insert the PA Zones into our map. At the same time cast them to IConfigPaTisMessageMap*
        // instead of IPaTisMessageMap*.
        for (IPaDvaMessagesIt it = items.begin(); it != items.end(); ++it)
        {
            IPaDvaMessage* item = *it;
            TA_Base_Core::IConfigPaDvaMessage* configItem = dynamic_cast<TA_Base_Core::IConfigPaDvaMessage*>(item);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getUniqueIdentifier();
                m_configItems[key] = configItem;
            }
        }

        refreshCachedMaps();

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::multimap<CString,unsigned long> PaStationDvaMessageDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for this class");

        std::multimap<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,PaStationDvaMessageDatabaseAccessor::PaStationDvaMessageRecord> PaStationDvaMessageDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,PaStationDvaMessageRecord> details; 

        // The maximum range is the number of locations times by 2. This is because we want to step for each
        // Location while it is loaded and then again while each Location is added to the list.
        progress.SetRange( 0, m_configItems.size() * 2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.

        CString buffer;
        for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end(); ++it)
        {
            if (it->second != NULL)
            {
                PaStationDvaMessageRecord record = getRecordFrom(*it->second);

                details.insert( std::map<unsigned long, PaStationDvaMessageRecord>::value_type( it->second->getUniqueIdentifier(), record ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return details;
    }


    PaStationDvaMessageDatabaseAccessor::PaStationDvaMessageRecord PaStationDvaMessageDatabaseAccessor::getItemDetails(unsigned long identifier)
    {
        FUNCTION_ENTRY("getItemDetails");

        // Attempt to find the record matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(identifier);
        if (matching == m_configItems.end() || 0 == matching->second)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the PA Zone with identifier %lu",identifier);
            FUNCTION_EXIT;
            PaStationDvaMessageRecord record;
            return record;
        }

        PaStationDvaMessageRecord record = getRecordFrom(*matching->second);

        FUNCTION_EXIT;
        return record;
    }


    PaStationDvaMessageDatabaseAccessor::PaStationDvaMessageRecord PaStationDvaMessageDatabaseAccessor::getRecordFrom(TA_Base_Core::IConfigPaDvaMessage& item)
    {
        PaStationDvaMessageRecord record;

        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getKey()))
        {
            record.key = "";
        }
        else
        {
            record.key.Format("%ld", item.getKey()); 
        }

        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getLocationKey()))
        {
            record.location = "";
        }
        else
        {        
            record.location = getLocationName(item.getLocationKey()).c_str();
        }

        if (TA_Base_Core::DatabaseKey::isInvalidKey(item.getId()))
        {
            record.id = "";
        }
        else
        {
            record.id.Format("%ld", item.getId());
        }

        record.label = item.getLabel().c_str();
        
        record.type = PaStationDvaMessageDatabaseAccessor::getTypeString(item.getType());
        
        
        return record;
    }

    
	// TODO
    CString PaStationDvaMessageDatabaseAccessor::getTypeString(const unsigned short type)
    {
        CString result;
		/*
        if (1 == type.length())
        {
            switch (type.at(0))
            {        
            case 'N':
                result.LoadString(IDS_DVA_TYPE_NORMAL);
                break;
            case 'E':
                result.LoadString(IDS_DVA_TYPE_EMERGENCY);
                break;
            default:    // FALLTHROUGH (if no valid type defined, default to deleted)
            case 'X':
                result.LoadString(IDS_DVA_TYPE_DELETED);
                break;
            }
        }
        else
        {
            // Any unrecognized types go to deleted
            result.LoadString(IDS_DVA_TYPE_DELETED);
        }
		*/

        return result;
    }


    TA_Base_Core::IConfigItem* PaStationDvaMessageDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the PA Zone matching the key passed in.
        ConfigItemsIt matching = m_configItems.find(key);
        if (matching == m_configItems.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the PA Zone with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void PaStationDvaMessageDatabaseAccessor::deleteItem(unsigned long key)
    {
		TA_ASSERT(false, "This method cannot be called for this class");

//        FUNCTION_ENTRY("deleteItem");
//
//        // Attempt to find the PA Zone matching the key passed in.
//        ConfigItemsIt matching = m_configItems.find(key);
//        if (matching == m_configItems.end() )
//        {
//            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the PA Zone with key %lu",key);
//            FUNCTION_EXIT;
//        }
//
//        TA_ASSERT(matching->second != NULL, "Stored PA Zone was equal to NULL");
//
//        if (!matching->second->isNew())
//        {
//            // TODO - check asserts
//            matching->second->deleteThisObject();
//        }
//        delete matching->second;
//        matching->second = NULL;
//        m_configItems.erase(matching);
//
//        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* PaStationDvaMessageDatabaseAccessor::newItem()
    {
		TA_ASSERT(false, "This method cannot be called for this class");
		return NULL;
//        FUNCTION_ENTRY("newItem");
//
//        TA_Base_Core::IConfigPaTisMessageMap* newAssoc = TA_Base_Core::PaDvaMessageAccessFactory::getInstance().createPaDvaMessage();
//        
//        // Because there must be a selection in the PA Dva message droplist
//        // default to 'deleted' type
//        newAssoc->setType(std::string("X"));
//
//        m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
//
//        return newAssoc;
//
//        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* PaStationDvaMessageDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
		TA_ASSERT(false, "This method cannot be called for this class");
		return NULL;
//        FUNCTION_ENTRY("copyItem");
//
//        TA_Base_Core::IConfigPaDvaMessage* newAssoc = NULL;
//
//        // Attempt to find the PA Zone matching the key passed in.
//        ConfigItemsIt matching = m_configItems.find(idOfItemToCopy);
//
//        TA_ASSERT(matching != m_configItems.end(), "The key passed could not be found in the map");
//        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigPaDvaMessage stored in the map");
//
//        newAssoc = TA_Base_Core::PaDvaMessageAccessFactory::getInstance().copyPaDvaMessage(matching->second);
//
//        if (newAssoc != NULL)
//        {
//            m_configItems.insert( ConfigItems::value_type( newAssoc->getUniqueIdentifier(), newAssoc ) );
//        }
//        
//        return newAssoc;
//
//        FUNCTION_EXIT;
    }


    bool PaStationDvaMessageDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& itemsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(itemsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (ConfigItemsIt it = m_configItems.begin(); it != m_configItems.end();  ++it)
        {
            if( (it->second != NULL) && (it->second->hasChanged() || it->second->isNew()) )
            {
                // We just add a single empty item if we know a change has been made. We don't list each
                // individual change for PA Zones
                itemsNotApplied.push_back("");
                break;
            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;
        return !itemsNotApplied.empty();
    }


    void PaStationDvaMessageDatabaseAccessor::refreshCachedMaps()
    {
        FUNCTION_ENTRY("refreshCachedMaps");

        // First retrieve all the subsystems from the database
        try
        {
            m_locationNameCache.refreshUsing(LocationAccessFactory::getInstance().getAllLocations());
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No Locations will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No Locations will be added to the map. An empty map will be returned");
            FUNCTION_EXIT;
            return;
        }
        FUNCTION_EXIT;
    }

    std::string PaStationDvaMessageDatabaseAccessor::getLocationName(unsigned long locationKey)
    {   
        return m_locationNameCache.getName(locationKey);
    }
        
    unsigned long PaStationDvaMessageDatabaseAccessor::getLocationKey(const std::string& locationName)
    {
        return m_locationNameCache.getKey(locationName);
    }

    /*
    bool PaStationDvaMessageDatabaseAccessor::isOccLocationKey(unsigned long locationKey)
    {
        return LocationAccessFactory::getInstance().getOccLocationKey() == locationKey;
    }
    */

}

    
