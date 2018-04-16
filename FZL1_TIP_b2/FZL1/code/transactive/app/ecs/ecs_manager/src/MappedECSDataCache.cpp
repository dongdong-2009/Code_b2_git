/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/MappedECSDataCache.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include "app/ecs/ecs_manager/src/MappedECSDataCache.h"

#include "core/data_access_interface/ecs/src/ECSMasterModeAccessFactory.h"
#include "core/data_access_interface/ecs/src/ECSStationModeAccessFactory.h"
#include "core/data_access_interface/ecs/src/ECSZoneAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/TAAssert.h"
#include <algorithm>

// using TA_IRS_App::MappedECSDataCache::OrderedLocations;
// using TA_IRS_App::MappedECSDataCache::OrderedLocation;
// using TA_IRS_App::MappedECSDataCache::StationModeKeys;
// using TA_IRS_App::MappedECSDataCache::MasterModeKeys;
// using TA_IRS_App::TA_ECSManagerCommonDefs::StationModeDetail;
// using TA_IRS_App::TA_ECSManagerCommonDefs::MasterModeDetail;
// using TA_IRS_App::TA_ECSManagerCommonDefs::LocationKeys;
// using TA_IRS_App::TA_ECSManagerCommonDefs::StationModes;
// using TA_IRS_App::TA_ECSManagerCommonDefs::StationModesAsStrings;
// using TA_IRS_App::TA_ECSManagerCommonDefs::ZoneDirectionKeys;
// using TA_IRS_App::TA_ECSManagerCommonDefs::EMERGENCY_COMPARE_STRING;

typedef TA_IRS_App::MappedECSDataCache::OrderedLocations OrderedLocations;
typedef TA_IRS_App::MappedECSDataCache::OrderedLocation OrderedLocation;
typedef TA_IRS_App::MappedECSDataCache::StationModeKeys StationModeKeys;
typedef TA_IRS_App::MappedECSDataCache::MasterModeKeys MasterModeKeys;
typedef TA_IRS_App::TA_ECSManagerCommonDefs::StationModeDetail StationModeDetail;
typedef TA_IRS_App::TA_ECSManagerCommonDefs::MasterModeDetail MasterModeDetail;
typedef TA_IRS_App::TA_ECSManagerCommonDefs::LocationKeys LocationKeys;
typedef TA_IRS_App::TA_ECSManagerCommonDefs::StationModes StationModes;
typedef TA_IRS_App::TA_ECSManagerCommonDefs::StationModesAsStrings StationModesAsStrings;
typedef TA_IRS_App::TA_ECSManagerCommonDefs::ZoneDirectionKeys ZoneDirectionKeys;
#define EMERGENCY_COMPARE_STRING TA_IRS_App::TA_ECSManagerCommonDefs::EMERGENCY_COMPARE_STRING

using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::IEntityData;
using TA_IRS_Core::ECSMasterModeAccessFactory;
using TA_IRS_Core::ECSStationModeAccessFactory;
using TA_IRS_Core::ECSZoneAccessFactory;
using TA_Base_Core::LocationAccessFactory;
using TA_IRS_Core::IECSMasterMode;
using TA_IRS_Core::IECSStationMode;
using TA_IRS_Core::IECSZone;
using TA_IRS_Core::IECSZones;
using TA_Base_Core::ILocation;
using TA_Base_Core::DebugUtil;


namespace TA_IRS_App
{

	//zn++ - already get it follow the sequence from DB
    //
    // Global compare function for sorting *ILocation by order.
    //
    /*bool locationOrderCompareFromILocation( ILocation* pIlocation1, ILocation* pIlocation2 )
    {
        //
        // TODO change this to use actual citybound order when ILocation is extended to provide it!
        // For now, we just assume the Key order is Citybound Station Order.
        //
        return ( pIlocation1->getKey() < pIlocation2->getKey() );
    }*/
	//++zn


    //
    // Constructor
    //
    MappedECSDataCache::MappedECSDataCache()
        : m_isStationModeDataForAllLocationsInitialised( false ), m_isNormalMasterModeDataInitialised ( false ), 
        m_isEmergencyMasterModeDataInitialised( false ), m_isOrderedLocationDataInitialised( false ), 
        m_isZoneEquipmentNameDataInitialised( false )
    {
        FUNCTION_ENTRY( "Constructor" );

        //
        // Nothing else to initialise until refresh or accessor methods are 
        // invoked by client code.
        //

        FUNCTION_EXIT;
    }


    //
    // Destructor
    //
    MappedECSDataCache::~MappedECSDataCache()
    {
        FUNCTION_ENTRY( "Destructor" );

        //
        // nothing to clean up
        //

        FUNCTION_EXIT;
    }


