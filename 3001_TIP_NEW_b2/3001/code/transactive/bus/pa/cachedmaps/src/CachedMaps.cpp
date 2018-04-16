/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/bus/pa/cachedmaps/src/CachedMaps.cpp $
  * @author Sean Liew
  * @version $Revision: #12 $
  * Last modification: $DateTime: 2015/12/07 18:28:43 $
  * Last modified by: $Author: xiaoxia.li $
  * 
  * Maintains all the cached maps for PA applications
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <algorithm>
#include <string>
#include <set>

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DataConversion.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaDvaVersionCachedMap.h"
#include "bus/pa/cachedmaps/src/ICachedMapObserver.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"


using namespace TA_Base_Core;
//using namespace TA_IRS_Bus;

namespace TA_IRS_Bus
{
    const char* const PA_STN_ZONEA_FIRST_STATUS_TOKEN = "diiPAZONEA-1stAmpNormalAlarm";
    const char* const PA_STN_ZONEA_SECOND_STATUS_TOKEN = "diiPAZONEA-2ndAmpNormalAlarm";
    const char* const PA_STN_ZONEB_STATUS_TOKEN = "diiPAZONEB-Status";
    const char* const PA_STN_ZONEC_STATUS_TOKEN = "diiPAZONEC-SumStatus";
    const char* const PA_STN_ZONED_STATUS_TOKEN = "diiPAZONED-Status";
    const char* const PA_OCC_ZONEA_FIRST_STATUS_TOKEN = "eiiPAZONEA-1stAmpNormalAlarm";
    const char* const PA_OCC_ZONEA_SECOND_STATUS_TOKEN = "eiiPAZONEA-2ndAmpNormalAlarm";
    const char* const PA_OCC_ZONEB_STATUS_TOKEN = "eiiPAZONEB-Status";
    const char* const PA_OCC_ZONEC_STATUS_TOKEN = "eiiPAZONEC-SumStatus";
    const char* const PA_OCC_ZONED_STATUS_TOKEN = "eiiPAZONED-Status";
    const char* const PA_ZONE_DVA_TOKEN = "diiPAC-Dva";
    const char* const PA_ZONE_LIVE_TOKEN = "diiPAC-Live";
    const char* const PA_ZONE_CYCLIC_TOKEN = "diiPAC-Cyclic";
    const char* const PA_ZONE_MUSIC_TOKEN = "diiPAC-Music";
    const char* const PA_OCC_MAJOR_ALARM_TOKEN = "eiiPAHW-MajorNormalAlarm";
    const char* const PA_OCC_MINOR_ALARM_TOKEN = "eiiPAHW-MinorNormalAlarm";

    CachedMaps* CachedMaps::m_me = NULL;

    NonReEntrantThreadLockable CachedMaps::m_lockForSingleton;

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    CachedMaps::CachedMaps() : 
       m_isDataPointMapInitilized(false),
       m_paZoneCachedMap(new PaZoneCachedMap(m_locationNameHelper)),
       m_paZoneGroupCachedMap(new PaZoneGroupCachedMap()),
       m_paStationDvaMessageCachedMap(new PaStationDvaMessageCachedMap()),
       m_paTrainDvaMessageCachedMap(new PaTrainDvaMessageCachedMap()),
       m_paDvaVersionCachedMap(new PaDvaVersionCachedMap()),
       m_paAtsTriggeredBroadcastCachedMap(new PaAtsTriggeredBroadcastCachedMap()),
       m_paStationCachedMap(new PaStationCachedMap())
    {
        m_zoneIdToStatusDataPointMap.clear();
        m_zoneIdToDvaDataPointMap.clear();
        m_zoneIdToLiveDataPointMap.clear();
        m_zoneIdToCyclicDataPointMap.clear();
        m_zoneIdToMusicDataPointMap.clear();

        refreshLocationKeyToNameMap();
    }


