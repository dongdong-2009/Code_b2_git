/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/bus/pa/cachedmaps/src/PaStationCachedMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Maintains a cache of Pa Zone mappings, also listens
  * out for changes to database regarding the PA_STATION table
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/PaStationCachedMap.h"


namespace TA_IRS_Bus
{
    const int MAX_RELATIVE_ZONE = 
        TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MAX_ZONE_ID - TA_Base_Bus::IPAAgentCorbaDef::PA_OCC_MIN_ZONE_ID + 1;

    PaStationCachedMap::PaStationCachedMap()
    :
    GenericCachedMap<PaStationRecord, TA_Base_Core::IPaStation>
        (TA_Base_Core::PA_STATION, "PaStation")
    {

    }


    TA_Base_Core::IPaStation* PaStationCachedMap::getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException)
    {
        return TA_Base_Core::PaStationAccessFactory::getInstance().getPaStation(pKey, readWrite);
    }


    void PaStationCachedMap::validateRecordData(const PaStationRecord& paStation)
    {
        // Validate various parameters (failure on a true condition met)
        performDatavalueValidation(0 == paStation.m_locationKey,           paStation.m_pKeyOrId);
    }


    unsigned long PaStationCachedMap::refreshKeyToPaStationRecordMap(unsigned long locationKey) 
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        TA_Base_Core::IPaStations paStations;

        if (0 == locationKey)
        {
            paStations = TA_Base_Core::PaStationAccessFactory::getInstance().getAllPaStations(false);
        }
        else
        {
            paStations = TA_Base_Core::PaStationAccessFactory::getInstance().getPaStationsByLocationKey(locationKey, false);
        }    
    
        unsigned long failedLoads = 0;

        try
        {
            failedLoads = refreshKeyToRecordMapUsing(paStations);
        }
        catch (...)
        {
            // Clean up (this is defined in GenericCachedMap.inl)
            deleteAllItemsInArray(paStations);
            throw;
        }

        // Clean up (this is defined in GenericCachedMap.inl)
        deleteAllItemsInArray(paStations);

        return failedLoads;
    }


    PaStationRecord PaStationCachedMap::createRecordFromInterface(TA_Base_Core::IPaStation& item)
            //throw (TA_IRS_Core::InvalidPasConfigurationException)
    {
        PaStationRecord result;

        result.m_pKeyOrId = item.getKey();
        result.m_locationKey = item.getLocationKey();

        return result;
    }


    unsigned long PaStationCachedMap::getLocationKeyFromPasStationId(unsigned long stationId)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        const PaStationRecord& record = getCachedDbRecordFromKey(stationId);
        return record.m_locationKey;
    }


    unsigned long PaStationCachedMap::getPasStationIdFromLocationKey(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        FilterByLocationKey filter(locationKey);
        PaStationRecords matchingZones = getFirstMatchingRecord(filter);

        if (matchingZones.size() > 0)
        {
            // Matching item found
            return matchingZones.front().m_pKeyOrId;
        }
    
        // Record not found
        std::ostringstream msg;

        msg << "Failed to locate PaStationRecord with locationKey=" 
            << locationKey << " in KeyToPaStationRecordMap";
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    
        return 0;   
    }

    void PaStationCachedMap::absoluteToRelativeZoneId( const unsigned long absoluteZoneId, unsigned long& relativeZoneId, unsigned long& locationKey ) /*throw(TA_Base_Core::CachedMappingNotFoundException)*/
    {
        unsigned long stationId = (unsigned long) (((float)absoluteZoneId-1) / MAX_RELATIVE_ZONE) +1;

        relativeZoneId = ((absoluteZoneId-1) % MAX_RELATIVE_ZONE) + 1;

        locationKey = getLocationKeyFromPasStationId(stationId);
    }

    void PaStationCachedMap::relativeToAbsoluteZoneId( unsigned long& absoluteZoneId, const unsigned long relativeZoneId, const unsigned long locationKey ) /*throw(TA_Base_Core::CachedMappingNotFoundException)*/
    {
        absoluteZoneId = MAX_RELATIVE_ZONE * (getPasStationIdFromLocationKey(locationKey)-1) + relativeZoneId;
    }

} // end namespace TA_IRS_Bus
