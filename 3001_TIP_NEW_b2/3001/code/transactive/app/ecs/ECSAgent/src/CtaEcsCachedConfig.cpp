/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/CtaEcsCachedConfig.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/02/06 17:06:26 $
  * Last modified by:  $Author: qi.huang $
  *
  */
// CtaEcsCachedConfig.cpp: implementation of the CtaEcsCachedConfig class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <string>
#include <vector>

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/src/GlobalParameterAccessFactory.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"
#include "bus/ecs/ecs_agent/IDL/src/CommonEcsAgentCorbaDef.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "app/ecs/ECSAgent/src/CtaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMapsInitialiser.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CtaEcsCachedConfig::CtaEcsCachedConfig(ItaEcsCachedMapsInitialiser& cachedMapsInitialiserInstance) : 
m_cachedMapsInitialiserInstance(cachedMapsInitialiserInstance),
m_lockForConfigUpdate(),
m_lockForObserverMap(),
m_nullEventProcessor(),
m_isMaster( false ),
m_isInControl( false ),
m_agentKey(0),
m_agentName(""),
m_agentTypeKey(0),
m_agentLocationKey(0),
m_agentSubsystemKey(0),
m_agentInRuntime(false),
m_assetName(""),
m_isOccOnline(true),
m_occGroupName("OCC"),
m_localATSAgentKey( 0u ),
m_localATSAgentName(""),
m_spareATSAgentKey(0),
m_spareATSAgentName(""),
m_spareATSAgentLocationKey( 0u ),
m_thresholdStallTimeInSecs( 0u ),
m_thresholdTemperature( 0u ),
m_temperatureDeadbandLowerLimit( 0u ),
m_congestionUsesStallTime( false ),
m_congestionUsesTemperature( false ),
m_workerThreadsSleepTimeInMSecs( 0u ),
m_maxModeExecutionTimeInSecs( 0u ),
m_normalStationMode( 0u ),
m_iscsAlivePollTimeInMSecs( 0u ),
m_myMasterModeLockDataPoint( NULL ),
m_myMasterModeLockControlDataPoint( NULL ),
m_mySelectedMasterModeDataPoint( NULL ),
m_mySelectedOverridingStationModeDataPoint( NULL ),
m_myLastAppliedMasterModeDataPoint( NULL ),
m_myLastAppliedStationModeDataPoint( NULL ),
m_myModeStatusDataPoint( NULL ),
m_myModeInProgressDataPoint( NULL ),
m_iscsAliveProxy(),
m_modeStatusProxy(),
m_localModeProxy(),
m_modeControlDpEntity( 0u ),
m_modeInProgressDpEntity( 0u ),
m_autoModeDpEntity( 0u ),
m_selectionTimeoutInMSecs( 0u ),
m_normalMasterMode( 0u )
{
    FUNCTION_ENTRY("CtaEcsCachedConfig");

    m_keyToObserversMap.clear();

    FUNCTION_EXIT;
}

CtaEcsCachedConfig::~CtaEcsCachedConfig()
{
	FUNCTION_ENTRY("~CtaEcsCachedConfig");

    m_keyToObserversMap.clear();
    m_dataPoints.clear();

    FUNCTION_EXIT;
}



/**
 * attachObserver
 *
 * Adds the ItaEcsCachedConfigObserver instance to the collection of observers.  If the observer 
 * already exists for the key, no change will be made.
 *
 * @param observer pointer to the ItaEcsCachedConfigObserver instance
 *
 * @param key the EItaEcsCachedConfigItemKey the observer is interested in
 *
 */
void CtaEcsCachedConfig::attachObserver(ItaEcsCachedConfigObserver* observer, EItaEcsCachedConfigItemKey key)
{
	FUNCTION_ENTRY("attachObserver");

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
        TA_Base_Core::ThreadGuard guard( m_lockForObserverMap );
        m_keyToObserversMap.insert(key2observer(key, observer));
    }
    FUNCTION_EXIT;
}


/**
 * detachObserver
 *
 * Removes all instances of the ItaEcsCachedConfigObserver pointer in the observer collection.  The
 * observer will be removed for all keys.
 *
 */
void CtaEcsCachedConfig::detachObserver(ItaEcsCachedConfigObserver* observer)
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
 * Calls the processItaEcsCachedConfigUpdate() method on all registered observers interested in the 
 * specified key.  
 *
 * @param key used to call specific observers
 *
 */
void CtaEcsCachedConfig::notifyInterestedObservers(EItaEcsCachedConfigItemKey key)
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
            (range.first)->second->processItaEcsCachedConfigUpdate(key);
        }
        catch(...)
        {
            // Do nothing and continue
        }
    }
    FUNCTION_EXIT;
}