    CachedMaps::~CachedMaps()
    {
        // Delete all existing heap objects
        try
        {
            delete m_paZoneCachedMap;
            m_paZoneCachedMap = 0;
    
            delete m_paZoneGroupCachedMap;
            m_paZoneGroupCachedMap = 0;
    
            delete m_paStationDvaMessageCachedMap;
            m_paStationDvaMessageCachedMap = 0;

            delete m_paTrainDvaMessageCachedMap;
            m_paTrainDvaMessageCachedMap = 0;

            delete m_paDvaVersionCachedMap;
            m_paDvaVersionCachedMap = 0;

            delete m_paAtsTriggeredBroadcastCachedMap;
            m_paAtsTriggeredBroadcastCachedMap = 0;

            delete m_paStationCachedMap;
            m_paStationCachedMap = 0;
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    CachedMaps* CachedMaps::getInstance()
    {
	    if ( 0 == m_me )
	    {
		    // Double checking to prevent multiple threads
		    // creating multiple instances.

            // Any lock is okay at this stage.
		    ThreadGuard guard( m_lockForSingleton );

		    if ( 0 == m_me )
		    {
			    m_me = new CachedMaps();
    	    }
	    }

	    return m_me;
    }

    void CachedMaps::removeInstance( )
    {
	    // 
	    // Guard this to prevent multiple threads atempting
	    // to delete/create simultaneously
	    //
	    ThreadGuard guard( m_lockForSingleton );
	    if ( m_me != NULL )
	    {
		    delete m_me;
		    m_me = NULL;
	    }
    }

    void CachedMaps::validateAllInitialisedMaps() //throw(TA_Base_Core::InvalidPasConfigurationException)
    {
        // These will only be executed if the associated maps have been initialised.
    }

    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Train DVA Message Access
    //
    ///////////////////////////////////////////////////////////////////////////

    void CachedMaps::refreshKeyToTrainDvaMessageRecordMap() 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        m_paTrainDvaMessageCachedMap->refreshKeyToTrainDvaMessageRecordMap();
    }


    TrainDvaMessageRecord CachedMaps::getTrainDvaMessageRecordFromKey(unsigned long key)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paTrainDvaMessageCachedMap->getCachedDbRecordFromKey(key);
    }


    TrainDvaMessageRecords CachedMaps::getTrainDvaMessageRecords(const std::string& filterKeyword)
    {
        return m_paTrainDvaMessageCachedMap->getTrainDvaMessageRecords(filterKeyword);
    }


    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Station DVA Message Access
    //
    ///////////////////////////////////////////////////////////////////////////

