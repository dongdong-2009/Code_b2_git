/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/pa_station_dva_message/src/PAStationDvaMessageDatabaseAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class uses the Data Access Interface to retrieve DB information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"

#include "core/data_access_interface/pa_4669/src/IConfigPaDvaMessage.h"
#include "core/data_access_interface/pa_4669/src/IConfigPaTisMessageMap.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#include "bus/pa_4669/pa_agent/idl/src/IPAAgentCorbaDef.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{


    PaStationDvaMessageDatabaseAccessor::PaStationDvaMessageDatabaseAccessor()
        : m_isLoaded(false)
    {
        m_configPaDvaMessages.clear();
        m_configPaTisMapItems.clear();
    }


    PaStationDvaMessageDatabaseAccessor::~PaStationDvaMessageDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (ConfigPaDvaMessagesIt it = m_configPaDvaMessages.begin(); it != m_configPaDvaMessages.end(); ++it)
            {
                if( (it->second) != NULL )
                {
                    delete it->second;
                    it->second = NULL;
                }
            }
            m_configPaDvaMessages.clear();

            for (ConfigPaTisMapItem it = m_configPaTisMapItems.begin(); it != m_configPaTisMapItems.end(); ++it)
            {
                if( (it->second) != NULL )
                {
                    delete it->second;
                    it->second = NULL;
                }
            }
            m_configPaTisMapItems.clear();


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
        for (ConfigPaTisMapItem assocToDelete = m_configPaTisMapItems.begin(); assocToDelete != m_configPaTisMapItems.end(); ++assocToDelete)
        {
            if( (assocToDelete->second) != NULL )
            {
                delete assocToDelete->second;
                assocToDelete->second = NULL;
            }
        }
        m_configPaTisMapItems.clear();

        for (ConfigPaDvaMessagesIt assocToDelete = m_configPaDvaMessages.begin(); assocToDelete != m_configPaDvaMessages.end(); ++assocToDelete)
        {
            if( (assocToDelete->second) != NULL )
            {
                delete assocToDelete->second;
                assocToDelete->second = NULL;
            }
        }
        m_configPaDvaMessages.clear();

        // Now reload all the Pa dva message
        IPaDvaMessages paDvaMsgItems = PaDvaMessageAccessFactory::getInstance().getAllPaDvaMessages(true);
        
        for (IPaDvaMessagesIt it = paDvaMsgItems.begin(); it != paDvaMsgItems.end(); ++it)
        {

            TA_Base_Core::IConfigPaDvaMessage* configItem = dynamic_cast<TA_Base_Core::IConfigPaDvaMessage*>(*it);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getUniqueIdentifier();
                m_configPaDvaMessages[key] = configItem;
            }
        }


        // Now reload all the PA Tis Map and PA Zones
        IPaTisMessageMaps items = PaTisMessageMappingAccessFactory::getInstance().getAllPaTisMessageMappings(true);//getAllPaDvaMessages(true);

        // And insert the PA Zones into our map. At the same time cast them to IConfigPaTisMessageMap*
        // instead of IPaTisMessageMap*.
        for (IPaTisMessageMapsIt it = items.begin(); it != items.end(); ++it)
        {
            IPaTisMessageMap* item = *it;
            TA_Base_Core::IConfigPaTisMessageMap* configItem = dynamic_cast<IConfigPaTisMessageMap*>(item);
            if( configItem != NULL )
            {
                unsigned long key = configItem->getPaDvaMessageKey();
                m_configPaTisMapItems[key] = configItem;
            }
        }

        refreshCachedMaps();

        m_isLoaded = true;
        
        FUNCTION_EXIT;
    }


    std::map<CString,unsigned long> PaStationDvaMessageDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        TA_ASSERT(false, "This method cannot be called for this class");

        std::map<CString, unsigned long> temp;
        return temp;
    }


    std::map<unsigned long,PaStationDvaMessageDatabaseAccessor::PaStationDvaMessageRecord> PaStationDvaMessageDatabaseAccessor::getAllItemDetails(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getAllItemDetails");

        std::map<unsigned long,PaStationDvaMessageRecord> details; 

        // The maximum range is the number of locations times by 2. This is because we want to step for each
        // Location while it is loaded and then again while each Location is added to the list.
        progress.SetRange( 0, m_configPaDvaMessages.size() * 2 );

        // Step through all the objects and retrieve the details for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.

        CString buffer;
        for (ConfigPaDvaMessagesIt it = m_configPaDvaMessages.begin(); it != m_configPaDvaMessages.end(); ++it)
        {
            if (it->second != NULL)
            {
                IConfigPaDvaMessage* msg = it->second;
                PaStationDvaMessageRecord record = getRecordFrom(*msg);

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
        ConfigPaDvaMessagesIt matching = m_configPaDvaMessages.find(identifier);
        if (matching == m_configPaDvaMessages.end() || 0 == matching->second)
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

        ConfigPaTisMapItem paTisMapItem = m_configPaTisMapItems.find(item.getKey());
        
        if (m_configPaTisMapItems.end() == paTisMapItem || NULL == paTisMapItem->second)
        {
            record.tisMessageTag = "";
            record.tisLibrarySection = "";
            record.tisLibraryVersion = "";
            record.tisMessageDescription = "#UNKNOWN#";
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not find the pa tis map for %ul", item.getId());
            return record;
        }

        if (TA_Base_Core::DatabaseKey::isInvalidKey(paTisMapItem->second->getTisMessageTag()) )
        {
            record.tisMessageTag = "";
        }
        else
        {
            record.tisMessageTag.Format("%d", paTisMapItem->second->getTisMessageTag());
        }

        if (TA_Base_Core::DatabaseKey::isInvalidKey(paTisMapItem->second->getTisLibrarySection()) )
        {
            record.tisLibrarySection = "";
        }
        else
        {
            record.tisLibrarySection.Format("%d", paTisMapItem->second->getTisLibrarySection());
        }

        if (TA_Base_Core::DatabaseKey::isInvalidKey(paTisMapItem->second->getTisLibraryVersion()) )
        {
            record.tisLibraryVersion = "";
        }
        else
        {
            record.tisLibraryVersion.Format("%d", paTisMapItem->second->getTisLibraryVersion());
        }
        
        if (TA_Base_Core::DatabaseKey::isValidKey(paTisMapItem->second->getTisMessageTag()) &&
            TA_Base_Core::DatabaseKey::isValidKey(paTisMapItem->second->getTisLibrarySection()) )
        {
            try
            {
                IConfigPaTisMessageMap* cfgPaTisMsgMap = paTisMapItem->second;
                TA_Base_Core::TisMessage tm = getTisMessage(*cfgPaTisMsgMap);
                record.tisMessageDescription = tm.description.c_str();
            }
            catch (const TA_Base_Core::TransactiveException&)
            {
                record.tisMessageDescription = "#UNKNOWN#";
            }
        }
        else
        {
            record.tisMessageDescription = "";
        }

        return record;
    }

    
    CString PaStationDvaMessageDatabaseAccessor::getTypeString(const unsigned short type)
    {
        CString result;

        if ( ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_EMERGENCY == type) ||
             ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY == type))
        {
            result.LoadString(IDS_DVA_TYPE_EMERGENCY);
        }
        else if ( ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_NORMAL == type)||
            (  TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_NORMAL == type))
        {
            result.LoadString(IDS_DVA_TYPE_NORMAL);
        }
        else if ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_ATS_MSG == type )
        {
            result.LoadString(IDS_DVA_TYPE_ATS);
        }
        else if (( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_DELETED == type ) || 
            ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK == type ))
        {
            result.LoadString(IDS_DVA_TYPE_DELETED);
        }
        else
        {
            // Any unrecognized types go to unknown
            result.LoadString(IDS_DVA_TYPE_UNKNOWN);
        }

        return result;
    }


    TA_Base_Core::IConfigItem* PaStationDvaMessageDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the PA Zone matching the key passed in.
        ConfigPaDvaMessagesIt matching = m_configPaDvaMessages.find(key);
        if (matching == m_configPaDvaMessages.end() )
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
        for (ConfigPaDvaMessagesIt it = m_configPaDvaMessages.begin(); it != m_configPaDvaMessages.end();  ++it)
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

        try
        {
            std::vector<TA_Base_Core::TisMessage> messages = 
                                        PaTisMessageMappingAccessFactory::getInstance().enumerateTisMessages();
            
            // Store all the data in a map
            for (unsigned int i = 0; i < messages.size(); i ++)
            {
                m_tisMessages[messages[i].pKey] = messages[i];
            }            
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

    bool PaStationDvaMessageDatabaseAccessor::updateConfigItemsTisMessage(IConfigPaTisMessageMap& configItem, unsigned long tisKey)
    {
        unsigned long tisMessageTag = TA_Base_Core::DatabaseKey::getInvalidKey();
        unsigned long tisLibrarySection = TA_Base_Core::DatabaseKey::getInvalidKey();
        unsigned long tisLibraryVersion = TA_Base_Core::DatabaseKey::getInvalidKey();

        if (TA_Base_Core::DatabaseKey::isValidKey(tisKey))
        {           
            TisMessages::iterator msg = m_tisMessages.find(tisKey);
        
            if (msg != m_tisMessages.end())
            {
                tisMessageTag = msg->second.tisMessageTag;
                tisLibrarySection = msg->second.tisLibrarySection;
                tisLibraryVersion = msg->second.tisLibraryVersion;
            }
            else
            {
                throw TA_Base_Core::TransactiveException("Invalid input key");
            }
        }
        
        bool changesMade = false;

        if (configItem.getTisMessageTag() != tisMessageTag)
        {
            configItem.setTisMessageTag(tisMessageTag);
            changesMade = true;
        }
         
        if (configItem.getTisLibrarySection() != tisLibrarySection)
        {
            configItem.setTisLibrarySection(tisLibrarySection);
            changesMade = true;
        }

        if (configItem.getTisLibraryVersion() != tisLibraryVersion)
        {
            configItem.setTisLibraryVersion(tisLibraryVersion);
            changesMade = true;
        }

        return changesMade;
    }


    TA_Base_Core::TisMessage PaStationDvaMessageDatabaseAccessor::getTisMessage(unsigned long key) const
    {
        TisMessages::const_iterator itr = m_tisMessages.find(key);

        if (m_tisMessages.end() != itr)
        {
            return itr->second;
        }
        else
        {
            throw TA_Base_Core::TransactiveException("Unable to find TIS Message");
        }
    }


    std::vector<TA_Base_Core::TisMessage> 
    PaStationDvaMessageDatabaseAccessor::getAllTisMessages()
    {
        std::vector<TA_Base_Core::TisMessage> result;

        for (TisMessages::iterator itr = m_tisMessages.begin(); itr != m_tisMessages.end(); itr ++)
        {
            result.push_back(itr->second);
        }
    
        return result;
    }


    TA_Base_Core::TisMessage 
    PaStationDvaMessageDatabaseAccessor::getTisMessage(IConfigPaTisMessageMap& configItem)
    {
        // The only way for a config DVA Message to obtain it's related TIS Message data is to
        // use its tisLibrarySection and tisMessageTag values
        std::vector<TA_Base_Core::TisMessage> result;
        unsigned long searchTag = configItem.getTisMessageTag();
        unsigned long searchSection = configItem.getTisLibrarySection();

        for (TisMessages::iterator itr = m_tisMessages.begin(); itr != m_tisMessages.end(); itr ++)
        {
            if (itr->second.tisMessageTag == searchTag && itr->second.tisLibrarySection == searchSection)
            {
                return itr->second;
            }
        }

        throw TA_Base_Core::TransactiveException("Unable to find TIS Message");
    }    

    TA_Base_Core::IConfigPaTisMessageMap* PaStationDvaMessageDatabaseAccessor::getPaTisMessageMapByPadmesId( unsigned long padmesId )
    {

        ConfigPaTisMapItem item = m_configPaTisMapItems.find(padmesId);
        if (item == m_configPaTisMapItems.end() || NULL == item->second)
        {
            return NULL;
        }
        else
        {
            return item->second;
        }

    }

    TA_Base_Core::IConfigPaTisMessageMap* PaStationDvaMessageDatabaseAccessor::createPaTisMessageMap( unsigned long padmesId )
    {
        TA_Base_Core::IConfigPaTisMessageMap* paTisMap = getPaTisMessageMapByPadmesId(padmesId);
        if (NULL == paTisMap)
        {
            paTisMap = PaTisMessageMappingAccessFactory::getInstance().createPaTisMessageMapping();
            paTisMap->setPaDvaMessageKey(padmesId);
            m_configPaTisMapItems[padmesId] = paTisMap;
        }
        return paTisMap;
    }

    void PaStationDvaMessageDatabaseAccessor::removePaTisMap( unsigned long padmesId )
    {
        ConfigPaTisMapItem item = m_configPaTisMapItems.find(padmesId);
        if (item != m_configPaTisMapItems.end())
        {
            m_configPaTisMapItems.erase(item);
        }
    }

}

    