boost::shared_ptr<TA_Base_Core::DataPointEntityData> CtaEcsCachedConfig::getEntityBySqlToken(const std::string& sqlToken)
    // throw InvalidECSConfigurationException
{
	FUNCTION_ENTRY("getEntityBySqlToken");

    TA_Base_Core::IEntityDataList entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocationWithNameLikeToken
        ( TA_Base_Core::DataPointEntityData::getStaticType(), getAgentLocationKey(), sqlToken );
    
    if ( entities.size() != 1 )
    {
        // Not found
        std::ostringstream msg;
        msg << "Could not resolve unique datapoint for sqlToken " << sqlToken;
        TA_THROW(TA_Base_Core::InvalidECSConfigurationException(msg.str().c_str()));
    }

    TA_Base_Core::DataPointEntityData* dpEntity = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entities.front());
    if ( NULL == dpEntity )
    {
        TA_THROW(TA_Base_Core::InvalidECSConfigurationException("The defined entity is not datapoint"));
    }

    FUNCTION_EXIT;
    return boost::shared_ptr<TA_Base_Core::DataPointEntityData>(dpEntity);
}


// ExceptionChecked
DataPoints CtaEcsCachedConfig::getDataPointsByToken(const std::string& token)
    // throw InvalidECSConfigurationException
{
    FUNCTION_ENTRY("getDataPointsByToken");

    DataPoints matches;

    if ( token.empty() )
    {
        return matches;
    }

    // Search for string matches
    DataPointsIt it = m_dataPoints.begin();
    for (; it!=m_dataPoints.end(); ++it)
    {
        std::string name = (*it)->getDataPointName();

        if (name.find(token) != std::string::npos)
        {
            // Ex: to ignore "..EmergencyApplied" if token is "Emergency"
            // Assuming the token is the last string in the name
            if ((name.find(token) + token.length()) == name.length())
            {
                matches.push_back(*it);
            }
        }
    }

    if( matches.empty() )
    {
        // Not found
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] Could not resolve any datapoints for token %s", token.c_str() );
    }

    FUNCTION_EXIT;
    return matches;
}

TA_Base_Bus::DataPoint* CtaEcsCachedConfig::getDataPointByToken(const std::string& token)
    // throw InvalidECSConfigurationException
{
    FUNCTION_ENTRY("getDataPointByToken");

    TA_Base_Bus::DataPoint* dataPoint = NULL;

    if ( token.empty() )
    {
        return dataPoint;
    }

    // Search for string matches
    DataPointsIt it = m_dataPoints.begin();
    for (; it!=m_dataPoints.end(); ++it)
    {
        std::string name = (*it)->getDataPointName();

        if (name.find(token) != std::string::npos)
        {
            if ( NULL == dataPoint )
            {
                dataPoint = (*it);
                // continue on for validation
            }
            else
            {
                // Already found, therefore we have found a duplicate
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "[DataError] Could not resolve unique datapoint for token %s", token.c_str() );
                continue;
            }
        }
    }

    if( NULL == dataPoint )
    {
        // Not found
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] Could not resolve unique datapoint for token %s", token.c_str() );
    }

    FUNCTION_EXIT;
    return dataPoint;
}

