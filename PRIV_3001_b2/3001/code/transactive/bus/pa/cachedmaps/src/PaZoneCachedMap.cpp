/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/bus/pa/cachedmaps/src/PaZoneCachedMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
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
        performDatavalueValidation(paZone.m_strDispLocName.length() == 0, paZone.m_pKey);
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
            
        if (0 == locationKey)
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
        result.m_strDispLocName = m_locationNameHelper.getLocationNameFromKey( result.m_locationKey );
        result.m_id = item.getId();
        result.m_orderId = m_locationNameHelper.getLocationOrderIdFromKey( result.m_locationKey );
        result.m_label = item.getLabel();
        result.m_stnEquipmentEntityName = item.getStnEntityName();
        result.m_occEquipmentEntityName = item.getOccEntityName();


        std::vector<std::string> stnTokens = getAllTokens(result.m_stnEquipmentEntityName, ".");
        std::vector<std::string> occTokens = getAllTokens(result.m_occEquipmentEntityName, ".");

        // The OCC entity name should be in format <OCC_STN>.PA.<Area>.<ShortName>
        if ( !result.m_occEquipmentEntityName.empty() )
        {
            if ( occTokens.size() > PaZoneRecord::ZONESHORTNAME )
            {
                result.m_strZoneOccLocName = occTokens[PaZoneRecord::STATION];
                result.m_area = occTokens[PaZoneRecord::AREA];
                result.m_occZoneShortName = occTokens[PaZoneRecord::ZONESHORTNAME];
            }
            else
            {
                // If there are insufficient input tokens to define one of these members
                TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Bad format for entity name"));
            }
        }

        // The entity name should be in format <STN>.PA.<Area>.<ShortName>
        if ( !result.m_stnEquipmentEntityName.empty() )
        {
            if ( stnTokens.size() > PaZoneRecord::ZONESHORTNAME )
            {
                result.m_strZoneStnLocName = stnTokens[PaZoneRecord::STATION];
                result.m_area = stnTokens[PaZoneRecord::AREA];
                result.m_stnZoneShortName = stnTokens[PaZoneRecord::ZONESHORTNAME];
            }
            else
            {
                // If there are insufficient input tokens to define one of these members
                TA_THROW(TA_Base_Core::InvalidPasConfigurationException("Bad format for entity name"));
            }
        }

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


    PaZoneRecords PaZoneCachedMap::getRecordsSimilarToEquipmentEntityName(
        const bool bOcc, const std::string& equipmentEntityName)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        std::set<std::string> setNames;
        setNames.insert( equipmentEntityName );

        FilterPaZoneSimilarToEquipmentEntityName filter(bOcc, setNames);

        // Matching item found
        return getAllRecords(&filter, false);
    }

    PaZoneRecords PaZoneCachedMap::getRecordsSimilarToEquipmentEntityNames( const bool bOcc, const std::set<std::string>& zoneNames )
    {
        FilterPaZoneSimilarToEquipmentEntityName filter(bOcc, zoneNames);

        // Matching item found
        return  getAllRecords(&filter, false);
    }

    PaZoneCachedMap::FilterPaZoneByEquipmentEntityName::FilterPaZoneByEquipmentEntityName(
        const std::string& equipmentEntityName) :
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

    PaZoneCachedMap::FilterPaZoneSimilarToEquipmentEntityName::FilterPaZoneSimilarToEquipmentEntityName(
        const bool bOcc, const std::set<std::string>& equipmentEntityNames) :
    m_bOcc( bOcc ),
    m_vecSplittedZoneName()
    {
        ZoneNameTokenVec vecEquipmentToken;
        for ( std::set<std::string>::const_iterator itLoop = equipmentEntityNames.begin(); 
            itLoop != equipmentEntityNames.end(); ++itLoop )
        {
            vecEquipmentToken = GenericCachedMapBase::getAllTokens( *itLoop, "." );
            if ( vecEquipmentToken.size() <= PaZoneRecord::ZONESHORTNAME )
            {
                continue;
            }

            m_vecSplittedZoneName.push_back( vecEquipmentToken );
        }
    }

    bool PaZoneCachedMap::FilterPaZoneSimilarToEquipmentEntityName::isMatch(
        const PaZoneRecord& record) const
    {
        // Initial value is false so if no condition, no record matched.
        bool bMatch = false;

        for ( std::vector<ZoneNameTokenVec>::const_iterator itLoop = m_vecSplittedZoneName.begin(); 
            itLoop != m_vecSplittedZoneName.end(); ++itLoop )
        {
            // For each condition, set to true first and then validate by the following assessment
            bMatch = true;

            bMatch &= ( 0 == record.m_area.compare( (*itLoop)[PaZoneRecord::AREA] ));
            if ( !bMatch )
            {
                continue;
            }

            if ( m_bOcc )
            {
                bMatch &= ( 0 == record.m_strZoneOccLocName.compare( (*itLoop)[PaZoneRecord::STATION] ));
                if ( !bMatch )
                {
                    continue;
                }
                bMatch &= ( 0 == record.m_occZoneShortName.compare( (*itLoop)[PaZoneRecord::ZONESHORTNAME] ));
                if ( !bMatch )
                {
                    continue;
                }
            }
            else
            {
                bMatch &= ( 0 == record.m_strZoneStnLocName.compare( (*itLoop)[PaZoneRecord::STATION] ));
                if ( !bMatch )
                {
                    continue;
                }
                bMatch &= ( 0 == record.m_stnZoneShortName.compare( (*itLoop)[PaZoneRecord::ZONESHORTNAME] ));
                if ( !bMatch )
                {
                    continue;
                }
            }

            if ( bMatch )
            {
                break;
            }
        }

        return bMatch;
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