    unsigned long CachedMaps::getKeyFromStationDvaMessageIdAndLocation(unsigned long messageId, unsigned long locationKey)
            //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paStationDvaMessageCachedMap->getKeyFromStationDvaMessageIdAndLocation(messageId, locationKey);
    }


    StationDvaMessageRecord CachedMaps::getStationDvaMessageRecordFromKey(unsigned long key)
       //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paStationDvaMessageCachedMap->getCachedDbRecordFromKey(key);
    }

    TA_IRS_Bus::StationDvaMessageRecord CachedMaps::getStationDvaMessageRecordFromIdAndLocation( 
        unsigned long messageId, unsigned long locationKey )
    //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paStationDvaMessageCachedMap->getStationDvaMessageRecordFromIdAndLocation(messageId, locationKey);
    }


    unsigned long CachedMaps::getStationDvaMessageLocationKeyFromKey(unsigned long key)
            //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return getStationDvaMessageRecordFromKey(key).ulLocationKey;
    }


    StationDvaMessageRecords CachedMaps::getClonedAdhocMessageRecords()
    {
        return m_paStationDvaMessageCachedMap->getClonedAdhocMessageRecords();
    }


    unsigned long CachedMaps::getStationDvaMessageIdFromKey(unsigned long key)
            //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return getStationDvaMessageRecordFromKey(key).usMsgId;
    }


    bool CachedMaps::processDvaMessageUpdates(const TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData& updateData)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paStationDvaMessageCachedMap->processDvaMessageUpdates(updateData);
    }

    bool CachedMaps::processDvaVersionUpdates( const TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords& updateData )
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paDvaVersionCachedMap->processDvaVersionUpdates(updateData);
    }

    void CachedMaps::setStationDvaMessageRecord(StationDvaMessageRecord& record, bool writeChangesToDatabase)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException)
    {
        m_paStationDvaMessageCachedMap->setStationDvaMessageRecord(record, writeChangesToDatabase);
    }


    StationDvaMessageRecords CachedMaps::getClonedPrerecordedMessageRecords( 
        const bool bIsOcc, const unsigned short messageType, const std::string& filterKeyword)
    {
        return m_paStationDvaMessageCachedMap->getClonedPrerecordedMessageRecords(
            bIsOcc, messageType, filterKeyword);
    }


    void CachedMaps::refreshKeyToStationDvaMessageRecordMap(unsigned long locationKey) 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        m_paStationDvaMessageCachedMap->refreshKeyToStationDvaMessageRecordMap(locationKey);
    }
    

    void CachedMaps::registerPaStationDvaMessageChangeObserver(ICachedMapObserver* observer)
    {
        m_paStationDvaMessageCachedMap->registerObserver(observer);
    }


    void CachedMaps::deRegisterPaStationDvaMessageChangeObserver(ICachedMapObserver* observer)
    {
        m_paStationDvaMessageCachedMap->deRegisterObserver(observer);
    }

    bool CachedMaps::isPreRecordedMessage(unsigned long messageKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        unsigned short usMsgType = getStationDvaMessageRecordFromKey(messageKey).usMsgType;

        if (( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_EMERGENCY == usMsgType ) || 
            ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_PRE_RECORD_NORMAL == usMsgType ))
        {
            return true;
        }

        return false;
    }

    bool CachedMaps::isAdhocMessage( unsigned long messageKey )
    {
        unsigned short usMsgType = getStationDvaMessageRecordFromKey(messageKey).usMsgType;

        if (( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_EMERGENCY == usMsgType ) || 
            ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_NORMAL == usMsgType ) || 
            ( TA_Base_Bus::IPAAgentCorbaDef::PAS_DVA_TYPE_AD_HOC_BLANK== usMsgType ))
        {
            return true;
        }

        return false;
    }

    ///////////////////////////////////////////////////////////////////////////
    //
    //                      Station DVA Message Version Access
    //
    ///////////////////////////////////////////////////////////////////////////

    void CachedMaps::refreshKeyToDvaVersionRecordMap(unsigned long locationKey) 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        m_paDvaVersionCachedMap->refreshKeyToRecordMap(locationKey);
    }

    DvaVersionRecord CachedMaps::getDvaVersionRecordFromLocationKey(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paDvaVersionCachedMap->getRecordFromLocation(locationKey);
    }

    unsigned long CachedMaps::getKeyFromDvaVersionRecordLocationKey(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paDvaVersionCachedMap->getKeyFromLocation(locationKey);
    }


    std::string CachedMaps::getDvaMessageVersionFromStationDvaMessageKey(unsigned long stationDvaMessageKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        StationDvaMessageRecord messageRecord = 
            getStationDvaMessageRecordFromKey(stationDvaMessageKey);

        DvaVersionRecord versionRecord = getDvaVersionRecordFromLocationKey(messageRecord.ulLocationKey);
        return getDvaMessageVersionFromStationDvaMessage(messageRecord, versionRecord);
    }


    std::string CachedMaps::getDvaMessageVersionFromStationDvaMessage(const StationDvaMessageRecord& messageRecord,
                            const DvaVersionRecord& versionRecord)
    {
		if (versionRecord.ulLocationKey != messageRecord.ulLocationKey)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Mismatched location keys" );
			return "";
		}

        return versionRecord.strVersionNumber;
    }

    void CachedMaps::setDvaMessageVersionRecord(DvaVersionRecord& record, bool writeChangesToDatabase)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::DataConfigurationException)
    {
        m_paDvaVersionCachedMap->setRecord(record, writeChangesToDatabase);
    }


    //////////////////////////////////////////////////////////////////////
    //
    // LocationKeyToNameMap methods
    //
    //////////////////////////////////////////////////////////////////////
    void CachedMaps::refreshLocationKeyToNameMap() 
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException)
    {
        // Get all location records. 

        ThreadGuard guard( m_lockLocationCacheMap );

        for ( LocationCacheMapIt itClearLoop = m_mapLocationCache.begin(); 
            itClearLoop != m_mapLocationCache.end(); ++itClearLoop )
        {
            if ( NULL != itClearLoop->second )
            {
                delete itClearLoop->second;
                itClearLoop->second = NULL;
            }
        }

        m_mapLocationCache.clear();

        std::vector<ILocation*> allLocations = LocationAccessFactory::getInstance().getAllLocations(false);
        
        for ( std::vector<ILocation*>::iterator itLocLoop = allLocations.begin();
            itLocLoop != allLocations.end(); ++itLocLoop )
        {
            unsigned long locationKey  = (*itLocLoop)->getKey();
        
            // No point checking for one-to-one relationship as the database constraints 
            // should prevent this.
            m_mapLocationCache[locationKey] = *itLocLoop;
        }
    }


    std::string CachedMaps::getLocationNameFromKey(unsigned long locationKey) 
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        ThreadGuard guard( m_lockLocationCacheMap);

        LocationCacheMapIt it = m_mapLocationCache.find(locationKey);
	    if (it == m_mapLocationCache.end())
	    {
            std::ostringstream msg;
            msg << "Failed to locate locationName for locationKey=" << locationKey << " in LocationCacheMap";
		    TA_THROW( CachedMappingNotFoundException(msg.str().c_str()) );
	    }

	    return it->second->getDisplayName();
    }

    int CachedMaps::getLocationTypeFromKey( unsigned long locationKey )
    {
        ThreadGuard guard( m_lockLocationCacheMap );

        LocationCacheMapIt it = m_mapLocationCache.find( locationKey );

        return ( it != m_mapLocationCache.end() ) ? it->second->getLocationType() : TA_Base_Core::ILocation::STATION;
    }

    unsigned long CachedMaps::getLocationOrderIdFromKey( unsigned long locationKey )
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        ThreadGuard guard( m_lockLocationCacheMap);

        LocationCacheMapIt it = m_mapLocationCache.find(locationKey);
        if (it == m_mapLocationCache.end())
        {
            std::ostringstream msg;
            msg << "Failed to locate location order for locationKey=" << locationKey << " in LocationCacheMap";
            TA_THROW( CachedMappingNotFoundException(msg.str().c_str()) );
        }

        return it->second->getOrderId();
    }


    unsigned long CachedMaps::getLocationKeyFromName(const std::string& locationName) 
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        ThreadGuard guard( m_lockLocationCacheMap );

        LocationCacheMapIt it = m_mapLocationCache.begin();
        for ( ; it != m_mapLocationCache.end(); ++it)
        {
            if (it->second->getName() == locationName)
            {
                return it->first;
            }
        }

        std::ostringstream msg;
        msg << "Failed to locate locationKey for locationName=" << locationName 
            << " in LocationKeyToNameMap";
	    TA_THROW(CachedMappingNotFoundException(msg.str().c_str()));
    }


    //////////////////////////////////////////////////////////////////////
    //
    // PasStationIdToLocationNameMap methods
    //
    //////////////////////////////////////////////////////////////////////
    void CachedMaps::refreshKeyToPaStationRecordMap(unsigned long locationKey)
    {
        m_paStationCachedMap->refreshKeyToPaStationRecordMap(locationKey);
    }


    unsigned int CachedMaps::getPasStationIdFromLocationKey(unsigned long locationKey)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paStationCachedMap->getPasStationIdFromLocationKey(locationKey);
    }


    unsigned long CachedMaps::getLocationKeyFromPasStationId(unsigned int stationId)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        return m_paStationCachedMap->getLocationKeyFromPasStationId(stationId);
    }


    //////////////////////////////////////////////////////////////////////
    //
    // ZoneIdToDataPointMaps methods
    //
    //////////////////////////////////////////////////////////////////////
    void CachedMaps::refreshDataPointMap( const std::vector<TA_Base_Bus::DataPoint*>& pasDataPoints, const bool bOcc )
        //throw(TA_Base_Core::InvalidPasConfigurationException)
    {
        ThreadGuard guardZone( m_lockForZoneIdToDataPointMaps );
        ThreadGuard guardStation( m_lockForStationIdPointMaps );

        m_zoneIdToStatusDataPointMap.clear();
        m_zoneIdToDvaDataPointMap.clear();
        m_zoneIdToCyclicDataPointMap.clear();
        m_zoneIdToLiveDataPointMap.clear();
        m_zoneIdToMusicDataPointMap.clear();

        m_majorAlarmDataPointMap.clear();
        m_minorAlarmDataPointMap.clear();

        std::vector<TA_Base_Bus::DataPoint*>::const_iterator it;
        for (it = pasDataPoints.begin(); it!=pasDataPoints.end(); ++it)
        {
            TA_Base_Bus::DataPoint* dp = *it;

            // Determine the target map
            std::string dpName = dp->getDataPointName();
            ZoneIdToDataPointMap* theZoneMap = NULL;
            StationIdToDataPointMap* theStationMap = NULL;

            bool bUnique = false;

            if (dpName.find(PA_ZONE_DVA_TOKEN) != std::string::npos)
            {
                theZoneMap = &m_zoneIdToDvaDataPointMap;
            }
            else if (dpName.find(PA_ZONE_CYCLIC_TOKEN) != std::string::npos)
            {
                theZoneMap = &m_zoneIdToCyclicDataPointMap;
            }
            else if (dpName.find(PA_ZONE_LIVE_TOKEN) != std::string::npos)
            {
                theZoneMap = &m_zoneIdToLiveDataPointMap;
            }
            else if (dpName.find(PA_ZONE_MUSIC_TOKEN) != std::string::npos)
            {
                theZoneMap = &m_zoneIdToMusicDataPointMap;
            }
            else if ( bOcc && ( dpName.find( PA_OCC_MAJOR_ALARM_TOKEN ) != std::string::npos ))
            {
                theStationMap = &m_majorAlarmDataPointMap;
            }
            else if ( bOcc && ( dpName.find( PA_OCC_MINOR_ALARM_TOKEN ) != std::string::npos ))
            {
                theStationMap = &m_minorAlarmDataPointMap;
            }
            else
            {
                if ( bOcc )
                {
                    if (( dpName.find(PA_OCC_ZONEA_FIRST_STATUS_TOKEN) != std::string::npos ) || 
                        ( dpName.find(PA_OCC_ZONEA_SECOND_STATUS_TOKEN) != std::string::npos ) || 
                        ( dpName.find(PA_OCC_ZONEB_STATUS_TOKEN) != std::string::npos ) || 
                        ( dpName.find(PA_OCC_ZONEC_STATUS_TOKEN) != std::string::npos ) || 
                        ( dpName.find(PA_OCC_ZONED_STATUS_TOKEN) != std::string::npos ))
                    {
                        bUnique = true;
                        theZoneMap = &m_zoneIdToStatusDataPointMap;
                    }
                }
                else
                {
                    if (( dpName.find(PA_STN_ZONEA_FIRST_STATUS_TOKEN) != std::string::npos ) || 
                        ( dpName.find(PA_STN_ZONEA_SECOND_STATUS_TOKEN) != std::string::npos ) || 
                        ( dpName.find(PA_STN_ZONEB_STATUS_TOKEN) != std::string::npos ) || 
                        ( dpName.find(PA_STN_ZONEC_STATUS_TOKEN) != std::string::npos ) || 
                        ( dpName.find(PA_STN_ZONED_STATUS_TOKEN) != std::string::npos ))
                    {
                        bUnique = true;
                        theZoneMap = &m_zoneIdToStatusDataPointMap;
                    }
                }
            }

            if ( NULL != theZoneMap )
            {
                refreshZoneToDataPointMap( bUnique, bOcc, *theZoneMap, *dp );
            }
            else if ( bOcc && ( NULL != theStationMap ))
            {
                refreshStationToDataPointMap( *theStationMap, *dp );
            }
        }

        m_isDataPointMapInitilized = true;
    }

    void CachedMaps::refreshZoneToDataPointMap( const bool bUnique, const bool bOcc, ZoneIdToDataPointMap& refMap, TA_Base_Bus::DataPoint& refData )
    {
        std::string dpName = refData.getDataPointName();
        unsigned long zoneId = 0u;
        std::string equipmentName = "";

        if ( bUnique )
        {
            equipmentName = dpName;

            PaZoneRecord stRecord;
            try
            {
                stRecord = m_paZoneCachedMap->getRecordFromEquipmentEntityName( equipmentName );
            }
            catch ( TA_Base_Core::CachedMappingNotFoundException& )
            {
                // Ignore - some OCC datapoints may not have been tied to PA Zones yet.
                return;
            }

            if ( bOcc )
            {
                // This is a OCC copy of the station datapoint.  So use absolute addressing
                m_paStationCachedMap->relativeToAbsoluteZoneId( zoneId, stRecord.m_id, stRecord.m_locationKey );
            }
            else
            {
                zoneId = stRecord.m_id;
            }

            // Check if mapping already exists.
            if ( refMap.find( zoneId ) != refMap.end() )
            {
                // Map has already been added, relationship is not one-to-one.
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Inconsistent PAS configuration detected. ZoneId=%lu is mapped to more than one datapoint.", zoneId );
                return;
            }

            refMap[zoneId] = &refData;
        }
        else
        {
            std::string::size_type lastDotOffset = dpName.find_last_of( '.' );
            if ( lastDotOffset == std::string::npos )
            {
                // Ignore
                return;
            }

            equipmentName = dpName.substr( 0, lastDotOffset );

            PaZoneRecords lstRecords;

            try
            {
                lstRecords = m_paZoneCachedMap->getRecordsSimilarToEquipmentEntityName( bOcc, equipmentName );
            }
            catch ( TA_Base_Core::CachedMappingNotFoundException& )
            {
                // Ignore - some OCC datapoints may not have been tied to PA Zones yet.
                return;
            }

            for ( PaZoneRecordsIt itLoop = lstRecords.begin(); lstRecords.end() != itLoop; ++itLoop )
            {
                if ( bOcc )
                {
                    // This is a OCC copy of the station datapoint.  So use absolute addressing
                    m_paStationCachedMap->relativeToAbsoluteZoneId( zoneId, itLoop->m_id, itLoop->m_locationKey );
                }
                else
                {
                    zoneId = itLoop->m_id;
                }

                // Check if mapping already exists.
                if ( refMap.find( zoneId ) != refMap.end() )
                {
                    // Map has already been added, relationship is not one-to-one.
                    std::ostringstream msg;
                    msg << "Inconsistent PAS configuration detected. ZoneId=" << zoneId
                        << " is mapped to more than one datapoint.";
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str() );
                    continue;
                }

                refMap[zoneId] = &refData;
            }
        }
    }

    void CachedMaps::refreshStationToDataPointMap( StationIdToDataPointMap& refMap, TA_Base_Bus::DataPoint& refData )
    {
        std::string strStationId = refData.getDataPointAddress();

        if ( strStationId.empty() || std::string::npos != strStationId.find_first_not_of( "0123456789" ) )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "[DataError] Invalid address %s for station DataPoint", strStationId.c_str() );
            return;
        }

        unsigned short usStationId = static_cast<unsigned short>(TA_Base_Core::DataConversion::toUnsignedInteger( strStationId ));

        refMap[usStationId] = &refData;
    }

    TA_Base_Bus::DataPoint* CachedMaps::getDataPointFromZoneId(unsigned long zoneId, ZoneIdToDataPointMap& theMap)
    {
		if (!m_isDataPointMapInitilized)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "ZoneIdToDataPointMaps not initialised" );
			return NULL;
		}

        ThreadGuard guard( m_lockForZoneIdToDataPointMaps );

        ZoneIdToDataPointMapIt it = theMap.find(zoneId);

	    if (it == theMap.end())
	    {
            return NULL;
	    }

	    return it->second;
    }

    TA_Base_Bus::DataPoint* CachedMaps::getDataPointFromStationId( unsigned short usStationId, StationIdToDataPointMap& theMap )
    {
        ThreadGuard guard( m_lockForStationIdPointMaps );

        StationIdToDataPointMapIt it = theMap.find( usStationId );

        if ( it == theMap.end() )
        {
            return NULL;
        }

        return it->second;
    }

} // end namespace TA_IRS_Bus