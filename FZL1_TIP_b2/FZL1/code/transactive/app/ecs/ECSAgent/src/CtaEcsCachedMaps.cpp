/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ECSAgent/src/CtaEcsCachedMaps.cpp $
  * @author:  seanl
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * implementation of the CtaEcsCachedMaps class
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <algorithm>
#include <string>

#include "app/ecs/ECSAgent/src/CtaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/StationECSAgentEntityData.h"
#include "core/data_access_interface/ecs/src/ECSStationModeAccessFactory.h"
#include "core/data_access_interface/ecs/src/ECSMasterModeAccessFactory.h"
#include "core/data_access_interface/ecs/src/ECSZoneAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/ECSAgentExceptions.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "app/ecs/ECSAgent/src/CtaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CtaEcsCachedMaps::CtaEcsCachedMaps()
:   m_masterModeLocksSqlToken(""),
    m_selectedMasterModesSqlToken(""),
    m_selectedOverridingStationModesSqlToken(""),
    m_lastAppliedMasterModesSqlToken(""),
    m_lastAppliedStationModesSqlToken(""),
    m_localModeSqlToken("")
{
	FUNCTION_ENTRY("CtaEcsCachedMaps");
    m_zoneRecords.clear();
    m_stationModeRecords.clear();
    m_masterModeRecords.clear();
    m_locationNames.clear();
    m_masterModeLockProxies.clear();
    m_selectedMasterModeProxies.clear();
    m_selectedOverridingStationModeProxies.clear();
    m_lastAppliedMasterModeProxies.clear();
    m_lastAppliedStationModeProxies.clear();
    m_localModeProxies.clear();
    m_myZoneDataPoints.clear();
    m_stationEcsAgentCorbaNames.clear();

    m_isMemberToInitialised.clear();

	m_emergencyKeyToZoneId.clear();

    m_zoneIdScope.clear();

	// get a handle to the ScadaProxyFactory
	m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
    TA_ASSERT ( ( 0 != m_proxyFactory ), "SCADA Proxy Factory not created");
	m_proxyFactory->setProxiesToControlMode();

    FUNCTION_EXIT;

}

CtaEcsCachedMaps::~CtaEcsCachedMaps()
{
	FUNCTION_ENTRY("~CtaEcsCachedMaps");
    m_zoneRecords.clear();
    m_stationModeRecords.clear();
    m_masterModeRecords.clear();
    m_locationNames.clear();
    m_masterModeLockProxies.clear();
    m_selectedMasterModeProxies.clear();
    m_selectedOverridingStationModeProxies.clear();
    m_lastAppliedMasterModeProxies.clear();
    m_lastAppliedStationModeProxies.clear();
    m_localModeProxies.clear();
    m_myZoneDataPoints.clear();
    m_stationEcsAgentCorbaNames.clear();

    m_isMemberToInitialised.clear();

	m_emergencyKeyToZoneId.clear();

    m_zoneIdScope.clear();

	// remove handle to ScadaProxyFactory
	m_proxyFactory->setProxiesToMonitorMode();
	TA_Base_Bus::ScadaProxyFactory::removeInstance();
	m_proxyFactory = NULL;

    FUNCTION_EXIT;
}

/**
 * attachObserver
 *
 * Adds the ItaEcsCachedMapsObserver instance to the collection of observers.  If the observer 
 * already exists for the key, no change will be made.
 *
 * @param observer pointer to the ItaEcsCachedMapsObserver instance
 *
 * @param key the EItaEcsCachedMapsItemKey the observer is interested in
 *
 */