void CtaEcsCachedConfig::initialiseAllConfigAndMaps(TA_Base_Core::IEntityDataPtr entityData, const DataPoints& datapoints)
{
	FUNCTION_ENTRY("initialiseAllConfigAndMaps");

    setEntityData(entityData);
    setDataPoints(datapoints);

    initialiseCachedMaps();
    processEntityData();
    processGlobalParameters();

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setEntityData(TA_Base_Core::IEntityDataPtr entityData)
{
	FUNCTION_ENTRY("setEntityData");
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

        m_masterEntityData = boost::dynamic_pointer_cast<TA_Base_Core::MasterECSAgentEntityData>(entityData);
        m_stationEntityData = boost::dynamic_pointer_cast<TA_Base_Core::StationECSAgentEntityData>(entityData);

        setIsMaster(m_masterEntityData.get()!=NULL);
    }

    // Process these immediately so we can raise alarms properly.
    
    setAgentKey(entityData->getKey());
    setAgentTypeKey(entityData->getTypeKey());
    setAgentLocationKey(entityData->getLocation());
    setAgentSubsystemKey(entityData->getSubsystem());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setDataPoints(const DataPoints& datapoints)
{
	FUNCTION_ENTRY("setDataPoints");

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_dataPoints = datapoints;

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processGlobalParameters()
{
	FUNCTION_ENTRY("processGlobalParameters");
    processThresholdStallTimeInSecs();
    processThresholdTemperature();
    processTemperatureDeadband();
    processCongestionUsesStallTime();
    processCongestionUsesTemperature();
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processEntityData()
{
	FUNCTION_ENTRY("processEntityData");

    processWorkerThreadsSleepTimeInMSecs();
    processMaxModeExecutionTimeInSecs();
    processSpareATSAgentKey();
    processLocalATSAgentKey();
	processAssetName();

    if (getIsMaster())
    {
        processSelectionTimeoutInSecs();
        processNormalMasterMode();

        processMyZonesCongestionToken();
        processMyZonesEmergencyToken();
        //processMyZonesCongestionAppliedToken(); // Datapoint is removed
        processMyZonesEmergencyAppliedToken();
        processMyZonesEvacuationSelectedToken();
        processMyZonesAirflowSelectedToken();
        processMyZonesEvacuationAppliedToken();
        processMyZonesAirflowAppliedToken();
        processMyModeStatusToken();
        processMyModeInProgressToken();

        processMasterModeLocksSqlToken();
        processSelectedMasterModesSqlToken();
        processSelectedOverridingStationModesSqlToken();
        processLastAppliedMasterModesSqlToken();
        processLastAppliedStationModesSqlToken();
        processLocalModesSqlToken();
    }
    else // Station
    {
        processNormalStationMode();
        processIscsAlivePollTimeInSecs();
    
        processMyZonesCongestionToken();
        processMyZonesEmergencyToken();
        processMyMasterModeLockToken();
        processMyMasterModeLockControlToken();
        processMySelectedMasterModeToken();
        processMySelectedOverridingStationModeToken();
        processMyLastAppliedMasterModeToken();
        processMyLastAppliedStationModeToken();

        processModeStatusSqlToken();
        processModeControlSqlToken();
        processModeInProgressSqlToken();
        processIscsAliveSqlToken();   
        processAutoModeSqlToken();   
        processLocalModeSqlToken();
    }
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::initialiseCachedMaps()
{
	FUNCTION_ENTRY("initialiseCachedMaps");
    m_cachedMapsInitialiserInstance.refreshLocationNames();
    m_cachedMapsInitialiserInstance.refreshZoneRecords();

    if (getIsMaster())
    {
        m_cachedMapsInitialiserInstance.refreshStationModeRecords();
        m_cachedMapsInitialiserInstance.refreshMasterModeRecords();
        m_cachedMapsInitialiserInstance.refreshStationEcsAgentCorbaNames();
    }
    else // Station
    {
        m_cachedMapsInitialiserInstance.refreshStationModeRecords(getAgentLocationKey());

        // can use congestion/emergency datapoint to fetch the zone ids
        std::string entityParameterValue = m_stationEntityData->getMyZonesCongestionToken();
        m_cachedMapsInitialiserInstance.refreshZoneIdInScope(getDataPointsByToken(entityParameterValue));
    }

    FUNCTION_EXIT;
}



///////////////////////////////////////////////////////////////////////////
// Global Parameters 
///////////////////////////////////////////////////////////////////////////


void CtaEcsCachedConfig::processThresholdStallTimeInSecs()
{
	FUNCTION_ENTRY("processThresholdStallTimeInSecs");

    std::string globalParameterValue = 
        TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterValueFromNameAndSubsystem
        (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_THRESHOLD_STALL_TIME, getAgentSubsystemKey());
    
    unsigned long convertedValue = atol(globalParameterValue.c_str());

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_thresholdStallTimeInSecs = convertedValue;

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processThresholdTemperature()
{
	FUNCTION_ENTRY("processThresholdTemperature");

    std::string globalParameterValue = 
        TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterValueFromNameAndSubsystem
        (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_THRESHOLD_TEMPERATURE, getAgentSubsystemKey());
    
    unsigned long convertedValue = atol(globalParameterValue.c_str());

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_thresholdTemperature = convertedValue;

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processTemperatureDeadband()
{
	FUNCTION_ENTRY("processTemperatureDeadband");

    std::string globalParameterValue = 
        TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterValueFromNameAndSubsystem
        (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_TEMPERATURE_DEADBAND, getAgentSubsystemKey());
    
    unsigned long convertedValue = atol(globalParameterValue.c_str());
    unsigned long thresholdTemperature = getThresholdTemperature();
    
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_temperatureDeadbandLowerLimit = thresholdTemperature - convertedValue;

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processCongestionUsesStallTime()
{
	FUNCTION_ENTRY("processCongestionUsesStallTime");

    std::string globalParameterValue = 
        TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterValueFromNameAndSubsystem
        (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_CONGESTION_USES_STALL_TIME, getAgentSubsystemKey());

    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_congestionUsesStallTime = EcsHelpers::isTrue(globalParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processCongestionUsesTemperature()
{
	FUNCTION_ENTRY("processCongestionUsesTemperature");

    std::string globalParameterValue = 
        TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterValueFromNameAndSubsystem
        (TA_IRS_Bus::ECS_GLOBAL_PARAMETER_CONGESTION_USES_TEMPERATURE, getAgentSubsystemKey());

    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_congestionUsesTemperature = EcsHelpers::isTrue(globalParameterValue.c_str());

    FUNCTION_EXIT;
}



///////////////////////////////////////////////////////////////////////////
// Common ECSAgent entity parameters
///////////////////////////////////////////////////////////////////////////

void CtaEcsCachedConfig::processWorkerThreadsSleepTimeInMSecs()
{
	FUNCTION_ENTRY("processWorkerThreadsSleepTimeInMSecs");
    unsigned long entityParameterValue = 0u;

    if (m_masterEntityData)
    {
        entityParameterValue = m_masterEntityData->getWorkerThreadsSleepTimeInMSecs();
    }
    else if (m_stationEntityData)
    {
        entityParameterValue = m_stationEntityData->getWorkerThreadsSleepTimeInMSecs();
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of ECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        if ( m_workerThreadsSleepTimeInMSecs == entityParameterValue )
        {
            // No change
	        FUNCTION_EXIT;
            return;
        }
        m_workerThreadsSleepTimeInMSecs = entityParameterValue;
    }

    notifyInterestedObservers(CONFIG_WORKER_THREADS_SLEEP_TIME_IN_MSECS);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter WorkerThreadsSleepTimeInMSecs=%ld", m_workerThreadsSleepTimeInMSecs);

    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processMaxModeExecutionTimeInSecs()
{
	FUNCTION_ENTRY("processMaxModeExecutionTimeInSecs");

    unsigned long entityParameterValue = 0u;

    if ( NULL != m_masterEntityData )
    {
        entityParameterValue = m_masterEntityData->getMaxModeExecutionTimeInSecs();
    }
    else if ( NULL != m_stationEntityData )
    {
        entityParameterValue = m_stationEntityData->getMaxModeExecutionTimeInSecs();
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of ECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        if ( m_maxModeExecutionTimeInSecs == entityParameterValue )
        {
            // No change
	        FUNCTION_EXIT;
            return;
        }
        m_maxModeExecutionTimeInSecs = entityParameterValue;
    }

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MaxModeExecutionTimeInSecs=%ld", m_maxModeExecutionTimeInSecs);

    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processMyZonesCongestionToken()
{
	FUNCTION_ENTRY("processMyZonesCongestionToken");

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = "";

    if ( NULL != m_masterEntityData )
    {
        entityParameterValue = m_masterEntityData->getMyZonesCongestionToken();
    }
    else if ( NULL != m_stationEntityData )
    {
        entityParameterValue = m_stationEntityData->getMyZonesCongestionToken();
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of ECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    DataPoints vecDataPoints = getDataPointsByToken(entityParameterValue);
    m_cachedMapsInitialiserInstance.refreshMyZoneCongestionDataPoints(vecDataPoints);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesCongestionToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyZonesEmergencyToken()
{
	FUNCTION_ENTRY("processMyZonesEmergencyToken");

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = "";

    if ( NULL != m_masterEntityData )
    {
        entityParameterValue = m_masterEntityData->getMyZonesEmergencyToken();
    }
    else if ( NULL != m_stationEntityData )
    {
        entityParameterValue = m_stationEntityData->getMyZonesEmergencyToken();
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of ECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    DataPoints vecDataPoints = getDataPointsByToken(entityParameterValue);
    m_cachedMapsInitialiserInstance.refreshMyZoneEmergencyDataPoints( vecDataPoints );

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesEmergencyToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}




///////////////////////////////////////////////////////////////////////////
// StationECSAgent entity parameters
///////////////////////////////////////////////////////////////////////////

void CtaEcsCachedConfig::processNormalStationMode()
{
    FUNCTION_ENTRY("processNormalStationMode");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    unsigned long entityParameterValue = m_stationEntityData->getNormalStationMode();

    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        if ( m_normalStationMode == entityParameterValue )
        {
            // No change
	        FUNCTION_EXIT;
            return;
        }
        m_normalStationMode = entityParameterValue;
    }

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter NormalStationMode=%ld", m_normalStationMode);

    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processIscsAlivePollTimeInSecs()
{
    FUNCTION_ENTRY("processIscsAlivePollTimeInSecs");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    unsigned long entityParameterValue = m_stationEntityData->getIscsAlivePollTimeInSecs() * 1000;

    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        if ( m_iscsAlivePollTimeInMSecs == entityParameterValue )
        {
            // No change
	        FUNCTION_EXIT;
            return;
        }
        m_iscsAlivePollTimeInMSecs = entityParameterValue;
    }

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter IscsAlivePollTimeInSecs=%ld", entityParameterValue);

    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processMyMasterModeLockToken()
{
	FUNCTION_ENTRY("processMyMasterModeLockToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMyMasterModeLockToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No MyMasterModeLock DataPoint for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myMasterModeLockDataPoint = datapoint;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        "EntityParameter MyMasterModeLockToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

  
void CtaEcsCachedConfig::processMyMasterModeLockControlToken()
{
    FUNCTION_ENTRY("processMyMasterModeLockControlToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMyMasterModeLockControlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No MyMasterModeLockControl DataPoint for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myMasterModeLockControlDataPoint = datapoint;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyMasterModeLockControlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMySelectedMasterModeToken()
{
    FUNCTION_ENTRY("processMySelectedMasterModeToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMySelectedMasterModeToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No MySelectedMasterMode DataPoint for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_mySelectedMasterModeDataPoint = datapoint;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MySelectedMasterModeToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMySelectedOverridingStationModeToken()
{
    FUNCTION_ENTRY("processMySelectedOverridingStationModeToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMySelectedOverridingStationModeToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No MySelectedOverridingStationMode DataPoint for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_mySelectedOverridingStationModeDataPoint = datapoint;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MySelectedOverridingStationModeToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyLastAppliedMasterModeToken()
{
    FUNCTION_ENTRY("processMyLastAppliedMasterModeToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMyLastAppliedMasterModeToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No MyLastAppliedMasterMode DataPoint for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myLastAppliedMasterModeDataPoint = datapoint;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyLastAppliedMasterModeToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyLastAppliedStationModeToken()
{
    FUNCTION_ENTRY("processMyLastAppliedStationModeToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getMyLastAppliedStationModeToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No MyLastAppliedStationMode DataPoint for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myLastAppliedStationModeDataPoint = datapoint;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyLastAppliedStationModeToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processModeStatusSqlToken()
{
    FUNCTION_ENTRY("processModeStatusSqlToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getModeStatusSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    try
    {
        boost::shared_ptr<TA_Base_Core::DataPointEntityData> param = getEntityBySqlToken(entityParameterValue);

        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(param, m_nullEventProcessor, m_modeStatusProxy);
    }
    catch ( const TA_Base_Core::InvalidECSConfigurationException& )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No ModeStatus Entity for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    notifyInterestedObservers(CONFIG_UNSURE);
    
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter ModeStatusSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processLocalModeSqlToken()
{
    FUNCTION_ENTRY("processLocalModeSqlToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getLocalModeSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    try
    {
        boost::shared_ptr<TA_Base_Core::DataPointEntityData> param = getEntityBySqlToken(entityParameterValue);

        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(param, m_nullEventProcessor, m_localModeProxy);
    }
    catch ( const TA_Base_Core::InvalidECSConfigurationException& )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No LocalMode Entity for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    notifyInterestedObservers(CONFIG_UNSURE);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter LocalModeSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processIscsAliveSqlToken()
{
    FUNCTION_ENTRY("processIscsAliveSqlToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getIscsAliveSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    try
    {
        boost::shared_ptr<TA_Base_Core::DataPointEntityData> param = getEntityBySqlToken(entityParameterValue);

        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy(param, m_nullEventProcessor, m_iscsAliveProxy);
    }
    catch ( const TA_Base_Core::InvalidECSConfigurationException& )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No IscsAlive Entity for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    notifyInterestedObservers(CONFIG_UNSURE);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter IscsAliveSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processModeControlSqlToken()
{
	FUNCTION_ENTRY("processModeControlSqlToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getModeControlSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
        FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    try
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_modeControlDpEntity = getEntityBySqlToken(entityParameterValue)->getKey();
    }
    catch ( const TA_Base_Core::InvalidECSConfigurationException& )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No ModeControl Entity for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    notifyInterestedObservers(CONFIG_MODE_CONTROL_CORBA_NAME);
    
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter ModeControlSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processModeInProgressSqlToken()
{
    FUNCTION_ENTRY("processModeInProgressSqlToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getModeInProgressSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
        FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    try
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_modeInProgressDpEntity = getEntityBySqlToken(entityParameterValue)->getKey();
    }
    catch ( const TA_Base_Core::InvalidECSConfigurationException& )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No ModeInProgress Entity for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    notifyInterestedObservers(CONFIG_MODE_IN_PROGRESS_CORBA_NAME);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter ModeInProgressSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processAutoModeSqlToken()
{
    FUNCTION_ENTRY("processAutoModeSqlToken");

    if ( NULL == m_stationEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of StationECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_stationEntityData->getAutoModeSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
        FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    try
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_autoModeDpEntity = getEntityBySqlToken(entityParameterValue)->getKey();
    }
    catch ( const TA_Base_Core::InvalidECSConfigurationException& )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No AutoMode Entity for token %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    notifyInterestedObservers(CONFIG_AUTO_MODE_CORBA_NAME);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter AutoModeSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}


///////////////////////////////////////////////////////////////////////////
// MasterECSAgent entity parameters
///////////////////////////////////////////////////////////////////////////


void CtaEcsCachedConfig::processSelectionTimeoutInSecs()
{
    FUNCTION_ENTRY("processSelectionTimeoutInSecs");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    const unsigned long entityParameterValue = m_masterEntityData->getSelectionTimeoutInSecs() * 1000;

    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

        if ( entityParameterValue == m_selectionTimeoutInMSecs )
        {
            // No change
            FUNCTION_EXIT;
            return;
        }

        m_selectionTimeoutInMSecs = entityParameterValue;
    }

    notifyInterestedObservers(CONFIG_SELECTION_TIMEOUT_IN_MSECS);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter SelectionTimeoutInSecs=%ld", entityParameterValue);

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processNormalMasterMode()
{
    FUNCTION_ENTRY("processNormalMasterMode");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    const unsigned long entityParameterValue = m_masterEntityData->getNormalMasterMode();

    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

        if ( entityParameterValue == m_normalMasterMode )
        {
            // No change
	        FUNCTION_EXIT;
            return;
        }

        m_normalMasterMode = entityParameterValue;
    }

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter NormalMasterMode=%ld", m_normalMasterMode);

    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processMyZonesEmergencyAppliedToken()
{
    FUNCTION_ENTRY("processMyZonesEmergencyAppliedToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyZonesEmergencyAppliedToken(); 

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    DataPoints vecDataPoints = getDataPointsByToken(entityParameterValue);
    m_cachedMapsInitialiserInstance.refreshMyZoneEmergencyAppliedDataPoints( vecDataPoints );

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesEmergencyAppliedToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyZonesEvacuationSelectedToken()
{
    FUNCTION_ENTRY("processMyZonesEvacuationSelectedToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyZonesEvacuationSelectedToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    DataPoints vecDataPoints = getDataPointsByToken(entityParameterValue);
    m_cachedMapsInitialiserInstance.refreshMyZoneEvacuationSelectedDataPoints(vecDataPoints);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesEvacuationSelectedToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyZonesAirflowSelectedToken()
{
    FUNCTION_ENTRY("processMyZonesAirflowSelectedToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyZonesAirflowSelectedToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    DataPoints vecDataPoints = getDataPointsByToken(entityParameterValue);
    m_cachedMapsInitialiserInstance.refreshMyZoneAirflowSelectedDataPoints(vecDataPoints);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesAirflowSelectedToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyZonesEvacuationAppliedToken()
{
    FUNCTION_ENTRY("processMyZonesEvacuationAppliedToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyZonesEvacuationAppliedToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    DataPoints vecDataPoints = getDataPointsByToken(entityParameterValue);
    m_cachedMapsInitialiserInstance.refreshMyZoneEvacuationAppliedDataPoints(vecDataPoints);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesEvacuationAppliedToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyZonesAirflowAppliedToken()
{
    FUNCTION_ENTRY("processMyZonesAirflowAppliedToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyZonesAirflowAppliedToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    DataPoints vecDataPoints = getDataPointsByToken(entityParameterValue);
    m_cachedMapsInitialiserInstance.refreshMyZoneAirflowAppliedDataPoints(vecDataPoints);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyZonesAirflowAppliedToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processMyModeStatusToken()
{
    FUNCTION_ENTRY("processMyModeStatusToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyModeStatusToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No MyModeStatus DataPoint for parameter %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myModeStatusDataPoint = datapoint;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyModeStatusToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processMyModeInProgressToken()
{
    FUNCTION_ENTRY("processMyModeInProgressToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMyModeInProgressToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    TA_Base_Bus::DataPoint* datapoint = getDataPointByToken(entityParameterValue);
    if (datapoint == NULL)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No MyModeInProgress DataPoint for parameter %s", entityParameterValue.c_str() );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_myModeInProgressDataPoint = datapoint;

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MyModeInProgressToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::processMasterModeLocksSqlToken()
{
    FUNCTION_ENTRY("processMasterModeLocksSqlToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getMasterModeLocksSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    m_cachedMapsInitialiserInstance.refreshMasterModeLockProxies(entityParameterValue);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter MasterModeLocksSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processSelectedMasterModesSqlToken()
{
    FUNCTION_ENTRY("processSelectedMasterModesSqlToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getSelectedMasterModesSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    m_cachedMapsInitialiserInstance.refreshSelectedMasterModeProxies(entityParameterValue);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter SelectedMasterModesSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processSelectedOverridingStationModesSqlToken()
{
    FUNCTION_ENTRY("processSelectedOverridingStationModesSqlToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getSelectedOverridingStationModesSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    m_cachedMapsInitialiserInstance.refreshSelectedOverridingStationModeProxies(entityParameterValue);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter SelectedOverridingStationModesSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processLastAppliedMasterModesSqlToken()
{
    FUNCTION_ENTRY("processLastAppliedMasterModesSqlToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getLastAppliedMasterModesSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    m_cachedMapsInitialiserInstance.refreshLastAppliedMasterModeProxies(entityParameterValue);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter LastAppliedMasterModesSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processLastAppliedStationModesSqlToken()
{
    FUNCTION_ENTRY("processLastAppliedStationModesSqlToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getLastAppliedStationModesSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
	    FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    m_cachedMapsInitialiserInstance.refreshLastAppliedStationModeProxies(entityParameterValue);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter LastAppliedStationModesSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::processLocalModesSqlToken()
{
    FUNCTION_ENTRY("processLocalModesSqlToken");

    if ( NULL == m_masterEntityData )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of MasterECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    static std::string lastEntityParameterValue = "";
    std::string entityParameterValue = m_masterEntityData->getLocalModesSqlToken();

    if (lastEntityParameterValue == entityParameterValue)
    {
        // No change
        FUNCTION_EXIT;
        return;
    }

    lastEntityParameterValue = entityParameterValue;

    m_cachedMapsInitialiserInstance.refreshLocalModeProxies(entityParameterValue);

	//TODO: need to verify with PMSAgent
	notifyInterestedObservers(CONFIG_UNSURE);

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "EntityParameter LocalModesSqlToken=%s", entityParameterValue.c_str());

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setIsMaster(bool param)
{
	FUNCTION_ENTRY("setIsMaster");

    // Implicitly threadsafe
    m_isMaster = param;

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setIsInControl(bool param)
{
	FUNCTION_ENTRY("setIsInControl");

    // Implicitly threadsafe
    m_isInControl = param;

    FUNCTION_EXIT;
}


void CtaEcsCachedConfig::setAgentKey(unsigned long param)
{
	FUNCTION_ENTRY("setAgentKey");

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_agentKey = param;

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setAgentTypeKey(unsigned long param)
{
	FUNCTION_ENTRY("setAgentTypeKey");

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_agentTypeKey = param;

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setAgentLocationKey(unsigned long param) 
//throw(TA_Base_Core::InvalidECSConfigurationException)
{
	FUNCTION_ENTRY("setAgentLocationKey");

    if (param == 0 || param == -1)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid LocationKey set" );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_agentLocationKey = param;

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setAgentSubsystemKey(unsigned long param) 
//throw(TA_Base_Core::InvalidECSConfigurationException)
{
	FUNCTION_ENTRY("setAgentSubsystemKey");

    if ( param == -1)
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid SubsystemKey set" );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_agentSubsystemKey = param;

    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setAgentInRuntime(bool param)
{
	FUNCTION_ENTRY("setAgentInRuntime");
    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_agentInRuntime = param;
    FUNCTION_EXIT;
}
    
void CtaEcsCachedConfig::setIsOccOnline(bool param)
{
	FUNCTION_ENTRY("setIsOccOnline");
    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_isOccOnline = param;
    FUNCTION_EXIT;
}

void CtaEcsCachedConfig::setOccGroupName(const std::string& param) //throw(TA_Base_Core::InvalidECSConfigurationException)
{
	FUNCTION_ENTRY("setOccGroupName");

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_occGroupName = param;

    FUNCTION_EXIT;
}


// Station or Master
void CtaEcsCachedConfig::processLocalATSAgentKey()
{
	FUNCTION_ENTRY("processLocalATSAgentKey");

    unsigned long ulLocation = 0u;

    if ( NULL != m_masterEntityData )
    {
        ulLocation = m_masterEntityData->getLocation();
    }
    else if ( NULL != m_stationEntityData )
    {
        ulLocation = m_stationEntityData->getLocation();
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of ECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::IEntityDataList entityDataList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation
        ( TA_Base_Core::AtsAgentEntityData::getStaticType(), ulLocation );

    if ( entityDataList.size() != 1u )
    {
        // Clean up first
        for (TA_Base_Core::IEntityDataList::iterator it = entityDataList.begin(); it != entityDataList.end(); ++it)
        {
            delete *it;
        }
        entityDataList.clear();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] Could not resolve a unique ATS Agent entity key for this location %d", ulLocation );

        FUNCTION_EXIT;
        return;
    }

    TA_Base_Core::IEntityData* atsEntityData = entityDataList[0];
    unsigned long atsAgentKey = atsEntityData->getKey();
    std::string atsAgentName = atsEntityData->getName();

    // clean up entityDataList
    delete atsEntityData;
    atsEntityData = NULL;
    entityDataList.clear();

    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    m_localATSAgentKey = atsAgentKey;
    m_localATSAgentName = atsAgentName;

    FUNCTION_EXIT;
}

// Station or Master
void CtaEcsCachedConfig::processAssetName()
{
	FUNCTION_ENTRY("processAssetName");

    if ( NULL != m_masterEntityData )
    {
		m_assetName = m_masterEntityData->getAssetName();
    }
    else if ( NULL != m_stationEntityData )
    {
		m_assetName = m_stationEntityData->getAssetName();
	}
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of ECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }
    
    FUNCTION_EXIT;
}


// Station or Master
void CtaEcsCachedConfig::processSpareATSAgentKey()
{
	FUNCTION_ENTRY("processSpareATSAgentKey");

    TA_Base_Core::IEntityData* atsEntityData = NULL;

    if ( NULL != m_masterEntityData )
    {
        if(!m_masterEntityData->getSpareAtsAgentName().empty())
        {
            atsEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_masterEntityData->getSpareAtsAgentName());
        }
    }
    else if ( NULL != m_stationEntityData )
    {
        if(!m_stationEntityData->getSpareAtsAgentName().empty())
        {
            atsEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_stationEntityData->getSpareAtsAgentName());
        }
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "NULL pointer of ECSAgentEntityDataPtr" );

        FUNCTION_EXIT;
        return;
    }

    if(atsEntityData != NULL)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_spareATSAgentKey = atsEntityData->getKey();
        m_spareATSAgentName = atsEntityData->getName();
        m_spareATSAgentLocationKey = atsEntityData->getLocation();

        // clean up entityDataList
        delete atsEntityData;
        atsEntityData = NULL;
    }

    FUNCTION_EXIT;
}


///////////////////////////////////////////////////////////////////////////
// Common GetMethods
///////////////////////////////////////////////////////////////////////////

bool CtaEcsCachedConfig::getIsMaster()
{
    // Implicitly threadsafe
    return m_isMaster;
}

bool CtaEcsCachedConfig::getIsInControl()
{
    // Implicitly threadsafe
    return m_isInControl;
}

unsigned long CtaEcsCachedConfig::getAgentKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_agentKey;
}

std::string CtaEcsCachedConfig::getAssetName()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_assetName;
}

std::string CtaEcsCachedConfig::getAgentName()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    if ( m_agentName.empty() )
    {
        m_agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
    }

    return m_agentName.c_str();
}

unsigned long CtaEcsCachedConfig::getAgentTypeKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_agentTypeKey;
}

unsigned long CtaEcsCachedConfig::getAgentLocationKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_agentLocationKey;
}

unsigned long CtaEcsCachedConfig::getAgentSubsystemKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_agentSubsystemKey;
}

bool CtaEcsCachedConfig::getAgentInRuntime()
{
    //Implicitly threadsafe
    return m_agentInRuntime;
}

unsigned long CtaEcsCachedConfig::getWorkerThreadsSleepTimeInMSecs()
{
    //Threadsafe enough - can live with temporary data corruption
    return m_workerThreadsSleepTimeInMSecs;
}


unsigned long CtaEcsCachedConfig::getMaxModeExecutionTimeInSecs()   
{
    //Threadsafe enough - can live with temporary data corruption
    return m_maxModeExecutionTimeInSecs;
}




///////////////////////////////////////////////////////////////////////////
// StationEcsAgent GetMethods
///////////////////////////////////////////////////////////////////////////

bool CtaEcsCachedConfig::getIsOccOnline()
{
    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_isOccOnline;
}

std::string CtaEcsCachedConfig::getOccGroupName()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_occGroupName;
}

unsigned long CtaEcsCachedConfig::getLocalATSAgentKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_localATSAgentKey;
}

std::string CtaEcsCachedConfig::getLocalATSAgentName()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_localATSAgentName;
}

unsigned long CtaEcsCachedConfig::getSpareATSAgentKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_spareATSAgentKey;
}

std::string CtaEcsCachedConfig::getSpareATSAgentName()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_spareATSAgentName;
}

unsigned long CtaEcsCachedConfig::getSpareATSAgentLocationKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_spareATSAgentLocationKey;
}

unsigned long CtaEcsCachedConfig::getThresholdStallTimeInSecs()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_thresholdStallTimeInSecs;
}


unsigned long CtaEcsCachedConfig::getThresholdTemperature()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_thresholdTemperature;
}


unsigned long CtaEcsCachedConfig::getTemperatureDeadbandLowerLimit()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_temperatureDeadbandLowerLimit;
}

bool CtaEcsCachedConfig::getCongestionUsesStallTime()
{
    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_congestionUsesStallTime;
}

bool CtaEcsCachedConfig::getCongestionUsesTemperature()
{
    //Implicitly threadsafe: TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_congestionUsesTemperature;
}

unsigned long CtaEcsCachedConfig::getIscsAlivePollTimeInMSecs()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_iscsAlivePollTimeInMSecs;
}

unsigned long CtaEcsCachedConfig::getNormalStationMode()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_normalStationMode;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyMasterModeLockDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_myMasterModeLockDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyMasterModeLockControlDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_myMasterModeLockControlDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMySelectedMasterModeDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_mySelectedMasterModeDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMySelectedOverridingStationModeDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_mySelectedOverridingStationModeDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyLastAppliedMasterModeDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_myLastAppliedMasterModeDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyLastAppliedStationModeDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_myLastAppliedStationModeDataPoint;
}

TA_Base_Bus::DataPointProxySmartPtr&  CtaEcsCachedConfig::getModeStatusProxy()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_modeStatusProxy;
}

TA_Base_Bus::DataPointProxySmartPtr&  CtaEcsCachedConfig::getLocalModeProxy()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_localModeProxy;
}

TA_Base_Bus::DataPointProxySmartPtr&  CtaEcsCachedConfig::getIscsAliveProxy()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_iscsAliveProxy;
}

unsigned long CtaEcsCachedConfig::getIscsEntityKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    std::string entityParameterValue = m_stationEntityData->getIscsAliveSqlToken();

    unsigned long ulIcscKey = 0u;

    try
    {
        ulIcscKey = getEntityBySqlToken(entityParameterValue)->getKey();
    }
    catch ( const TA_Base_Core::InvalidECSConfigurationException& )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No ISCS key for entity parameter %s", entityParameterValue.c_str() );
    }

	return ulIcscKey;
}

std::string CtaEcsCachedConfig::getIscsAgentName()
{
	TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );

    std::string entityParameterValue = m_stationEntityData->getIscsAliveSqlToken();

    std::string strAgentName = "";

    try
    {
        strAgentName = getEntityBySqlToken(entityParameterValue)->getAgentName();
    }
    catch ( const TA_Base_Core::InvalidECSConfigurationException& )
    {
        strAgentName = "Unknown";
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
            "[DataError] No Agent name for entity parameter %s", entityParameterValue.c_str() );
    }

    return strAgentName;
}

unsigned long CtaEcsCachedConfig::getModeControlDpEntityKey()
{
    TA_Base_Core::ThreadGuard guard(m_lockForConfigUpdate);
    return m_modeControlDpEntity;
}

unsigned long CtaEcsCachedConfig::getModeInProgressDpEntityKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_modeInProgressDpEntity;
}

unsigned long CtaEcsCachedConfig::getAutoModeDpEntityKey()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_autoModeDpEntity;
}

//////////////////////////////////////////////////////////////////////////
// MasterEcsAgent GetMethods
///////////////////////////////////////////////////////////////////////////

unsigned long  CtaEcsCachedConfig::getSelectionTimeoutInMSecs()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_selectionTimeoutInMSecs;
}

unsigned long  CtaEcsCachedConfig::getNormalMasterMode()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_normalMasterMode;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyModeStatusDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_myModeStatusDataPoint;
}

TA_Base_Bus::DataPoint*  CtaEcsCachedConfig::getMyModeInProgressDataPoint()
{
    TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
    return m_myModeInProgressDataPoint;
}