    //
    // refreshOrderedLocationData
    //
    void MappedECSDataCache::refreshOrderedLocationData()
    {
        FUNCTION_ENTRY( "refreshOrderedLocationData" );

		//zn++ - Launching manager optimise
		if (m_isOrderedLocationDataInitialised)
		{
			return;
		}
		//++zn

        // Reset all related data caching members.
        m_orderedLocations.clear();
        m_locationKeyToOrderIndex.clear();

        /*// Get all location's configuration data and sort them by order.
        std::vector< ILocation* > allLocations = LocationAccessFactory::getInstance().getAllLocations();
        std::sort( allLocations.begin(), allLocations.end(), locationOrderCompareFromILocation );
        std::vector< ILocation* >::iterator allLocationsIt;

        // load OrderedLocation data for all locations that are stations.
        OrderedLocation orderedLocation;
        allLocationsIt = allLocations.begin();
        while ( allLocationsIt != allLocations.end() )
        {
            // Verify if location is Station by checking for existance of 
            // associated StationModes
            if ( false ==  ECSStationModeAccessFactory::getInstance()
                .getECSStationModeKeysWhere( ( *allLocationsIt )->getKey(), -1, "", "", "" ).empty() )
            {
                orderedLocation.key = ( *allLocationsIt )->getKey();
                orderedLocation.name = ( *allLocationsIt )->getName();
                m_orderedLocations.push_back( orderedLocation );
                m_locationKeyToOrderIndex[ orderedLocation.key ] = ( m_orderedLocations.size() - 1 );
            }
            allLocationsIt++;
        }

        m_isOrderedLocationDataInitialised = true;

        //
        // Clean up Location objects allocated on the heap by location access factory.
        //
        allLocationsIt = allLocations.begin();
        while ( allLocationsIt != allLocations.end() )
        {
            delete ( *allLocationsIt );
            allLocationsIt++;
        }*/

		//zn++ - Launching manager optimise
		m_orderedLocations = ECSStationModeAccessFactory::getInstance().getOrderedLocations();
		
		OrderedLocations::iterator iter;
		unsigned int count(0);

		for (iter = m_orderedLocations.begin(); iter != m_orderedLocations.end(); ++iter)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[LM] Key %d, Name %s", iter->key, iter->name.c_str());
			m_locationKeyToOrderIndex[ iter->key ] = count;
			++count;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[LM] total locations: %d", count);

		m_isOrderedLocationDataInitialised = true;
		//++zn

        FUNCTION_EXIT;
    }

    
    //
    // refreshStationModeData
    //
    void MappedECSDataCache::refreshStationModeData( unsigned long locationKey /*=0*/ )
    {
        FUNCTION_ENTRY( "refreshStationModeData" );

        // Reset all related cached data.
        if ( locationKey == 0 )
        {
            m_stationModeKeyToDetail.clear();
            m_locationKeyToStationModeKeys.clear();
            m_locationAndModeToStationModeKey.clear();
            m_isStationModeDataForAllLocationsInitialised = false;
        }
        else
        {
            // Only reset that data related to the specified locationKey.
            StationModeKeyToDetail::iterator stationModeKeyToDetailIt;
            std::pair< LocationKeyToStationModeKeys::iterator, LocationKeyToStationModeKeys::iterator >
                range = m_locationKeyToStationModeKeys.equal_range( locationKey );
            while ( range.first != range.second )
            {
                m_stationModeKeyToDetail.erase( ( *range.first ).second );
                m_locationKeyToStationModeKeys.erase( range.first );
                range.first++;
            } 
            m_locationAndModeToStationModeKey.erase( locationKey );
        }


        // Ensure OrderedLocations Data is initialised.
        if ( false == m_isOrderedLocationDataInitialised )
        {
            refreshOrderedLocationData();
        }

		//zn++
        /*// load StationMode data for all ordered locations as specified by locationKey param 
        // (i.e. if equal 0 then for all, else just the key specified).
        StationModeKeys stationModeKeys;
        StationModeKeys::iterator stationModeKeysIt;
        OrderedLocations::iterator orderedLocationsIt = m_orderedLocations.begin();
        StationModeDetail stationModeDetail;
        IECSStationMode* pIECSStationMode;
        while ( orderedLocationsIt != m_orderedLocations.end() )
        {
            if ( ( locationKey == 0 ) || ( locationKey == ( *orderedLocationsIt ).key ) )
            {
                stationModeKeys = ECSStationModeAccessFactory::getInstance()
                        .getECSStationModeKeysWhere( ( *orderedLocationsIt ).key, -1, "", "", "" );
                stationModeKeysIt = stationModeKeys.begin();
                while ( stationModeKeysIt != stationModeKeys.end() )
                {
                    pIECSStationMode = ECSStationModeAccessFactory::getInstance()
                        .getECSStationMode( ( *stationModeKeysIt ), false );
                    stationModeDetail.condition = pIECSStationMode->getCondition();
                    stationModeDetail.operation = pIECSStationMode->getOperation();
                    stationModeDetail.stationMode = pIECSStationMode->getStationMode();
                    std::ostringstream ostr;
                    ostr << pIECSStationMode->getStationMode();
                    stationModeDetail.stationModeAsString = ostr.str();
                    stationModeDetail.track = pIECSStationMode->getTrack();
                    stationModeDetail.locationKey = pIECSStationMode->getLocationKey();
                    m_stationModeKeyToDetail[ ( *stationModeKeysIt ) ] = stationModeDetail;
                    m_locationKeyToStationModeKeys.insert( LocationKeyToStationModeKeys::value_type
                        ( ( *orderedLocationsIt ).key, ( *stationModeKeysIt ) ) );
                    m_locationAndModeToStationModeKey[ ( *orderedLocationsIt ).key ][ pIECSStationMode->getStationMode() ]
                        = ( *stationModeKeysIt );
                    delete pIECSStationMode;
                    stationModeKeysIt++;                
                }
            }
            orderedLocationsIt++;
        }*/

		populateAllStationModeData();
		//++zn

        if ( locationKey == 0 )
        {
            m_isStationModeDataForAllLocationsInitialised = true;
        }

        FUNCTION_EXIT;
    }