void CtaEcsCachedMaps::attachObserver(ItaEcsCachedMapsObserver* observer, EItaEcsCachedMapsItemKey key)
{
	FUNCTION_ENTRY("attachObserver");
    TA_Base_Core::ThreadGuard guard( m_lockForObserverMap );

    // Search for all observers associated with the key
    std::pair<KeyToObserversMapIt, KeyToObserversMapIt> range =
        m_keyToObserversMap.equal_range (key);

    bool found(false);
    for ( ; range.first != range.second; ++range.first)
    {
        if ( (range.first)->second == observer)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {    
        m_keyToObserversMap.insert(key2observer(key, observer));
    }
    FUNCTION_EXIT;
}


/**
 * detachObserver
 *
 * Removes all instances of the ItaEcsCachedMapsObserver pointer in the observer collection.  The
 * observer will be removed for all keys.
 *
 */
void CtaEcsCachedMaps::detachObserver(ItaEcsCachedMapsObserver* observer)
{
	FUNCTION_ENTRY("detachObserver");
    TA_Base_Core::ThreadGuard guard( m_lockForObserverMap );

    KeyToObserversMapIt it = m_keyToObserversMap.begin();
    while (it != m_keyToObserversMap.end())
    {
        if (it->second == observer)
        {
            m_keyToObserversMap.erase(it++);
        }
        else
        {
            ++it;
        }
    }
    FUNCTION_EXIT;
}


/**
 * notifyInterestedObservers
 *
 * Calls the processItaEcsCachedMapsUpdate() method on all registered observers interested in the 
 * specified key.
 *
 * @param key used to call specific observers
 *
 */
void CtaEcsCachedMaps::notifyInterestedObservers(EItaEcsCachedMapsItemKey key)
{
	FUNCTION_ENTRY("notifyInterestedObservers");
    TA_Base_Core::ThreadGuard guard( m_lockForObserverMap );

    // Search for all observers associated with a key
    std::pair<KeyToObserversMapIt, KeyToObserversMapIt> range =
        m_keyToObserversMap.equal_range (key);

    for ( ; range.first != range.second; ++range.first)
    {
        try
        {
            (range.first)->second->processItaEcsCachedMapsUpdate(key);
        }
        catch(...)
        {
            // Do nothing and continue
        }
    }
    FUNCTION_EXIT;
}

/**
 * refreshX methods
 *
 * Methods to refresh/initialise the contents of the maps
 *
 * @throws DatabaseException 
 *         If there is a problem establishing a connection with the database
 *         or if an error is encountered while retrieving data.
 * @throws DataException      
 *         If the data cannot be converted to the required format or if the 
 *         wrong ammount of data is retrieved.
 * @throws InvalidECSConfigurationException 
 *         If a single zoneId is mapped to more than one record (i.e. 
 *         zoneIds are not unique).  This indicates invalid configuration 
 *         in the databases.
 */



// Reads all zoneId records
void CtaEcsCachedMaps::refreshZoneRecords()
{
    FUNCTION_ENTRY("refreshZoneRecords");
    
    TA_IRS_Core::IECSZones zones = TA_IRS_Core::ECSZoneAccessFactory::getInstance().getAllECSZones(false);    

    TA_Base_Core::ThreadGuard guard( m_zoneRecordsLock );
    m_zoneRecords.clear();

    for (TA_IRS_Core::IECSZonesIt it = zones.begin(); it != zones.end(); ++it)
    {
        TA_IRS_Core::IECSZone* daiObject = (*it);

        TA_IRS_Bus::EcZoneRecord record;

        record.ecsZoneId                    = daiObject->getECSZoneId();
        record.ecsLocation                  = daiObject->getECSLocation().c_str();
        record.track                        = daiObject->getTrack().c_str();
        record.occEquipmentEntityKey        = daiObject->getOccEquipmentEntityKey();
        record.stnEquipmentEntityKey        = daiObject->getStnEquipmentEntityKey();
        record.isIgnoringCongestion         = daiObject->getIsIgnoringCongestion();

        try
        {
            prevalidateSingleZoneRecord(record);
        }
        catch(TA_Base_Core::InvalidECSConfigurationException&)
        {
            delete daiObject;
            daiObject = NULL;
            throw;
        }

        //if (RunParams::getInstance().isSet( "DemoOverride" ))
        //{
        //    // Only know about the first 29 zones (DBG/MSM)
        //    static const int MAXZONEID=29;
        //    if ((*it)->getECSZoneId() <= MAXZONEID)
        //    {
        //        m_zoneIdToRecordMap[record.ecsZoneId] = record;
        //    }
        //}
        //else
        //{
        //    m_zoneIdToRecordMap[record.ecsZoneId] = record;
        //}



        // Use insert istead of [] for efficiency
        m_zoneRecords.insert(ZoneIdToEcZoneRecordMap::value_type(record.ecsZoneId, record));
        
        delete daiObject;
        daiObject = NULL;
    } // for loop

    // Flag as initialised
    setMemberInitialised(&m_zoneRecords);

    notifyInterestedObservers(MAP_ZONE_RECORDS);

    FUNCTION_EXIT;
}


// Only refreshes for given zoneIds
void CtaEcsCachedMaps::refreshZoneRecords(const std::vector<unsigned long>& zoneIds)
{
    FUNCTION_ENTRY("refreshZoneRecords");

    TA_Base_Core::ThreadGuard guard( m_zoneRecordsLock );
    m_zoneRecords.clear();

    std::vector<unsigned long>::const_iterator it;
    for (it = zoneIds.begin(); it!=zoneIds.end(); ++it)
    {
        TA_IRS_Core::IECSZone* daiObject = TA_IRS_Core::ECSZoneAccessFactory::getInstance().getECSZone(*it,false);

        TA_IRS_Bus::EcZoneRecord record;

        record.ecsZoneId                    = daiObject->getECSZoneId();
        record.ecsLocation                  = daiObject->getECSLocation().c_str();
        record.track                        = daiObject->getTrack().c_str();
        record.occEquipmentEntityKey        = daiObject->getOccEquipmentEntityKey();
        record.stnEquipmentEntityKey        = daiObject->getStnEquipmentEntityKey();

        try
        {
            prevalidateSingleZoneRecord(record);
        }
        catch(TA_Base_Core::InvalidECSConfigurationException&)
        {
            delete daiObject;
            daiObject = NULL;
            throw;
        }

        //if (RunParams::getInstance().isSet( "DemoOverride" ))
        //{
        //    // Only know about the first 29 zones (DBG/MSM)
        //    static const int MAXZONEID=29;
        //    if ((*it)->getECSZoneId() <= MAXZONEID)
        //    {
        //        m_zoneIdToRecordMap[record.ecsZoneId] = record;
        //    }
        //}
        //else
        //{
        //    m_zoneIdToRecordMap[record.ecsZoneId] = record;
        //}

        // Use insert istead of [] for efficiency
        m_zoneRecords.insert(ZoneIdToEcZoneRecordMap::value_type(record.ecsZoneId, record));

        delete daiObject;
        daiObject = NULL;
    } // for loop

    // Flag as initialised
    setMemberInitialised(&m_zoneRecords);

    notifyInterestedObservers(MAP_ZONE_RECORDS);

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::prevalidateSingleZoneRecord(const TA_IRS_Bus::EcZoneRecord& record)
{
	FUNCTION_ENTRY("prevalidateSingleZoneRecord");

    // Validate zoneId field.
    if (record.ecsZoneId == -1)
    {
        TA_THROW( TA_Base_Core::InvalidECSConfigurationException("Null ZoneId (-1) found in EcZone table") );
    }

    // Validate ecsLocation field.
    if (strlen(record.ecsLocation) == 0)
    {
        std::ostringstream msg;
        msg << "ZoneId " <<  record.ecsZoneId
            << " is mapped to a record with a null ECS_LOCATION field.";

        TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
    }

    // Validate track field.
    if (strlen(record.track) == 0)
    {
        std::ostringstream msg;
        msg << "ZoneId " <<  record.ecsZoneId
            << " is mapped to a record with a null TRACK field.";

        TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
    }

    // Validate occEquipmentEntityKey field.
    if (record.occEquipmentEntityKey == 0)
    {
        std::ostringstream msg;
        msg << "ZoneId " <<  record.ecsZoneId
            << " is mapped to a record with an invalid occEquipmentEntityKey.";

        TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
    }

    // Validate occEquipmentEntityKey field.
    if (record.stnEquipmentEntityKey == 0)
    {
        std::ostringstream msg;
        msg << "ZoneId " <<  record.ecsZoneId
            << " is mapped to a record with an invalid stnEquipmentEntityKey.";

        TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
    }
    FUNCTION_EXIT;

}


// specify locationKey to refresh for a given location only
void CtaEcsCachedMaps::refreshStationModeRecords(unsigned long locationKey)
{
    FUNCTION_ENTRY("refreshStationModeRecords");

    TA_IRS_Core::IECSStationModes stationModes = TA_IRS_Core::ECSStationModeAccessFactory::getInstance().getECSStationModesWhere(false,locationKey);

    TA_Base_Core::ThreadGuard guard( m_stationModeRecordsLock );
    m_stationModeRecords.clear();

    for (TA_IRS_Core::IECSStationModesIt it=stationModes.begin();it!=stationModes.end(); ++it)
    {
        TA_IRS_Core::IECSStationMode* daiObject = (*it);

        TA_IRS_Bus::EcStationModeRecord record;

        record.pKey             = daiObject->getKey();
        record.stationMode      = daiObject->getStationMode();
        record.locationKey      = daiObject->getLocationKey();
        record.operation        = daiObject->getOperation().c_str();
        record.track            = daiObject->getTrack().c_str();
        record.condition        = daiObject->getCondition().c_str();
        record.congestionLogic  = daiObject->getCongestionLogic().c_str();

        try
        {
            prevalidateSingleStationModeRecord(record);
        }
        catch(TA_Base_Core::InvalidECSConfigurationException&)
        {
            delete daiObject;
            daiObject = NULL;
            throw;
        }

        // Use insert istead of [] for efficiency
        m_stationModeRecords.insert(StationModeKeyToEcStationModeRecordMap::value_type(record.pKey, record));

        delete daiObject;
        daiObject = NULL;
    } // for

    // Flag as initialised
    setMemberInitialised(&m_stationModeRecords);


    notifyInterestedObservers(MAP_STATION_MODE_RECORDS);

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::prevalidateSingleStationModeRecord(const TA_IRS_Bus::EcStationModeRecord& record)
{
	FUNCTION_ENTRY("prevalidateSingleStationModeRecord");
    // Skip TRACK validation as this can be null.

    // Validate CONDITION field.
    if (strlen(record.condition) == 0 )
    {
        std::ostringstream msg;
        msg << "StationModeKey " <<  record.pKey
            << " is mapped to a record with a null condition field.";

        TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
    }

    // Validate locationKey field.
    if (record.locationKey == 0)
    {
        std::ostringstream msg;
        msg << "StationModeKey " <<  record.pKey
            << " is mapped to a record with an unassigned (0) locationKey field.";

        TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
    }
    FUNCTION_EXIT;

}

void CtaEcsCachedMaps::refreshMasterModeRecords()
{
    FUNCTION_ENTRY("refreshMasterModeRecords");

    TA_IRS_Core::IECSMasterModes masterModes = TA_IRS_Core::ECSMasterModeAccessFactory::getInstance().getAllECSMasterModes(false);

    TA_Base_Core::ThreadGuard guard( m_masterModeRecordsLock );
    m_masterModeRecords.clear();

    for (TA_IRS_Core::IECSMasterModesIt mmit=masterModes.begin();mmit!=masterModes.end(); ++mmit)
    {

        TA_IRS_Core::IECSMasterMode* daiObject = (*mmit);

        TA_IRS_Bus::EcMasterModeRecord record;

        record.pKey                 = daiObject->getKey();
        record.masterMode           = daiObject->getMasterMode();
        record.condition            = daiObject->getCondition().c_str();
        record.ecsZoneId            = daiObject->getECSZoneId();
        record.evacuationDirection  = daiObject->getEvacuationDirection().c_str();
        record.airflowDirection     = daiObject->getAirflowDirection().c_str();
        record.displayDirection     = daiObject->getDisplayDirection().c_str();
        record.verificationMessage  = daiObject->getVerificationMessage().c_str();

        std::vector<unsigned long>::iterator smkIt;
        std::vector<unsigned long> smk = 
            daiObject->getAssociatedStationModeKeys();

        record.stationModeKeys.length(smk.size());

        int index=0;
        smkIt=smk.begin();

        for (; smkIt!=smk.end(); ++index, ++smkIt)
        {
            record.stationModeKeys[index]=*smkIt;
        }

        // Use insert istead of [] for efficiency
        m_masterModeRecords.insert(MasterModeKeyToEcMasterModeRecordMap::value_type(record.pKey, record));

        delete daiObject;
        daiObject = NULL;

    } // for

    // Flag as initialised
    setMemberInitialised(&m_masterModeRecords);

    notifyInterestedObservers(MAP_MASTER_MODE_RECORDS);


    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::refreshLocationNames()
{
    FUNCTION_ENTRY("refreshLocationNames");

    // Get all location records.

    std::vector<TA_Base_Core::ILocation*> allLocations =
        TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations(false);

    TA_Base_Core::ThreadGuard guard( m_locationNamesLock );
    m_locationNames.clear();

    std::vector<TA_Base_Core::ILocation*>::iterator it = allLocations.begin();
    for (;it!= allLocations.end(); ++it)
    {
        unsigned long locationKey  = (*it)->getKey();
        std::string   locationName = (*it)->getName();

        // No point checking for one-to-one relationship as the database constraints
        // should prevent this.

         // Use insert istead of [] for efficiency
        m_locationNames.insert(LocationKeyToNameMap::value_type(locationKey, locationName));

        delete (*it);
    }

    // Flag as initialised
    setMemberInitialised(&m_locationNames);

    notifyInterestedObservers(MAP_LOCATION_NAMES);

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::refreshMasterModeLockProxies(const std::string& sqlToken)
{
    FUNCTION_ENTRY("refreshMasterModeLockProxies");

    refreshLocationProxiesHelper( sqlToken, 
                                     m_masterModeLockProxies, 
                                     m_masterModeLockProxiesLock, 
                                     TA_Base_Core::DataPointEntityData::getStaticType(),
                                     MAP_UNSURE,
                                     "MasterModeLock datapoint");

    m_masterModeLocksSqlToken = sqlToken;

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::refreshSelectedMasterModeProxies(const std::string& sqlToken)
{
    FUNCTION_ENTRY("refreshSelectedMasterModeProxies");

    refreshLocationProxiesHelper( sqlToken, 
                                     m_selectedMasterModeProxies, 
                                     m_selectedMasterModeProxiesLock, 
                                     TA_Base_Core::DataPointEntityData::getStaticType(),
                                     MAP_UNSURE,
                                     "SelectedMasterMode datapoint");

    m_selectedMasterModesSqlToken = sqlToken;

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::refreshSelectedOverridingStationModeProxies(const std::string& sqlToken)
{
    FUNCTION_ENTRY("refreshSelectedOverridingStationModeProxies");

    refreshLocationProxiesHelper( sqlToken, 
                                     m_selectedOverridingStationModeProxies, 
                                     m_selectedOverridingStationModeProxiesLock, 
                                     TA_Base_Core::DataPointEntityData::getStaticType(),
                                     MAP_UNSURE,
                                     "SelectedOverridingStationMode datapoint");

    m_selectedOverridingStationModesSqlToken = sqlToken;

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::refreshLastAppliedMasterModeProxies(const std::string& sqlToken)
{
    FUNCTION_ENTRY("refreshLastAppliedMasterModeProxies");

    refreshLocationProxiesHelper( sqlToken, 
                                     m_lastAppliedMasterModeProxies, 
                                     m_lastAppliedMasterModeProxiesLock, 
                                     TA_Base_Core::DataPointEntityData::getStaticType(),
                                     MAP_UNSURE,
                                     "LastAppliedMasterMode datapoint");

    m_lastAppliedMasterModesSqlToken = sqlToken;

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::refreshLastAppliedStationModeProxies(const std::string& sqlToken)
{
    FUNCTION_ENTRY("refreshLastAppliedStationModeProxies");

    refreshLocationProxiesHelper( sqlToken, 
                                     m_lastAppliedStationModeProxies, 
                                     m_lastAppliedStationModeProxiesLock, 
                                     TA_Base_Core::DataPointEntityData::getStaticType(),
                                     MAP_UNSURE,
                                     "LastAppliedStationMode datapoint");

    m_lastAppliedStationModesSqlToken = sqlToken;

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::refreshLocalModeProxies(const std::string& sqlToken)
{
    FUNCTION_ENTRY("refreshLocalModeProxies");

    refreshLocationProxiesHelper( sqlToken, 
                                     m_localModeProxies, 
                                     m_localModeProxiesLock, 
                                     TA_Base_Core::DataPointEntityData::getStaticType(),
                                     MAP_UNSURE,
                                     "LocalMode datapoint");

    m_localModeSqlToken = sqlToken;

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::refreshLocationCorbaNamesHelper(const std::string& sqlToken, 
                                                 LocationKeyToCorbaNameMap& theMap, 
                                                 TA_Base_Core::ReEntrantThreadLockable& theMapLock,
                                                 const std::string& entityTypeName,
                                                 EItaEcsCachedMapsItemKey notificationMessage,
                                                 const std::string& objectLabelForErrorMessages)
{
    FUNCTION_ENTRY("refreshLocationCorbaNamesHelper");

    TA_Base_Core::ThreadGuard guard( theMapLock );
    theMap.clear();

    TA_Base_Core::CorbaNameList corbaNames = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeWithNameLikeToken
        ( entityTypeName, sqlToken,false );

    TA_Base_Core::CorbaNameList::iterator it = corbaNames.begin();

    for (;it != corbaNames.end(); ++it)
    {
        unsigned long locationKey = (*it).getLocationKey();

        if (locationKey!=0)
        {
            if (theMap.find(locationKey) != theMap.end())
            {
                // We have encountered this datapoint for this location already
                std::ostringstream msg;
                msg << "Cannot locate unique " 
                    << objectLabelForErrorMessages 
                    << " for locationKey " 
                    << locationKey;

                TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
            }

            // Use insert istead of [] for efficiency
            theMap.insert(LocationKeyToCorbaNameMap::value_type(locationKey, CorbaNameWrapperForMaps((*it))));
        }
    }
    
    // Flag as initialised
    setMemberInitialised(&theMap);

    notifyInterestedObservers(notificationMessage);

    
    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::refreshLocationProxiesHelper(const std::string& sqlToken, 
                                              LocationKeyToProxyMap& theMap, 
                                              TA_Base_Core::ReEntrantThreadLockable& theMapLock,
                                              const std::string& entityTypeName,
                                              EItaEcsCachedMapsItemKey notificationMessage,
                                              const std::string& objectLabelForErrorMessages)
{
    FUNCTION_ENTRY("refreshLocationCorbaNamesHelper");

    TA_Base_Core::ThreadGuard guard( theMapLock );
    theMap.clear();

    TA_Base_Core::IEntityDataList entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken
        ( entityTypeName, sqlToken );
    
    TA_Base_Core::IEntityDataList::iterator it = entities.begin();

    for (;it != entities.end(); ++it)
    {
        unsigned long locationKey = (*it)->getLocation();

        if (locationKey!=0)
        {
            if (theMap.find(locationKey) != theMap.end())
            {
                // We have encountered this datapoint for this location already
                std::ostringstream msg;
                msg << "Cannot locate unique " 
                    << objectLabelForErrorMessages 
                    << " for locationKey " 
                    << locationKey;

                TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
            }

            TA_Base_Core::DataPointEntityData* dpEntity = dynamic_cast<TA_Base_Core::DataPointEntityData*>(*it);
            if (dpEntity == NULL)
            {
                // We have encountered this datapoint for this location already
                std::ostringstream msg;
                msg << "Entity is not a DataPoint " 
                    << objectLabelForErrorMessages 
                    << " for locationKey " 
                    << locationKey;

                TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
            }

            TA_Base_Bus::DataPointProxySmartPtr * proxy = new TA_Base_Bus::DataPointProxySmartPtr(); 
            m_proxyFactory->createDataPointProxy(boost::shared_ptr<TA_Base_Core::DataPointEntityData>(dpEntity), m_nullEventProcessor, *proxy);
   
            theMap.insert(LocationKeyToProxyMap::value_type(locationKey, proxy));
        }
    }
    
    // Flag as initialised
    setMemberInitialised(&theMap);

    notifyInterestedObservers(notificationMessage);

    
    FUNCTION_EXIT;
}

// Pre: refreshZoneRecords() called
void CtaEcsCachedMaps::refreshMyZoneCongestionDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints)
{
    FUNCTION_ENTRY("refreshMyZoneCongestionDataPoints");

    refreshMyZoneDataPointsHelper( dataPoints, 
                                   EcsZoneDataPoints::CONGESTION, 
                                   MAP_MY_ZONE_CONGESTION_DATAPOINTS,
                                   "Congestion datapoint");


    FUNCTION_EXIT;
}


// Pre: refreshZoneRecords() called
void CtaEcsCachedMaps::refreshMyZoneEmergencyDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints)
{
    FUNCTION_ENTRY("refreshMyZoneEmergencyDataPoints");

    refreshMyZoneDataPointsHelper( dataPoints, 
                                   EcsZoneDataPoints::EMERGENCY, 
                                   MAP_MY_ZONE_EMERGENCY_DATAPOINTS,
                                   "Emergency datapoint");


    FUNCTION_EXIT;
}

/* Datapoint is removed
// Pre: refreshZoneRecords() called
void CtaEcsCachedMaps::refreshMyZoneCongestionAppliedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints)
{
    FUNCTION_ENTRY("refreshMyZoneCongestionAppliedDataPoints");

    refreshMyZoneDataPointsHelper( dataPoints, 
                                   EcsZoneDataPoints::CONGESTION_APPLIED, 
                                   MAP_MY_ZONE_CONGESTION_APPLIED_DATAPOINTS,
                                   "CongestionApplied datapoint");


    FUNCTION_EXIT;
}
*/


// Pre: refreshZoneRecords() called
void CtaEcsCachedMaps::refreshMyZoneEmergencyAppliedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints)
{
    FUNCTION_ENTRY("refreshMyZoneEmergencyAppliedDataPoints");

    refreshMyZoneDataPointsHelper( dataPoints, 
                                   EcsZoneDataPoints::EMERGENCY_APPLIED, 
                                   MAP_MY_ZONE_EMERGENCY_APPLIED_DATAPOINTS,
                                   "EmergencyApplied datapoint");


    FUNCTION_EXIT;
}


// Pre: refreshZoneRecords() called
void CtaEcsCachedMaps::refreshMyZoneEvacuationSelectedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints)
{
    FUNCTION_ENTRY("refreshMyZoneEvacuationSelectedDataPoints");

    refreshMyZoneDataPointsHelper( dataPoints, 
                                   EcsZoneDataPoints::EVACUATION_SELECTED, 
                                   MAP_MY_ZONE_EVACUATION_SELECTED_DATAPOINTS,
                                   "Evacuation Selected datapoint");


    FUNCTION_EXIT;
}


// Pre: refreshZoneRecords() called
void CtaEcsCachedMaps::refreshMyZoneAirflowSelectedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints)
{
    FUNCTION_ENTRY("refreshMyZoneAirflowSelectedDataPoints");

    refreshMyZoneDataPointsHelper( dataPoints, 
                                   EcsZoneDataPoints::AIRFLOW_SELECTED, 
                                   MAP_MY_ZONE_AIRFLOW_SELECTED_DATAPOINTS,
                                   "Airflow Selected datapoint");


    FUNCTION_EXIT;
}


// Pre: refreshZoneRecords() called
void CtaEcsCachedMaps::refreshMyZoneEvacuationAppliedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints)
{
    FUNCTION_ENTRY("refreshMyZoneEvacuationAppliedDataPoints");

    refreshMyZoneDataPointsHelper( dataPoints, 
                                   EcsZoneDataPoints::EVACUATION_APPLIED, 
                                   MAP_MY_ZONE_EVACUATION_APPLIED_DATAPOINTS,
                                   "Evacuation Applied datapoint");


    FUNCTION_EXIT;
}


// Pre: refreshZoneRecords() called
void CtaEcsCachedMaps::refreshMyZoneAirflowAppliedDataPoints(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints)
{
    FUNCTION_ENTRY("refreshMyZoneAirflowAppliedDataPoints");

    refreshMyZoneDataPointsHelper( dataPoints, 
                                   EcsZoneDataPoints::AIRFLOW_APPLIED, 
                                   MAP_MY_ZONE_AIRFLOW_APPLIED_DATAPOINTS,
                                   "Airflow Applied datapoint");


    FUNCTION_EXIT;
}


// Pre: refreshZoneRecords() called
void CtaEcsCachedMaps::refreshMyZoneDataPointsHelper( const std::vector<TA_Base_Bus::DataPoint*>& dataPoints, 
                                                EcsZoneDataPoints::EcsZoneDataPointTypes dataPointType, 
                                                EItaEcsCachedMapsItemKey notificationMessage,
                                                const std::string& objectLabelForErrorMessages)
{
    FUNCTION_ENTRY("refreshMyZoneDataPointsHelper");

    TA_ASSERT(isMemberInitialised(&m_zoneRecords), 
              "m_zoneRecords read before initialised");

    TA_Base_Core::ThreadGuard guard( m_myZoneDataPointsLock );

    // Clear out existing datapoints of the given type
    ZoneIdToEcsZoneDataPointsMap::iterator mapIt = m_myZoneDataPoints.begin();
    for (;mapIt!=m_myZoneDataPoints.end();++mapIt)
    {
        (mapIt->second).setDataPoint(dataPointType, NULL);
    }


    // Repopulate:
    std::vector<TA_Base_Bus::DataPoint*>::const_iterator dpit = dataPoints.begin();
    for(; dpit!=dataPoints.end(); ++dpit)
    {

        unsigned long equipmentEntityKey = (*dpit)->getParentEntityKey();
        unsigned long zoneId = getZoneIdFromEquipmentEntityKey(equipmentEntityKey);

        // return the existing EcsZoneDataPoints structure if it exists, otherwise
        // default construct a new one (filled with NULLs).
        EcsZoneDataPoints& theDataPoints = m_myZoneDataPoints[zoneId];

        // Check if we have come across the same entityKey earlier in this for loop.
        if (theDataPoints.getDataPoint(dataPointType) != NULL)
        {
            // DP has already been added, relationship is not one-to-one.

            std::ostringstream msg;
            msg << "ZoneId " 
                <<  zoneId
                << " is mapped to more than one " 
                << objectLabelForErrorMessages;
            TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
        }
        
        theDataPoints.setDataPoint(dataPointType, (*dpit));

		if (dataPointType == EcsZoneDataPoints::EMERGENCY)
		{
			m_emergencyKeyToZoneId[(*dpit)->getEntityKey()] = zoneId;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"Add emergency entity key %d and zone id %d into map.", 
						(*dpit)->getEntityKey(), zoneId);
		}
    }

    // Flag as initialised
    setMemberInitialised(&m_myZoneDataPoints);

    notifyInterestedObservers(notificationMessage);

    FUNCTION_EXIT;
}


void CtaEcsCachedMaps::refreshStationEcsAgentCorbaNames()
{
    FUNCTION_ENTRY("refreshStationEcsAgentCorbaNames");
    
    refreshLocationCorbaNamesHelper( "%",  // Any agent name is fine 
                                     m_stationEcsAgentCorbaNames, 
                                     m_stationEcsAgentCorbaNamesLock, 
                                     TA_Base_Core::StationECSAgentEntityData::getStaticType(),
                                     MAP_STATION_ECS_AGENT_CORBA_NAMES,
                                     "Station ECS Agent");

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::refreshZoneIdInScope(const std::vector<TA_Base_Bus::DataPoint*>& dataPoints)
{
    FUNCTION_ENTRY("refreshZoneIdInScope");

    TA_Base_Core::ThreadGuard guard( m_zoneIdScopeLock);
    m_zoneIdScope.clear();
    m_zoneIdScope.reserve(dataPoints.size());

    std::vector<TA_Base_Bus::DataPoint*>::const_iterator dpit = dataPoints.begin();
    for(; dpit!=dataPoints.end(); ++dpit)
    {
        unsigned long equipmentEntityKey = (*dpit)->getParentEntityKey();
        unsigned long zoneId = getZoneIdFromEquipmentEntityKey(equipmentEntityKey);
        m_zoneIdScope.push_back(zoneId);
    }
    // Flag as initialised
    setMemberInitialised(&m_zoneIdScope);

    FUNCTION_EXIT;
}
/**
 * validateX methods
 *
 * Verification methods to validate loaded data after all required maps have
 * refreshed.
 *
 * @throws InvalidECSConfigurationException if data is invalid
 *
 */


void CtaEcsCachedMaps::validateAllInitialisedMaps()
{
	FUNCTION_ENTRY("validateAllInitialisedMaps");
    
    validateZoneRecords();
    validateStationModeRecords(); 
    validateMasterModeRecords();
    validateLocationNames();
    validateMasterModeLockProxies();
    validateSelectedMasterModeProxies();
    validateSelectedOverridingStationModeProxies();
    validateLastAppliedMasterModeProxies();
    validateLastAppliedStationModeProxies();
    validateLocalModeProxies();
    validateMyZoneDataPoints();
    validateStationEcsAgentCorbaNames();
    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::validateZoneRecords()
{
    FUNCTION_ENTRY("validateZoneRecords");
    if (!isMemberInitialised(&m_zoneRecords))
    {
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_zoneRecordsLock );


    FUNCTION_EXIT;
}
void CtaEcsCachedMaps::validateStationModeRecords()
{
    FUNCTION_ENTRY("validateStationModeRecords");
    if (!isMemberInitialised(&m_stationModeRecords))
    {
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_stationModeRecordsLock );


    FUNCTION_EXIT;
}
void CtaEcsCachedMaps::validateMasterModeRecords()
{
    FUNCTION_ENTRY("validateMasterModeRecords");
    if (!isMemberInitialised(&m_masterModeRecords))
    {
	    FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_masterModeRecordsLock );

    ///////////////////////////////////////////////////////////////////////////
    // Verifying 1 Zone to 1/2 Master Mode relationship
    ///////////////////////////////////////////////////////////////////////////

    MasterModeKeyToEcMasterModeRecordMap::iterator mmit 
        = m_masterModeRecords.begin();

    for (; mmit != m_masterModeRecords.end(); ++mmit)
    {
        unsigned long ecsZoneOfInterest = (mmit->second).ecsZoneId;

        if (ecsZoneOfInterest == TA_IRS_Bus::ECS_NONE_OR_INVALID)
        {
            // Ignore special master modes
            continue;
        }
        int count(1);

        // Only need to start check from mmit+1 as any discrepencies would have 
        // been detected in earlier iterations of mmit

        MasterModeKeyToEcMasterModeRecordMap::iterator mmitLoopTwo = mmit;
        ++mmitLoopTwo;
    
        for (; mmitLoopTwo != m_masterModeRecords.end(); ++mmitLoopTwo)
        {
            if ((mmitLoopTwo->second).ecsZoneId == ecsZoneOfInterest)
            {
                ++count;
            }

            if (count>2)
            {
                std::ostringstream msg;

                msg << "ZoneId " <<  ecsZoneOfInterest
                    << " is mapped to more than two MasterModes.";

                TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
                
            }

        } // for mmitLoopTwo

    } // for mmit


    ///////////////////////////////////////////////////////////////////////////
    // Verifying at least 1 Station Mode mapped to each Master Mode 
    ///////////////////////////////////////////////////////////////////////////

    mmit = m_masterModeRecords.begin();

    for (; mmit != m_masterModeRecords.end(); ++mmit)
    {
        if ((mmit->second).stationModeKeys.length() == 0)
        {
            std::ostringstream msg;

            msg << "MasterMode " <<  (mmit->second).masterMode
                << " is mapped to no StationModes.";

            TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );

        }
    } // for mmit


    ///////////////////////////////////////////////////////////////////////////
    // Verifying the Station Modes mapped to each Master Mode are from 
    // different locations
    ///////////////////////////////////////////////////////////////////////////

    TA_Base_Core::ThreadGuard guard2( m_stationModeRecordsLock );

    mmit = m_masterModeRecords.begin();
    for (; mmit != m_masterModeRecords.end(); ++mmit)
    {
        TA_IRS_Bus::EcStationModeKeys& stationKeys = (mmit->second).stationModeKeys;

        for (unsigned long index=0; index < stationKeys.length(); ++index)
        {
            unsigned long locationKeyOfInterest 
                = (m_stationModeRecords[stationKeys[index]]).locationKey;

            // Only need to start check from index+1 as any discrepencies would 
            // have been detected in earlier iterations of index

            unsigned long indexLoopTwo = index+1;
            for (; indexLoopTwo < stationKeys.length(); ++indexLoopTwo)
            {
                if (locationKeyOfInterest == 
                    m_stationModeRecords[stationKeys[indexLoopTwo]].locationKey )
                {
                    std::ostringstream msg;

                    msg << "MasterMode " <<  (mmit->second).masterMode
                        << " is mapped to more than one"
                        << " StationMode in the same location.";

                    TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
                
                }
            } // for indexLoopTwo

        } // for index


    } // for mmit

    FUNCTION_EXIT;
}
void CtaEcsCachedMaps::validateLocationNames()
{
    FUNCTION_ENTRY("validateLocationNames");
    if (!isMemberInitialised(&m_locationNames))
    {
		FUNCTION_EXIT;
		return;
    }

    TA_Base_Core::ThreadGuard guard( m_locationNamesLock );


    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::validateMasterModeLockProxies()
{
    FUNCTION_ENTRY("validateMasterModeLockProxies");
    if (!isMemberInitialised(&m_masterModeLockProxies))
    {
        return;
    }

    // Determine all location keys valid for ECS (where ECS Agents are found)
    std::vector<unsigned long> locationKeys 
        = getAllStationEcsAgentLocationKeys();

    std::vector<unsigned long>::iterator it = locationKeys.begin();
    for (; it!=locationKeys.end(); ++it)
    {
        if ( (*it) == TA_Base_Core::LocationAccessFactory::getInstance().getDepotLocationKey())
        {
            // Don't check for KCD ECS Agents - this may change in the future
            continue;
        }

        try
        {
            TA_Base_Bus::DataPointProxySmartPtr & proxy = getMasterModeLockProxy(*it);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            std::ostringstream msg;
            msg << "MasterModeLock proxy not found for location " 
                << getLocationName(*it);          
            TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
        }
    }

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::validateSelectedMasterModeProxies()
{
    FUNCTION_ENTRY("validateSelectedMasterModeProxies");
    if (!isMemberInitialised(&m_selectedMasterModeProxies))
    {
	    FUNCTION_EXIT;
        return;
    }

    // Determine all location keys valid for ECS (where ECS Agents are found)
    std::vector<unsigned long> locationKeys 
        = getAllStationEcsAgentLocationKeys();

    std::vector<unsigned long>::iterator it = locationKeys.begin();
    for (; it!=locationKeys.end(); ++it)
    {
        if ( (*it) == TA_Base_Core::LocationAccessFactory::getInstance().getDepotLocationKey())
        {
            // Don't check for KCD ECS Agents - this may change in the future
            continue;
        }

        try
        {
            TA_Base_Bus::DataPointProxySmartPtr &proxy = getSelectedMasterModeProxy(*it);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            std::ostringstream msg;
            msg << "SelectedMasterMode proxy not found for location " 
                << getLocationName(*it);          
            TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
        }
    }

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::validateSelectedOverridingStationModeProxies()
{
    FUNCTION_ENTRY("validateSelectedOverridingStationModeProxies");
    if (!isMemberInitialised(&m_selectedOverridingStationModeProxies))
    {
	    FUNCTION_EXIT;
        return;
    }

    // Determine all location keys valid for ECS (where ECS Agents are found)
    std::vector<unsigned long> locationKeys 
        = getAllStationEcsAgentLocationKeys();

    std::vector<unsigned long>::iterator it = locationKeys.begin();
    for (; it!=locationKeys.end(); ++it)
    {
        if ( (*it) == TA_Base_Core::LocationAccessFactory::getInstance().getDepotLocationKey())
        {
            // Don't check for KCD ECS Agents - this may change in the future
            continue;
        }

        try
        {
            TA_Base_Bus::DataPointProxySmartPtr &proxy = getSelectedOverridingStationModeProxy(*it);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            std::ostringstream msg;
            msg << "SelectedOverridingStationMode proxy not found for location " 
                << getLocationName(*it);          
            TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
        }
    }

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::validateLastAppliedMasterModeProxies()
{
    FUNCTION_ENTRY("validateLastAppliedMasterModeProxies");
    if (!isMemberInitialised(&m_lastAppliedMasterModeProxies))
    {
	    FUNCTION_EXIT;
        return;
    }

    // Determine all location keys valid for ECS (where ECS Agents are found)
    std::vector<unsigned long> locationKeys 
        = getAllStationEcsAgentLocationKeys();

    std::vector<unsigned long>::iterator it = locationKeys.begin();
    for (; it!=locationKeys.end(); ++it)
    {
        if ( (*it) == TA_Base_Core::LocationAccessFactory::getInstance().getDepotLocationKey())
        {
            // Don't check for KCD ECS Agents - this may change in the future
            continue;
        }

        try
        {
            TA_Base_Bus::DataPointProxySmartPtr & proxy = getLastAppliedMasterModeProxy(*it);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            std::ostringstream msg;
            msg << "LastAppliedMasterMode proxy not found for location " 
                << getLocationName(*it);          
            TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
        }
    }

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::validateLastAppliedStationModeProxies()
{
    FUNCTION_ENTRY("validateLastAppliedStationModeProxies");
    if (!isMemberInitialised(&m_lastAppliedStationModeProxies))
    {
	    FUNCTION_EXIT;
        return;
    }

    // Determine all location keys valid for ECS (where ECS Agents are found)
    std::vector<unsigned long> locationKeys 
        = getAllStationEcsAgentLocationKeys();

    std::vector<unsigned long>::iterator it = locationKeys.begin();
    for (; it!=locationKeys.end(); ++it)
    {
        if ( (*it) == TA_Base_Core::LocationAccessFactory::getInstance().getDepotLocationKey())
        {
            // Don't check for KCD ECS Agents - this may change in the future
            continue;
        }

        try
        {
            TA_Base_Bus::DataPointProxySmartPtr &proxy = getLastAppliedStationModeProxy(*it);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            std::ostringstream msg;
            msg << "LastAppliedStationMode proxy not found for location " 
                << getLocationName(*it);          
            TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
        }
    }

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::validateLocalModeProxies()
{
    FUNCTION_ENTRY("validateLocalModeProxies");
    if (!isMemberInitialised(&m_localModeProxies))
    {
	    FUNCTION_EXIT;
        return;
    }

    // Determine all location keys valid for ECS (where ECS Agents are found)
    std::vector<unsigned long> locationKeys 
        = getAllStationEcsAgentLocationKeys();

    std::vector<unsigned long>::iterator it = locationKeys.begin();
    for (; it!=locationKeys.end(); ++it)
    {
        if ( (*it) == TA_Base_Core::LocationAccessFactory::getInstance().getDepotLocationKey())
        {
            // Don't check for KCD ECS Agents - this may change in the future
            continue;
        }

        try
        {
            TA_Base_Bus::DataPointProxySmartPtr &proxy = getLocalModeProxy(*it);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            std::ostringstream msg;
            msg << "LocalMode proxy not found for location " 
                << getLocationName(*it);          
            TA_THROW( TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()) );
        }
    }


    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::validateMyZoneDataPoints()
{
    FUNCTION_ENTRY("validateMyZoneDataPoints");
    if (!isMemberInitialised(&m_myZoneDataPoints))
    {
        return;
    }

    FUNCTION_EXIT;
}


void CtaEcsCachedMaps::validateStationEcsAgentCorbaNames()
{
    FUNCTION_ENTRY("validateStationEcsAgentCorbaNames");
    if (!isMemberInitialised(&m_stationEcsAgentCorbaNames))
    {
        return;
    }

    //TA_Base_Core::ThreadGuard guard( m_stationEcsAgentCorbaNamesLock );


    FUNCTION_EXIT;
}



/**
 * getX methods for ZoneRecords map
 *
 * Methods to retrieve specific data from the map
 *
 * @throws CachedMappingNotFoundException if the mapping could not be found.
 *
 */

TA_IRS_Bus::EcZoneRecord CtaEcsCachedMaps::getEcZoneRecord(unsigned long zoneId)
{
    FUNCTION_ENTRY("getEcZoneRecord");

    TA_ASSERT(isMemberInitialised(&m_zoneRecords), 
              "m_zoneRecords read before initialised");

    TA_Base_Core::ThreadGuard guard( m_zoneRecordsLock );

    // We have to iterate through the map to find the key so we can detect when if the key
    // is absent.  If we juest rely on referencing the map via operator[], we can only tell
    // absence of a record by checking whether the item is default constructed, which is 
    // a little messy.  I imagine that this process below is almost the equivalent of what
    // the operator[] is doing.

    ZoneIdToEcZoneRecordMap::iterator it = m_zoneRecords.find(zoneId);

    if (it == m_zoneRecords.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate EcZone record for ZoneId " 
            << zoneId;
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    return it->second;
}


std::string CtaEcsCachedMaps::getTrack(unsigned long zoneId)
{
    FUNCTION_ENTRY("getTrack");

    TA_ASSERT(isMemberInitialised(&m_zoneRecords), 
              "m_zoneRecords read before initialised");

    TA_Base_Core::ThreadGuard guard( m_zoneRecordsLock );

    // We have to iterate through the map to find the key so we can detect when if the key
    // is absent.  If we juest rely on referencing the map via operator[], we can only tell
    // absence of a record by checking whether the item is default constructed, which is 
    // a little messy.  I imagine that this process below is almost the equivalent of what
    // the operator[] is doing.

    ZoneIdToEcZoneRecordMap::iterator it = m_zoneRecords.find(zoneId);

    if (it == m_zoneRecords.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate EcZone record for ZoneId " 
            << zoneId;
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    const char* track = (it->second).track;
    // Implicit conversion of CORBA char* to std::string
    return track;
}


std::string CtaEcsCachedMaps::getEcsLocation(unsigned long zoneId)
{
    FUNCTION_ENTRY("getEcsLocation");

    TA_ASSERT(isMemberInitialised(&m_zoneRecords), 
              "m_zoneRecords read before initialised");

    TA_Base_Core::ThreadGuard guard( m_zoneRecordsLock );

    // We have to iterate through the map to find the key so we can detect when if the key
    // is absent.  If we juest rely on referencing the map via operator[], we can only tell
    // absence of a record by checking whether the item is default constructed, which is 
    // a little messy.  I imagine that this process below is almost the equivalent of what
    // the operator[] is doing.

    ZoneIdToEcZoneRecordMap::iterator it = m_zoneRecords.find(zoneId);

    if (it == m_zoneRecords.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate EcZone record for ZoneId " 
            << zoneId;
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    const char* ecsLocation = (it->second).ecsLocation;
    // Implicit conversion of CORBA char* to std::string
    return ecsLocation;
}


bool CtaEcsCachedMaps::getIsIgnoringCongestion(unsigned long zoneId)
{
    FUNCTION_ENTRY("getIsIgnoringCongestion");

    TA_ASSERT(isMemberInitialised(&m_zoneRecords), 
              "m_zoneRecords read before initialised");

    TA_Base_Core::ThreadGuard guard( m_zoneRecordsLock );

    // We have to iterate through the map to find the key so we can detect when if the key
    // is absent.  If we juest rely on referencing the map via operator[], we can only tell
    // absence of a record by checking whether the item is default constructed, which is 
    // a little messy.  I imagine that this process below is almost the equivalent of what
    // the operator[] is doing.

    ZoneIdToEcZoneRecordMap::iterator it = m_zoneRecords.find(zoneId);

    if (it == m_zoneRecords.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate EcZone record for ZoneId " 
            << zoneId;
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;
    return (it->second).isIgnoringCongestion;
}

// No exception
std::vector<unsigned long> CtaEcsCachedMaps::getAllZoneIds()
{
    FUNCTION_ENTRY("getAllZoneIds");

    TA_ASSERT(isMemberInitialised(&m_zoneRecords), 
              "m_zoneRecords read before initialised");

    std::vector<unsigned long> allZoneIds;
    TA_Base_Core::ThreadGuard guard( m_zoneRecordsLock );

    // Prevent unnecessary reallocations
    allZoneIds.reserve(m_zoneRecords.size());

    ZoneIdToEcZoneRecordMap::iterator it = m_zoneRecords.begin();

    for (; it!=m_zoneRecords.end(); ++it)
    {
        allZoneIds.push_back(it->first);
    }

    FUNCTION_EXIT;

    return allZoneIds;
}

// No exception
bool CtaEcsCachedMaps::isZoneIdInScope(unsigned long zoneId)
{
    FUNCTION_ENTRY("isZoneIdInScope");

    TA_ASSERT(isMemberInitialised(&m_zoneIdScope), 
              "m_zoneIdScope read before initialised");

    TA_Base_Core::ThreadGuard guard( m_zoneIdScopeLock);

    std::vector<unsigned long>::const_iterator zoneIter;
    zoneIter = std::find(m_zoneIdScope.begin(), m_zoneIdScope.end(), zoneId); // Search the list.

    FUNCTION_EXIT;
    return (zoneIter != m_zoneIdScope.end());
}


unsigned long CtaEcsCachedMaps::getZoneIdFromEquipmentEntityKey(unsigned long equipmentEntityKey)
{
    FUNCTION_ENTRY("getZoneIdFromEquipmentEntityKey");

    TA_ASSERT(isMemberInitialised(&m_zoneRecords), 
              "m_zoneRecords read before initialised");

    TA_Base_Core::ThreadGuard guard( m_zoneRecordsLock );

    ZoneIdToEcZoneRecordMap::iterator it = m_zoneRecords.begin();

    for (; it!=m_zoneRecords.end(); ++it)
    {
        if ((it->second).occEquipmentEntityKey == equipmentEntityKey ||
            (it->second).stnEquipmentEntityKey == equipmentEntityKey )
        {
            return it->first;
        }
    }

    std::ostringstream msg;
    msg << "Failed to locate EcZone record for EquipmentEntityKey " 
        << equipmentEntityKey;
	TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));

    FUNCTION_EXIT;

    // Never reach here
    return -1;
}




/**
 * getX methods for StationModeRecords map
 *
 * Methods to retrieve specific data from the map
 *
 * @throws CachedMappingNotFoundException if the mapping could not be found.
 *
 */

void CtaEcsCachedMaps::getEcStationModeRecords(TA_IRS_Bus::EcStationModeRecords& retVal)
{
    FUNCTION_ENTRY("getEcStationModeRecords");

    TA_ASSERT(isMemberInitialised(&m_stationModeRecords), 
              "m_stationModeRecordsLock read before initialised");

    TA_Base_Core::ThreadGuard guard( m_stationModeRecordsLock );

    retVal.length(m_stationModeRecords.size());

	StationModeKeyToEcStationModeRecordMap::iterator it = m_stationModeRecords.begin();

	int index = 0;
    for (; it!=m_stationModeRecords.end(); ++it)
    {
			retVal[index++] = (it->second);
    } // for

    FUNCTION_EXIT;
}

TA_IRS_Bus::EcStationModeRecord CtaEcsCachedMaps::getEcStationModeRecord(unsigned long key)
{
    FUNCTION_ENTRY("getEcStationModeRecord");

    TA_ASSERT(isMemberInitialised(&m_stationModeRecords), 
              "m_stationModeRecordsLock read before initialised");

    TA_Base_Core::ThreadGuard guard( m_stationModeRecordsLock );

    // We have to iterate through the map to find the key so we can detect when if the key
    // is absent.  If we juest rely on referencing the map via operator[], we can only tell
    // absence of a record by checking whether the item is default constructed, which is 
    // a little messy.  I imagine that this process below is almost the equivalent of what
    // the operator[] is doing.

    StationModeKeyToEcStationModeRecordMap::iterator it = m_stationModeRecords.find(key);

    if (it == m_stationModeRecords.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate EcStationMode record for key " 
            << key;
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    return it->second;
}

void CtaEcsCachedMaps::getEcStationModeRecordsForLocation(unsigned long locationKey, TA_IRS_Bus::EcStationModeRecords& retVal)
{
    FUNCTION_ENTRY("getEcStationModeRecordsForLocation");

    TA_ASSERT(isMemberInitialised(&m_stationModeRecords), 
              "m_stationModeRecordsLock read before initialised");

    // clear existing list
	retVal.length(0);

    TA_Base_Core::ThreadGuard guard( m_stationModeRecordsLock );

	TA_IRS_Bus::EcStationModeRecords records;


	int index=0;
    StationModeKeyToEcStationModeRecordMap::iterator it = m_stationModeRecords.begin();

    for (; it!=m_stationModeRecords.end(); ++it)
    {
		if ((it->second).locationKey == locationKey)
		{
            // This may be a efficiency concern

            // Expand the sequence
            retVal.length(index+1);

            // Set the element
			retVal[index++] = (it->second);
		}
    } // for

    FUNCTION_EXIT;
}

void CtaEcsCachedMaps::getEcStationModeRecord(unsigned long key, 
                                        unsigned long& locationKey,
                                        unsigned long& stationMode)
{
    FUNCTION_ENTRY("getEcStationModeRecord");

    TA_ASSERT(isMemberInitialised(&m_stationModeRecords), 
              "m_stationModeRecordsLock read before initialised");

    TA_Base_Core::ThreadGuard guard( m_stationModeRecordsLock );

    // We have to iterate through the map to find the key so we can detect when if the key
    // is absent.  If we juest rely on referencing the map via operator[], we can only tell
    // absence of a record by checking whether the item is default constructed, which is 
    // a little messy.  I imagine that this process below is almost the equivalent of what
    // the operator[] is doing.

    StationModeKeyToEcStationModeRecordMap::iterator it = m_stationModeRecords.find(key);

    if (it == m_stationModeRecords.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate EcStationMode record for key " 
            << key;
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    // Write to client variables while locked
    locationKey = it->second.locationKey;
    stationMode = it->second.stationMode;

    FUNCTION_EXIT;
}

unsigned long CtaEcsCachedMaps::getStationModeFromStationState
    (const std::string& condition, const std::string& track)
{
    FUNCTION_ENTRY("getStationModeFromStationState");

    TA_ASSERT(isMemberInitialised(&m_stationModeRecords), 
              "m_stationModeRecords read before initialised");

    // EFFICIENCY CONCERN - 
    // 1. If performance is a problem, it may be wise to add a mapping from a structure
    // containing Track and Condition strings and a comparison operator, to a StationMode.
    // 2. It may be good to convert the track and condition fields in the map to upper
    // case beforehand.  Only problem is if this class is to be used by the ECS Managers,
    // the display of the contents would also be in uppercase.

    TA_Base_Core::ThreadGuard guard( m_stationModeRecordsLock );

    StationModeKeyToEcStationModeRecordMap::iterator it = m_stationModeRecords.begin();

    for (; it!=m_stationModeRecords.end(); ++it)
    {
        std::string myConditionUC = EcsHelpers::toUpper((it->second).condition);
        std::string conditionUC   = EcsHelpers::toUpper(condition);

        if (myConditionUC == conditionUC)
        {
            std::string myTrackUC = EcsHelpers::toUpper((it->second).track);
            std::string trackUC   = EcsHelpers::toUpper(track);

            if (myTrackUC == trackUC)
            {
                // Found matching state  
                return (it->second).stationMode;
            }
        }
    } // for

    // Didn't find StationModeState
    std::ostringstream msg;
    msg << "Failed to find StationMode for condition "
        << condition << " track " 
        << track;
	TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));    

    FUNCTION_EXIT;

    return TA_IRS_Bus::ECS_NONE_OR_INVALID;

    FUNCTION_EXIT;
}


/**
 * getX methods for MasterModeRecords map
 *
 * Methods to retrieve specific data from the map
 *
 * @throws CachedMappingNotFoundException if the mapping could not be found.
 *
 */

TA_IRS_Bus::EcMasterModeRecord CtaEcsCachedMaps::getEcMasterModeRecord(unsigned long key)
{
    FUNCTION_ENTRY("getEcMasterModeRecord");

    TA_ASSERT(isMemberInitialised(&m_masterModeRecords), 
              "m_masterModeRecords read before initialised");

    TA_Base_Core::ThreadGuard guard( m_masterModeRecordsLock );

    // We have to iterate through the map to find the key so we can detect when if the key
    // is absent.  If we juest rely on referencing the map via operator[], we can only tell
    // absence of a record by checking whether the item is default constructed, which is 
    // a little messy.  I imagine that this process below is almost the equivalent of what
    // the operator[] is doing.

    MasterModeKeyToEcMasterModeRecordMap::iterator it = m_masterModeRecords.find(key);

    if (it == m_masterModeRecords.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate EcMasterMode record for key " 
            << key;
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    return it->second;
}


TA_IRS_Bus::EcMasterModeRecord CtaEcsCachedMaps::getEcMasterModeRecordFromMasterMode(unsigned long masterMode)
{
    FUNCTION_ENTRY("getEcMasterModeRecordFromMasterMode");

    TA_ASSERT(isMemberInitialised(&m_masterModeRecords), 
              "m_masterModeRecords read before initialised");

    TA_Base_Core::ThreadGuard guard( m_masterModeRecordsLock );

    // We have to iterate through the map to find the key so we can detect when if the key
    // is absent.  If we juest rely on referencing the map via operator[], we can only tell
    // absence of a record by checking whether the item is default constructed, which is 
    // a little messy.  I imagine that this process below is almost the equivalent of what
    // the operator[] is doing.

    MasterModeKeyToEcMasterModeRecordMap::iterator it 
        = m_masterModeRecords.begin();

    for (; it!=m_masterModeRecords.end(); ++it)
    {
        if( (it->second).masterMode == masterMode )
        {
            return (it->second);
        }
    
    }

    std::ostringstream msg;
    msg << "Failed to locate record for MasterMode " 
        << masterMode;
	TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));

    FUNCTION_EXIT;

    // Never reach here
    return TA_IRS_Bus::EcMasterModeRecord();
}


MasterModePair CtaEcsCachedMaps::getMasterModesFromZoneId(unsigned long zoneId)
{
    FUNCTION_ENTRY("getMasterModesFromZoneId");

    TA_ASSERT(isMemberInitialised(&m_masterModeRecords), 
              "m_masterModeRecords read before initialised");

    TA_Base_Core::ThreadGuard guard( m_masterModeRecordsLock );

    // EFFICIENCY CONCERN - 
    // If performance is a problem, it may be wise to add a mapping from ECS Zone IDs to 
    // Master Modes

    MasterModePair modes;
    bool foundOne=false;

    MasterModeKeyToEcMasterModeRecordMap::iterator it = m_masterModeRecords.begin();

    for (; it!=m_masterModeRecords.end(); ++it)
    {
        // Only a reference is accessed
        if( (it->second).ecsZoneId == zoneId)
        {
            if (!foundOne)
            {
                // First one encountered
                modes.first = (it->second).masterMode;
                foundOne = true;
            }
            else
            {
                // Second one encountered (tunnel section)
                modes.second = (it->second).masterMode;
                // Don't expect any more
                break;
            }
        }
    } // for

    if (!foundOne)
    {
        // Didn't find any
        std::ostringstream msg;
        msg << "Failed to find MasterModes for ZoneId " 
            << zoneId;
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));    
    }

    FUNCTION_EXIT;

    return modes;
}

unsigned long CtaEcsCachedMaps::getZoneIdFromMasterMode(unsigned long masterMode)
{
    FUNCTION_ENTRY("getZoneIdFromMasterMode");

    TA_Base_Core::ThreadGuard guard( m_masterModeRecordsLock );

    // We have to iterate through the map to find the key so we can detect when if the key
    // is absent.  If we juest rely on referencing the map via operator[], we can only tell
    // absence of a record by checking whether the item is default constructed, which is 
    // a little messy.  I imagine that this process below is almost the equivalent of what
    // the operator[] is doing.

    MasterModeKeyToEcMasterModeRecordMap::iterator it 
        = m_masterModeRecords.begin();

    for (; it!=m_masterModeRecords.end(); ++it)
    {
        if( (it->second).masterMode == masterMode )
        {
            return (it->second).ecsZoneId;
        }
    
    }

    std::ostringstream msg;
    msg << "Failed to locate ZoneId for MasterMode " 
        << masterMode;
	TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));

    FUNCTION_EXIT;

    // Never reach here
    return -1;
}

MasterModePair CtaEcsCachedMaps::getMasterModePairFromMasterMode(unsigned long masterMode)
{
    return getMasterModesFromZoneId(getZoneIdFromMasterMode(masterMode));
}


TA_IRS_Bus::EcStationModeKeys CtaEcsCachedMaps::getEcStationModeKeysFromMasterMode(unsigned long masterMode)
{
    FUNCTION_ENTRY("getEcStationModeKeysFromMasterMode");

    TA_ASSERT(isMemberInitialised(&m_masterModeRecords), 
              "m_masterModeRecords read before initialised");

    TA_Base_Core::ThreadGuard guard( m_masterModeRecordsLock );

    // EFFICIENCY CONCERN - 
    // If performance is a problem, it may be wise to add a mapping from MasterMode to 
    // StationModeKeys

    MasterModeKeyToEcMasterModeRecordMap::iterator it = m_masterModeRecords.begin();

    for (; it!=m_masterModeRecords.end(); ++it)
    {
        // Only a reference is accessed
        if( (it->second).masterMode==masterMode)
        {
            return (it->second).stationModeKeys;
        }
    } // for

    // Didn't find masterMode
    std::ostringstream msg;
    msg << "Failed to find record for MasterMode " 
        << masterMode;
	TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));    

    FUNCTION_EXIT;

    // Never get here
    return TA_IRS_Bus::EcStationModeKeys();
}

TA_IRS_Bus::EcStationModeRecords CtaEcsCachedMaps::getEcStationModeRecordsFromMasterMode(unsigned long masterMode)
{
    TA_IRS_Bus::EcStationModeKeys stationModeKeys= getEcStationModeKeysFromMasterMode(masterMode);
    TA_IRS_Bus::EcStationModeRecords stationModeRecords;

    // Initialise expected structure items
    stationModeRecords.length(stationModeKeys.length());

    for (int i=0; i<stationModeKeys.length(); ++i)
    {
        stationModeRecords[i] = getEcStationModeRecord(stationModeKeys[i]);
    }

    return stationModeRecords;
}


/**
 * getX methods for LocationNames map
 *
 * Methods to retrieve specific data from the map
 *
 * @throws CachedMappingNotFoundException if the mapping could not be found.
 *
 */

std::string CtaEcsCachedMaps::getLocationName(unsigned long locationKey)
{
    FUNCTION_ENTRY("getLocationName");

    TA_ASSERT(isMemberInitialised(&m_locationNames), 
              "m_locationNames read before initialised");

    TA_Base_Core::ThreadGuard guard( m_locationNamesLock );

    std::string locationName = m_locationNames[locationKey];

    if (locationName.empty())
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
            "Unrecognised locationKey %ld.  Returning the location name Unknown", locationKey);

		locationName = "Unknown";
    }

    FUNCTION_EXIT;

    return locationName;
}


unsigned long CtaEcsCachedMaps::getLocationKey(const std::string& locationName)
{
    FUNCTION_ENTRY("getLocationKey");

    TA_ASSERT(isMemberInitialised(&m_locationNames), 
              "m_locationNames read before initialised");

    TA_Base_Core::ThreadGuard guard( m_locationNamesLock );

    LocationKeyToNameMap::iterator it = m_locationNames.begin();
    for (; it!=m_locationNames.end(); ++it)
    {
        if (it->second == locationName)
        {
            return (it->first);
        }
    }

    std::ostringstream msg;
    msg << "Failed to locate LocationKey for LocationName " 
        << locationName;
	TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));

    FUNCTION_EXIT;

    return -1;
}



/**
 * getX methods for MasterModeLockProxies map
 *
 * Methods to retrieve specific data from the map
 *
 * @throws CachedMappingNotFoundException if the mapping could not be found.
 *
 */

TA_Base_Bus::DataPointProxySmartPtr& CtaEcsCachedMaps::getMasterModeLockProxy(unsigned long locationKey)
{
    FUNCTION_ENTRY("getMasterModeLockProxy");

    TA_ASSERT(isMemberInitialised(&m_masterModeLockProxies), 
              "m_masterModeLockProxies read before initialised");

    TA_Base_Core::ThreadGuard guard( m_masterModeLockProxiesLock );

    LocationKeyToProxyMap::iterator it;
    it = m_masterModeLockProxies.find(locationKey);

    if (it==m_masterModeLockProxies.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate MasterModeLock proxy at location " 
            << getLocationName(locationKey);
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    return *(it->second);
}

TA_Base_Bus::DataPointProxySmartPtr& CtaEcsCachedMaps::getSelectedMasterModeProxy(unsigned long locationKey)
{
    FUNCTION_ENTRY("getSelectedMasterModeProxy");

    TA_ASSERT(isMemberInitialised(&m_selectedMasterModeProxies), 
              "m_selectedMasterModeProxies read before initialised");

    TA_Base_Core::ThreadGuard guard( m_selectedMasterModeProxiesLock );

    LocationKeyToProxyMap::iterator it;
    it = m_selectedMasterModeProxies.find(locationKey);

    if (it==m_selectedMasterModeProxies.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate SelectedMasterMode proxy at location " 
            << getLocationName(locationKey);
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    return *(it->second);
}

TA_Base_Bus::DataPointProxySmartPtr& CtaEcsCachedMaps::getSelectedOverridingStationModeProxy(unsigned long locationKey)
{
    FUNCTION_ENTRY("getSelectedOverridingStationModeProxy");

    TA_ASSERT(isMemberInitialised(&m_selectedOverridingStationModeProxies), 
              "m_selectedOverridingStationModeProxies read before initialised");

    TA_Base_Core::ThreadGuard guard( m_selectedOverridingStationModeProxiesLock );

    LocationKeyToProxyMap::iterator it;
    it = m_selectedOverridingStationModeProxies.find(locationKey);

    if (it==m_selectedOverridingStationModeProxies.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate SelectedOverridingStationMode proxy at location " 
            << getLocationName(locationKey);
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    return *(it->second);
}

TA_Base_Bus::DataPointProxySmartPtr& CtaEcsCachedMaps::getLastAppliedMasterModeProxy(unsigned long locationKey)
{
    FUNCTION_ENTRY("getLastAppliedMasterModeProxy");

    TA_ASSERT(isMemberInitialised(&m_lastAppliedMasterModeProxies), 
              "m_lastAppliedMasterModeProxies read before initialised");

    TA_Base_Core::ThreadGuard guard( m_lastAppliedMasterModeProxiesLock );

    LocationKeyToProxyMap::iterator it;
    it = m_lastAppliedMasterModeProxies.find(locationKey);

    if (it==m_lastAppliedMasterModeProxies.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate LastAppliedMasterMode proxy at location " 
            << getLocationName(locationKey);
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    return *(it->second);
}

TA_Base_Bus::DataPointProxySmartPtr &CtaEcsCachedMaps::getLastAppliedStationModeProxy(unsigned long locationKey)
{
    FUNCTION_ENTRY("getLastAppliedStationModeProxy");

    TA_ASSERT(isMemberInitialised(&m_lastAppliedStationModeProxies), 
              "m_lastAppliedStationModeProxies read before initialised");

    TA_Base_Core::ThreadGuard guard( m_lastAppliedStationModeProxiesLock );

    LocationKeyToProxyMap::iterator it;
    it = m_lastAppliedStationModeProxies.find(locationKey);

    if (it==m_lastAppliedStationModeProxies.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate LastAppliedStationMode proxy at location " 
            << getLocationName(locationKey);
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    return *(it->second);
}

TA_Base_Bus::DataPointProxySmartPtr & CtaEcsCachedMaps::getLocalModeProxy(unsigned long locationKey)
{
    FUNCTION_ENTRY("getLocalModeProxy");

    TA_ASSERT(isMemberInitialised(&m_localModeProxies), 
              "m_localModeProxies read before initialised");

    TA_Base_Core::ThreadGuard guard( m_localModeProxiesLock );

    LocationKeyToProxyMap::iterator it;
    it = m_localModeProxies.find(locationKey);

    if (it==m_localModeProxies.end())
    {
        std::ostringstream msg;
        msg << "Failed to locate LocalMode proxy at location " 
            << getLocationName(locationKey);
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    return *(it->second);
}


/**
 * getX methods for MyZoneCongestionDataPoint map
 *
 * Methods to retrieve specific data from the map.  Note no exceptions are thrown
 *
 * @return NULL if datapoint cannot be found (out of scope)
 *
 */

TA_Base_Bus::DataPoint* CtaEcsCachedMaps::getMyZoneCongestionDataPoint(unsigned long zoneId)
{
    FUNCTION_ENTRY("getMyZoneCongestionDataPoint");

    TA_ASSERT(isMemberInitialised(&m_myZoneDataPoints), 
              "m_myZoneDataPoints read before initialised");

    TA_Base_Core::ThreadGuard guard( m_myZoneDataPointsLock );

    // If zoneId does not exist, toperator[] will create a new EcsZoneDataPoint
    // initialised with NULLs.  getDataPoint will return a NULL

    FUNCTION_EXIT;
    return m_myZoneDataPoints[zoneId].getDataPoint(EcsZoneDataPoints::CONGESTION);
}

/**
 * getX methods for MyZoneEmergencyDataPoint map
 *
 * Methods to retrieve specific data from the map.  Note no exceptions are thrown
 *
 * @return NULL if datapoint cannot be found (out of scope)
 *
 */

TA_Base_Bus::DataPoint* CtaEcsCachedMaps::getMyZoneEmergencyDataPoint(unsigned long zoneId)
{
    FUNCTION_ENTRY("getMyZoneEmergencyDataPoint");

    TA_ASSERT(isMemberInitialised(&m_myZoneDataPoints), 
              "m_myZoneDataPoints read before initialised");

    TA_Base_Core::ThreadGuard guard( m_myZoneDataPointsLock );

    // If zoneId does not exist, toperator[] will create a new EcsZoneDataPoint
    // initialised with NULLs.  getDataPoint will return a NULL

    FUNCTION_EXIT;
    return m_myZoneDataPoints[zoneId].getDataPoint(EcsZoneDataPoints::EMERGENCY);
}

//Datapoint is removed
/*
TA_Base_Bus::DataPoint* CtaEcsCachedMaps::getMyZoneCongestionAppliedDataPoint(unsigned long zoneId)
{
    FUNCTION_ENTRY("getMyZoneCongestionAppliedDataPoint");

    TA_ASSERT(isMemberInitialised(&m_myZoneDataPoints), 
              "m_myZoneDataPoints read before initialised");

    TA_Base_Core::ThreadGuard guard( m_myZoneDataPointsLock );

    // If zoneId does not exist, toperator[] will create a new EcsZoneDataPoint
    // initialised with NULLs.  getDataPoint will return a NULL

    FUNCTION_EXIT;
    return m_myZoneDataPoints[zoneId].getDataPoint(EcsZoneDataPoints::CONGESTION_APPLIED);
}
*/

TA_Base_Bus::DataPoint* CtaEcsCachedMaps::getMyZoneEmergencyAppliedDataPoint(unsigned long zoneId)
{
    FUNCTION_ENTRY("getMyZoneEmergencyAppliedDataPoint");

    TA_ASSERT(isMemberInitialised(&m_myZoneDataPoints), 
              "m_myZoneDataPoints read before initialised");

    TA_Base_Core::ThreadGuard guard( m_myZoneDataPointsLock );

    // If zoneId does not exist, toperator[] will create a new EcsZoneDataPoint
    // initialised with NULLs.  getDataPoint will return a NULL

    FUNCTION_EXIT;
    return m_myZoneDataPoints[zoneId].getDataPoint(EcsZoneDataPoints::EMERGENCY_APPLIED);
}

TA_Base_Bus::DataPoint* CtaEcsCachedMaps::getMyZoneEvacuationSelectedDataPoint(unsigned long zoneId)
{
    FUNCTION_ENTRY("getMyZoneEvacuationSelectedDataPoint");

    TA_ASSERT(isMemberInitialised(&m_myZoneDataPoints), 
              "m_myZoneDataPoints read before initialised");

    TA_Base_Core::ThreadGuard guard( m_myZoneDataPointsLock );

    // If zoneId does not exist, toperator[] will create a new EcsZoneDataPoint
    // initialised with NULLs.  getDataPoint will return a NULL

    FUNCTION_EXIT;
    return m_myZoneDataPoints[zoneId].getDataPoint(EcsZoneDataPoints::EVACUATION_SELECTED);
}

TA_Base_Bus::DataPoint* CtaEcsCachedMaps::getMyZoneAirflowSelectedDataPoint(unsigned long zoneId)
{
    FUNCTION_ENTRY("getMyZoneAirflowSelectedDataPoint");

    TA_ASSERT(isMemberInitialised(&m_myZoneDataPoints), 
              "m_myZoneDataPoints read before initialised");

    TA_Base_Core::ThreadGuard guard( m_myZoneDataPointsLock );

    // If zoneId does not exist, toperator[] will create a new EcsZoneDataPoint
    // initialised with NULLs.  getDataPoint will return a NULL

    FUNCTION_EXIT;
    return m_myZoneDataPoints[zoneId].getDataPoint(EcsZoneDataPoints::AIRFLOW_SELECTED);
}

TA_Base_Bus::DataPoint* CtaEcsCachedMaps::getMyZoneEvacuationAppliedDataPoint(unsigned long zoneId)
{
    FUNCTION_ENTRY("getMyZoneEvacuationAppliedDataPoint");

    TA_ASSERT(isMemberInitialised(&m_myZoneDataPoints), 
              "m_myZoneDataPoints read before initialised");

    TA_Base_Core::ThreadGuard guard( m_myZoneDataPointsLock );

    // If zoneId does not exist, toperator[] will create a new EcsZoneDataPoint
    // initialised with NULLs.  getDataPoint will return a NULL

    FUNCTION_EXIT;
    return m_myZoneDataPoints[zoneId].getDataPoint(EcsZoneDataPoints::EVACUATION_APPLIED);
}

TA_Base_Bus::DataPoint* CtaEcsCachedMaps::getMyZoneAirflowAppliedDataPoint(unsigned long zoneId)
{
    FUNCTION_ENTRY("getMyZoneAirflowAppliedDataPoint");

    TA_ASSERT(isMemberInitialised(&m_myZoneDataPoints), 
              "m_myZoneDataPoints read before initialised");

    TA_Base_Core::ThreadGuard guard( m_myZoneDataPointsLock );

    // If zoneId does not exist, toperator[] will create a new EcsZoneDataPoint
    // initialised with NULLs.  getDataPoint will return a NULL

    FUNCTION_EXIT;
    return m_myZoneDataPoints[zoneId].getDataPoint(EcsZoneDataPoints::AIRFLOW_APPLIED);
}

std::map<unsigned long, unsigned long> CtaEcsCachedMaps::getEmergencyEntityKeyToZoneId()
{
	FUNCTION_ENTRY("getMyZoneEvacuationAppliedDataPoint");

	FUNCTION_EXIT;
	return m_emergencyKeyToZoneId;
}

//Datapoint no longer exists
/*
TA_Base_Bus::DataPoint* CtaEcsCachedMaps::getMyZoneCongestionAppliedDataPointForCongestionEntityKey(unsigned long entityKey)
{
    FUNCTION_ENTRY("getMyZoneCongestionAppliedDataPointForCongestionEntityKey");

    TA_ASSERT(isMemberInitialised(&m_myZoneDataPoints), 
              "m_myZoneDataPoints read before initialised");

    TA_Base_Core::ThreadGuard guard( m_myZoneDataPointsLock );

    ZoneIdToEcsZoneDataPointsMap::iterator it = m_myZoneDataPoints.begin();
    for (;it!=m_myZoneDataPoints.end();++it)
    {
        TA_Base_Bus::DataPoint* congestionDp = (it->second).getDataPoint(EcsZoneDataPoints::CONGESTION);
        if (congestionDp)
        {
            if (congestionDp->getEntityKey() == entityKey)
            {
                // found
                return (it->second).getDataPoint(EcsZoneDataPoints::CONGESTION_APPLIED);
            }
        }
        
    }

    FUNCTION_EXIT;
    return NULL;
}
*/

/**
 * getX methods for StationEcsAgentNames map
 *
 * Methods to retrieve specific data from the map
 *
 * @throws CachedMappingNotFoundException if the mapping could not be found.
 *
 */

TA_Base_Core::CorbaName CtaEcsCachedMaps::getStationEcsAgentCorbaName(unsigned long locationKey)
{
    FUNCTION_ENTRY("getStationEcsAgentCorbaName");

    TA_ASSERT(isMemberInitialised(&m_stationEcsAgentCorbaNames), 
              "m_stationEcsAgentCorbaNames read before initialised");


    TA_Base_Core::ThreadGuard guard( m_stationEcsAgentCorbaNamesLock );

    CorbaNameWrapperForMaps& corbaNameWrapper = m_stationEcsAgentCorbaNames[locationKey];

    if (!corbaNameWrapper.isValid())
    {
        std::ostringstream msg;
        msg << "Failed to locate Station ECS Agent at Location " 
            << getLocationName(locationKey);
	    TA_THROW(TA_Base_Core::CachedMappingNotFoundException(msg.str().c_str()));
    }

    FUNCTION_EXIT;

    return corbaNameWrapper;
}


// No exceptions
std::vector<unsigned long> CtaEcsCachedMaps::getAllStationEcsAgentLocationKeys()
{
    FUNCTION_ENTRY("getAllStationEcsAgentLocationKeys");

    TA_ASSERT(isMemberInitialised(&m_stationEcsAgentCorbaNames), 
              "m_stationEcsAgentCorbaNames read before initialised");


    std::vector<unsigned long> allLocations;
    TA_Base_Core::ThreadGuard guard( m_stationEcsAgentCorbaNamesLock );


    // Prevent unnecessary reallocations
    allLocations.reserve(m_stationEcsAgentCorbaNames.size());

    LocationKeyToCorbaNameMap::iterator it = m_stationEcsAgentCorbaNames.begin();

    for (; it!=m_stationEcsAgentCorbaNames.end(); ++it)
    {
        allLocations.push_back(it->first);
    }

    FUNCTION_EXIT;

    return allLocations;

}

