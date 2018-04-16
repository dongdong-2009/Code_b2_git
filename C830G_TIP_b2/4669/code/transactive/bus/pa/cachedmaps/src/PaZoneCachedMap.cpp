/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/cachedmaps/src/PaZoneCachedMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Maintains a cache of Pa Zone mappings, also listens
  * out for changes to database regarding the PA_ZONE table
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "bus/pa/cachedmaps/src/PaZoneCachedMap.h"
#include "core/exceptions/src/PasExceptions.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
// liuyu++ TD8849
#include "core/data_access_interface/src/LocationAccessFactory.h"
// ++liuyu TD8849
#include <stdexcept>


namespace TA_IRS_Bus
{

    PaZoneCachedMap::PaZoneCachedMap(ILocationNameHelper& locationNameHelper)
    :
    GenericCachedMap<PaZoneRecord, TA_Base_Core::IPaZone>
        (TA_Base_Core::PA_ZONE, "PaZone"),
        m_locationNameHelper(locationNameHelper)
    {

    }


    TA_Base_Core::IPaZone* PaZoneCachedMap::getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException)
    {
        return TA_Base_Core::PaZoneAccessFactory::getInstance().getPaZone(pKey, readWrite);
    }


    void PaZoneCachedMap::validateRecordData(const PaZoneRecord& paZone)
    {
        // Validate various parameters (failure on a true condition met)
        performDatavalueValidation(0 == paZone.m_locationKey,           paZone.m_pKey);
        performDatavalueValidation(paZone.m_locationName.length() == 0, paZone.m_pKey);
        performDatavalueValidation(paZone.m_id == 0,                    paZone.m_pKey);
        // liuyu++ TD8849
        performDatavalueValidation(paZone.m_orderId == 0,               paZone.m_pKey);
        // ++liuyu TD8849
        performDatavalueValidation(paZone.m_label.length() == 0,        paZone.m_pKey);
        performDatavalueValidation(paZone.m_area.length() == 0,         paZone.m_pKey);

        // Can't validate entity key, since it may be null    
    }


    unsigned long PaZoneCachedMap::refreshKeyToPaZoneRecordMap(unsigned long locationKey) 
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        TA_Base_Core::IPaZones paZones;
            
        if (-1 == locationKey)
        {
            paZones = TA_Base_Core::PaZoneAccessFactory::getInstance().getAllPaZones(false);
        }
        else
        {
            paZones = TA_Base_Core::PaZoneAccessFactory::getInstance().getPaZonesByLocationKey(locationKey, false);
        }    
    
        unsigned long failedLoads = 0;

        try
        {
            failedLoads = refreshKeyToRecordMapUsing(paZones);
        }
        catch (...)
        {
            // Clean up (this is defined in GenericCachedMap.inl)
            deleteAllItemsInArray(paZones);
            throw;
        }

        // Clean up (this is defined in GenericCachedMap.inl)
        deleteAllItemsInArray(paZones);

        return failedLoads;
    }


    PaZoneRecord PaZoneCachedMap::createRecordFromInterface(TA_Base_Core::IPaZone& item)
            //throw (TA_Base_Core::InvalidPasConfigurationException)
    {
        PaZoneRecord result;

        result.m_pKey = item.getKey();
        result.m_locationKey = item.getLocationKey();
        result.m_locationName = m_locationNameHelper.getLocationNameFromKey(result.m_locationKey);
        result.m_id = item.getId();
        // liuyu++ TD8849
        result.m_orderId = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(result.m_locationKey)->getOrderId();
        // ++liuyu TD8849
        result.m_label = item.getLabel();
        
        std::vector<std::string> stnTokens = getAllTokens(item.getStnEntityName(), ".");
        std::vector<std::string> occTokens = getAllTokens(item.getOccEntityName(), ".");

        try
        {
            // The entity name should be in format <STN>.PA.<Area>.<ShortName>
            // The OCC entity name should be in format <OCC_STN>.PA....

            // First off, take the area detail from wherever we can get it
            if (item.getStnEntityName().length() > 0)
            {
                result.m_area = stnTokens.at(PaZoneRecord::AREA);
            }
            else if (item.getOccEntityName().length() > 0)
            {
                result.m_area = occTokens.at(PaZoneRecord::AREA);
            }
            else
            {
                // Neither entity name is valid, no datapoint associated with this zone
                result.m_area = "Unknown";                  
            }

            // Station entity name isn't always available
            if (item.getStnEntityName().length() > 0)
            {
                result.m_stnZoneShortName = stnTokens.at(PaZoneRecord::ZONESHORTNAME);
            }

            // OCC entity name isn't always available
            if (item.getOccEntityName().length() > 0)
            {
                result.m_occZoneShortName = occTokens.at(PaZoneRecord::ZONESHORTNAME);
                result.m_occLocationName = occTokens.at(PaZoneRecord::STATION);
            }
        }   
        catch (const std::out_of_range&)
        {
            // If there are insufficient input tokens to define one of these members
            TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Bad format for entity name"));
        }   
        

        // Empty entity name is typical case for paging consoles
        result.m_stnEquipmentEntityName = item.getStnEntityName();
        result.m_occEquipmentEntityName = item.getOccEntityName();

        //result.m_entityKey = item.getStatusEntityKey();
        result.m_equipmentEntities = item.getAssociatedEquipmentEntities();

        return result;
    }


    unsigned long PaZoneCachedMap::getKeyFromPaZoneIdAndLocation(unsigned long zoneId, unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        FilterPaZoneByIdAndLocation filter(zoneId, locationKey);
        PaZoneRecords matchingZones = getFirstMatchingRecord(filter);
    
        if (matchingZones.size() > 0)
        {
            // Matching item found
            return matchingZones.front().m_pKey;
        }
    
        // Record not found
        std::ostringstream msg;

        msg << "Failed to locate PaZoneRecord with locationKey=" 
            << locationKey << " and zoneId=" 
            << zoneId << " in KeyToPaZoneRecordMap";
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    
        return 0;   
    }//


    PaZoneCachedMap::FilterPaZoneByIdAndLocation::FilterPaZoneByIdAndLocation(
        unsigned long zoneId,
        unsigned long locationKey)
        :
        m_zoneId(zoneId),
        m_locationKey(locationKey)
    {
    }


    bool PaZoneCachedMap::FilterPaZoneByIdAndLocation::isMatch(
            const PaZoneRecord& record) const
    {
        return record.m_id          == m_zoneId && 
               record.m_locationKey == m_locationKey;
    }


    PaZoneRecords PaZoneCachedMap::getPaZoneRecordsByGroupId(unsigned long groupId) 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::CachedMappingNotFoundException)
    {
        // Obtain array of all zones in this group first off
        std::list<unsigned long> paZoneKeys = 
            TA_Base_Core::PaZoneAccessFactory::getInstance().getPaZoneKeysByGroupId(groupId);

        PaZoneRecords result;

        // Must then confirm each of the zones already exists in local zone cache
        // (otherwise invalid configuration error)
        for (std::list<unsigned long>::iterator it = paZoneKeys.begin(); 
                        it != paZoneKeys.end(); 
                        it ++)
        {       
            // Perform copy assignment into resulting array
            // If the function throws exception, allow it to propogate
            result.push_back(getCachedDbRecordFromKey((*it)));
        }

        return result;
    }

    
    PaZoneRecord PaZoneCachedMap::getRecordFromEquipmentEntityName(const std::string& equipmentEntityName)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
    
        FilterPaZoneByEquipmentEntityName filter(equipmentEntityName);
        PaZoneRecords matchingZones = getFirstMatchingRecord(filter);

        if (matchingZones.size() > 0)
        {
            // Matching item found
            return matchingZones.front();
        }

        // Record not found
        std::ostringstream msg;

        msg << "Failed to locate PaZoneRecord with equipmentEntityName=" 
            << equipmentEntityName << " in NameToPaZoneRecordMap";
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }


    PaZoneCachedMap::FilterPaZoneByEquipmentEntityName::FilterPaZoneByEquipmentEntityName(
        const std::string& equipmentEntityName)
        :
        m_equipmentEntityName(equipmentEntityName)
    {
    }


    bool PaZoneCachedMap::FilterPaZoneByEquipmentEntityName::isMatch(
            const PaZoneRecord& record) const
    {
        // Either match will do - the stn and occ entity names are unique within
        // the ENTITY table
        return record.m_stnEquipmentEntityName == m_equipmentEntityName ||
                record.m_occEquipmentEntityName == m_equipmentEntityName;
    }
    

    PaZoneRecords PaZoneCachedMap::getPaZoneRecordsFromArea(const std::string& area)
    {   
        FilterPaZoneByArea filter(area);
        return getAllRecords(&filter, false);
    }


    PaZoneRecords PaZoneCachedMap::getPaZoneRecordsFromLocationKey(unsigned long locationKey)
    {
        FilterPaZoneByLocationKey filter(locationKey);
        return getAllRecords(&filter, false);
    }


    std::vector<unsigned long> PaZoneCachedMap::getZoneIdsUsingFilter(ICachedMapFilter<PaZoneRecord>* filter)
    {
        std::vector<unsigned long> result;
    
        // Fetch all records matching supplied details
        PaZoneRecords matchingRecords = getAllRecords(filter, false);
    
        for (PaZoneRecordsIt it = matchingRecords.begin();
                it != matchingRecords.end();
                it ++)
        {
            result.push_back((*it).m_pKey);
        }

        return result;
    }

} // end namespace TA_IRS_Bus