    //
    // populateCachedMasterModeDataFromKeys
    //
    void MappedECSDataCache::populateCachedMasterModeDataFromKeys( const MasterModeKeys& masterModeKeys )
    {
        FUNCTION_ENTRY( "populateCachedMasterModeDataFromKeys" );

        // Ensure StationModeData is initialise for all locations.
        if ( false == m_isStationModeDataForAllLocationsInitialised )
        {
            refreshStationModeData();
        }

        // Run through the Master Mode keys constructing and caching
        // a related MasterModeDetail for each.
        IECSMasterMode* pIECSMasterMode = NULL;

        IECSZone* pIECSZone;
        MasterModeDetail masterModeDetail;
        StationModeDetail stationModeDetail;
        StationModeKeys stationModeKeys;
        StationModeKeys::iterator stationModeKeysIt;
        StationModeKeyToDetail::iterator stationModeKeyToDetailIt;
        LocationKeyToOrderIndex::iterator locationKeyToOrderIt;
        MasterModeKeys::const_iterator masterModeKeysIt = masterModeKeys.begin();

		//zn++ - Launching manager optimise
		//TA_IRS_Core::IECSMasterModes allMasterModeData = ECSMasterModeAccessFactory::getInstance().getAllECSMasterModes();
		//++zn
        while ( masterModeKeysIt != masterModeKeys.end() )
        {
			//zn++
            pIECSMasterMode = ECSMasterModeAccessFactory::getInstance().getECSMasterMode( ( *masterModeKeysIt ), false );
			/*int i = 0;
			while ( i < allMasterModeData.size())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[LM] all master mode data: %d", allMasterModeData[i]->getKey());
				if (allMasterModeData[i]->getKey() == (*masterModeKeysIt) )
				{
					pIECSMasterMode = allMasterModeData[i];
					break;
				}
				else
				{
					++i;
				}
			}

			if (pIECSMasterMode == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not find master mode key %d", *masterModeKeysIt);
				continue;
			}
			//++zn*/
            masterModeDetail.masterMode = pIECSMasterMode->getMasterMode();
            std::ostringstream ostr;
            ostr << pIECSMasterMode->getMasterMode();
            masterModeDetail.masterModeAsString = ostr.str();
            masterModeDetail.condition = pIECSMasterMode->getCondition();
            masterModeDetail.displayDirection = pIECSMasterMode->getDisplayDirection();
            masterModeDetail.evacuationDirection = pIECSMasterMode->getEvacuationDirection();
            masterModeDetail.airflowDirection = pIECSMasterMode->getAirflowDirection();
			masterModeDetail.verificationMessage = pIECSMasterMode->getVerificationMessage();
            if ( -1 == pIECSMasterMode->getECSZoneId() )
            {
                // There is no zone associated with "normal" MasterModes, so set related 
                // fields to default values.
                masterModeDetail.zoneID = -1;
                masterModeDetail.zoneDirectionKey = -1;
                masterModeDetail.zoneLocation = "";
                masterModeDetail.zoneTrack = "";
            }
            else
            {
                masterModeDetail.zoneID = pIECSMasterMode->getECSZoneId();
                pIECSZone = ECSZoneAccessFactory::getInstance()
                    .getECSZone( pIECSMasterMode->getECSZoneId() );
//SBL - No longer relevant:  masterModeDetail.zoneDirectionKey = pIECSZone->getDirectionEntityKey();
                masterModeDetail.zoneLocation = pIECSZone->getECSLocation();
                masterModeDetail.zoneTrack = pIECSZone->getTrack();
                m_zoneIDToMasterModeKeys.insert( ZoneIDToMasterModeKeys::value_type
                    ( pIECSZone->getECSZoneId(), ( *masterModeKeysIt ) ) );
                delete pIECSZone;
            }
            // Now populate the related station mode values in order corresponding to
            // the OrderedLocations order (i.e. Stations in innerbound direction).
            masterModeDetail.associatedStationModes = StationModes( m_orderedLocations.size(), 0 );
            masterModeDetail.associatedStationModesAsStrings = StationModesAsStrings( m_orderedLocations.size(), std::string("") );
            stationModeKeys = pIECSMasterMode->getAssociatedStationModeKeys();
            stationModeKeysIt = stationModeKeys.begin();
            while ( stationModeKeysIt != stationModeKeys.end() )
            {
                stationModeKeyToDetailIt = m_stationModeKeyToDetail.find( ( *stationModeKeysIt ) );
                if ( stationModeKeyToDetailIt != m_stationModeKeyToDetail.end() )
                {
                    stationModeDetail = ( *stationModeKeyToDetailIt ).second;
                    locationKeyToOrderIt = m_locationKeyToOrderIndex.find( stationModeDetail.locationKey );
                    if ( locationKeyToOrderIt != m_locationKeyToOrderIndex.end() )
                    {
                        masterModeDetail.associatedStationModes[ locationKeyToOrderIt->second ] = stationModeDetail.stationMode;
                        ostr.str( "" );
                        ostr << stationModeDetail.stationMode;
                        masterModeDetail.associatedStationModesAsStrings[ locationKeyToOrderIt->second ] = ostr.str();
                    }
                    else
                    {
                        LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Associated ECS Station Mode location key had no mapping to Ordered Location." );
                    }
                }
                else
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Associated ECS Station Mode no mapping to Station Mode Detail." );
                }
                stationModeKeysIt++;
            }
            m_masterModeKeyToDetail[ ( *masterModeKeysIt ) ] = masterModeDetail;
            m_masterModeToKey[ masterModeDetail.masterMode ] = ( *masterModeKeysIt );
            delete pIECSMasterMode;
            masterModeKeysIt++;
        }

        FUNCTION_EXIT;
    }


    //
    // refreshNormalMasterModeData
    //
    void MappedECSDataCache::refreshNormalMasterModeData()
    {
        FUNCTION_ENTRY( "refreshNormalMasterModeData" );

        // Clear any Normal Mode Data currently cached.
        if ( false == m_normalMasterModeKeys.empty() )
        {
            MasterModeKeyToDetail::iterator masterModeKeyToDetailIt;
            MasterModeKeys::iterator normalMasterModeKeysIt = m_normalMasterModeKeys.begin();
            while ( normalMasterModeKeysIt != m_normalMasterModeKeys.end() )
            {
                masterModeKeyToDetailIt = m_masterModeKeyToDetail.find( ( *normalMasterModeKeysIt ) );
                TA_ASSERT( masterModeKeyToDetailIt != m_masterModeKeyToDetail.end(), 
                    "MappedECSDataCache holding MasterMode key with no matching detail struct." );
                m_masterModeToKey.erase( ( *masterModeKeyToDetailIt ).second.masterMode );
                m_masterModeKeyToDetail.erase( masterModeKeyToDetailIt );
                normalMasterModeKeysIt++;
            }
            m_normalMasterModeKeys.clear();
        }

        // Ensure OrderedLocations Data is initialised.
        if ( false == m_isOrderedLocationDataInitialised )
        {
            refreshOrderedLocationData();
        }

        // Retrieve all normal MasterMode Keys (i.e. where condition NOT equal to "Emergency")
        m_normalMasterModeKeys = ECSMasterModeAccessFactory::getInstance()
            .getECSMasterModeKeysWhereNot( -1, EMERGENCY_COMPARE_STRING, -1, "", "", "" );
        // Call helper function to construct and cache MasterMode data from the
        // normal MasterMode keys
        populateCachedMasterModeDataFromKeys( m_normalMasterModeKeys );
        m_isNormalMasterModeDataInitialised = true;

        FUNCTION_EXIT;
    }


    //
    // refreshEmergencyMasterModeData
    //
    void MappedECSDataCache::refreshEmergencyMasterModeData( int zoneID /*=-1*/ )
    {
        FUNCTION_ENTRY( "refreshEmergencyMasterModeData" );

        // Reset any related Master Mode Data currently cached.
        if ( false == m_emergencyMasterModeKeys.empty() )
        {
            MasterModeKeyToDetail::iterator masterModeKeyToDetailIt;
            MasterModeKeys::iterator emergencyMasterModeKeysIt;
            if ( zoneID != -1 )
            {
                // Just reset the Emergency MasterMode data related to the specified zone.
                std::pair<ZoneIDToMasterModeKeys::iterator, ZoneIDToMasterModeKeys::iterator> 
                    range = m_zoneIDToMasterModeKeys.equal_range( zoneID );
                while ( range.first != range.second )
                {
                    masterModeKeyToDetailIt = m_masterModeKeyToDetail.find( ( *range.first ).second );
                    TA_ASSERT( masterModeKeyToDetailIt != m_masterModeKeyToDetail.end(), 
                        "MappedECSDataCache holding MasterMode key with no matching detail struct." );
                    m_masterModeToKey.erase( ( *masterModeKeyToDetailIt ).second.masterMode );
                    m_masterModeKeyToDetail.erase( masterModeKeyToDetailIt );
                    emergencyMasterModeKeysIt = m_emergencyMasterModeKeys.begin();
                    while ( emergencyMasterModeKeysIt != m_emergencyMasterModeKeys.end() )
                    {
                        if ( ( *emergencyMasterModeKeysIt ) == ( *range.first ).second )
                        {
                            emergencyMasterModeKeysIt = m_emergencyMasterModeKeys.erase( emergencyMasterModeKeysIt );	// must get return iterator
                        }
						else
						{
							emergencyMasterModeKeysIt++;
						}
                    }
                    m_zoneIDToMasterModeKeys.erase( range.first );
                    range.first++;
                }
            }
            else
            {
                // Reset all Emergency MasterMode data.
                emergencyMasterModeKeysIt = m_emergencyMasterModeKeys.begin();
                while ( emergencyMasterModeKeysIt != m_emergencyMasterModeKeys.end() )
                {
                    masterModeKeyToDetailIt = m_masterModeKeyToDetail.find( ( *emergencyMasterModeKeysIt ) );
                    TA_ASSERT( masterModeKeyToDetailIt != m_masterModeKeyToDetail.end(), 
                        "MappedECSDataCache holding MasterMode key with no matching detail struct." );
                    m_masterModeToKey.erase( ( *masterModeKeyToDetailIt ).second.masterMode );
                    m_masterModeKeyToDetail.erase( masterModeKeyToDetailIt );
                    emergencyMasterModeKeysIt++;
                }
                m_emergencyMasterModeKeys.clear();
                m_zoneIDToMasterModeKeys.clear();
            }
        }

        // Ensure OrderedLocations Data is initialised.
        if ( false == m_isOrderedLocationDataInitialised )
        {
            refreshOrderedLocationData();
        }

        // Retrieve MasterMode keys for Emergency MasterModes as specified by zoneID param
        // (i.e. All if equal to -1, else just those related to the specified zoneID)
        MasterModeKeys additionalEmergencyModeKeys;
        if ( zoneID != -1 )
        {
            // Retrieve Emergency MasterMode Keys for specified zone 
            // (i.e. where condition equal to "Emergency" AND ECZONE_ID = zoneID)
            additionalEmergencyModeKeys = ECSMasterModeAccessFactory::getInstance()
                .getECSMasterModeKeysWhere( -1, EMERGENCY_COMPARE_STRING, zoneID, "", "", "" );
        }
        else
        {
            // Retrieve all Emergency MasterMode Keys
            // (i.e. where condition equal to "Emergency")
            additionalEmergencyModeKeys = ECSMasterModeAccessFactory::getInstance()
                .getECSMasterModeKeysWhere( -1, EMERGENCY_COMPARE_STRING, -1, "", "", "" );
        }

        // Call helper function to construct and cache MasterMode data from the
        // additional emergency MasterMode keys.
        populateCachedMasterModeDataFromKeys( additionalEmergencyModeKeys );
        // Add the new keys to the member vector of additional keys.
        m_emergencyMasterModeKeys.insert( m_emergencyMasterModeKeys.end(), 
            additionalEmergencyModeKeys.begin(), additionalEmergencyModeKeys.end() );
        m_isEmergencyMasterModeDataInitialised = true;

        FUNCTION_EXIT;
    }
    
    
    //
    // refreshMasterModeData
    //
    void MappedECSDataCache::refreshMasterModeData( unsigned long masterMode, bool doForceRefresh /*=false*/ )
    {
        FUNCTION_ENTRY( "refreshMasterModeData" );

        // Ensure OrderedLocations Data is initialised.
        if ( false == m_isOrderedLocationDataInitialised )
        {
            refreshOrderedLocationData();
        }

        MasterModeToKey::iterator masterModeToKeyIt = m_masterModeToKey.find( masterMode );
        if ( masterModeToKeyIt != m_masterModeToKey.end() )
        {
            if ( doForceRefresh )
            {
                m_masterModeKeyToDetail.erase( m_masterModeKeyToDetail.find( ( *masterModeToKeyIt ).second ) );
                m_masterModeToKey.erase( masterModeToKeyIt );
            }
            else
            {
                return;
            }
        }

        MasterModeKeys additionalMasterModeKeys = ECSMasterModeAccessFactory::getInstance()
                .getECSMasterModeKeysWhere( masterMode, "", -1, "", "", "" );

        populateCachedMasterModeDataFromKeys( additionalMasterModeKeys );

        FUNCTION_EXIT;
    }


    // No longer relevant , now using zone equipment name to get the zone id
    // refreshZoneDirectionKeys
    //
    /*void MappedECSDataCache::refreshZoneDirectionKeys( bool doForceRefresh)
    {
        FUNCTION_ENTRY( "refreshZoneDirectionKeys" );

        if ( ( m_isZoneDirectionDataInitialised ) && ( false == doForceRefresh ) )
        {
            FUNCTION_EXIT;
            return;
        }

        //SBL - No longer relevant:   

        m_zoneDirectionKeys.clear();
        m_isZoneDirectionDataInitialised = false;

        IECSZones ecsZonesVector = ECSZoneAccessFactory::getInstance().getAllECSZones();
        for ( IECSZones::iterator ecsZonesVectorIt = ecsZonesVector.begin(); 
        ecsZonesVectorIt < ecsZonesVector.end(); ecsZonesVectorIt++ )
        {
            m_zoneDirectionKeys.push_back( ( *ecsZonesVectorIt )->getDirectionEntityKey() );
            m_zoneDirectionKeyToZoneID.insert( ZoneDirectionKeyToZoneID::value_type( ( *ecsZonesVectorIt )->getDirectionEntityKey(),
                ( *ecsZonesVectorIt )->getECSZoneId() ) );
            delete ( *ecsZonesVectorIt );
        }
        m_isZoneDirectionDataInitialised = true;

        FUNCTION_EXIT;
    }


    // No longer relevant , now using zone equipment name to get the zone id 
    // getZoneIdFromZoneDirectionName
    //
    unsigned long MappedECSDataCache::getZoneIdFromZoneDirectionName( const std::string& zoneDirectionName )
    {
        FUNCTION_ENTRY( "getZoneIdFromZoneDirectionName" );

        IEntityData* pDirectionEntity = EntityAccessFactory::getInstance().getEntity( zoneDirectionName );

        // Make sure Zone Direction Data is initialised.
        if ( false == m_isZoneDirectionDataInitialised )
        {
            refreshZoneDirectionKeys();
        }

        ZoneDirectionKeyToZoneID::const_iterator zoneDirectionKeyToZoneIdIt = m_zoneDirectionKeyToZoneID.find( pDirectionEntity->getKey() );
        delete pDirectionEntity;
        if ( zoneDirectionKeyToZoneIdIt != m_zoneDirectionKeyToZoneID.end() )
        {
            FUNCTION_EXIT;
            return ( *zoneDirectionKeyToZoneIdIt ).second;
        }

        FUNCTION_EXIT;
        return -1;
    }
    */
    void MappedECSDataCache::refreshZoneEquipmentData( bool doForceRefresh)
    {
        FUNCTION_ENTRY( "refreshZoneEquipmentData" );

        if ( ( m_isZoneEquipmentNameDataInitialised ) && ( false == doForceRefresh ) )
        {
            FUNCTION_EXIT;
            return;
        }

        m_isZoneEquipmentNameDataInitialised = false;

        IECSZones ecsZonesVector = ECSZoneAccessFactory::getInstance().getAllECSZones();
        IECSZones::iterator ecsZonesVectorIt = ecsZonesVector.begin(); 
        for ( ; ecsZonesVectorIt != ecsZonesVector.end(); ++ecsZonesVectorIt )
        {
            // can use getOccEquipmentEntityKey or getStnEquipmentEntityKey both should be the same
            m_zoneEquipmentKeyToZoneID.insert( ZoneEquipmentKeyToZoneID::value_type( ( *ecsZonesVectorIt )->getOccEquipmentEntityKey(),
                ( *ecsZonesVectorIt )->getECSZoneId() ) );
            delete ( *ecsZonesVectorIt );
        }
        m_isZoneEquipmentNameDataInitialised = true;

        FUNCTION_EXIT;
    }

    //
    // getZoneIdFromZoneEquipmentName
    //
    unsigned long MappedECSDataCache::getZoneIdFromZoneEquipmentName( const std::string& zoneEquipmentName )
    {
        FUNCTION_ENTRY( "getZoneIdFromZoneEquipmentName" );

        IEntityData* pEquipmentEntity = EntityAccessFactory::getInstance().getEntity( zoneEquipmentName );

        // Make sure Zone Direction Data is initialised.
        if ( false == m_isZoneEquipmentNameDataInitialised )
        {
            refreshZoneEquipmentData();
        }

        ZoneEquipmentKeyToZoneID::const_iterator zoneEquipmentKeyToZoneIdIt = m_zoneEquipmentKeyToZoneID.find( pEquipmentEntity->getKey() );
        delete pEquipmentEntity;
        if ( zoneEquipmentKeyToZoneIdIt != m_zoneEquipmentKeyToZoneID.end() )
        {
            FUNCTION_EXIT;
            return ( *zoneEquipmentKeyToZoneIdIt).second;
        }

        FUNCTION_EXIT;
        return -1;
    }

    //
    // getOrderedLocations
    //
    OrderedLocations MappedECSDataCache::getOrderedLocations() const
    {
        FUNCTION_ENTRY( "getOrderedLocations" );

        return m_orderedLocations;

        FUNCTION_EXIT;
    }


    // No longer relevant
    /*
    // getZoneDirectionKeys
    //
    ZoneDirectionKeys MappedECSDataCache::getZoneDirectionKeys() const
    {
        FUNCTION_ENTRY( "getZoneDirectionKeys" );
     
        FUNCTION_EXIT;
        return m_zoneDirectionKeys;
    }
    */


    //
    // getOrderedLocationIndexFromKey
    //
    int MappedECSDataCache::getOrderedLocationIndexFromKey( unsigned long locationKey ) const
    {
        FUNCTION_ENTRY( "getOrderedLocationIndexFromKey" );

        LocationKeyToOrderIndex::const_iterator locationKeyToOrderIndexIt = m_locationKeyToOrderIndex.find( locationKey );
        if ( locationKeyToOrderIndexIt  != m_locationKeyToOrderIndex.end() )
        {
            return ( *locationKeyToOrderIndexIt ).second;
        }

        FUNCTION_EXIT;
        return -1;          
    }


    //
    // isValidCachedOrderedLocationIndex
    //
    bool MappedECSDataCache::isValidCachedOrderedLocationIndex( int orderedLocationIndex ) const
    {
        return ( ( orderedLocationIndex > -1 ) && ( static_cast< unsigned int >( orderedLocationIndex) < m_orderedLocations.size() ) ); 
    }


    //
    // getOrderedLocationFromIndex
    //
    OrderedLocation MappedECSDataCache::getOrderedLocationFromIndex( int orderedLocationIndex ) const
    {
        FUNCTION_ENTRY( "getOrderedLocationFromIndex" );
        
        TA_ASSERT( isValidCachedOrderedLocationIndex( orderedLocationIndex ), "getOrderedLocationFromIndex called with invalid OrderedLocation index." );
        FUNCTION_EXIT;
        return m_orderedLocations[ orderedLocationIndex ];
    }


    //
    // getStationModeKeysFromLocationKey
    //
    StationModeKeys MappedECSDataCache::getStationModeKeysFromLocationKey( unsigned long locationKey ) const
    {
        FUNCTION_ENTRY( "getStationModeKeysFromLocationKey" );

        std::pair< LocationKeyToStationModeKeys::const_iterator, LocationKeyToStationModeKeys::const_iterator >
            range = m_locationKeyToStationModeKeys.equal_range( locationKey );
        StationModeKeys stationModeKeys;
        while ( range.first != range.second )
        {
            stationModeKeys.push_back( ( *range.first ).second );
            range.first++;
        }

        FUNCTION_EXIT;
        return stationModeKeys;
    }


    //
    // getStationModeKeyFromLocationKeyAndMode
    //
    unsigned long MappedECSDataCache::getStationModeKeyFromLocationKeyAndMode( unsigned long locationKey, unsigned long mode ) const
    {
        FUNCTION_ENTRY( "getStationModeKeyFromLocationKeyAndMode" );

        LocationAndModeToStationModeKey::const_iterator locationAndModeToStationModeKeyIt
            = m_locationAndModeToStationModeKey.find( locationKey );
        if ( locationAndModeToStationModeKeyIt != m_locationAndModeToStationModeKey.end() )
        {
            StationModeToKey::const_iterator stationModeToKeyIt 
                =  ( *locationAndModeToStationModeKeyIt ).second.find( mode );
            if ( stationModeToKeyIt != ( *locationAndModeToStationModeKeyIt ).second.end() )
            {
                return ( *stationModeToKeyIt ).second;
            }
        }

        // If mapping not found return 0
        FUNCTION_EXIT;
        return 0;
    }


    //
    // isValidCachedStationModeKey
    //
    bool MappedECSDataCache::isValidCachedStationModeKey( unsigned long stationModeKey ) const
    {
        StationModeKeyToDetail::const_iterator stationModeKeyToDetailIt
            = m_stationModeKeyToDetail.find( stationModeKey );
        return ( stationModeKeyToDetailIt != m_stationModeKeyToDetail.end() );
    }


    //
    // getStationModeDetailFromKey
    //
    StationModeDetail MappedECSDataCache::getStationModeDetailFromKey( unsigned long stationModeKey ) const
    {
        FUNCTION_ENTRY( "getStationModeDetailFromKey" );

        TA_ASSERT( isValidCachedStationModeKey( stationModeKey ), "getStationModeDetailFromKey called using stationModeKey not currently cached." );
        StationModeKeyToDetail::const_iterator stationModeKeyToDetailIt
            = m_stationModeKeyToDetail.find( stationModeKey );
        FUNCTION_EXIT;
        return ( *stationModeKeyToDetailIt ).second;
    }


    //
    // getEmergencyMasterModeKeys
    //
    MasterModeKeys MappedECSDataCache::getEmergencyMasterModeKeys() const
    {
        FUNCTION_ENTRY( "getEmergencyMasterModeKeys" );

        FUNCTION_EXIT;
        return m_emergencyMasterModeKeys;
    }


    //
    // getNormalMasterModeKeys
    //
    MasterModeKeys MappedECSDataCache::getNormalMasterModeKeys() const
    {
        FUNCTION_ENTRY( "getNormalMasterModeKeys" );

        FUNCTION_EXIT;
        return m_normalMasterModeKeys;
    }


    //
    // getMasterModeKeysFromZoneID
    //
    MasterModeKeys MappedECSDataCache::getMasterModesKeysFromZoneID( unsigned long zoneID ) const
    {
        FUNCTION_ENTRY( "getMasterModesKeysFromZoneID" );

        std::pair< ZoneIDToMasterModeKeys::const_iterator, ZoneIDToMasterModeKeys::const_iterator >
            range = m_zoneIDToMasterModeKeys.equal_range( zoneID );
        MasterModeKeys masterModeKeys;
        while ( range.first != range.second )
        {
            masterModeKeys.push_back( ( *range.first ).second );
            range.first++;
        }

        FUNCTION_EXIT;
        return masterModeKeys;
    }


    //
    // getMasterKeyModeKeyFromMode
    //
    unsigned long MappedECSDataCache::getMasterModeKeyFromMode( unsigned long masterMode ) const
    {
        FUNCTION_ENTRY( "getMasterModeKeyFromMode" );

        MasterModeToKey::const_iterator masterModeToKeyIt = m_masterModeToKey.find( masterMode );
        if ( masterModeToKeyIt != m_masterModeToKey.end() )
        {
            return ( *masterModeToKeyIt ).second;
        }

        // If mapping not found return 0
        FUNCTION_EXIT;
        return 0;
    }


    //
    // isValidCachedMasterModeKey
    //
    bool MappedECSDataCache::isValidCachedMasterModeKey( unsigned long masterModeKey ) const
    {
        MasterModeKeyToDetail::const_iterator masterModeKeyToDetailIt
            = m_masterModeKeyToDetail.find( masterModeKey );
        return ( masterModeKeyToDetailIt != m_masterModeKeyToDetail.end() );
    }


    //
    // getMasterModeDetailFromKey
    //
    MasterModeDetail MappedECSDataCache::getMasterModeDetailFromKey( unsigned long masterModeKey ) const
    {
        FUNCTION_ENTRY( "getMasterModeDetailFromKey" );

        TA_ASSERT( isValidCachedMasterModeKey( masterModeKey ), "getMasterModeDetailFromKey called using masterModeKey not currently cached." );
        MasterModeKeyToDetail::const_iterator masterModeKeyToDetailIt
            = m_masterModeKeyToDetail.find( masterModeKey );
        FUNCTION_EXIT;
        return ( *masterModeKeyToDetailIt ).second;
    }

	//zn++ - Launching manager optimise
	void MappedECSDataCache::populateAllStationModeData()
	{
		FUNCTION_ENTRY("populateAllStationModeData()");

		// Should delete all items after loading all station mode data
		TA_IRS_Core::IECSStationModes allStationModeData = ECSStationModeAccessFactory::getInstance().getAllECSStationModes();
		TA_IRS_Core::IECSStationModesIt iter;

		for (iter = allStationModeData.begin(); iter != allStationModeData.end(); ++iter)
		{
			StationModeDetail stationModeDetail;

			stationModeDetail.condition = (*iter)->getCondition();
			stationModeDetail.operation = (*iter)->getOperation();
			stationModeDetail.stationMode = (*iter)->getStationMode();
			std::ostringstream ostr;
			ostr << (*iter)->getStationMode();
			stationModeDetail.stationModeAsString = ostr.str();
			stationModeDetail.track = (*iter)->getTrack();
			stationModeDetail.locationKey = (*iter)->getLocationKey();
            
			m_stationModeKeyToDetail[ (*iter)->getKey() ] = stationModeDetail;

			m_locationKeyToStationModeKeys.insert( LocationKeyToStationModeKeys::value_type( (*iter)->getLocationKey(), (*iter)->getKey() ) );
			m_locationAndModeToStationModeKey[ (*iter)->getLocationKey() ][ (*iter)->getStationMode() ] = (*iter)->getKey();

			delete *iter;
		}

		FUNCTION_EXIT;
	}
	//++zn

} // namespace TA_IRS_App
